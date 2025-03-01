// Copyright 2018 Ulf Adams
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

// Runtime compiler options:
// -DRYU_DEBUG Generate verbose debugging output to stdout.

#include "ryu.h"

#include "common.h"
#include "f2s_intrinsics.h"
#include "digit_table.h"

#include <nanohttp-itoa.h>

#ifdef RYU_DEBUG
#include <nanohttp-logging.h>
#endif

#define FLOAT_MANTISSA_BITS 23
#define FLOAT_EXPONENT_BITS 8
#define FLOAT_BIAS 127

// A floating decimal representing m * 10^e.
typedef struct floating_decimal_32 {
  ng_uint32_t mantissa;
  // Decimal exponent's range is -45 to 38
  // inclusive, and can fit in a short if needed.
  ng_int32_t exponent;
} floating_decimal_32;

static inline floating_decimal_32 f2d(const ng_uint32_t ieeeMantissa, const ng_uint32_t ieeeExponent) {
  ng_int32_t e2;
  ng_uint32_t m2;
  if (ieeeExponent == 0) {
    // We subtract 2 so that the bounds computation has 2 additional bits.
    e2 = 1 - FLOAT_BIAS - FLOAT_MANTISSA_BITS - 2;
    m2 = ieeeMantissa;
  } else {
    e2 = (ng_int32_t) ieeeExponent - FLOAT_BIAS - FLOAT_MANTISSA_BITS - 2;
    m2 = (1u << FLOAT_MANTISSA_BITS) | ieeeMantissa;
  }
  const ng_bool_t even = (m2 & 1) == 0;
  const ng_bool_t acceptBounds = even;

#ifdef RYU_DEBUG
  log_debug("-> %u * 2^%d", m2, e2 + 2);
#endif

  // Step 2: Determine the interval of valid decimal representations.
  const ng_uint32_t mv = 4 * m2;
  const ng_uint32_t mp = 4 * m2 + 2;
  // Implicit bool -> int conversion. True is 1, false is 0.
  const ng_uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
  const ng_uint32_t mm = 4 * m2 - 1 - mmShift;

  // Step 3: Convert to a decimal power base using 64-bit arithmetic.
  ng_uint32_t vr, vp, vm;
  ng_int32_t e10;
  ng_bool_t vmIsTrailingZeros = ng_FALSE;
  ng_bool_t vrIsTrailingZeros = ng_FALSE;
  ng_uint8_t lastRemovedDigit = 0;
  if (e2 >= 0) {
    const ng_uint32_t q = log10Pow2(e2);
    e10 = (ng_int32_t) q;
    const ng_int32_t k = FLOAT_POW5_INV_BITCOUNT + pow5bits((ng_int32_t) q) - 1;
    const ng_int32_t i = -e2 + (ng_int32_t) q + k;
    vr = mulPow5InvDivPow2(mv, q, i);
    vp = mulPow5InvDivPow2(mp, q, i);
    vm = mulPow5InvDivPow2(mm, q, i);
#ifdef RYU_DEBUG
    log_debug("%u * 2^%d / 10^%u", mv, e2, q);
    log_debug("V+=%u", vp);
    log_debug("V =%u", vr);
    log_debug("V-=%u", vm);
#endif
    if (q != 0 && (vp - 1) / 10 <= vm / 10) {
      // We need to know one removed digit even if we are not going to loop below. We could use
      // q = X - 1 above, except that would require 33 bits for the result, and we've found that
      // 32-bit arithmetic is faster even on 64-bit machines.
      const ng_int32_t l = FLOAT_POW5_INV_BITCOUNT + pow5bits((ng_int32_t) (q - 1)) - 1;
      lastRemovedDigit = (ng_uint8_t) (mulPow5InvDivPow2(mv, q - 1, -e2 + (ng_int32_t) q - 1 + l) % 10);
    }
    if (q <= 9) {
      // The largest power of 5 that fits in 24 bits is 5^10, but q <= 9 seems to be safe as well.
      // Only one of mp, mv, and mm can be a multiple of 5, if any.
      if (mv % 5 == 0) {
        vrIsTrailingZeros = multipleOfPowerOf5_32(mv, q);
      } else if (acceptBounds) {
        vmIsTrailingZeros = multipleOfPowerOf5_32(mm, q);
      } else {
        vp -= multipleOfPowerOf5_32(mp, q);
      }
    }
  } else {
    const ng_uint32_t q = log10Pow5(-e2);
    e10 = (ng_int32_t) q + e2;
    const ng_int32_t i = -e2 - (ng_int32_t) q;
    const ng_int32_t k = pow5bits(i) - FLOAT_POW5_BITCOUNT;
    ng_int32_t j = (ng_int32_t) q - k;
    vr = mulPow5divPow2(mv, (ng_uint32_t) i, j);
    vp = mulPow5divPow2(mp, (ng_uint32_t) i, j);
    vm = mulPow5divPow2(mm, (ng_uint32_t) i, j);
#ifdef RYU_DEBUG
    log_debug("%u * 5^%d / 10^%u", mv, -e2, q);
    log_debug("%u %d %d %d", q, i, k, j);
    log_debug("V+=%u", vp);
    log_debug("V =%u", vr);
    log_debug("V-=%u", vm);
#endif
    if (q != 0 && (vp - 1) / 10 <= vm / 10) {
      j = (ng_int32_t) q - 1 - (pow5bits(i + 1) - FLOAT_POW5_BITCOUNT);
      lastRemovedDigit = (ng_uint8_t) (mulPow5divPow2(mv, (ng_uint32_t) (i + 1), j) % 10);
    }
    if (q <= 1) {
      // {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
      // mv = 4 * m2, so it always has at least two trailing 0 bits.
      vrIsTrailingZeros = ng_TRUE;
      if (acceptBounds) {
        // mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
        vmIsTrailingZeros = mmShift == 1;
      } else {
        // mp = mv + 2, so it always has at least one trailing 0 bit.
        --vp;
      }
    } else if (q < 31) { // TODO(ulfjack): Use a tighter bound here.
      vrIsTrailingZeros = multipleOfPowerOf2_32(mv, q - 1);
#ifdef RYU_DEBUG
      log_debug("vr is trailing zeros=%s", vrIsTrailingZeros ? "true" : "false");
#endif
    }
  }
#ifdef RYU_DEBUG
  log_debug("e10=%d", e10);
  log_debug("V+=%u", vp);
  log_debug("V =%u", vr);
  log_debug("V-=%u", vm);
  log_debug("vm is trailing zeros=%s", vmIsTrailingZeros ? "true" : "false");
  log_debug("vr is trailing zeros=%s", vrIsTrailingZeros ? "true" : "false");
#endif

  // Step 4: Find the shortest decimal representation in the interval of valid representations.
  ng_int32_t removed = 0;
  ng_uint32_t output;
  if (vmIsTrailingZeros || vrIsTrailingZeros) {
    // General case, which happens rarely (~4.0%).
    while (vp / 10 > vm / 10) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=23106
      // The compiler does not realize that vm % 10 can be computed from vm / 10
      // as vm - (vm / 10) * 10.
      vmIsTrailingZeros &= vm - (vm / 10) * 10 == 0;
#else
      vmIsTrailingZeros &= vm % 10 == 0;
#endif
      vrIsTrailingZeros &= lastRemovedDigit == 0;
      lastRemovedDigit = (ng_uint8_t) (vr % 10);
      vr /= 10;
      vp /= 10;
      vm /= 10;
      ++removed;
    }
#ifdef RYU_DEBUG
    log_debug("V+=%u", vp);
    log_debug("V =%u", vr);
    log_debug("V-=%u", vm);
    log_debug("d-10=%s", vmIsTrailingZeros ? "true" : "false");
#endif
    if (vmIsTrailingZeros) {
      while (vm % 10 == 0) {
        vrIsTrailingZeros &= lastRemovedDigit == 0;
        lastRemovedDigit = (ng_uint8_t) (vr % 10);
        vr /= 10;
        vp /= 10;
        vm /= 10;
        ++removed;
      }
    }
#ifdef RYU_DEBUG
    log_debug("%u %d", vr, lastRemovedDigit);
    log_debug("vr is trailing zeros=%s", vrIsTrailingZeros ? "true" : "false");
#endif
    if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
      // Round even if the exact number is .....50..0.
      lastRemovedDigit = 4;
    }
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
  } else {
    // Specialized for the common case (~96.0%). Percentages below are relative to this.
    // Loop iterations below (approximately):
    // 0: 13.6%, 1: 70.7%, 2: 14.1%, 3: 1.39%, 4: 0.14%, 5+: 0.01%
    while (vp / 10 > vm / 10) {
      lastRemovedDigit = (ng_uint8_t) (vr % 10);
      vr /= 10;
      vp /= 10;
      vm /= 10;
      ++removed;
    }
#ifdef RYU_DEBUG
    log_debug("%u %d", vr, lastRemovedDigit);
    log_debug("vr is trailing zeros=%s", vrIsTrailingZeros ? "true" : "false");
#endif
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + (vr == vm || lastRemovedDigit >= 5);
  }
  const ng_int32_t exp = e10 + removed;

