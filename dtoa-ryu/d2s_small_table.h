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
#ifndef RYU_D2S_SMALL_TABLE_H
#define RYU_D2S_SMALL_TABLE_H

// Defines HAS_UINT128 and uint128_t if applicable.
#include "d2s_intrinsics.h"

// These tables are generated by PrintDoubleLookupTable.
#define DOUBLE_POW5_INV_BITCOUNT 125
#define DOUBLE_POW5_BITCOUNT 125

static const uint64_t DOUBLE_POW5_INV_SPLIT2[15][2] = {
  {                    1ull, 2305843009213693952ull },
  {  5955668970331000884ull, 1784059615882449851ull },
  {  8982663654677661702ull, 1380349269358112757ull },
  {  7286864317269821294ull, 2135987035920910082ull },
  {  7005857020398200553ull, 1652639921975621497ull },
  { 17965325103354776697ull, 1278668206209430417ull },
  {  8928596168509315048ull, 1978643211784836272ull },
  { 10075671573058298858ull, 1530901034580419511ull },
  {   597001226353042382ull, 1184477304306571148ull },
  {  1527430471115325346ull, 1832889850782397517ull },
  { 12533209867169019542ull, 1418129833677084982ull },
  {  5577825024675947042ull, 2194449627517475473ull },
  { 11006974540203867551ull, 1697873161311732311ull },
  { 10313493231639821582ull, 1313665730009899186ull },
  { 12701016819766672773ull, 2032799256770390445ull }
};
static const uint32_t POW5_INV_OFFSETS[19] = {
  0x54544554u, 0x04055545u, 0x10041000u, 0x00400414u, 0x40010000u, 0x41155555u,
  0x00000454u, 0x00010044u, 0x40000000u, 0x44000041u, 0x50454450u, 0x55550054u,
  0x51655554u, 0x40004000u, 0x01000001u, 0x00010500u, 0x51515411u, 0x05555554u,
  0x00000000u
};

static const uint64_t DOUBLE_POW5_SPLIT2[13][2] = {
  {                    0ull, 1152921504606846976ull },
  {                    0ull, 1490116119384765625ull },
  {  1032610780636961552ull, 1925929944387235853ull },
  {  7910200175544436838ull, 1244603055572228341ull },
  { 16941905809032713930ull, 1608611746708759036ull },
  { 13024893955298202172ull, 2079081953128979843ull },
  {  6607496772837067824ull, 1343575221513417750ull },
  { 17332926989895652603ull, 1736530273035216783ull },
  { 13037379183483547984ull, 2244412773384604712ull },
  {  1605989338741628675ull, 1450417759929778918ull },
  {  9630225068416591280ull, 1874621017369538693ull },
  {   665883850346957067ull, 1211445438634777304ull },
  { 14931890668723713708ull, 1565756531257009982ull }
};
static const uint32_t POW5_OFFSETS[21] = {
  0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x40000000u, 0x59695995u,
  0x55545555u, 0x56555515u, 0x41150504u, 0x40555410u, 0x44555145u, 0x44504540u,
  0x45555550u, 0x40004000u, 0x96440440u, 0x55565565u, 0x54454045u, 0x40154151u,
  0x55559155u, 0x51405555u, 0x00000105u
};

#define POW5_TABLE_SIZE 26
static const uint64_t DOUBLE_POW5_TABLE[POW5_TABLE_SIZE] = {
1ull, 5ull, 25ull, 125ull, 625ull, 3125ull, 15625ull, 78125ull, 390625ull,
1953125ull, 9765625ull, 48828125ull, 244140625ull, 1220703125ull, 6103515625ull,
30517578125ull, 152587890625ull, 762939453125ull, 3814697265625ull,
19073486328125ull, 95367431640625ull, 476837158203125ull,
2384185791015625ull, 11920928955078125ull, 59604644775390625ull,
298023223876953125ull //, 1490116119384765625ull
};

#if defined(HAS_UINT128)

// Computes 5^i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computePow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = i / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = i - base2;
  const uint64_t* const mul = DOUBLE_POW5_SPLIT2[base];
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset];
  const uint128_t b0 = ((uint128_t) m) * mul[0];
  const uint128_t b2 = ((uint128_t) m) * mul[1];
  const uint32_t delta = pow5bits(i) - pow5bits(base2);
  const uint128_t shiftedSum = (b0 >> delta) + (b2 << (64 - delta)) + ((POW5_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[0] = (uint64_t) shiftedSum;
  result[1] = (uint64_t) (shiftedSum >> 64);
}

// Computes 5^-i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computeInvPow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = (i + POW5_TABLE_SIZE - 1) / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = base2 - i;
  const uint64_t* const mul = DOUBLE_POW5_INV_SPLIT2[base]; // 1/5^base2
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset]; // 5^offset
  const uint128_t b0 = ((uint128_t) m) * (mul[0] - 1);
  const uint128_t b2 = ((uint128_t) m) * mul[1]; // 1/5^base2 * 5^offset = 1/5^(base2-offset) = 1/5^i
  const uint32_t delta = pow5bits(base2) - pow5bits(i);
  const uint128_t shiftedSum =
    ((b0 >> delta) + (b2 << (64 - delta))) + 1 + ((POW5_INV_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[0] = (uint64_t) shiftedSum;
  result[1] = (uint64_t) (shiftedSum >> 64);
}

#else // defined(HAS_UINT128)

// Computes 5^i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computePow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = i / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = i - base2;
  const uint64_t* const mul = DOUBLE_POW5_SPLIT2[base];
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset];
  uint64_t high1;
  const uint64_t low1 = umul128(m, mul[1], &high1);
  uint64_t high0;
  const uint64_t low0 = umul128(m, mul[0], &high0);
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  // high1 | sum | low0
  const uint32_t delta = pow5bits(i) - pow5bits(base2);
  result[0] = shiftright128(low0, sum, delta) + ((POW5_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[1] = shiftright128(sum, high1, delta);
}

// Computes 5^-i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computeInvPow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = (i + POW5_TABLE_SIZE - 1) / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = base2 - i;
  const uint64_t* const mul = DOUBLE_POW5_INV_SPLIT2[base]; // 1/5^base2
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset];
  uint64_t high1;
  const uint64_t low1 = umul128(m, mul[1], &high1);
  uint64_t high0;
  const uint64_t low0 = umul128(m, mul[0] - 1, &high0);
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  // high1 | sum | low0
  const uint32_t delta = pow5bits(base2) - pow5bits(i);
  result[0] = shiftright128(low0, sum, delta) + 1 + ((POW5_INV_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[1] = shiftright128(sum, high1, delta);
}

#endif // defined(HAS_UINT128)

#endif // RYU_D2S_SMALL_TABLE_H