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
#ifndef __ngRTOS_ITOA_H__
#define __ngRTOS_ITOA_H__

#ifdef __cplusplus
    extern "C" {
#endif

ng_size_t u32toa_jeaiii(ng_uint32_t u, char* b, ng_size_t len);
ng_size_t i32toa_jeaiii(ng_int32_t i, char* b, ng_size_t len);
ng_size_t u64toa_jeaiii(ng_uint64_t n, char* b, ng_size_t len);
ng_size_t i64toa_jeaiii(ng_int64_t i, char* b, ng_size_t len);

static __ng_inline__ ng_size_t __u32toa_jeaiii(ng_uint32_t u, char* b)
{
  return u32toa_jeaiii(u, b, NG_UINT_MAX);
}

static __ng_inline__ ng_size_t __i32toa_jeaiii(ng_int32_t i, char* b)
{
  return i32toa_jeaiii(i, b, NG_UINT_MAX);
}

static __ng_inline__ ng_size_t __u64toa_jeaiii(ng_uint64_t n, char* b)
{
  return u64toa_jeaiii(n, b, NG_UINT_MAX);
}

static __ng_inline__ ng_size_t __i64toa_jeaiii(ng_int64_t i, char* b)
{
  return i64toa_jeaiii(i, b, NG_UINT_MAX);
}

int u32toh_jeaiii(ng_uint32_t num, char *s, int len, int lowerAlpha);
int u64toh_jeaiii(ng_uint64_t num, char *s, int len, int lowerAlpha);

#define ng_u32toa(u,b,l) u32toa_jeaiii(u,b,l)
#define ng_i32toa(i,b,l) i32toa_jeaiii(i,b,l)
#define ng_u64toa(u,b,l) u32toa_jeaiii(u,b,l)
#define ng_i64toa(i,b,l) u32toa_jeaiii(i,b,l)
#define ng_u32toh_generic(num,s,len) u32toa_jeaiii(num,s,len)
#define ng_u64toh_generic(num,s,len) u64toa_jeaiii(num,s,len)

#ifdef __cplusplus
    }
#endif

#endif /* __ngRTOS_ITOA_H__ */