#ifdef RYU_DEBUG
  log_debug("V+=%u", vp);
  log_debug("V =%u", vr);
  log_debug("V-=%u", vm);
  log_debug("O=%u", output);
  log_debug("EXP=%d", exp);
#endif

  floating_decimal_32 fd;
  fd.exponent = exp;
  fd.mantissa = output;
  return fd;
}

static inline int 
f2s_to_chars(const floating_decimal_32 v, const ng_bool_t sign, 
  char* const result, int result_len, int *decimal_point) {
  // Step 5: Print the decimal representation.
  int index = 0;
  if (sign) {
    result[index++] = '-';
  }

#if 0
  ng_uint32_t output = v.mantissa;
  const ng_uint32_t olength = decimalLength9(output);

#ifdef RYU_DEBUG
  log_debug("DIGITS=%u", v.mantissa);
  log_debug("OLEN=%u", olength);
  log_debug("EXP=%u", v.exponent + olength);
#endif

  // Print the decimal digits.
  // The following code is equivalent to:
  // for (ng_uint32_t i = 0; i < olength - 1; ++i) {
  //   const ng_uint32_t c = output % 10; output /= 10;
  //   result[index + olength - i] = (char) ('0' + c);
  // }
  // result[index] = '0' + output % 10;
  ng_uint32_t i = 0;
  while (output >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
    const ng_uint32_t c = output - 10000 * (output / 10000);
#else
    const ng_uint32_t c = output % 10000;
#endif
    output /= 10000;
    const ng_uint32_t c0 = (c % 100) << 1;
    const ng_uint32_t c1 = (c / 100) << 1;
    ng_memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
    ng_memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
    i += 4;
  }
  if (output >= 100) {
    const ng_uint32_t c = (output % 100) << 1;
    output /= 100;
    ng_memcpy(result + index + olength - i - 1, DIGIT_TABLE + c, 2);
    i += 2;
  }
  if (output >= 10) {
    const ng_uint32_t c = output << 1;
    // We can't use memcpy here: the decimal dot goes between these two digits.
    result[index + olength - i] = DIGIT_TABLE[c + 1];
    result[index] = DIGIT_TABLE[c];
  } else {
    result[index] = (char) ('0' + output);
  }

  // Print decimal point if needed.
  if (olength > 1) {
    result[index + 1] = '.';
    index += olength + 1;
  } else {
    ++index;
  }

#else
  int has_decimal = (v.mantissa > 9);
  int consumed_len = index + has_decimal;
  const ng_uint32_t olength = u32toa_jeaiii(v.mantissa, 
    result + consumed_len, result_len - consumed_len);

  // Print decimal point if needed.
  if (has_decimal) 
  {
    result[index] = result[index+1];
    ++index;
    if (decimal_point != NULL)
      *decimal_point = index;
    result[index] = '.';
    index += olength;
  } 
  else 
  {
    ++index;
  }
#endif

  rte_prefetch0(EXPONENT_TABLE);

  // Print the exponent.
  result[index++] = 'E';
  ng_int32_t exp = v.exponent + (ng_int32_t) olength - 1;
  if (exp < 0) {
    result[index++] = '-';
    exp = -exp;
  }

#if 0
  if (exp >= 10) {
    ng_memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
    index += 2;
  } else {
    result[index++] = (char) ('0' + exp);
  }

  return index;
#else
  NG_ASSERT(exp  <= __float_power_of_ten_MAX);
  *(ng_uint16_t *)(result + index) = *(const ng_uint16_t *)EXPONENT_TABLE[exp];
  return index + (exp >= 10 ? 2 : 2);
#endif
}

