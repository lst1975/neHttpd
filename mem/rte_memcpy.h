/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_MEMCPY_X86_64_H_
#define _RTE_MEMCPY_X86_64_H_

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
#define RTE_MEMCPY_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MEMCPY_AVX
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
rte_memcpy(void *dst, const void *src, ng_size_t n);

/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline ng_uint8_t *
rte_mov15_or_less(ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
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
  if (n & 8) 
  {
    ((struct rte_uint64_alias *)dst)->val =
      ((const struct rte_uint64_alias *)src)->val;
    src += 8;
    dst += 8;
  }

  if (n & 4) 
  {
    ((struct rte_uint32_alias *)dst)->val =
      ((const struct rte_uint32_alias *)src)->val;
    src += 4;
    dst += 4;
  }
  
  if (n & 2) 
  {
    ((struct rte_uint16_alias *)dst)->val =
      ((const struct rte_uint16_alias *)src)->val;
    src += 2;
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
rte_mov16(ng_uint8_t *dst, const ng_uint8_t *src)
{
  __m128i xmm0;

  xmm0 = _mm_loadu_si128((const __m128i *)(const void *)src);
  _mm_storeu_si128((__m128i *)(void *)dst, xmm0);
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_mov32(ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined RTE_MEMCPY_AVX
  __m256i ymm0;
  ymm0 = _mm256_loadu_si256((const __m256i *)(const void *)src);
  _mm256_storeu_si256((__m256i *)(void *)dst, ymm0);
#else /* SSE implementation */
  rte_mov16(dst + 0 * 16, src + 0 * 16);
  rte_mov16(dst + 1 * 16, src + 1 * 16);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_mov64(ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  __m512i zmm0;
  zmm0 = _mm512_loadu_si512((const void *)src);
  _mm512_storeu_si512((void *)dst, zmm0);
#else /* AVX2, AVX & SSE implementation */
  rte_mov32(dst + 0 * 32, src + 0 * 32);
  rte_mov32(dst + 1 * 32, src + 1 * 32);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_mov128(ng_uint8_t *dst, const ng_uint8_t *src)
{
  rte_mov64(dst + 0 * 64, src + 0 * 64);
  rte_mov64(dst + 1 * 64, src + 1 * 64);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_mov256(ng_uint8_t *dst, const ng_uint8_t *src)
{
  rte_mov128(dst + 0 * 128, src + 0 * 128);
  rte_mov128(dst + 1 * 128, src + 1 * 128);
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
rte_mov128blocks(ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1;

  while (n >= 128) 
  {
    zmm0 = _mm512_loadu_si512((const void *)(src + 0 * 64));
    n -= 128;
    zmm1 = _mm512_loadu_si512((const void *)(src + 1 * 64));
    src += 128;
    _mm512_storeu_si512((void *)(dst + 0 * 64), zmm0);
    _mm512_storeu_si512((void *)(dst + 1 * 64), zmm1);
    dst += 128;
  }
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static inline void
rte_mov512blocks(ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1, zmm2, zmm3, zmm4, zmm5, zmm6, zmm7;

  while (n >= 512) 
  {
    zmm0 = _mm512_loadu_si512((const void *)(src + 0 * 64));
    n -= 512;
    zmm1 = _mm512_loadu_si512((const void *)(src + 1 * 64));
    zmm2 = _mm512_loadu_si512((const void *)(src + 2 * 64));
    zmm3 = _mm512_loadu_si512((const void *)(src + 3 * 64));
    zmm4 = _mm512_loadu_si512((const void *)(src + 4 * 64));
    zmm5 = _mm512_loadu_si512((const void *)(src + 5 * 64));
    zmm6 = _mm512_loadu_si512((const void *)(src + 6 * 64));
    zmm7 = _mm512_loadu_si512((const void *)(src + 7 * 64));
    src += 512;
    _mm512_storeu_si512((void *)(dst + 0 * 64), zmm0);
    _mm512_storeu_si512((void *)(dst + 1 * 64), zmm1);
    _mm512_storeu_si512((void *)(dst + 2 * 64), zmm2);
    _mm512_storeu_si512((void *)(dst + 3 * 64), zmm3);
    _mm512_storeu_si512((void *)(dst + 4 * 64), zmm4);
    _mm512_storeu_si512((void *)(dst + 5 * 64), zmm5);
    _mm512_storeu_si512((void *)(dst + 6 * 64), zmm6);
    _mm512_storeu_si512((void *)(dst + 7 * 64), zmm7);
    dst += 512;
  }
}

static __rte_always_inline ng_uint8_t *
rte_memcpy_generic(ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  void *ret = dst;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_mov15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    rte_mov32(dst, src);
    return ret;
  }

  if (n <= 32) 
  {
    rte_mov16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_mov16(dst - 16 + n, src - 16 + n);
    return ret;
  }

  if (__rte_constant(n) && n == 64) 
  {
    rte_mov64(dst, src);
    return ret;
  }

  if (n <= 64) 
  {
    rte_mov32(dst, src);
    rte_mov32(dst - 32 + n, src - 32 + n);
    return ret;
  }

  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n -= 256;
      rte_mov256(dst, src);
      src += 256;
      dst += 256;
    }
    
    if (n >= 128) 
    {
      n -= 128;
      rte_mov128(dst, src);
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK63:
    if (n > 64) 
    {
      rte_mov64(dst, src);
      rte_mov64(dst - 64 + n, src - 64 + n);
      return ret;
    }
    
    if (n > 0)
      rte_mov64(dst - 64 + n, src - 64 + n);
    
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
    rte_mov64(dst, src);
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  rte_mov512blocks(dst, src, n);
  bits = n;
  n &= 511;
  bits -= n;
  src += bits;
  dst += bits;

  /**
   * Copy 128-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  if (n >= 128) 
  {
    rte_mov128blocks(dst, src, n);
    bits = n;
    n &= 127;
    bits -= n;
    src += bits;
    dst += bits;
  }

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_128_BACK63;
}

#elif defined RTE_MEMCPY_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline void
rte_mov128blocks(ng_uint8_t *dst, const ng_uint8_t *src, ng_size_t n)
{
  __m256i ymm0, ymm1, ymm2, ymm3;

  while (n >= 128) 
  {
    ymm0 = _mm256_loadu_si256((const __m256i *)(const void *)(src + 0 * 32));
    n -= 128;
    ymm1 = _mm256_loadu_si256((const __m256i *)(const void *)(src + 1 * 32));
    ymm2 = _mm256_loadu_si256((const __m256i *)(const void *)(src + 2 * 32));
    ymm3 = _mm256_loadu_si256((const __m256i *)(const void *)(src + 3 * 32));
    src += 128;
    _mm256_storeu_si256((__m256i *)(void *)(dst + 0 * 32), ymm0);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 1 * 32), ymm1);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 2 * 32), ymm2);
    _mm256_storeu_si256((__m256i *)(void *)(dst + 3 * 32), ymm3);
    dst += 128;
  }
}

static __rte_always_inline ng_uint8_t *
rte_memcpy_generic(ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  void *ret = dst;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_mov15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    rte_mov32(dst, src);
    return ret;
  }

  if (n <= 32) 
  {
    rte_mov16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_mov16(dst - 16 + n, src - 16 + n);
    return ret;
  }
  
  if (n <= 64) 
  {
    rte_mov32(dst, src);
    rte_mov32(dst - 32 + n, src - 32 + n);
    return ret;
  }
  
  if (n <= 256) 
  {
    if (n >= 128) 
    {
      n -= 128;
      rte_mov128(dst, src);
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK31:
    if (n >= 64) 
    {
      n -= 64;
      rte_mov64(dst, src);
      src += 64;
      dst += 64;
    }
    
    if (n > 32) 
    {
      rte_mov32(dst, src);
      rte_mov32(dst - 32 + n, src - 32 + n);
      return ret;
    }
    
    if (n > 0) 
    {
      rte_mov32(dst - 32 + n, src - 32 + n);
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
    rte_mov32(dst, src);
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  rte_mov128blocks(dst, src, n);
  bits = n;
  n &= 127;
  bits -= n;
  src += bits;
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
#define MOVEUNALIGNED_LEFT47_IMM(dst, src, len, offset)                                              \
{                                                                                                    \
  ng_size_t tmp;                                                                                     \
  while (len >= 128 + 16 - offset) {                                                                 \
    xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0 * 16));                  \
    len -= 128;                                                                                      \
    xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 1 * 16));                  \
    xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 2 * 16));                  \
    xmm3 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 3 * 16));                  \
    xmm4 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 4 * 16));                  \
    xmm5 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 5 * 16));                  \
    xmm6 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 6 * 16));                  \
    xmm7 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 7 * 16));                  \
    xmm8 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 8 * 16));                  \
    src += 128;                                                                                      \
    _mm_storeu_si128((__m128i *)(void *)(dst + 0 * 16), _mm_alignr_epi8(xmm1, xmm0, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 1 * 16), _mm_alignr_epi8(xmm2, xmm1, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 2 * 16), _mm_alignr_epi8(xmm3, xmm2, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 3 * 16), _mm_alignr_epi8(xmm4, xmm3, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 4 * 16), _mm_alignr_epi8(xmm5, xmm4, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 5 * 16), _mm_alignr_epi8(xmm6, xmm5, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 6 * 16), _mm_alignr_epi8(xmm7, xmm6, offset));        \
    _mm_storeu_si128((__m128i *)(void *)(dst + 7 * 16), _mm_alignr_epi8(xmm8, xmm7, offset));        \
    dst += 128;                                                                                      \
  }                                                                                                  \
  tmp = len;                                                                                         \
  len = ((len - 16 + offset) & 127) + 16 - offset;                                                   \
  tmp -= len;                                                                                        \
  src += tmp;                                                                                        \
  dst += tmp;                                                                                        \
  if (len >= 32 + 16 - offset) {                                                                     \
    while (len >= 32 + 16 - offset) {                                                                \
      xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0 * 16));                \
      len -= 32;                                                                                     \
      xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 1 * 16));                \
      xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 2 * 16));                \
      src += 32;                                                                                     \
      _mm_storeu_si128((__m128i *)(void *)(dst + 0 * 16), _mm_alignr_epi8(xmm1, xmm0, offset));      \
      _mm_storeu_si128((__m128i *)(void *)(dst + 1 * 16), _mm_alignr_epi8(xmm2, xmm1, offset));      \
      dst += 32;                                                                                     \
    }                                                                                                \
    tmp = len;                                                                                       \
    len = ((len - 16 + offset) & 31) + 16 - offset;                                                  \
    tmp -= len;                                                                                      \
    src += tmp;                                                                                      \
    dst += tmp;                                                                                      \
  }                                                                                                  \
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
 * - __m128i <xmm0> ~ <xmm8> used in MOVEUNALIGNED_LEFT47_IMM must be pre-defined
 */
#define MOVEUNALIGNED_LEFT47(dst, src, len, offset)                   \
{                                                                     \
  switch (offset) {                                                   \
  case 0x01: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x01); break;      \
  case 0x02: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x02); break;      \
  case 0x03: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x03); break;      \
  case 0x04: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x04); break;      \
  case 0x05: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x05); break;      \
  case 0x06: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x06); break;      \
  case 0x07: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x07); break;      \
  case 0x08: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x08); break;      \
  case 0x09: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x09); break;      \
  case 0x0A: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0A); break;      \
  case 0x0B: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0B); break;      \
  case 0x0C: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0C); break;      \
  case 0x0D: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0D); break;      \
  case 0x0E: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0E); break;      \
  case 0x0F: MOVEUNALIGNED_LEFT47_IMM(dst, src, n, 0x0F); break;      \
  default:;                                                           \
  }                                                                   \
}

