/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_MEMRCHR_X86_64_H_
#define _RTE_MEMRCHR_X86_64_H_

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
#define RTE_MEMRCHR_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MEMRCHR_AVX
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
rte_memrchr(const void *dst, const ng_uint8_t c, ng_size_t n);

struct __rte_cache_aligned memrchr_const{
  __m128i A128;
#ifdef RTE_MEMRCHR_AVX  
  __m256i A256;
#endif
#if defined __AVX512F__ && defined RTE_MEMRCHR_AVX512
  __m512i A512;
#endif
};

extern struct memrchr_const memrchr_const__C;

/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr15_or_less(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  ng_uint32_t mask;

  if (n & 8) 
  {
    __m128i xmm0,xmm1,xmm2;
    xmm1 = _mm_set1_epi64(*(__m64 *)(dst - 8));
    xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
    xmm0 = _mm_cmpeq_epi8(memrchr_const__C.A128,xmm2);
    mask = _mm_movemask_epi8(xmm0);
    if (mask)
    {
      int clz = __builtin_clz(mask) - 24;
      return dst - clz - 1;
    }
    dst -= 8;
  }

  if (n & 4) 
  {
    __m128i xmm0,xmm1,xmm2;
    xmm1 = _mm_set1_epi32(*(int *)(dst - 4));
    xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
    xmm0 = _mm_cmpeq_epi8(memrchr_const__C.A128,xmm2);
    mask = _mm_movemask_epi8(xmm0);
    if (mask)
    {
      int clz = __builtin_clz(mask) - 28;
      return dst - clz - 1;
    }
    dst -= 4;
  }

  if (n & 2) 
  {
    if (dst[-1] == src[0])
      return dst - 1;
    if (dst[-2] == src[0])
      return dst - 2;
    dst -= 2;
  }

  if (n & 1)
    if (dst[-1] == *src)
      return dst - 1;

  return NULL;
}

/**
 * Copy 16 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr16(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  __m128i xmm0,xmm1,xmm2;
  xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(dst - 16));
  xmm2 = _mm_cmpeq_epi8(*((const __m128i *)src), xmm1);
  xmm0 = _mm_cmpeq_epi8(memrchr_const__C.A128,xmm2);
  ng_uint32_t mask = _mm_movemask_epi8(xmm0);
  if (!mask) return NULL;
  int clz = __builtin_clz(mask) - 16;
  return dst - clz - 1;
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr32(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX2__
  __m256i ymm0,ymm1,ymm2;
  ng_uint32_t mask;
  ymm1 = _mm256_loadu_si256((const __m256i *)(const void *)(dst - 32));
  ymm2 = _mm256_cmpeq_epi8(*(const __m256i *)src,ymm1);
  ymm0 = _mm256_cmpeq_epi8(memrchr_const__C.A256,ymm2);
  mask = _mm256_movemask_epi8(ymm0);
  if (!mask) return NULL;
  int clz = __builtin_clz(mask);
  return dst - clz - 1;
#else /* SSE implementation */
  const ng_uint8_t *ret;
  ret = rte_memrchr16(dst - 0 * 16, src);
  if (ret != NULL) return ret;
  return rte_memrchr16(dst - 1 * 16, src);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr64(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMRCHR_AVX512
  __m512i zmm0,zmm1,zmm2;
  zmm1 = _mm512_loadu_si512((const void *)dst - 64);
  zmm2 = _mm512_cmpeq_epi8(*((const __m512i *)src), zmm1);
  zmm0 = _mm512_cmpeq_epi8(memrchr_const__C.A512,zmm2);
  ng_uint64_t mask = _mm512_movemask_epi8(zmm0);
  if (!mask) return NULL;
  int clz = __builtin_clzll(mask);
  return dst - clz - 1;
#else /* AVX2, AVX & SSE implementation */
  const ng_uint8_t *ret;
  ret = rte_memrchr32(dst - 0 * 32, src);
  if (ret != NULL) return ret;
  return rte_memrchr32(dst - 1 * 32, src);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr128(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const ng_uint8_t *ret;
  ret = rte_memrchr64(dst - 0 * 64, src);
  if (ret != NULL) return ret;
  return rte_memrchr64(dst - 1 * 64, src);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr256(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const ng_uint8_t *ret;
  ret = rte_memrchr128(dst - 0 * 128, src);
  if (ret != NULL) return ret;
  return rte_memrchr128(dst - 1 * 128, src);
}

#if defined __AVX512F__ && defined RTE_MEMRCHR_AVX512

/**
 * AVX512 implementation below
 */

#define ALIGNMENT_MASK 0x3F

#define __m512_LOAD_MEMRCHR(i) do {                         \
  zmm1 = _mm512_loadu_si512((const void *)dst - (i << 6));  \
  zmm2 = _mm512_cmpeq_epi8(*((const __m512i *)src), zmm1);  \
  zmm0 = _mm512_cmpeq_epi8(memrchr_const__C.A512,zmm2);     \
  uint64_t mask = _mm512_movemask_epi8(zmm0);               \
  if (mask) {                                               \
    int clz = __builtin_clzll(mask);                        \
    return dst - clz - 1;                                   \
  }                                                         \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr128blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1, zmm2;
  ng_uint64_t mask;
  const ng_uint8_t *md;

  while (n >= 128) 
  {
    n   -= 128;
    __m512_LOAD_MEMRCHR(0);
    __m512_LOAD_MEMRCHR(1);
    dst -= 128;
  }

  return NULL;
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr512blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1;

  while (n >= 512) 
  {
    n   -= 512;
    __m512_LOAD_MEMRCHR(0);
    __m512_LOAD_MEMRCHR(1);
    __m512_LOAD_MEMRCHR(2);
    __m512_LOAD_MEMRCHR(3);
    __m512_LOAD_MEMRCHR(4);
    __m512_LOAD_MEMRCHR(5);
    __m512_LOAD_MEMRCHR(6);
    __m512_LOAD_MEMRCHR(7);
    dst -= 512;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memrchr_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_memrchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_memrchr32(dst, src);
  }
  
  if (n <= 32) 
  {
    ret = rte_memrchr16(dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memrchr16(dst + (16 - n), src);
  }
  
  if (__rte_constant(n) && n == 64) 
  {
    return rte_memrchr64(dst, src);
  }
  
  if (n <= 64) 
  {
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    return rte_memrchr32(dst + (32 - n), src);
  }
  
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n   -= 256;
      ret = rte_memrchr256(dst, src);
      if (ret != NULL) return ret;
      dst -= 256;
    }
    
    if (n >= 128) 
    {
      n   -= 128;
      ret = rte_memrchr128(dst, src);
      if (ret != NULL) return ret;
      dst -= 128;
    }
    
COPY_BLOCK_128_BACK63:
    if (n > 64) 
    {
      ret = rte_memrchr64(dst, src);
      if (ret != NULL) return ret;
      return rte_memrchr64(dst + (64 - n), src);
    }
    
    if (n > 0)
      return rte_memrchr64(dst + (64 - n), src);
    
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes
   */
  dstofss = ((ng_uintptr_t)dst & 0x3F);
  if (dstofss > 0) 
  {
    n   -= dstofss;
    ret = rte_memrchr64(dst, src);
    if (ret != NULL) return ret;
    dst -= dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  ret = rte_memrchr512blocks(dst, src, n);
  if (ret != NULL) return ret;
  bits = n;
  n &= 511;
  bits -= n;
  dst -= bits;

  /**
   * Copy 128-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  if (n >= 128) 
  {
    ret = rte_memrchr128blocks(dst, src, n);
    if (ret != NULL) return ret;
    bits = n;
    n &= 127;
    bits -= n;
    dst -= bits;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_128_BACK63;
}

#elif defined RTE_MEMRCHR_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

#define __m256_LOAD_MEMRCHR(dst) do {                       \
  ymm1 = _mm256_loadu_si256((const void *)dst - 32);        \
  ymm2 = _mm256_cmpeq_epi8(*((const __m256i *)src), ymm1);  \
  ymm0 = _mm256_cmpeq_epi8(memrchr_const__C.A256, ymm2);    \
  mask = _mm256_movemask_epi8(ymm0);                        \
  if (mask) {                                               \
    int clz = __builtin_clz(mask);                          \
    return dst - clz - 1;                                   \
  }                                                         \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_memrchr128blocks(const ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m256i ymm0, ymm1, ymm2;
  ng_uint32_t mask;
  
  while (n >= 128) 
  {
    n   -= 128;
    __m256_LOAD_MEMRCHR(dst -  0);
    __m256_LOAD_MEMRCHR(dst - 32);
    __m256_LOAD_MEMRCHR(dst - 64);
    __m256_LOAD_MEMRCHR(dst - 96);
    dst -= 128;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memrchr_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_memrchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_memrchr32(dst, src);
  }
  
  if (n <= 32) 
  {
    ret = rte_memrchr16(dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memrchr16(dst + (16 - n), src);
  }

  if (n <= 64) 
  {
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    return rte_memrchr32(dst + (32 - n), src);
  }
  
  if (n <= 256) 
  {
    if (n >= 128) 
    {
      n -= 128;
      ret = rte_memrchr128(dst, src);
      if (ret != NULL) return ret;
      dst -= 128;
    }
    
COPY_BLOCK_128_BACK31:
    if (n >= 64) 
    {
      n -= 64;
      ret = rte_memrchr64(dst, src);
      if (ret != NULL) return ret;
      dst -= 64;
    }
    
    if (n > 32) 
    {
      ret = rte_memrchr32(dst, src);
      if (ret != NULL) return ret;
      return rte_memrchr32(dst + (32 - n), src);
    }
    
    if (n > 0) 
    {
      return rte_memrchr32(dst + (32 - n), src);
    }
    
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 256 bytes
   */
  dstofss = (ng_uintptr_t)dst & 0x1F;
  if (dstofss > 0) 
  {
    n -= dstofss;
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    dst -= dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  ret = rte_memrchr128blocks(dst, src, n);
  if (ret != NULL) return ret;
  bits = n;
  n &= 127;
  bits -= n;
  dst -= bits;

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
rte_memrchr_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
#if !defined __AVX512F__ || !defined RTE_MEMRCHR_AVX512
  __m128i mm;
#endif

  const ng_uint8_t *ret;
  ng_size_t dstofss;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_memrchr15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) 
  {
    ret = rte_memrchr16(dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memrchr16(dst + (16 - n), src);
  }

  if (n <= 64) 
  {
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    if (n > 48)
    {  
      ret = rte_memrchr16(dst - 32, src);
      if (ret != NULL) return ret;
    }
    return rte_memrchr16(dst + (16 - n), src);
  }
  
  if (n <= 128) 
  {
    goto COPY_BLOCK_128_BACK15;
  }
  
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n   -= 256;
      ret = rte_memrchr128(dst, src);
      if (ret != NULL) return ret;
      ret = rte_memrchr128(dst - 128, src);
      if (ret != NULL) return ret;
      dst -= 256;
    }
    
COPY_BLOCK_255_BACK15:
    if (n >= 128) 
    {
      n   -= 128;
      ret = rte_memrchr128(dst, src);
      if (ret != NULL) return ret;
      dst -= 128;
    }
    
COPY_BLOCK_128_BACK15:
    if (n >= 64) 
    {
      n   -= 64;
      ret = rte_memrchr64(dst, src);
      if (ret != NULL) return ret;
      dst -= 64;
    }
    
COPY_BLOCK_64_BACK15:
    if (n >= 32) 
    {
      n   -= 32;
      ret = rte_memrchr32(dst, src);
      if (ret != NULL) return ret;
      dst -= 32;
    }

    if (n > 16) 
    {
      ret = rte_memrchr16(dst, src);
      if (ret != NULL) return ret;
      return rte_memrchr16(dst + (16 - n), src);
    }

    if (n > 0) 
    {
      return rte_memrchr16(dst + (16 - n), src);
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
  if (dstofss > 0) 
  {
    n   -= dstofss;
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    dst -= dstofss;
  }

  /**
   * For aligned copy
   */
  /**
   * Copy 256-byte blocks
   */
  for (; n >= 256; n -= 256) 
  {
    ret = rte_memrchr256(dst, src);
    if (ret != NULL) return ret;
    dst -= 256;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_255_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline const ng_uint8_t *
rte_memrchr_aligned(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const ng_uint8_t *ret;
  
  /* Copy size < 16 bytes */
  if (n < 16) 
  {
    return rte_memrchr15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_memrchr32(dst, src);
  }
  
  if (n <= 32) 
  {
    ret = rte_memrchr16(dst, src);
    if (ret != NULL) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_memrchr16(dst + (16 - n), src);
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) 
  {
    return rte_memrchr64(dst, src);
  }
  
  if (n <= 64) 
  {
    ret = rte_memrchr32(dst, src);
    if (ret != NULL) return ret;
    return rte_memrchr32(dst + (32 - n), src);
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) 
  {
    ret = rte_memrchr64(dst, src);
    if (ret != NULL) return ret;
    dst -= 64;
  }

  /* Copy whatever left */
  return rte_memrchr64(dst + (64 - n), src);
}

static __rte_always_inline char *
rte_memrchr(const void *dst, const ng_uint8_t c, ng_size_t n)
{
#define _(x) x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x
#if !defined __AVX512F__ || !defined RTE_MEMRCHR_AVX512
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
  if (!((ng_uintptr_t)(dst+n) & ALIGNMENT_MASK))
    return (char *)rte_memrchr_aligned((const ng_uint8_t *)dst+n, src, n);
  else
    return (char *)rte_memrchr_generic((const ng_uint8_t *)dst+n, src, n);
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_memrchr(dst, c, n) rte_memrchr(dst, c, n)

#else

#define ng_memrchr(dst, c, n) memrchr(dst, c, n)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_X86_64_H_ */

