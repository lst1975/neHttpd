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
#ifndef __nanohttp_atoi_h
#define __nanohttp_atoi_h

#ifdef __cplusplus
extern "C" {
#endif

uint64_t ng_atou64_s(const char *s, const char **end);
uint32_t ng_atou32_s(const char *s, const char **end);
uint64_t ng_atou64(const char *str, uint8_t len, const char **end);
uint32_t ng_atou32(const char *str, uint8_t len, const char **end);

uint32_t ng_htou32(const char *str, uint8_t len, const char **end);
uint64_t ng_htou64(const char *str, uint8_t len, const char **end);
int ng_u64toh(uint64_t v, char *str, uint8_t len, int is_upper, char seporator);
int ng_u32toh(uint32_t v, char *str, uint8_t len, int is_upper, char seporator);

void test_perf_atoi(void);

static __ng_inline__ int64_t 
ng_atoi64(const char *str, uint8_t len, const char **end)
{
  if (len > NG_INT64_STR_LEN_MAX)
  {
    *end = str;
    return 0;
  } 
  
  if (*str == '-')
  {
    return -ng_atou64(str+1, len-1, end);
  }
  else if (*str == '+')
  {
    return ng_atou64(str+1, len-1, end);
  }
  else
  {
    return ng_atou64(str, len, end);
  }
}

static __ng_inline__ int32_t 
ng_atoi32(const char *str, uint8_t len, const char **end)
{
  if (len > NG_INT32_STR_LEN_MAX)
  {
    *end = str;
    return 0;
  } 
  
  if (*str == '-')
  {
    return -ng_atou32(str+1, len-1, end);
  }
  else if (*str == '+')
  {
    return ng_atou32(str+1, len-1, end);
  }
  else
  {
    return ng_atou32(str, len, end);
  }
}

static __ng_inline__ int64_t 
ng_atoi64_s(const char *s, const char **end)
{
  if (*s == '-')
  {
    return -ng_atou64_s(s+1, end);
  }
  else if (*s == '+')
  {
    return ng_atou64_s(s+1, end);
  }
  else
  {
    return ng_atou64_s(s, end);
  }
}

static __ng_inline__ int32_t 
ng_atoi32_s(const char *s, const char **end)
{
  if (*s == '-')
  {
    return -ng_atou32_s(s+1, end);
  }
  else if (*s == '+')
  {
    return ng_atou32_s(s+1, end);
  }
  else
  {
    return ng_atou32_s(s, end);
  }
}

static __ng_inline__ int64_t 
ng_atoi(const char *str, uint8_t len)
{
  const char *end;
  if (len == 0)
    len = strlen(str);
  return ng_atoi64(str, len, &end);
}

static __ng_inline__ uint64_t 
ng_atoui(const char *str, uint8_t len)
{
  const char *end;
  return ng_atou64(str, len, &end);
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif

