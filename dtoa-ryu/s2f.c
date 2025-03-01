// Copyright 2019 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#include "ryu_parse.h"

#ifdef RYU_DEBUG
#include <stdio.h>
#include <nanohttp-logging.h>
#endif

#include "common.h"
#include "f2s_intrinsics.h"

#define FLOAT_MANTISSA_BITS 23
#define FLOAT_EXPONENT_BITS 8
#define FLOAT_EXPONENT_BIAS 127

#if RTE_TOOLCHAIN_MSVC
#include <intrin.h>

static inline ng_uint32_t floor_log2(const ng_uint32_t value) {
  unsigned long index;
  return _BitScanReverse(&index, value) ? index : 32;
}

#else

static inline ng_uint32_t floor_log2(const ng_uint32_t value) {
  return 31 - __builtin_clz(value);
}

#endif

// The max function is already defined on Windows.
static inline ng_int32_t max32(ng_int32_t a, ng_int32_t b) {
  return a < b ? b : a;
}

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#endif

static inline float int32Bits2Float(ng_uint32_t bits) {
  return *(float *)&bits;
}

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic pop
#endif
#endif

enum Status s2f_n(const char * buffer, const int len, float * result) {
  if (len == 0) {
    return INPUT_TOO_SHORT;
  }
  int m10digits = 0;
  int e10digits = 0;
  int dotIndex = len;
  int eIndex = len;
  ng_uint32_t m10 = 0;
  ng_int32_t e10 = 0;
  ng_bool_t signedM = ng_FALSE;
  ng_bool_t signedE = ng_FALSE;
  int i = 0;
  if (buffer[i] == '-') {
    signedM = ng_TRUE;
    i++;
  }
  for (; i < len; i++) {
    char c = buffer[i];
    if (c == '.') {
      if (dotIndex != len) {
        return MALFORMED_INPUT;
      }
      dotIndex = i;
      continue;
    }
    if ((c < '0') || (c > '9')) {
      break;
    }
    if (m10digits >= 9) {
      return INPUT_TOO_LONG;
    }
    m10 = 10 * m10 + (c - '0');
    if (m10 != 0) {
      m10digits++;
    }
  }
  if (i < len && ((buffer[i] == 'e') || (buffer[i] == 'E'))) {
    eIndex = i;
    i++;
    if (i < len && ((buffer[i] == '-') || (buffer[i] == '+'))) {
      signedE = buffer[i] == '-';
      i++;
    }
    for (; i < len; i++) {
      char c = buffer[i];
      if ((c < '0') || (c > '9')) {
        return MALFORMED_INPUT;
      }
      if (e10digits > 3) {
        // TODO: Be more lenient. Return +/-Infinity or +/-0 instead.
        return INPUT_TOO_LONG;
      }
      e10 = 10 * e10 + (c - '0');
      if (e10 != 0) {
        e10digits++;
      }
    }
  }
  if (i < len) {
    return MALFORMED_INPUT;
  }
  if (signedE) {
    e10 = -e10;
  }
  e10 -= dotIndex < eIndex ? eIndex - dotIndex - 1 : 0;
  if (m10 == 0) {
    *result = signedM ? -0.0f : 0.0f;
    return SUCCESS;
  }

#ifdef RYU_DEBUG
  log_debug("Input=%s", buffer);
  log_debug("m10digits = %d", m10digits);
  log_debug("e10digits = %d", e10digits);
  log_debug("m10 * 10^e10 = %u * 10^%d", m10, e10);
#endif

  if ((m10digits + e10 <= -46) || (m10 == 0)) {
    // Number is less than 1e-46, which should be rounded down to 0; return +/-0.0.
    ng_uint32_t ieee = ((ng_uint32_t) signedM) << (FLOAT_EXPONENT_BITS + FLOAT_MANTISSA_BITS);
    *result = int32Bits2Float(ieee);
    return SUCCESS;
  }
  if (m10digits + e10 >= 40) {
    // Number is larger than 1e+39, which should be rounded to +/-Infinity.
    ng_uint32_t ieee = (((ng_uint32_t) signedM) << (FLOAT_EXPONENT_BITS + FLOAT_MANTISSA_BITS)) | (0xffu << FLOAT_MANTISSA_BITS);
    *result = int32Bits2Float(ieee);
    return SUCCESS;
  }

  // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion
  // was exact (trailingZeros).
  ng_int32_t e2;
  ng_uint32_t m2;
  ng_bool_t trailingZeros;
  if (e10 >= 0) {
    // The length of m * 10^e in bits is:
    //   log2(m10 * 10^e10) = log2(m10) + e10 log2(10) = log2(m10) + e10 + e10 * log2(5)
    //
    // We want to compute the FLOAT_MANTISSA_BITS + 1 top-most bits (+1 for the implicit leading
    // one in IEEE format). We therefore choose a binary output exponent of
    //   log2(m10 * 10^e10) - (FLOAT_MANTISSA_BITS + 1).
    //
    // We use floor(log2(5^e10)) so that we get at least this many bits; better to
    // have an additional bit than to not have enough bits.
    e2 = floor_log2(m10) + e10 + log2pow5(e10) - (FLOAT_MANTISSA_BITS + 1);

    // We now compute [m10 * 10^e10 / 2^e2] = [m10 * 5^e10 / 2^(e2-e10)].
    // To that end, we use the FLOAT_POW5_SPLIT table.
    int j = e2 - e10 - ceil_log2pow5(e10) + FLOAT_POW5_BITCOUNT;
    NG_ASSERT(j >= 0);
    m2 = mulPow5divPow2(m10, e10, j);

    // We also compute if the result is exact, i.e.,
    //   [m10 * 10^e10 / 2^e2] == m10 * 10^e10 / 2^e2.
    // This can only be the case if 2^e2 divides m10 * 10^e10, which in turn requires that the
    // largest power of 2 that divides m10 + e10 is greater than e2. If e2 is less than e10, then
    // the result must be exact. Otherwise we use the existing multipleOfPowerOf2 function.
    trailingZeros = e2 < e10 || (e2 - e10 < 32 && multipleOfPowerOf2_32(m10, e2 - e10));
  } else {
    e2 = floor_log2(m10) + e10 - ceil_log2pow5(-e10) - (FLOAT_MANTISSA_BITS + 1);

    // We now compute [m10 * 10^e10 / 2^e2] = [m10 / (5^(-e10) 2^(e2-e10))].
    int j = e2 - e10 + ceil_log2pow5(-e10) - 1 + FLOAT_POW5_INV_BITCOUNT;
    m2 = mulPow5InvDivPow2(m10, -e10, j);

    // We also compute if the result is exact, i.e.,
    //   [m10 / (5^(-e10) 2^(e2-e10))] == m10 / (5^(-e10) 2^(e2-e10))
    //
    // If e2-e10 >= 0, we need to check whether (5^(-e10) 2^(e2-e10)) divides m10, which is the
    // case iff pow5(m10) >= -e10 AND pow2(m10) >= e2-e10.
    //
    // If e2-e10 < 0, we have actually computed [m10 * 2^(e10 e2) / 5^(-e10)] above,
    // and we need to check whether 5^(-e10) divides (m10 * 2^(e10-e2)), which is the case iff
    // pow5(m10 * 2^(e10-e2)) = pow5(m10) >= -e10.
    trailingZeros = (e2 < e10 || (e2 - e10 < 32 && multipleOfPowerOf2_32(m10, e2 - e10)))
        && multipleOfPowerOf5_32(m10, -e10);
  }

#ifdef RYU_DEBUG
  log_debug("m2 * 2^e2 = %u * 2^%d", m2, e2);
#endif

  // Compute the final IEEE exponent.
  ng_uint32_t ieee_e2 = (ng_uint32_t) max32(0, e2 + FLOAT_EXPONENT_BIAS + floor_log2(m2));

  if (ieee_e2 > 0xfe) {
    // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
    ng_uint32_t ieee = (((ng_uint32_t) signedM) << (FLOAT_EXPONENT_BITS + FLOAT_MANTISSA_BITS)) | (0xffu << FLOAT_MANTISSA_BITS);
    *result = int32Bits2Float(ieee);
    return SUCCESS;
  }

  // We need to figure out how much we need to shift m2. The tricky part is that we need to take
  // the final IEEE exponent into account, so we need to reverse the bias and also special-case
  // the value 0.
  ng_int32_t shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - FLOAT_EXPONENT_BIAS - FLOAT_MANTISSA_BITS;
  NG_ASSERT(shift >= 0);
#ifdef RYU_DEBUG
  log_debug("ieee_e2 = %d", ieee_e2);
  log_debug("shift   = %d", shift);
#endif

  // We need to round up if the exact value is more than 0.5 above the value we computed. That's
  // equivalent to checking if the last removed bit was 1 and either the value was not just
  // trailing zeros or the result would otherwise be odd.
  //
  // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
  trailingZeros &= (m2 & ((1u << (shift - 1)) - 1)) == 0;
  ng_uint32_t lastRemovedBit = (m2 >> (shift - 1)) & 1;
  ng_bool_t roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

#ifdef RYU_DEBUG
  log_debug("roundUp = %d", roundUp);
  log_debug("ieee_m2 = %u", (m2 >> shift) + roundUp);
#endif
  ng_uint32_t ieee_m2 = (m2 >> shift) + roundUp;
  NG_ASSERT(ieee_m2 <= (1u << (FLOAT_MANTISSA_BITS + 1)));
  ieee_m2 &= (1u << FLOAT_MANTISSA_BITS) - 1;
  if (ieee_m2 == 0 && roundUp) {
    // Rounding up may overflow the mantissa.
    // In this case we move a trailing zero of the mantissa into the exponent.
    // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
    ieee_e2++;
  }
  ng_uint32_t ieee = (((((ng_uint32_t) signedM) << FLOAT_EXPONENT_BITS) 
    | (ng_uint32_t)ieee_e2) << FLOAT_MANTISSA_BITS) | ieee_m2;
  *result = int32Bits2Float(ieee);
  return SUCCESS;
}

enum Status s2f(const char * buffer, float * result) {
  return s2f_n(buffer, ng_strlen(buffer), result);
}