static __rte_always_inline ng_uint8_t *
rte_memcpy_generic(ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
  void *ret = dst;
  ng_size_t dstofss;
  ng_size_t srcofs;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_mov15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) 
  {
    rte_mov16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_mov16(dst - 16 + n, src - 16 + n);
    return ret;
  }
  
  if (n <= 64) 
  {
    rte_mov32(dst, src);
    if (n > 48)
      rte_mov16(dst + 32, src + 32);
    rte_mov16(dst - 16 + n, src - 16 + n);
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
      rte_mov128(dst, src);
      rte_mov128(dst + 128, src + 128);
      src += 256;
      dst += 256;
    }
    
COPY_BLOCK_255_BACK15:
    if (n >= 128) 
    {
      n -= 128;
      rte_mov128(dst, src);
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK15:
    if (n >= 64) 
    {
      n -= 64;
      rte_mov64(dst, src);
      src += 64;
      dst += 64;
    }
    
COPY_BLOCK_64_BACK15:
    if (n >= 32) 
    {
      n -= 32;
      rte_mov32(dst, src);
      src += 32;
      dst += 32;
    }
    
    if (n > 16) 
    {
      rte_mov16(dst, src);
      rte_mov16(dst - 16 + n, src - 16 + n);
      return ret;
    }
    
    if (n > 0) 
    {
      rte_mov16(dst - 16 + n, src - 16 + n);
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
    rte_mov32(dst, src);
    src += dstofss;
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
      rte_mov256(dst, src);
      dst += 256;
      src += 256;
    }

    /**
     * Copy whatever left
     */
    goto COPY_BLOCK_255_BACK15;
  }

  /**
   * For copy with unaligned load
   */
  MOVEUNALIGNED_LEFT47(dst, src, n, srcofs);

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_64_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline ng_uint8_t *
rte_memcpy_aligned(ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  void *ret = dst;

  /* Copy size < 16 bytes */
  if (n < 16) 
  {
    return rte_mov15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) 
  {
    rte_mov32(dst, src);
    return ret;
  }
  
  if (n <= 32) 
  {
    rte_mov16(dst, src);
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    rte_mov16(dst - 16 + n, src - 16 + n);

    return ret;
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) 
  {
    rte_mov64(dst, src);
    return ret;
  }
  
  if (n <= 64) 
  {
    rte_mov32(dst, src);
    rte_mov32(dst - 32 + n, src - 32 + n);

    return ret;
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) 
  {
    rte_mov64(dst, src);
    dst += 64;
    src += 64;
  }

  /* Copy whatever left */
  rte_mov64(dst - 64 + n, src - 64 + n);

  return ret;
}

static __rte_always_inline void *
rte_memcpy(void *dst, const void *src, ng_size_t n)
{
  if (!(((ng_uintptr_t)dst | (ng_uintptr_t)src) & ALIGNMENT_MASK))
    return rte_memcpy_aligned((ng_uint8_t *)dst, (const ng_uint8_t *)src, n);
  else
    return rte_memcpy_generic((ng_uint8_t *)dst, (const ng_uint8_t *)src, n);
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_memcpy(dst, src, n) rte_memcpy(dst, src, n)

#else

#define ng_memcpy(dst, src, n) memcpy(dst, src, n)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_X86_64_H_ */
