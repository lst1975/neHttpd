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
#ifndef __nanohttp_types_h
#define __nanohttp_types_h

#define NG_INT16_STR_LEN_MAX   (sizeof("-32767") - 1)
#define NG_INT32_STR_LEN_MAX   (sizeof("-2147483648") - 1)
#define NG_INT64_STR_LEN_MAX   (sizeof("-9223372036854775808") - 1)
#define NG_UINT16_STR_LEN_MAX  (sizeof("65535") - 1)
#define NG_UINT32_STR_LEN_MAX  (sizeof("4294967295") - 1)
#define NG_UINT64_STR_LEN_MAX  (sizeof("18446744073709551615") - 1)
#define NG_INET_ADDRSTRLEN     (sizeof("111.112.113.114") - 1)
#define NG_INET6_ADDRSTRLEN    (sizeof("0001:0002:0003:0004:0005:ffff:111.112.113.114") - 1)

typedef unsigned int       ng_uint32_t;
typedef unsigned short     ng_uint16_t;
typedef unsigned char      ng_uint8_t;
typedef signed int         ng_int32_t;
typedef signed short       ng_int16_t;
typedef signed char        ng_int8_t;

#ifndef UINT64_C
#define UINT64_C(c) c ## ULL
#endif
#ifndef UINT32_C
#define UINT32_C(c) c ## U
#endif
#ifndef UINT16_C
#define UINT16_C(c) c ## U
#endif
#ifndef UINT8_C
#define UINT8_C(c) c
#endif
#ifndef INT64_C
#define INT64_C(c) c ## LL
#endif
#ifndef INT32_C
#define INT32_C(c) c ## L
#endif
#ifndef INT16_C
#define INT16_C(c) c
#endif
#ifndef INT8_C
#define INT8_C(c) c
#endif

#define NG_UINT_MAX    UINT32_C(0xFFFFFFFF)
#define NG_UINT16_MAX  UINT16_C(0xFFFF)
#define NG_UINT32_MAX  NG_UINT_MAX
#define NG_UINT64_MAX  UINT64_C(0xFFFFFFFFFFFFFFFF)

#define NG_INT_MAX     UINT32_C(0x7FFFFFFF)
#define NG_INT16_MAX   UINT16_C(0x7FFF)
#define NG_INT32_MAX   NG_INT_MAX
#define NG_INT64_MAX   UINT64_C(0x7FFFFFFFFFFFFFFF)

#define NG_UINT8_MIN     UINT8_C(0)
#define NG_UINT16_MIN   UINT16_C(0)
#define NG_UINT32_MIN   UINT32_C(0)
#define NG_UINT64_MIN   UINT64_C(0)

#define NG_INT8_MIN     INT8_C(0x80)
#define NG_INT16_MIN   INT16_C(0x8000)
#define NG_INT32_MIN   INT32_C(0x80000000)
#define NG_INT64_MIN   INT64_C(0x8000000000000000)

typedef unsigned long long ng_uint64_t;
typedef signed long long   ng_int64_t;

typedef ng_uint32_t ng_uintmax_t;
typedef ng_int64_t ng_size_t;
typedef ng_uint64_t ng_time_t;

#if NG_IS_64BIT_SYS
#define NG_SIZET_STR_LEN_MAX NG_UINT64_STR_LEN_MAX
#define __NG_BITS_PER_LONG 64
typedef ng_uint64_t        ng_uintptr_t;
#define NG_SIZE_MAX NG_INT64_MAX
#else
#define NG_SIZET_STR_LEN_MAX NG_UINT32_STR_LEN_MAX
#define __NG_BITS_PER_LONG 32
#define NG_SIZE_MAX NG_INT32_MAX
typedef ng_int32_t         ng_uintptr_t;
#endif

//--------------------- wordsize ----------------------------------------------
#ifndef __WORDSIZE
#define __WORDSIZE __NG_BITS_PER_LONG
#endif

#ifndef PRIu32
  #define PRIu32 "u"
#endif /* PRIu32 */

#ifndef PRIu64
#if __WORDSIZE == 64
  #define PRIu64 "lu"
#else
  #define PRIu64 "llu"
#endif /* __WORDSIZE */
#endif

/* -1.79769313486231570E+308 -4.94065645841246544E-324 1.79769313486231570E+308 */
#define NG_DOUBLE_STR_LEN_MAX     (sizeof("-1.79769313486231570E+308") - 1)
/* 1.175494351e-38F 3.402823466e+038F */
#define NG_FLOAT_STR_LEN_MAX      (sizeof("3.402823466e+038") - 1)

#endif
