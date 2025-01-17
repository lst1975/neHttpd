/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_MEMCMP_X86_64_H_
#define _RTE_MEMCMP_X86_64_H_

/**
 * @file
 *
 * Functions for SSE/AVX/AVX2/AVX512 implementation of memcmp().
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
#define RTE_MEMCMP_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MEMCMP_AVX
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
static __rte_always_inline int
rte_memcmp(const void *dst, const void *src, ng_size_t n);

/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp15_or_less(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
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

  if (n & 8) {
    if (((const struct rte_uint64_alias *)dst)->val !=
      ((const struct rte_uint64_alias *)src)->val)
      return dst;
    dst += 8;
  }

  if (n & 4) {
    if (((const struct rte_uint32_alias *)dst)->val !=
      ((const struct rte_uint32_alias *)src)->val)
      return dst;
    dst += 4;
  }

  if (n & 2) {
    if (((const struct rte_uint16_alias *)dst)->val !=
      ((const struct rte_uint16_alias *)src)->val)
      return dst;
    dst += 2;
  }

  if (n & 1)
    if (*dst != *src)
      return dst;

  return NULL;
}

/**
 * Copy 16 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp16(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  __m128i xmm0,xmm1;
  xmm0 = _mm_loadu_si128((const __m128i *)(const void *)src);
  xmm1 = _mm_loadu_si128((const __m128i *)(const void *)dst);
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  return _mm_cmpeq_epi64_mask(xmm0,xmm1) == 0x3U ? NULL : dst;
#else
#if defined __SSE4_1__
  __m128i xmm2 = _mm_cmpeq_epi64(xmm0, xmm1);
#elif defined __SSE2__
  __m128i xmm2 = _mm_cmpeq_epi32(xmm0, xmm1);
#else
  __m128i xmm2 = _mm_cmpeq_epi8(xmm0, xmm1);
#endif
  return _mm_movemask_epi8(xmm2) == 0xffffU ? NULL : dst;
#endif
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp32(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined RTE_MEMCMP_AVX
  __m256i ymm0,ymm1;
  ymm0 = _mm256_loadu_si256((const __m256i *)(const void *)src);
  ymm1 = _mm256_loadu_si256((const __m256i *)(const void *)dst);
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  return _mm256_cmpeq_epi64_mask(ymm0,ymm1) == 0xfU ? NULL : dst;
#else
  __m256i ymm2 = _mm256_cmpeq_epi64(ymm0,ymm1);
  return _mm256_movemask_epi8(ymm2) == -1 ? NULL : dst;
#endif
#else /* SSE implementation */
  const void *ret;
  ret = rte_cmp16(dst + 0 * 16, src + 0 * 16);
  if (ret) return ret;
  return rte_cmp16(dst + 1 * 16, src + 1 * 16);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp64(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  __m512i zmm0,zmm1;

  zmm0 = _mm512_loadu_si512((const void *)src);
  zmm1 = _mm512_loadu_si512((const void *)dst);
  return _mm512_cmpeq_epi64_mask(zmm0, zmm1) == 0xffU ? NULL : dst;
#else /* AVX2, AVX & SSE implementation */
  const void *ret;
  ret = rte_cmp32(dst + 0 * 32, src + 0 * 32);
  if (ret) return ret;
  return rte_cmp32(dst + 1 * 32, src + 1 * 32);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp128(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const void *ret;
  ret = rte_cmp64(dst + 0 * 64, src + 0 * 64);
  if (ret) return ret;
  return rte_cmp64(dst + 1 * 64, src + 1 * 64);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp256(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  const void *ret;
  ret = rte_cmp128(dst + 0 * 128, src + 0 * 128);
  if (ret) return ret;
  return rte_cmp128(dst + 1 * 128, src + 1 * 128);
}

#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512

/**
 * AVX512 implementation below
 */

#define ALIGNMENT_MASK 0x3F

#define __m512_LOAD_MEMCMP(i) do {\
  zmm0 = _mm512_loadu_si512((const void *)(src + (i << 6))); \
  md = dst + (i << 6); zmm1 = _mm512_loadu_si512((const void *)md); \
  if (_mm512_cmpeq_epi64_mask(zmm0, zmm1)!=0xffU) return md; \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp128blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *md;
  __m512i zmm0, zmm1;

  while (n >= 128) {
    n -= 128;
    __m512_LOAD_MEMCMP(0);
    __m512_LOAD_MEMCMP(1);
    src = src + 128;
    dst = dst + 128;
  }

  return NULL;
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static inline const ng_uint8_t *
rte_cmp512blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *md;
  __m512i zmm0, zmm1;

  while (n >= 512) {
    n -= 512;
    __m512_LOAD_MEMCMP(0);
    __m512_LOAD_MEMCMP(1);
    __m512_LOAD_MEMCMP(2);
    __m512_LOAD_MEMCMP(3);
    __m512_LOAD_MEMCMP(4);
    __m512_LOAD_MEMCMP(5);
    __m512_LOAD_MEMCMP(6);
    __m512_LOAD_MEMCMP(7);
    src = src + 512;
    dst = dst + 512;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memcmp_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_cmp15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) {
    ret = rte_cmp32(dst, src);
    return ret;
  }
  if (n <= 32) {
    ret = rte_cmp16(dst, src);
    if (ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    ret = rte_cmp16(dst - 16 + n, src - 16 + n);
    return ret;
  }
  if (__rte_constant(n) && n == 64) {
    ret = rte_cmp64(dst, src);
    return ret;
  }
  if (n <= 64) {
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    ret = rte_cmp32(dst - 32 + n, src - 32 + n);
    return ret;
  }
  if (n <= 512) {
    if (n >= 256) {
      n -= 256;
      ret = rte_cmp256(dst, src);
      if (ret) return ret;
      src += 256;
      dst += 256;
    }
    if (n >= 128) {
      n -= 128;
      ret = rte_cmp128(dst, src);
      if (ret) return ret;
      src += 128;
      dst += 128;
    }
COPY_BLOCK_128_BACK63:
    if (n > 64) {
      ret = rte_cmp64(dst, src);
      if (ret) return ret;
      ret = rte_cmp64(dst - 64 + n, src - 64 + n);
      return ret;
    }
    if (n > 0)
      return rte_cmp64(dst - 64 + n, src - 64 + n);
    return NULL;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes
   */
  dstofss = ((ng_uintptr_t)dst & 0x3F);
  if (dstofss > 0) {
    dstofss = 64 - dstofss;
    n -= dstofss;
    ret = rte_cmp64(dst, src);
    if (ret) return ret;
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  ret = rte_cmp512blocks(dst, src, n);
  if (ret) return ret;
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
  if (n >= 128) {
    ret = rte_cmp128blocks(dst, src, n);
    if (ret) return ret;
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

#elif defined RTE_MEMCMP_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
#define __m256_LOAD_MEMCMP(i) do {\
  ymm0 = _mm256_loadu_si256((const void *)(src + (i << 5))); \
  md = dst + (i << 5); ymm1 = _mm256_loadu_si256((const void *)md); \
  if (_mm256_cmpeq_epi64_mask(ymm2)!=0xfU) return md; \
}while(0)
#else
#define __m256_LOAD_MEMCMP(i) do {\
  __m256i ymm2;\
  ymm0 = _mm256_loadu_si256((const void *)(src + (i << 5))); \
  md = dst + (i << 5); ymm1 = _mm256_loadu_si256((const void *)md); \
  ymm2 = _mm256_cmpeq_epi64(ymm0, ymm1); \
  if (_mm256_movemask_epi8(ymm2)!=-1) return md; \
}while(0)
#endif

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline const ng_uint8_t *
rte_cmp128blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *md;
  __m256i ymm0, ymm1;

  while (n >= 128) {
    n -= 128;
    __m256_LOAD_MEMCMP(0);
    __m256_LOAD_MEMCMP(1);
    __m256_LOAD_MEMCMP(2);
    __m256_LOAD_MEMCMP(3);
    src += 128;
    dst += 128;
  }

  return NULL;
}

static __rte_always_inline const ng_uint8_t *
rte_memcmp_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_cmp15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) {
    return rte_cmp32(dst, src);
  }
  if (n <= 32) {
    ret = rte_cmp16(dst, src);
    if (ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    if (ret) return ret;
    return rte_cmp16(dst - 16 + n, src - 16 + n);
  }
  if (n <= 64) {
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    ret = rte_cmp32(dst - 32 + n, src - 32 + n);
    return ret;
  }
  if (n <= 256) {
    if (n >= 128) {
      n -= 128;
      ret = rte_cmp128(dst, src);
      if (ret) return ret;
      src += 128;
      dst += 128;
    }
COPY_BLOCK_128_BACK31:
    if (n >= 64) {
      n -= 64;
      ret = rte_cmp64(dst, src);
      if (ret) return ret;
      src += 64;
      dst += 64;
    }
    if (n > 32) {
      ret = rte_cmp32(dst, src);
      if (ret) return ret;
      ret = rte_cmp32(dst - 32 + n, src - 32 + n);
      return ret;
    }
    if (n > 0) {
      return rte_cmp32(dst - 32 + n, src - 32 + n);
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
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  ret = rte_cmp128blocks(dst, src, n);
  if (ret) return ret;
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
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  #define CMPUNALIGNED_LEFT47_IMM_CMP(ymm0, xmm1, xmm0, offset, i) do { \
      if (_mm_cmpeq_epi64_mask(ymm0, _mm_alignr_epi8(xmm1, xmm0, offset)!=0x3U)) return dst + (i << 4);        \
    }while(0)
#elif defined __SSE4_1__
  #define CMPUNALIGNED_LEFT47_IMM_CMP(ymm0,xmm1, xmm0, offset, i) do { \
      mm = _mm_cmpeq_epi64(ymm0, _mm_alignr_epi8(xmm1, xmm0, offset)); \
      if (_mm_movemask_epi8(mm)!=0xffffU) return return dst + i;    \
    }while(0)
#elif defined __SSE2__
#define CMPUNALIGNED_LEFT47_IMM_CMP(ymm0,xmm1, xmm0, offset, i) do { \
      mm = _mm_cmpeq_epi32(ymm0, _mm_alignr_epi8(xmm1, xmm0, offset)); \
      if (_mm_movemask_epi8(mm)!=0xffffU) return return dst + i;    \
    }while(0)
#endif
#define CMPUNALIGNED_LEFT47_IMM(dst, src, len, offset)                                                     \
{                                                                                            \
    ng_size_t tmp;                                                                                                \
    while (len >= 128 + 16 - offset) {                                                                      \
        len -= 128;                                                                                         \
        xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0 * 16));                  \
        xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 1 * 16));                  \
        xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 2 * 16));                  \
        xmm3 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 3 * 16));                  \
        xmm4 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 4 * 16));                  \
        xmm5 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 5 * 16));                  \
        xmm6 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 6 * 16));                  \
        xmm7 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 7 * 16));                  \
        xmm8 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 8 * 16));                  \
        ymm0 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 0 * 16));                  \
        ymm1 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 1 * 16));                  \
        ymm2 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 2 * 16));                  \
        ymm3 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 3 * 16));                  \
        ymm4 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 4 * 16));                  \
        ymm5 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 5 * 16));                  \
        ymm6 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 6 * 16));                  \
        ymm7 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 7 * 16));                  \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm0, xmm1, xmm0, offset, 0); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm1, xmm2, xmm1, offset, 16); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm2, xmm3, xmm2, offset, 32); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm3, xmm4, xmm3, offset, 48); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm4, xmm5, xmm4, offset, 64); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm5, xmm6, xmm5, offset, 80); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm6, xmm7, xmm6, offset, 96); \
        CMPUNALIGNED_LEFT47_IMM_CMP(ymm7, xmm8, xmm7, offset, 112); \
        src += 128;                                                                   \
        dst += 128;                                                                         \
    }                                                                                                       \
    tmp = len;                                                                                              \
    len = ((len - 16 + offset) & 127) + 16 - offset;                                                        \
    tmp -= len;                                                                                             \
    src += tmp;                                                                       \
    dst += tmp;                                                                             \
    if (len >= 32 + 16 - offset) {                                                                          \
        while (len >= 32 + 16 - offset) {                                                                   \
            len -= 32;                                                                                      \
            xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0 * 16));              \
            xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 1 * 16));              \
            xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 2 * 16));              \
            ymm0 = _mm_loadu_si128((__m128i *)(void *)(dst + 0 * 16));                  \
            ymm1 = _mm_loadu_si128((__m128i *)(void *)(dst + 1 * 16));                  \
            CMPUNALIGNED_LEFT47_IMM_CMP(ymm0, xmm1, xmm0, offset, 0); \
            CMPUNALIGNED_LEFT47_IMM_CMP(ymm1, xmm2, xmm1, offset, 16); \
            src += 32;                                                                \
            dst += 32;                                                                      \
        }                                                                                                   \
        tmp = len;                                                                                          \
        len = ((len - 16 + offset) & 31) + 16 - offset;                                                     \
        tmp -= len;                                                                                         \
        src += tmp;                                                                   \
        dst += tmp;                                                                         \
    }                                                                                                       \
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
 * - __m128i <xmm0> ~ <xmm8> used in CMPUNALIGNED_LEFT47_IMM must be pre-defined
 */
