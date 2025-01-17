/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_MEMSET_X86_64_H_
#define _RTE_MEMSET_X86_64_H_

/**
 * @file
 *
 * Functions for SSE/AVX/AVX2/AVX512 implementation of memcpy().
 */

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

/*
 * GCC older than version 11 doesn't compile AVX properly, so use SSE instead.
 * There are no problems with AVX2.
 */
#if defined __AVX2__
#define RTE_MEMSET_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MEMSET_AVX
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
static __rte_always_inline void *
rte_memset(void *dst, int c, ng_size_t n);

/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline ng_uint8_t *
rte_set15_or_less(ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  /**
   * Use the following structs to avoid violating C standard
   * alignment requirements and to avoid strict aliasing bugs
   */
  struct rte_uint64_alias {
    ng_uint64_t val;
  } __rte_packed __rte_may_alias;
  struct rte_uint32_alias {
    ng_uint32_t val;
  } __rte_packed __rte_may_alias;
  struct rte_uint16_alias {
    ng_uint16_t val;
  } __rte_packed __rte_may_alias;

  void *ret = dst;
  if (n & 8) {
    ((struct rte_uint64_alias *)dst)->val =
      ((const struct rte_uint64_alias *)src)->val;
    dst += 8;
  }
  if (n & 4) {
    ((struct rte_uint32_alias *)dst)->val =
      ((const struct rte_uint32_alias *)src)->val;
    dst += 4;
  }
  if (n & 2) {
    ((struct rte_uint16_alias *)dst)->val =
      ((const struct rte_uint16_alias *)src)->val;
    dst += 2;
  }
  if (n & 1)
    *dst = *src;
  return ret;
}