int __f2s_buffered_n(float f, char* result, int result_len, int *decimal_point) {
  // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
  const ng_uint32_t bits = float_to_bits(f);

#ifdef RYU_DEBUG
  log_print("IN=");
  for (ng_int32_t bit = 31; bit >= 0; --bit) {
    log_print("%u", (bits >> bit) & 1);
  }
  log_print("\n");
#endif

  // Decode bits into sign, mantissa, and exponent.
  const ng_bool_t ieeeSign = ((bits >> (FLOAT_MANTISSA_BITS + FLOAT_EXPONENT_BITS)) & 1) != 0;
  const ng_uint32_t ieeeMantissa = bits & ((1u << FLOAT_MANTISSA_BITS) - 1);
  const ng_uint32_t ieeeExponent = (bits >> FLOAT_MANTISSA_BITS) & ((1u << FLOAT_EXPONENT_BITS) - 1);

  // Case distinction; exit early for the easy cases.
  if (ieeeExponent == ((1u << FLOAT_EXPONENT_BITS) - 1u) || (ieeeExponent == 0 && ieeeMantissa == 0)) {
    return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
  }

  const floating_decimal_32 v = f2d(ieeeMantissa, ieeeExponent);
  return f2s_to_chars(v, ieeeSign, result, result_len, decimal_point);
}

#if RYU_TEST
void f2s_buffered(float f, char* result) {
  const int index = f2s_buffered_n(f, result);

  // Terminate the string.
  result[index] = '\0';
}

char* f2s(float f) {
  char* const result = (char*) ng_malloc(16);
  f2s_buffered(f, result);
  return result;
}
#endif
