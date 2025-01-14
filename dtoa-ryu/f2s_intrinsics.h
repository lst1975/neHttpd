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
#ifndef RYU_F2S_INTRINSICS_H
#define RYU_F2S_INTRINSICS_H

// Defines RYU_32_BIT_PLATFORM if applicable.
#include "common.h"

#define RYU_FLOAT_FULL_TABLE
#undef  RYU_OPTIMIZE_SIZE

#if defined(RYU_FLOAT_FULL_TABLE)
#include "f2s_full_table.h"
#else
#if defined(RYU_OPTIMIZE_SIZE)
#include "d2s_small_table.h"
#else
#include "d2s_full_table.h"
#endif
#define FLOAT_POW5_INV_BITCOUNT (DOUBLE_POW5_INV_BITCOUNT - 64)
#define FLOAT_POW5_BITCOUNT (DOUBLE_POW5_BITCOUNT - 64)
#endif

static inline ng_uint32_t pow5factor_32(ng_uint32_t value) {
  ng_uint32_t count = 0;
  for (;;) {
    assert(value != 0);
    const ng_uint32_t q = value / 5;
    const ng_uint32_t r = value % 5;
    if (r != 0) {
      break;
    }
    value = q;
    ++count;
  }
  return count;
}

// Returns true if value is divisible by 5^p.
static inline ng_bool_t multipleOfPowerOf5_32(const ng_uint32_t value, const ng_uint32_t p) {
  return pow5factor_32(value) >= p;
}

// Returns true if value is divisible by 2^p.
static inline ng_bool_t multipleOfPowerOf2_32(const ng_uint32_t value, const ng_uint32_t p) {
  // __builtin_ctz doesn't appear to be faster here.
  return (value & ((1u << p) - 1)) == 0;
}

// It seems to be slightly faster to avoid uint128_t here, although the
// generated code for uint128_t looks slightly nicer.
static inline ng_uint32_t mulShift32(const ng_uint32_t m, const ng_uint64_t factor, const ng_int32_t shift) {
  assert(shift > 32);

  // The casts here help MSVC to avoid calls to the __allmul library
  // function.
  const ng_uint32_t factorLo = (ng_uint32_t)(factor);
  const ng_uint32_t factorHi = (ng_uint32_t)(factor >> 32);
  const ng_uint64_t bits0 = (ng_uint64_t)m * factorLo;
  const ng_uint64_t bits1 = (ng_uint64_t)m * factorHi;

#if defined(RYU_32_BIT_PLATFORM)
  // On 32-bit platforms we can avoid a 64-bit shift-right since we only
  // need the upper 32 bits of the result and the shift value is > 32.
  const ng_uint32_t bits0Hi = (ng_uint32_t)(bits0 >> 32);
  ng_uint32_t bits1Lo = (ng_uint32_t)(bits1);
  ng_uint32_t bits1Hi = (ng_uint32_t)(bits1 >> 32);
  bits1Lo += bits0Hi;
  bits1Hi += (bits1Lo < bits0Hi);
  if (shift >= 64) {
    // s2f can call this with a shift value >= 64, which we have to handle.
    // This could now be slower than the !defined(RYU_32_BIT_PLATFORM) case.
    return (ng_uint32_t)(bits1Hi >> (shift - 64));
  } else {
    const ng_int32_t s = shift - 32;
    return (bits1Hi << (32 - s)) | (bits1Lo >> s);
  }
#else // RYU_32_BIT_PLATFORM
  const ng_uint64_t sum = (bits0 >> 32) + bits1;
  const ng_uint64_t shiftedSum = sum >> (shift - 32);
  assert(shiftedSum <= NG_UINT32_MAX);
  return (ng_uint32_t) shiftedSum;
#endif // RYU_32_BIT_PLATFORM
}

static inline ng_uint32_t mulPow5InvDivPow2(const ng_uint32_t m, const ng_uint32_t q, const ng_int32_t j) {
#if defined(RYU_FLOAT_FULL_TABLE)
  return mulShift32(m, FLOAT_POW5_INV_SPLIT[q], j);
#elif defined(RYU_OPTIMIZE_SIZE)
  // The inverse multipliers are defined as [2^x / 5^y] + 1; the upper 64 bits from the double lookup
  // table are the correct bits for [2^x / 5^y], so we have to add 1 here. Note that we rely on the
  // fact that the added 1 that's already stored in the table never overflows into the upper 64 bits.
  ng_uint64_t pow5[2];
  double_computeInvPow5(q, pow5);
  return mulShift32(m, pow5[1] + 1, j);
#else
  return mulShift32(m, DOUBLE_POW5_INV_SPLIT[q][1] + 1, j);
#endif
}

static inline ng_uint32_t mulPow5divPow2(const ng_uint32_t m, const ng_uint32_t i, const ng_int32_t j) {
#if defined(RYU_FLOAT_FULL_TABLE)
  return mulShift32(m, FLOAT_POW5_SPLIT[i], j);
#elif defined(RYU_OPTIMIZE_SIZE)
  ng_uint64_t pow5[2];
  double_computePow5(i, pow5);
  return mulShift32(m, pow5[1], j);
#else
  return mulShift32(m, DOUBLE_POW5_SPLIT[i][1], j);
#endif
}

#endif // RYU_F2S_INTRINSICS_H
