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
#ifndef RYU_COMMON_H
#define RYU_COMMON_H

#include "ryu.h"

#if defined(_M_IX86) || defined(_M_ARM)
#define RYU_32_BIT_PLATFORM
#endif

// Returns the number of decimal digits in v, which must not contain more than 9 digits.
static inline ng_uint32_t decimalLength9(const ng_uint32_t v) {
  // Function precondition: v is not a 10-digit number.
  // (f2s: 9 digits are sufficient for round-tripping.)
  // (d2fixed: We print 9-digit blocks.)
  NG_ASSERT(v < 1000000000);
  if (v >= 100000000) { return 9; }
  if (v >= 10000000) { return 8; }
  if (v >= 1000000) { return 7; }
  if (v >= 100000) { return 6; }
  if (v >= 10000) { return 5; }
  if (v >= 1000) { return 4; }
  if (v >= 100) { return 3; }
  if (v >= 10) { return 2; }
  return 1;
}

// Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
static inline ng_int32_t log2pow5(const ng_int32_t e) {
  // This approximation works up to the point that the multiplication overflows at e = 3529.
  // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
  // than 2^9297.
  NG_ASSERT(e >= 0);
  NG_ASSERT(e <= 3528);
  return (ng_int32_t) ((((ng_uint32_t) e) * 1217359) >> 19);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
static inline ng_int32_t pow5bits(const ng_int32_t e) {
  // This approximation works up to the point that the multiplication overflows at e = 3529.
  // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
  // than 2^9297.
  NG_ASSERT(e >= 0);
  NG_ASSERT(e <= 3528);
  return (ng_int32_t) (((((ng_uint32_t) e) * 1217359) >> 19) + 1);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
static inline ng_int32_t ceil_log2pow5(const ng_int32_t e) {
  return log2pow5(e) + 1;
}

// Returns floor(log_10(2^e)); requires 0 <= e <= 1650.
static inline ng_uint32_t log10Pow2(const ng_int32_t e) {
  // The first value this approximation fails for is 2^1651 which is just greater than 10^297.
  NG_ASSERT(e >= 0);
  NG_ASSERT(e <= 1650);
  return (((ng_uint32_t) e) * 78913) >> 18;
}

// Returns floor(log_10(5^e)); requires 0 <= e <= 2620.
static inline ng_uint32_t log10Pow5(const ng_int32_t e) {
  // The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
  NG_ASSERT(e >= 0);
  NG_ASSERT(e <= 2620);
  return (((ng_uint32_t) e) * 732923) >> 20;
}

static inline int copy_special_str(char * const result, const ng_bool_t sign, const ng_bool_t exponent, const ng_bool_t mantissa) {
  if (mantissa) {
    D2X_memcpy_NaN(result);
    return 3;
  }
  if (sign) {
    result[0] = '-';
  }
  if (exponent) {
    D2X_memcpy_Infinity(result + sign);
    return sign + 8;
  }
  D2X_memcpy_3Byte(result + sign, "0E0");
  return sign + 3;
}

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#endif

static inline ng_uint32_t float_to_bits(const float f) {
  return *(const ng_uint32_t *)&f;
}

static inline ng_uint64_t double_to_bits(const double d) {
  return *(const ng_uint64_t *)&d;
}

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic pop
#endif
#endif

#endif // RYU_COMMON_H
