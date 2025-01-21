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
#ifndef RYU_H
#define RYU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <nanohttp-defs.h>

#define RYU_TEST 0

#define RYU_NaN      "NaN"
#define RYU_Infinity "Infinity"
#define RYU_nan      "nan"
#define RYU_snan     "snan"

static __ng_inline__ void D2X_memcpy_Infinity(char *b) 
{
  *(ng_uint64_t *)b = *(const ng_uint64_t *)RYU_Infinity;
}

static __ng_inline__ void D2X_memcpy_snan(char *b) 
{
  *(ng_uint32_t *)b = *(const ng_uint32_t *)RYU_snan;
}

static __ng_inline__ void D2X_memcpy_3Byte(char *b, const char *s) 
{
  *(ng_uint16_t *)b = *(const ng_uint16_t *)s;
  b[2] = s[2];
}

static __ng_inline__ void D2X_memcpy_NaN(char *b) 
{
  D2X_memcpy_3Byte(b, RYU_NaN);
}

static __ng_inline__ void D2X_memcpy_nan(char *b) 
{
  D2X_memcpy_3Byte(b, RYU_nan);
}

int __d2s_buffered_n(double f, char* result, int result_len, int *decimal_point);
int __f2s_buffered_n(float f, char* result, int result_len, int *decimal_point);

int d2fixed_buffered_n(double d, ng_uint32_t precision, char* result);
int d2exp_buffered_n(double d, ng_uint32_t precision, char* result);

static __ng_inline__ int d2s_buffered_n(double f, char* result, int result_len) 
{
  return __d2s_buffered_n(f, result, result_len, NULL);
}

static __ng_inline__ int f2s_buffered_n(float f, char* result, int result_len) 
{
  return __f2s_buffered_n(f, result, result_len, NULL);
}

#if RYU_TEST
void d2s_buffered(double f, char* result);
char* d2s(double f);
void f2s_buffered(float f, char* result);
char* f2s(float f);
void d2fixed_buffered(double d, ng_uint32_t precision, char* result);
char* d2fixed(double d, ng_uint32_t precision);
void d2exp_buffered(double d, ng_uint32_t precision, char* result);
char* d2exp(double d, ng_uint32_t precision);
#endif

#ifdef __cplusplus
}
#endif

#endif // RYU_H
