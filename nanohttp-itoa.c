/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 **************************************************************************************
 *                              
 *                    https://github.com/lst1975/TDMA-ng-Wifi
 *                              
 **************************************************************************************
 */
/*************************************************************************************
 *                               ngRTOS Kernel V2.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 *************************************************************************************
 *                              https://github.com/lst1975/ngRTOS
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
 */
/*
MIT License

Copyright (c) 2017 James Edward Anhalt III - https://github.com/jeaiii/itoa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "nanohttp-defs.h"
#include "nanohttp-itoa.h"

// form a 4.32 fixed point number: t = u * 2^32 / 10^log10(u)
// use as much precision as possible when needed (log10(u) >= 5) 
// so shift up then down afterwards by log10(u) * log2(10) ~= 53/16
// need to round up before and or after in some cases
// once we have the fixed point number we can read off the digit in the upper 32 bits
// and multiply the lower 32 bits by 10 to get the next digit and so on
// we can do 2 digits at a time by multiplying by 100 each time

// TODO:
// x64 optimized verison (no need to line up on 32bit boundary, so can multiply by 5 instead of 10 using lea instruction)
// full 64 bit LG()
// try splitting the number into chucks that can be processed independently
// try odd digit first
// try writing 4 chars at a time

struct _pair { char t, o; };
#define P(T) {T, '0'},  {T, '1'}, {T, '2'}, {T, '3'}, {T, '4'}, {T, '5'}, {T, '6'}, {T, '7'}, {T, '8'}, {T, '9'}
static const struct _pair s_pairs[] = { P('0'), P('1'), P('2'), P('3'), P('4'), P('5'), P('6'), P('7'), P('8'), P('9') };

#define W(N, I) *(struct _pair*)&b[N] = s_pairs[I]
#define A(N) t = ((ng_uint64_t)(1) << (32 + ((N / 5 * N * 53) >> 4))) / (ng_uint32_t)(1e##N) + 1 + N/6 - (N>>3), \
                    t *= u, t >>= ((N / 5 * N * 53) >> 4), t += (N / 6) << 2, W(0, t >> 32)
#define S(N) b[N] = (char)((ng_uint64_t)(10) * (ng_uint32_t)(t) >> 32) + '0'
#define D(N) t = (ng_uint64_t)(100) * (ng_uint32_t)(t), W(N, t >> 32)

#define C0 b[0] = (char)(u) + '0'
#define C1 W(0, u)
#define C2 A(1), S(2)
#define C3 A(2), D(2)
#define C4 A(3), D(2), S(4)
#define C5 A(4), D(2), D(4)
#define C6 A(5), D(2), D(4), S(6)
#define C7 A(6), D(2), D(4), D(6)
#define C8 A(7), D(2), D(4), D(6), S(8)
#define C9 A(8), D(2), D(4), D(6), D(8)

#define L09(F) u < 100        ? L01(F) : L29(F)
#define L29(F) u < 1000000    ? L25(F) : L69(F)
#define L25(F) u < 10000      ? L23(F) : L45(F)
#define L69(F) u < 100000000  ? L67(F) : L89(F)
#define L03(F) u < 100        ? L01(F) : L23(F)

#define L01(F) u < 10         ? F(0) : F(1)
#define L23(F) u < 1000       ? F(2) : F(3)
#define L45(F) u < 100000     ? F(4) : F(5)
#define L67(F) u < 10000000   ? F(6) : F(7)
#define L89(F) u < 1000000000 ? F(8) : F(9)

#define POS(N) (N < length ? C##N, N + 1 : N + 1)
#define NEG(N) (N + 1 < length ? *b++ = '-', C##N, N + 2 : N + 2)

static __ng_inline__ ng_size_t u32to_chars_jeaiii(char* b, ng_size_t length, ng_uint32_t u)
{
  ng_uint64_t t;
  return L09(POS);
}

static __ng_inline__ ng_size_t i32to_chars_jeaiii(char* b, ng_size_t length, ng_int32_t i)
{
  ng_uint64_t t;
  ng_uint32_t u = i;
  return i < 0 ? u = 0 - u, L09(NEG) : L09(POS);
}

static __ng_inline__ ng_size_t u64to_chars_jeaiii(char* b, ng_size_t length, ng_uint64_t n)
{
  rte_prefetch0(s_pairs);
  
  ng_size_t count;
  ng_uint32_t u = (ng_uint32_t)(n);
  ng_uint64_t t;

  if (u == n)
    return L09(POS);

  ng_uint64_t a = n / 100000000;

  if ((ng_uint32_t)(a) == a)
  {
    u = (ng_uint32_t)(a);
    b += count = L09(POS);
    count += 8;
    if (count > length)
      return count;
  }
  else
  {
    u = (ng_uint32_t)(a / 100000000);
    b += count = L03(POS);
    count += 16;
    if (count > length)
      return count;

    u = a % 100000000;
    C7;
    b += 8;
  }

  u = n % 100000000;
  C7;
  return count;
}

static __ng_inline__ ng_size_t i64to_chars_jeaiii(char* b, ng_size_t length, ng_int64_t i)
{
  return i < 0
    ? u64to_chars_jeaiii(b + 1, length > 0 ? b[0] = '-', length - 1 : 0, (ng_uint64_t)(0) - (ng_uint64_t)(i)) + 1
    : u64to_chars_jeaiii(b, length, (ng_uint64_t)(i));
}

ng_size_t u32toa_jeaiii(ng_uint32_t u, char* b, ng_size_t length)
{
  return u32to_chars_jeaiii(b, length, u);
}

ng_size_t i32toa_jeaiii(ng_int32_t i, char* b, ng_size_t length)
{
  return i32to_chars_jeaiii(b, length, i);
}

ng_size_t u64toa_jeaiii(ng_uint64_t n, char* b, ng_size_t length)
{
  return u64to_chars_jeaiii(b, length, n);
}

ng_size_t i64toa_jeaiii(ng_int64_t i, char* b, ng_size_t length)
{
  return i64to_chars_jeaiii(b, length, i);
}

static const char digitsUpperAlpha[513] =
  "000102030405060708090A0B0C0D0E0F"
  "101112131415161718191A1B1C1D1E1F"
  "202122232425262728292A2B2C2D2E2F"
  "303132333435363738393A3B3C3D3E3F"
  "404142434445464748494A4B4C4D4E4F"
  "505152535455565758595A5B5C5D5E5F"
  "606162636465666768696A6B6C6D6E6F"
  "707172737475767778797A7B7C7D7E7F"
  "808182838485868788898A8B8C8D8E8F"
  "909192939495969798999A9B9C9D9E9F"
  "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
  "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
  "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
  "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
  "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
  "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

static const char digitsLowerAlpha[513] =
  "000102030405060708090a0b0c0d0e0f"
  "101112131415161718191a1b1c1d1e1f"
  "202122232425262728292a2b2c2d2e2f"
  "303132333435363738393a3b3c3d3e3f"
  "404142434445464748494a4b4c4d4e4f"
  "505152535455565758595a5b5c5d5e5f"
  "606162636465666768696a6b6c6d6e6f"
  "707172737475767778797a7b7c7d7e7f"
  "808182838485868788898a8b8c8d8e8f"
  "909192939495969798999a9b9c9d9e9f"
  "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
  "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
  "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
  "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
  "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
  "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

int 
u32toh_jeaiii(ng_uint32_t num, char *s, int len, int lowerAlpha)
{
  ng_uint32_t x = (ng_uint32_t)num;
	int i = 3;
	const char *lut = ((lowerAlpha) ? digitsLowerAlpha : digitsUpperAlpha);
	while (i >= 0 && i < len)
	{
		int pos = (x & 0xFF) << 1;
		char ch = lut[pos];
		s[i << 1] = ch;

		ch = lut[pos + 1];
		s[(i << 1) + 1] = ch;

		x >>= 8;
		i -= 1;
	}

  return 8;
}

int 
u64toh_jeaiii(ng_uint64_t num, char *s, int len, int lowerAlpha)
{
  int l = u32toh_jeaiii(num>>32, s, len, lowerAlpha);
  if (l < len)
    return l+u32toh_jeaiii(num, s+l, len-l, lowerAlpha);
  return l;
}