#define CMPUNALIGNED_LEFT47(dst, src, len, offset)                   \
{                                                      \
    switch (offset) {                                                 \
    case 0x01: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x01); break;    \
    case 0x02: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x02); break;    \
    case 0x03: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x03); break;    \
    case 0x04: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x04); break;    \
    case 0x05: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x05); break;    \
    case 0x06: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x06); break;    \
    case 0x07: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x07); break;    \
    case 0x08: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x08); break;    \
    case 0x09: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x09); break;    \
    case 0x0A: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0A); break;    \
    case 0x0B: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0B); break;    \
    case 0x0C: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0C); break;    \
    case 0x0D: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0D); break;    \
    case 0x0E: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0E); break;    \
    case 0x0F: CMPUNALIGNED_LEFT47_IMM(dst, src, n, 0x0F); break;    \
    default:;                                                         \
    }                                                                 \
}

static __rte_always_inline const ng_uint8_t *
rte_memcmp_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m128i mm;
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
  __m128i ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
  const void *ret;
  ng_size_t dstofss;
  ng_size_t srcofs;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) {
    return rte_cmp15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) {
    ret = rte_cmp16(dst, src);
    if (ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_cmp16(dst - 16 + n, src - 16 + n);
  }
  if (n <= 64) {
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    if (n > 48)
    {
      ret = rte_cmp16(dst + 32, src + 32);
      if (ret) return ret;
    }
    return rte_cmp16(dst - 16 + n, src - 16 + n);
  }
  if (n <= 128) {
    goto COPY_BLOCK_128_BACK15;
  }
  if (n <= 512) {
    if (n >= 256) {
      n -= 256;
      ret = rte_cmp128(dst, src);
      if (ret) return ret;
      ret = rte_cmp128(dst + 128, src + 128);
      if (ret) return ret;
      src += 256;
      dst += 256;
    }
COPY_BLOCK_255_BACK15:
    if (n >= 128) {
      n -= 128;
      ret = rte_cmp128(dst, src);
      if (ret) return ret;
      src += 128;
      dst += 128;
    }
COPY_BLOCK_128_BACK15:
    if (n >= 64) {
      n -= 64;
      ret = rte_cmp64(dst, src);
      if (ret) return ret;
      src += 64;
      dst += 64;
    }
COPY_BLOCK_64_BACK15:
    if (n >= 32) {
      n -= 32;
      ret = rte_cmp32(dst, src);
      if (ret) return ret;
      src += 32;
      dst += 32;
    }
    if (n > 16) {
      ret = rte_cmp16(dst, src);
      if (ret) return ret;
      return rte_cmp16(dst - 16 + n, src - 16 + n);
    }
    if (n > 0) {
      return rte_cmp16(dst - 16 + n, src - 16 + n);
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
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    src += dstofss;
    dst += dstofss;
  }
  srcofs = ((ng_uintptr_t)src & 0x0F);

  /**
   * For aligned copy
   */
  if (srcofs == 0) {
    /**
     * Copy 256-byte blocks
     */
    for (; n >= 256; n -= 256) {
      ret = rte_cmp256(dst, src);
      if (ret) return ret;
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
  CMPUNALIGNED_LEFT47(dst, src, n, srcofs);

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_64_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline const ng_uint8_t *
rte_memcmp_aligned(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  const void *ret;

  /* Copy size < 16 bytes */
  if (n < 16) {
    return rte_cmp15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) {
    return rte_cmp32(dst, src);
  }
  if (n <= 32) {
    ret = rte_cmp16(dst, src);
    if (ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_cmp16(dst - 16 + n, src - 16 + n);
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) {
    return rte_cmp64(dst, src);
  }
  if (n <= 64) {
    ret = rte_cmp32(dst, src);
    if (ret) return ret;
    return rte_cmp32(dst - 32 + n, src - 32 + n);
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) {
    ret = rte_cmp64(dst, src);
    if (ret) return ret;
    dst += 64;
    src += 64;
  }

  /* Copy whatever left */
  return rte_cmp64(dst - 64 + n, src - 64 + n);
}
static __rte_always_inline int
rte_memcmp8(const ng_uint8_t *d, 
  const ng_uint8_t *s, const ng_uint8_t *e)
{
  while (d < e)
  {
    ng_size_t n;
    if ((n = *d++ - *s++))
      return n;
  }
  RTE_ASSERT(1);
  return 0;
}
static __rte_always_inline int
rte_memcmp(const void *dst, const void *src, ng_size_t n)
{
  const ng_uint8_t *ret, *d, *s, *e;
  if (!(((ng_uintptr_t)dst | (ng_uintptr_t)src) & ALIGNMENT_MASK))
    ret = rte_memcmp_aligned(dst, src, n);
  else
    ret = rte_memcmp_generic(dst, src, n);

  if (ret == NULL)
    return 0;

  d = ret;
  e = (const ng_uint8_t *)dst + n;
  n = d - (const ng_uint8_t *)dst;
  s = (const ng_uint8_t *)src + n;
  while (e - d >= 8)
  {
    if (*(const ng_uint64_t *)d != *(const ng_uint64_t *)s)
      return rte_memcmp8(d,s,e);
    d += 8;
    s += 8;
  }
  while (e - d >= 4)
  {
    if (*(const ng_uint32_t *)d != *(const ng_uint32_t *)s)
      return rte_memcmp8(d,s,e);
    d += 4;
    s += 4;
  }
  while (e - d >= 2)
  {
    if (*(const ng_uint16_t *)d != *(const ng_uint16_t *)s)
      return rte_memcmp8(d,s,e);
    d += 2;
    s += 2;
  }
  if (e - d == 1)
  {
    n = *d - *s;
    RTE_ASSERT(n==0);
    return n;
  }
    
  RTE_ASSERT(1);
  return 0;
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_memcmp(dst, src, n) rte_memcmp(dst, src, n)

#else

#define ng_memcmp(dst, src, n) memcmp(dst, src, n)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_X86_64_H_ */