/**
 * Copy 16 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set16(ng_uint8_t *dst, const ng_uint8_t *src)
{
  _mm_storeu_si128((__m128i *)(void *)dst, *(const __m128i *)src);
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set32(ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined RTE_MEMSET_AVX
  _mm256_storeu_si256((__m256i *)(void *)dst, *(const __m256i *)src);
#else /* SSE implementation */
  rte_set16(dst + 0 * 16, src);
  rte_set16(dst + 1 * 16, src);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set64(ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  _mm512_storeu_si512((void *)dst, *(const __m512i *)src);
#else /* AVX2, AVX & SSE implementation */
  rte_set32(dst + 0 * 32, src);
  rte_set32(dst + 1 * 32, src);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set128(ng_uint8_t *dst, const ng_uint8_t *src)
{
  rte_set64(dst + 0 * 64, src);
  rte_set64(dst + 1 * 64, src);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set256(ng_uint8_t *dst, const ng_uint8_t *src)
{
  rte_set128(dst + 0 * 128, src);
  rte_set128(dst + 1 * 128, src);
}

#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512

/**
 * AVX512 implementation below
 */

#define ALIGNMENT_MASK 0x3F

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set128blocks(ng_uint8_t *dst, const __m512i src, ng_size_t n)
{
  while (n >= 128) 
  {
    n -= 128;
    _mm512_storeu_si512((void *)(dst + 0 * 64), src);
    _mm512_storeu_si512((void *)(dst + 1 * 64), src);
    dst = dst + 128;
  }
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static inline void
rte_set512blocks(ng_uint8_t *dst, const __m512i src, ng_size_t n)
{
  while (n >= 512) 
  {
    n -= 512;
    _mm512_storeu_si512((void *)(dst + 0 * 64), src);
    _mm512_storeu_si512((void *)(dst + 1 * 64), src);
    _mm512_storeu_si512((void *)(dst + 2 * 64), src);
    _mm512_storeu_si512((void *)(dst + 3 * 64), src);
    _mm512_storeu_si512((void *)(dst + 4 * 64), src);
    _mm512_storeu_si512((void *)(dst + 5 * 64), src);
    _mm512_storeu_si512((void *)(dst + 6 * 64), src);
    _mm512_storeu_si512((void *)(dst + 7 * 64), src);
    dst = dst + 512;
  }
}

static __rte_always_inline ng_uint8_t *
rte_memset_generic(ng_uint8_t *dst, ng_uint8_t x, ng_size_t n)
{
  void *ret = dst;
  ng_size_t dstofss;
  ng_size_t bits;

  __m512i zmm0 = _mm512_set1_epi8(x);
  ng_uint8_t *src = (ng_uint8_t *)&zmm0;
  
  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_set15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    rte_set32(dst, src);
    return ret;
  }
  if (n <= 32) 
  {
    rte_set16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_set16((ng_uint8_t *)dst - 16 + n, src);
    return ret;
  }
  if (__rte_constant(n) && n == 64) 
  {
    rte_set64(dst, src);
    return ret;
  }
  if (n <= 64) 
  {
    rte_set32(dst, src);
    rte_set32(dst - 32 + n, src);
    return ret;
  }
  
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n -= 256;
      rte_set256(dst, src);
      dst += 256;
    }
    if (n >= 128) 
    {
      n -= 128;
      rte_set128(dst, src);
      dst += 128;
    }
COPY_BLOCK_128_BACK63:
    if (n > 64) 
    {
      rte_set64(dst, src);
      rte_set64(dst - 64 + n, src);
      return ret;
    }
    if (n > 0)
      rte_set64(dst - 64 + n, src);
    return ret;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes
   */
  dstofss = ((ng_uintptr_t)dst & 0x3F);
  if (dstofss > 0) 
  {
    dstofss = 64 - dstofss;
    n -= dstofss;
    rte_set64(dst, src);
    dst += dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  rte_set512blocks(dst, zmm0, n);
  bits = n;
  n &= 511;
  bits -= n;
  dst += bits;

  /**
   * Copy 128-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  if (n >= 128) 
  {
    rte_set128blocks(dst, zmm0, n);
    bits = n;
    n &= 127;
    bits -= n;
    dst += bits;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_128_BACK63;
}

#elif defined RTE_MEMSET_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_set128blocks(ng_uint8_t *dst, const __m256i zmm0, ng_size_t n)
{
  while (n >= 128) {
    n -= 128;
    _mm256_storeu_si256((__m256i *)(void *)(dst + 0 * 32), zmm0);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 1 * 32), zmm0);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 2 * 32), zmm0);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 3 * 32), zmm0);
    dst += 128;
  }
}

static __rte_always_inline ng_uint8_t *
rte_memset_generic(ng_uint8_t *dst, ng_uint8_t x, ng_size_t n)
{
  ng_uint8_t *ret = dst;
  ng_size_t dstofss;
  ng_size_t bits;

  __m256i zmm0 = _mm256_set1_epi8(x);
  ng_uint8_t *src = (ng_uint8_t *)&zmm0;
  
  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_set15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    rte_set32(dst, src);
    return ret;
  }
  if (n <= 32) 
  {
    rte_set16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_set16(dst - 16 + n,src);
    return ret;
  }
  if (n <= 64) 
  {
    rte_set32(dst, src);
    rte_set32(dst - 32 + n,src);
    return ret;
  }
  if (n <= 256) 
  {
    if (n >= 128) 
    {
      n -= 128;
      rte_set128(dst, src);
      dst += 128;
    }
COPY_BLOCK_128_BACK31:
    if (n >= 64) 
    {
      n -= 64;
      rte_set64(dst, src);
      dst += 64;
    }
    if (n > 32) 
    {
      rte_set32(dst, src);
      rte_set32(dst - 32 + n,src);
      return ret;
    }
    if (n > 0) 
    {
      rte_set32(dst - 32 + n,src);
    }
    return ret;
  }

  /**
   * Make store aligned when copy size exceeds 256 bytes
   */
  dstofss = (ng_uintptr_t)dst & 0x1F;
  if (dstofss > 0) 
  {
    dstofss = 32 - dstofss;
    n -= dstofss;
    rte_set32(dst, src);
    dst += dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  rte_set128blocks((ng_uint8_t *)dst, zmm0, n);
  bits = n;
  n &= 127;
  bits -= n;
  dst += bits;

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

/**
 * Macro for copying unaligned block from one location to another with constant load offset,
 * 47 bytes leftover maximum,
 * locations should not overlap.
 * Requirements:
 * - Store is aligned
 * - Load offset is <offset>, which must be immediate value within [1, 15]
 * - For <src>, make sure <offset> bit backwards & <16 - offset> bit forwards are available for loading
 * - <dst>, <src>, <len> must be variables
 * - __m128i <xmm0> ~ <xmm8> must be pre-defined
 */
#define SETUNALIGNED_LEFT47_IMM(dst, xmm0, len, offset)                              \
{                                                                                    \
  ng_size_t tmp;                                                                     \
  while (len >= 128 + 16 - offset) {                                                 \
    len -= 128;                                                                      \
    _mm_storeu_si128((__m128i *)(void *)(dst + 0 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 1 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 2 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 3 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 4 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 5 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 6 * 16), xmm0);                       \
    _mm_storeu_si128((__m128i *)(void *)(dst + 7 * 16), xmm0);                       \
    dst = (ng_uint8_t *)dst + 128;                                                    \
  }                                                                                   \
  tmp = len;                                                                          \
  len = ((len - 16 + offset) & 127) + 16 - offset;                                    \
  tmp -= len;                                                                         \
  dst += tmp;                                                                         \
  if (len >= 32 + 16 - offset) {                                                      \
    while (len >= 32 + 16 - offset) {                                                 \
      len -= 32;                                                                      \
      _mm_storeu_si128((__m128i *)(void *)(dst + 0 * 16), xmm0);                      \
      _mm_storeu_si128((__m128i *)(void *)(dst + 1 * 16), xmm0);                      \
      dst += 32;                                                                      \
    }                                                                                 \
    tmp = len;                                                                        \
    len = ((len - 16 + offset) & 31) + 16 - offset;                                   \
    tmp -= len;                                                                       \
    dst += tmp;                                                                       \
  }                                                                                   \
}

/**
 * Macro for copying unaligned block from one location to another,
 * 47 bytes leftover maximum,
 * locations should not overlap.
 * Use switch here because the aligning instruction requires immediate value for shift count.
 * Requirements:
 * - Store is aligned
 * - Load offset is <offset>, which must be within [1, 15]
 * - For <src>, make sure <offset> bit backwards & <16 - offset> bit forwards are available for loading
 * - <dst>, <src>, <len> must be variables
 * - __m128i <xmm0> ~ <xmm8> used in SETUNALIGNED_LEFT47_IMM must be pre-defined
 */
#define SETUNALIGNED_LEFT47(dst, src, len, offset)                   \
{                                                      \
    switch (offset) {                                                 \
    case 0x01: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x01); break;    \
    case 0x02: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x02); break;    \
    case 0x03: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x03); break;    \
    case 0x04: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x04); break;    \
    case 0x05: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x05); break;    \
    case 0x06: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x06); break;    \
    case 0x07: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x07); break;    \
    case 0x08: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x08); break;    \
    case 0x09: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x09); break;    \
    case 0x0A: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0A); break;    \
    case 0x0B: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0B); break;    \
    case 0x0C: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0C); break;    \
    case 0x0D: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0D); break;    \
    case 0x0E: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0E); break;    \
    case 0x0F: SETUNALIGNED_LEFT47_IMM(dst, src, n, 0x0F); break;    \
    default:;                                                        \
    }                                                                \
}

