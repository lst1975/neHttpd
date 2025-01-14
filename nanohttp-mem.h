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
#ifndef __nanohttp_mem_h
#define __nanohttp_mem_h

#include "nanohttp-defs.h"

void *__http_malloc(ng_size_t size
#if __NHTTP_MEM_DEBUG  
  , const char *file, int line
#endif
);
void __http_free(void *ptr);

void http_memcache_free(void);
int http_memcache_init(void);

#if __NHTTP_MEM_DEBUG  
#define http_malloc(size) __http_malloc(size, __FILE__, __LINE__)
#define http_strdup(ptr) __http_strdup(ptr, __FILE__, __LINE__)
#define http_strdup_len(ptr,len) __http_strdup_len(ptr, len, __FILE__, __LINE__)
#define http_strdup_size(ptr,len) __http_strdup_size(ptr, len, __FILE__, __LINE__)
#define http_calloc(n,size) __http_calloc(n,size, __FILE__, __LINE__)
#else
#define http_malloc(size) __http_malloc(size)
#define http_strdup(ptr) __http_strdup(ptr)
#define http_strdup_len(ptr,len) __http_strdup_len(ptr, len)
#define http_strdup_size(ptr,len) __http_strdup_size(ptr, len)
#define http_calloc(n,size) __http_calloc(n,size)
#endif

#define http_free(ptr) __http_free(ptr)
#define os_malloc(size) malloc(size)
#define os_free(ptr) free(ptr)

static inline char *__http_strdup(const char *p
#if __NHTTP_MEM_DEBUG  
    , const char *file, int line
#endif
)
{
  if (p == NULL)
    return NULL;

  ng_size_t len = strlen(p)+1;
  void *np = __http_malloc(len
#if __NHTTP_MEM_DEBUG  
      , file, line
#endif
    );
  if (np == NULL)
    return NULL;
  memcpy(np, p, len);
  return np;
}

static inline char *__http_strdup_size(const char *p, int len
#if __NHTTP_MEM_DEBUG  
    , const char *file, int line
#endif
)
{
  if (p == NULL)
    return NULL;

  char *np = (char *)__http_malloc(len+1
#if __NHTTP_MEM_DEBUG  
      , file, line
#endif
    );
  if (np == NULL)
    return NULL;
  memcpy(np, p, len);
  np[len] = '\0';
  return np;
}

static inline char *__http_strdup_len(const char *p, int *_len
#if __NHTTP_MEM_DEBUG  
    , const char *file, int line
#endif
)
{
  if (p == NULL)
    return NULL;

  ng_size_t len = strlen(p)+1;
  void *np = __http_malloc(len
#if __NHTTP_MEM_DEBUG  
      , file, line
#endif
    );
  if (np == NULL)
    return NULL;
  memcpy(np, p, len);
  *_len = len - 1;
  return np;
}

static inline char *__http_calloc(ng_size_t n, ng_size_t size
#if __NHTTP_MEM_DEBUG  
    , const char *file, int line
#endif
)
{
  return __http_malloc(n*size
#if __NHTTP_MEM_DEBUG  
      , file, line
#endif
    );
}

#endif

