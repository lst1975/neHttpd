/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_MEMCHR_X86_64_H_
#define _RTE_MEMCHR_X86_64_H_

/**
 * @file
 *
 * Functions for SSE/AVX/AVX2/AVX512 implementation of memcmp().
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

/*
 * GCC older than version 11 doesn't compile AVX properly, so use SSE instead.
 * There are no problems with AVX2.
 */
#if defined __AVX2__
#define RTE_MEMCHR_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MEMCHR_AVX
#endif

/**
 * Copy bytes from one location to another. The locations must not overlap.
 *
 * @note This is implemented as a macro, so it's address should not be taken
 * and care is needed as parameter expressions may be evaluated multiple times.
 *
 * @param dst
 *   Pointer to the destination of the data.
 * @param src
 *   Pointer to the source data.
 * @param n
 *   Number of bytes to copy.
 * @return
 *   Pointer to the destination data.
 */
static __rte_always_inline char *
rte_memchr(const void *dst, const ng_uint8_t c, ng_size_t n);

struct __rte_cache_aligned memchr_const{
  __m128i A128;
#ifdef RTE_MEMCHR_AVX  
  __m256i A256;
#endif
#if defined __AVX512F__ && defined RTE_MEMCHR_AVX512
  __m512i A512;
#endif
};

extern struct memchr_const memchr_const__C;

/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr15_or_less(const ng_uint8_t *dst, const ng_uint8_t *src, 
  ng_size_t n)
{
  ng_uint32_t mask;

  if (n & 8) {
    __m128i xmm0,xmm1,xmm2;
    xmm1 = _mm_set1_epi64(*(__m64 *)dst);
    xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
    xmm0 = _mm_cmpeq_epi8(memchr_const__C.A128,xmm2);
    mask = _mm_movemask_epi8(xmm0);
    if (!mask) return NULL;
    return dst + __builtin_ctz(mask);
  }

  if (n & 4) {
    __m128i xmm0,xmm1,xmm2;
    xmm1 = _mm_set1_epi32(*(int *)dst);
    xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
    xmm0 = _mm_cmpeq_epi8(memchr_const__C.A128,xmm2);
    mask = _mm_movemask_epi8(xmm0);
    if (!mask) return NULL;
    return dst + __builtin_ctz(mask);
  }

  if (n & 2) {
    if (dst[0] == src[0])
      return dst;
    if (dst[1] == src[0])
      return dst+1;
  }

  if (n & 1)
    if (*(const ng_uint8_t *)dst == *(const ng_uint8_t *)src)
      return dst;

  return NULL;
}