static __rte_always_inline ng_uint8_t *
rte_memset_generic(ng_uint8_t *dst, ng_uint8_t x, ng_size_t n)
{
  ng_uint8_t *ret = dst;
  ng_size_t dstofss;
  ng_size_t srcofs;
  
  __m128i xmm0 = _mm_set1_epi8(x);
  ng_uint8_t *src = (ng_uint8_t *)&xmm0;
  
  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_set15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) 
  {
    rte_set16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_set16(dst - 16 + n, src);
    return ret;
  }
  
  if (n <= 64) 
  {
    rte_set32(dst, src);
    if (n > 48)
      rte_set16(dst + 32, src);
    rte_set16(dst - 16 + n, src);
    return ret;
  }
  
  if (n <= 128) 
  {
    goto COPY_BLOCK_128_BACK15;
  }
  
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n -= 256;
      rte_set128(dst, src);
      rte_set128(dst + 128, src);
      dst += 256;
    }
    
COPY_BLOCK_255_BACK15:
    if (n >= 128) 
    {
      n -= 128;
      rte_set128(dst, src);
      dst += 128;
    }
    
COPY_BLOCK_128_BACK15:
    if (n >= 64) 
    {
      n -= 64;
      rte_set64(dst, src);
      dst += 64;
    }
    
