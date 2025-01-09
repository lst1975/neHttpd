/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
#ifndef __nanohttp_buffer_h
#define __nanohttp_buffer_h

#include <stdint.h>
#include <string.h>
#include "nanohttp-defs.h"

#define __STRUCT_BLOCK_DEF \
  union{ \
    void *data; \
    const char *cptr; \
    unsigned char *ptr;\
    char *buf; \
  }; \
  uint64_t len:54; \
  uint64_t n:10

typedef struct {
  __STRUCT_BLOCK_DEF;
} ng_block_s;

typedef struct {
  __STRUCT_BLOCK_DEF; // MUST start from the first byte
  char *p;
  size_t size;
  size_t nbytes;
} httpd_buf_t;

typedef httpd_buf_t ng_str_s;

#define DECL_CONST_STR_N(a,v) { .n=a, .cptr=v, .len=sizeof(v)-1 }
#define DECL_CONST_STR_NULL() { .cptr=NULL, .len=0 }
#define DECL_CONST_STR(v) { .cptr=v, .len=sizeof(v)-1 }

#define __HTTPD_BUF_INIT_DECL() \
  { .data = NULL, .p = NULL, .len = 0, .size = 0, .nbytes= 0}

static inline void BUF_GO(httpd_buf_t *b, size_t len)
{
  b->p   += len;
  b->len += len;
}

static inline void BUF_SET_CHR(httpd_buf_t *b, char c)
{
  b->p[0] = (char)c;
}

static inline size_t BUF_LEN(httpd_buf_t *b)
{
  return b->len;
}

static inline char *BUF_CUR_PTR(httpd_buf_t *b)
{
  return b->p;
}

static inline size_t BUF_REMAIN(httpd_buf_t *b)
{
  return b->size - b->len;
}

static inline void BUF_SET(httpd_buf_t *b, char *ptr, size_t len)
{
  b->buf = ptr;
  b->len = len;
}

static inline void BUF_SIZE_INIT(httpd_buf_t *b, char *buf, size_t size)
{
  b->p      = buf;
  b->data   = buf;
  b->len    = 0;
  b->size   = size;
  b->nbytes = 0;
}

static inline void ng_block_init(ng_block_s *blk)
{ 
  blk->data = NULL;
  blk->len  = 0;
}

static inline int ng_block_isequal(const void *blk, const void *buf, size_t size)
{
  const ng_block_s *b = (const ng_block_s *)blk;
  return b->len == size && !ng_memcmp(b->data, buf, size);
}

static inline int ng_block_isequal__(const void *blka, const void *blkb)
{
  const ng_block_s *a = (const ng_block_s *)blka;
  const ng_block_s *b = (const ng_block_s *)blkb;
  return ng_block_isequal(a, b->data, b->len);
}

static inline int ng_block_isequal_case(const void *blka, const void *blkb)
{
  const ng_block_s *a = (const ng_block_s *)blka;
  const ng_block_s *b = (const ng_block_s *)blkb;
  return ng_block_isequal(a, b->buf, b->len);
}

static inline int ng_block_isequal_nocase(const void *blka, const void *blkb)
{
  const ng_block_s *a = (const ng_block_s *)blka;
  const ng_block_s *b = (const ng_block_s *)blkb;
  if (a->len != b->len) return 0;
  return !strncasecmp(a->cptr, b->cptr, b->len);
}

extern void ng_free_data_buffer(httpd_buf_t *data);
extern void ng_free_data_block(ng_block_s *block);
extern int ng_dup_data_block(ng_block_s *block, const ng_block_s *n);
extern int ng_dup_data_block_str(ng_block_s *block, const ng_block_s *n);

#endif
