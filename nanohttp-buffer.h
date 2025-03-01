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
#ifndef __nanohttp_buffer_h
#define __nanohttp_buffer_h

#include <string.h>
#include "nanohttp-defs.h"

#define __STRUCT_BLOCK_DEF \
  union{ \
    void *data; \
    const char *cptr; \
    unsigned char *ptr;\
    char *buf; \
  }; \
  ng_uint64_t len:53; \
  ng_uint64_t n:10; \
  ng_uint64_t x:1

typedef struct {
  __STRUCT_BLOCK_DEF;
} ng_block_s;

struct _httpd_buf {
  union{
    struct{
      __STRUCT_BLOCK_DEF; // MUST start from the first byte
    };
    ng_block_s b;
  };
  char *p;
  ng_size_t size;
  ng_size_t nbytes;
};

typedef struct _httpd_buf ng_buffer_s;

#define DECL_CONST_STR_N(a,v) { .n=a, .cptr=v, .len=sizeof(v)-1 }
#define DECL_CONST_STR_NULL() { .cptr=NULL, .len=0 }
#define DECL_CONST_STR(v) { .cptr=v, .len=sizeof(v)-1 }

static inline void BUF_GO(ng_buffer_s *b, ng_size_t len)
{
  b->p   += len;
  b->len += len;
}

static inline void BUF_SET_CHR(ng_buffer_s *b, char c)
{
  b->p[0] = (char)c;
}

static inline ng_size_t BUF_LEN(ng_buffer_s *b)
{
  return b->len;
}

static inline char *BUF_CUR_PTR(ng_buffer_s *b)
{
  return b->p;
}

static inline ng_size_t BUF_REMAIN(ng_buffer_s *b)
{
  return b->size - b->len;
}

static inline void BUF_SET(ng_buffer_s *b, char *ptr, ng_size_t len)
{
  b->buf = ptr;
  b->len = len;
}

static inline void BUF_SIZE_INIT(ng_buffer_s *b, char *buf, ng_size_t size)
{
  b->p      = buf;
  b->data   = buf;
  b->len    = 0;
  b->size   = size;
  b->nbytes = 0;
}

static inline void BUF_CLEAR(ng_buffer_s *b)
{
  b->p      = b->buf;
  b->len    = 0;
  b->nbytes = 0;
}

static inline void ng_block_init(ng_block_s *blk)
{ 
  blk->data = NULL;
  blk->len  = 0;
}

static inline void ng_block_set(ng_block_s *blk, const char *ptr, ng_size_t len)
{ 
  blk->cptr = ptr;
  blk->len  = len;
}

static inline int ng_block_isequal(const void *blk, const void *buf, ng_size_t size)
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
  return !ng_strnocasecmp(a->cptr, b->cptr, b->len);
}

static inline const char *ng_block_end(const void *blka)
{
  const ng_block_s *a = (const ng_block_s *)blka;
  return a->cptr + a->len;
}

extern void ng_free_data_buffer(ng_buffer_s *data);
extern void ng_free_data_block(ng_block_s *block);
extern int ng_dup_data_block(ng_block_s *block, const ng_block_s *n, int free_old);
extern int ng_dup_data_block_str(ng_block_s *block, const ng_block_s *n, int free_old);

extern const ng_block_s __ng_uint8_string[];

#endif