COPY_BLOCK_64_BACK15:
    if (n >= 32) 
    {
      n -= 32;
      rte_set32(dst, src);
      dst += 32;
    }
    
    if (n > 16) 
    {
      rte_set16(dst, src);
      rte_set16(dst - 16 + n, src);
      return ret;
    }
    
    if (n > 0) 
    {
      rte_set16(dst - 16 + n, src);
    }
    
    return ret;
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
    dstofss = 16 - dstofss + 16;
    n -= dstofss;
    rte_set32(dst, src);
    dst += dstofss;
  }
  srcofs = ((ng_uintptr_t)src & 0x0F);

  /**
   * For aligned copy
   */
  if (srcofs == 0) 
  {
    /**
     * Copy 256-byte blocks
     */
    for (; n >= 256; n -= 256) 
    {
      rte_set256(dst, src);
      dst += 256;
    }

    /**
     * Copy whatever left
     */
    goto COPY_BLOCK_255_BACK15;
  }

  /**
   * For copy with unaligned load
   */
  SETUNALIGNED_LEFT47(dst, xmm0, n, srcofs);

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_64_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline ng_uint8_t *
rte_memset_aligned(ng_uint8_t *dst, int x, ng_size_t n)
{
  ng_uint8_t *ret = dst;

  __m128i mm04[4]={
      _mm_set1_epi8(x),
      _mm_set1_epi8(x),
      _mm_set1_epi8(x),
      _mm_set1_epi8(x),
    };
  const ng_uint8_t *src = (const ng_uint8_t *)mm04;
  
  /* Copy size < 16 bytes */
  if (n < 16) 
  {
    return rte_set15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) 
  {
    rte_set32(dst, src);
    return ret;
  }
  if (n <= 32) 
  {
    rte_set16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_set16(dst - 16 + n, src);

    return ret;
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) 
  {
    rte_set64(dst, src);
    return ret;
  }
  if (n <= 64) 
  {
    rte_set32(dst, src);
    rte_set32(dst - 32 + n, src);

    return ret;
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) 
  {
    rte_set64(dst, src);
    dst += 64;
  }

  /* Copy whatever left */
  rte_set64(dst - 64 + n, src);

  return ret;
}

static __rte_always_inline void *
rte_memset(void *dst, int c, ng_size_t n)
{
  if (!((ng_uintptr_t)dst & ALIGNMENT_MASK))
    return rte_memset_aligned((ng_uint8_t *)dst, (ng_uint8_t)c, n);
  else
    return rte_memset_generic((ng_uint8_t *)dst, (ng_uint8_t)c, n);
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_memset(dst, c, n) rte_memset(dst, c, n)

#else

#define ng_memset(dst, c, n) memset(dst, c, n)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_X86_64_H_ */
