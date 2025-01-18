/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Intel Corporation
 */

#include <nanohttp-defs.h>

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)

#include "rte_strieq.h"

struct strieq_const strieq_const__C;
struct memchr_const memchr_const__C;
struct memrchr_const memrchr_const__C;

/*
   A        Z
01000001 01011010
01100001 01111010
   a        z
00100000 00100000 0x20
  A^a      Z^z
*/
RTE_INIT(strcasecmp_init_const)
{
	strieq_const__C.A128 = _mm_set1_epi8(0b00100000);
#ifdef RTE_MESTRIEQ_AVX  
	strieq_const__C.A256 = _mm256_set1_epi8(0b00100000);
#endif
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
	strieq_const__C.A512 = _mm512_set1_epi8(0b00100000);
#endif

	memchr_const__C.A128 = _mm_set1_epi8(0xff);
#ifdef RTE_MEMCHR_AVX  
	memchr_const__C.A256 = _mm256_set1_epi8(0xff);
#endif
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
	memchr_const__C.A512 = _mm512_set1_epi8(0xff);
#endif

	memrchr_const__C.A128 = _mm_set1_epi8(0xff);
#ifdef RTE_MEMCHR_AVX  
	memrchr_const__C.A256 = _mm256_set1_epi8(0xff);
#endif
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
	memrchr_const__C.A512 = _mm512_set1_epi8(0xff);
#endif
}

#include <nanohttp-logging.h>
#include <nanohttp-buffer.h>
#define M_TEST "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$%^&*()_+{}[]|\\\":;',.?/\r\n"
int memchr_test(void)
{
  ng_block_s t = DECL_CONST_STR(M_TEST);
    
  for (int i=0;i<t.len;i++)
  {
    const char *p = ng_memchr(t.cptr,t.cptr[i],t.len);
    if (p-t.cptr!=i)
    {
      log_error("memchr_test failed");
      return -i;
    }
  }

  return 0;
}

static const char *memrchr_wrap(const char *dst, char c, int len)
{
  return ng_memrchr(dst,c,len);
}

int memrchr_test(void)
{
  const char *p;
  ng_block_s t = DECL_CONST_STR(M_TEST);
    
  for (int i=0;i<t.len;i++)
  {
    p = ng_memrchr(t.cptr,t.cptr[i],t.len);
    if (p-t.cptr!=i)
    {
      log_error("memrchr_test failed");
      return -1;
    }
  }

  {
    t.cptr = "./httpd";
    t.len = 7;

    p = memrchr_wrap(t.cptr, '/', t.len);
    if (p - t.cptr != 1)
    {
      log_error("memrchr_test failed");
      return -1;
    }
  }
  
  return 0;
}

#endif