/**
 * Copy 16 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr16(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  __m128i xmm0,xmm1,xmm2;
  xmm1 = _mm_loadu_si128((const __m128i *)(const void *)dst);
  xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
  xmm0 = _mm_cmpeq_epi8(memchr_const__C.A128,xmm2);
  ng_uint32_t mask = _mm_movemask_epi8(xmm0);
  if (!mask) return NULL;
  return dst + __builtin_ctz(mask);
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr32(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX2__
  __m256i ymm0,ymm1,ymm2;
  ng_uint32_t mask;
  ymm1 = _mm256_loadu_si256((const __m256i *)(const void *)dst);
  ymm2 = _mm256_cmpeq_epi8(*(const __m256i *)src,ymm1);
  ymm0 = _mm256_cmpeq_epi8(memchr_const__C.A256,ymm2);
  mask = _mm256_movemask_epi8(ymm0);
  if (!mask) return NULL;
  return dst + __builtin_ctz(mask);
#else /* SSE implementation */
  const ng_uint8_t *ret;
  ret = rte_memchr16((const ng_uint8_t *)dst + 0, src);
  if (ret != NULL) return ret;
  return rte_memchr16((const ng_uint8_t *)dst + 16, src);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr64(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMCHR_AVX512
  __m512i zmm0,zmm1,zmm2;
  zmm1 = _mm512_loadu_si512((const void *)dst);
  zmm2 = _mm512_cmpeq_epi8(*((const __m512i *)src), zmm1);
  zmm0 = _mm512_cmpeq_epi8(memchr_const__C.A512,zmm2);
  ng_uint64_t mask = _mm512_movemask_epi8(zmm0);
  if (!mask) return NULL;
  return dst + __builtin_ctzll(mask);
#else /* AVX2, AVX & SSE implementation */
  const ng_uint8_t *ret;
  ret = rte_memchr32((const ng_uint8_t *)dst + 0 * 32, src);
  if (ret != NULL) return ret;
  return rte_memchr32((const ng_uint8_t *)dst + 1 * 32, src);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr128(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const ng_uint8_t *ret;
  ret = rte_memchr64(dst + 0 * 64, src);
  if (ret != NULL) return ret;
  return rte_memchr64(dst + 1 * 64, src);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr256(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const ng_uint8_t *ret;
  ret = rte_memchr128(dst + 0 * 128, src);
  if (ret != NULL) return ret;
  return rte_memchr128(dst + 1 * 128, src);
}

#if defined __AVX512F__ && defined RTE_MEMCHR_AVX512

/**
 * AVX512 implementation below
 */

#define ALIGNMENT_MASK 0x3F

#define __m512_LOAD_MEMCHR(i) do {\
  md = dst + (i << 6); \
  zmm1 = _mm512_loadu_si512((const void *)md); \
  zmm2 = _mm512_cmpeq_epi8(*((const __m512i *)src), zmm1); \
  zmm0 = _mm512_cmpeq_epi8(memchr_const__C.A512,zmm2); \
  uint64_t mask = _mm512_movemask_epi8(zmm0); \
  if (mask) return md + __builtin_ctzll(mask); \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr128blocks(const ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1, zmm2;
  ng_uint64_t mask;
  const ng_uint8_t *md;

  while (n >= 128) 
  {
    n -= 128;
    __m512_LOAD_MEMCHR(0);
    __m512_LOAD_MEMCHR(1);
    src = src + 128;
    dst = dst + 128;
  }

  return NULL;
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr512blocks(const ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1;

  while (n >= 512) 
  {
    n -= 512;
    __m512_LOAD_MEMCHR(0);
    __m512_LOAD_MEMCHR(1);
    __m512_LOAD_MEMCHR(2);
    __m512_LOAD_MEMCHR(3);
    __m512_LOAD_MEMCHR(4);
    __m512_LOAD_MEMCHR(5);
    __m512_LOAD_MEMCHR(6);
    __m512_LOAD_MEMCHR(7);
    src = src + 512;
    dst = dst + 512;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memchr_generic(const void *dst, const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_memchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_memchr32((ng_uint8_t *)dst, src);
  }
  if (n <= 32) 
  {
    ret = rte_memchr16((ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memchr16((ng_uint8_t *)dst - 16 + n, src);
  }
  if (__rte_constant(n) && n == 64) 
  {
    return rte_memchr64((ng_uint8_t *)dst, src);
  }
  if (n <= 64) 
  {
    ret = rte_memchr32((ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    return rte_memchr32((ng_uint8_t *)dst - 32 + n, src);
  }
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n -= 256;
      ret = rte_memchr256((ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (ng_uint8_t *)dst + 256;
    }
    if (n >= 128) 
    {
      n -= 128;
      ret = rte_memchr128((ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (ng_uint8_t *)dst + 128;
    }
COPY_BLOCK_128_BACK63:
    if (n > 64) 
    {
      ret = rte_memchr64((ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      return rte_memchr64((ng_uint8_t *)dst - 64 + n, src);
    }
    if (n > 0)
      return rte_memchr64((ng_uint8_t *)dst - 64 + n, src);
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes
   */
  dstofss = ((ng_uintptr_t)dst & 0x3F);
  if (dstofss > 0) {
    dstofss = 64 - dstofss;
    n -= dstofss;
    ret = rte_memchr64((ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    dst = (ng_uint8_t *)dst + dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  ret = rte_memchr512blocks((ng_uint8_t *)dst, src, n);
  if (ret != NULL) return ret;
  bits = n;
  n = n & 511;
  bits -= n;
  dst = (ng_uint8_t *)dst + bits;

  /**
   * Copy 128-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  if (n >= 128) {
    ret = rte_memchr128blocks((ng_uint8_t *)dst, src, n);
    if (ret != NULL) return ret;
    bits = n;
    n = n & 127;
    bits -= n;
    dst = (ng_uint8_t *)dst + bits;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_128_BACK63;
}

#elif defined RTE_MEMCHR_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

#define __m256_LOAD_MEMCHR(dst) do {\
  ymm1 = _mm256_loadu_si256((const void *)dst); \
  ymm2 = _mm256_cmpeq_epi8(*((const __m256i *)src), ymm1); \
  ymm0 = _mm256_cmpeq_epi8(memchr_const__C.A256, ymm2); \
  mask = _mm256_movemask_epi8(ymm0); \
  if (mask) return dst + __builtin_ctz(mask); \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memchr128blocks(const ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m256i ymm0, ymm1, ymm2;
  ng_uint32_t mask;
  
  while (n >= 128) {
    n -= 128;
    __m256_LOAD_MEMCHR(dst +  0);
    __m256_LOAD_MEMCHR(dst + 32);
    __m256_LOAD_MEMCHR(dst + 64);
    __m256_LOAD_MEMCHR(dst + 96);
    dst += 128;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memchr_generic(const void *dst, const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_memchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) {
    return rte_memchr32((const ng_uint8_t *)dst, src);
  }
  if (n <= 32) {
    ret = rte_memchr16((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
  }
  if (n <= 64) {
    ret = rte_memchr32((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    return rte_memchr32((const ng_uint8_t *)dst - 32 + n, src);
  }
  if (n <= 256) {
    if (n >= 128) {
      n -= 128;
      ret = rte_memchr128((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 128;
    }
COPY_BLOCK_128_BACK31:
    if (n >= 64) {
      n -= 64;
      ret = rte_memchr64((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 64;
    }
    if (n > 32) {
      ret = rte_memchr32((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      return rte_memchr32((const ng_uint8_t *)dst - 32 + n, src);
    }
    if (n > 0) {
      return rte_memchr32((const ng_uint8_t *)dst - 32 + n, src);
    }
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 256 bytes
   */
  dstofss = (ng_uintptr_t)dst & 0x1F;
  if (dstofss > 0) {
    dstofss = 32 - dstofss;
    n -= dstofss;
    ret = rte_memchr32((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    dst = (const ng_uint8_t *)dst + dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  ret = rte_memchr128blocks((const ng_uint8_t *)dst, src, n);
  if (ret != NULL) return ret;
  bits = n;
  n = n & 127;
  bits -= n;
  dst = (const ng_uint8_t *)dst + bits;

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_128_BACK31;
}

#else /* __AVX512F__ */

/**
 * SSE implementation below
 */

#define ALIGNMENT_MASK 0x0F

static __rte_always_inline const ng_uint8_t *
rte_memchr_generic(const void *dst, const ng_uint8_t *src, ng_size_t n)
{
#if !defined __AVX512F__ || !defined RTE_MEMCHR_AVX512
  __m128i mm;
#endif

  const ng_uint8_t *ret;
  ng_size_t dstofss;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_memchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) {
    ret = rte_memchr16((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
  }
  if (n <= 64) {
    ret = rte_memchr32((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    if (n > 48)
    {  
      ret = rte_memchr16((const ng_uint8_t *)dst + 32, src);
      if (ret != NULL) return ret;
    }
    return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
  }
  if (n <= 128) {
    goto COPY_BLOCK_128_BACK15;
  }
  if (n <= 512) {
    if (n >= 256) {
      n -= 256;
      ret = rte_memchr128((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      ret = rte_memchr128((const ng_uint8_t *)dst + 128, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 256;
    }
COPY_BLOCK_255_BACK15:
    if (n >= 128) {
      n -= 128;
      ret = rte_memchr128((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 128;
    }
COPY_BLOCK_128_BACK15:
    if (n >= 64) {
      n -= 64;
      ret = rte_memchr64((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 64;
    }
COPY_BLOCK_64_BACK15:
    if (n >= 32) {
      n -= 32;
      ret = rte_memchr32((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      dst = (const ng_uint8_t *)dst + 32;
    }
    if (n > 16) {
      ret = rte_memchr16((const ng_uint8_t *)dst, src);
      if (ret != NULL) return ret;
      return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
    }
    if (n > 0) {
      return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
    }
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes,
   * and make sure the first 15 bytes are copied, because
   * unaligned copy functions require up to 15 bytes
   * backwards access.
   */
  dstofss = (ng_uintptr_t)dst & 0x0F;
  if (dstofss > 0) {
    dstofss = 16 - dstofss + 16;
    n -= dstofss;
    ret = rte_memchr32((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    dst = (const ng_uint8_t *)dst + dstofss;
  }

  /**
   * For aligned copy
   */
  /**
   * Copy 256-byte blocks
   */
  for (; n >= 256; n -= 256) {
    ret = rte_memchr256((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    dst = (const ng_uint8_t *)dst + 256;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_255_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline const ng_uint8_t *
rte_memchr_aligned(const void *dst, const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  
  /* Copy size < 16 bytes */
  if (n < 16) {
    return rte_memchr15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) {
    return rte_memchr32((const ng_uint8_t *)dst, src);
  }
  
  if (n <= 32) {
    ret = rte_memchr16((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memchr16((const ng_uint8_t *)dst - 16 + n, src);
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) {
    return rte_memchr64((const ng_uint8_t *)dst, src);
  }
  
  if (n <= 64) {
    ret = rte_memchr32((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    return rte_memchr32((const ng_uint8_t *)dst - 32 + n, src);
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) {
    ret = rte_memchr64((const ng_uint8_t *)dst, src);
    if (ret != NULL) return ret;
    dst = (const ng_uint8_t *)dst + 64;
  }

  /* Copy whatever left */
  return rte_memchr64((const ng_uint8_t *)dst - 64 + n, src);
}

static __rte_always_inline char *
rte_memchr(const void *dst, const ng_uint8_t c, ng_size_t n)
{
#define _(x) x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x
#if !defined __AVX512F__ || !defined RTE_MEMCHR_AVX512
const ng_uint8_t _src[64+ALIGNMENT_MASK] = {
  _(c),_(c),_(c),_(c),_(c)
};
#elif defined __AVX2__
const ng_uint8_t _src[32+ALIGNMENT_MASK] = {
  _(c),_(c),_(c)
};
#else
  const ng_uint8_t _src[16+ALIGNMENT_MASK] = {
    _(c),_(c)
  };
#endif
#undef _
  const ng_uint8_t *src;

  src = (const ng_uint8_t *)RTE_ALIGN((const ng_uintptr_t)_src, ALIGNMENT_MASK+1);
  if (!((ng_uintptr_t)dst & ALIGNMENT_MASK))
    return (char *)rte_memchr_aligned(dst, src, n);
  else
    return (char *)rte_memchr_generic(dst, src, n);
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_memchr(dst, c, n) rte_memchr(dst, c, n)

#else

#define ng_memchr(dst, c, n) memchr(dst, c, n)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_X86_64_H_ */

