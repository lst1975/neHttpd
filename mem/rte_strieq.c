/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Intel Corporation
 */

#include <nanohttp-defs.h>

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)

#include "rte_strieq.h"

struct strieq_const strieq_const__C;
struct memchr_const memchr_const__C;

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
}

#include <nanohttp-logging.h>
int memchr_test(void)
{
  const char *t="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$%^&*()_+{}[]|\\\":;',.?/\r\n";
  int len = strlen(t);

  for (int i=0;i<len;i++)
  {
    const char *p = ng_memchr(t,t[i],len);
    if (p-t!=i)
    {
      log_error("memchr_test failed");
      return -i;
    }
  }

  return 0;
}

#endif
