/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_STRIEQ_X86_64_H_
#define _RTE_STRIEQ_X86_64_H_

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
#define RTE_MESTRIEQ_AVX
#elif defined __AVX__ && !(defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION < 110000))
#define RTE_MESTRIEQ_AVX
#endif

struct __rte_cache_aligned strieq_const{
  __m128i A128;
#ifdef RTE_MESTRIEQ_AVX  
  __m256i A256;
#endif
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  __m512i A512;
#endif
};
extern struct strieq_const strieq_const__C;

struct __rte_cache_aligned mem_const_ff{
  __m128i A128;
#if defined __AVX2__  
  __m256i A256;
#endif
#if defined __AVX512F__
  __m512i A512;
#endif
};
extern struct mem_const_ff mem_const__ff_C;

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
rte_strieq(const void *dst, const void *src, ng_size_t n);

/**
 * strncasecmp - Case insensitive, length-limited string comparison
 * @s1: One string
 * @s2: The other string
 * @len: the maximum number of characters to compare
 *
 * Keep in mind bit different implementation of tolower() in kernel and GLIBC.
 */
/**
 * Copy bytes from one location to another,
 * locations should not overlap.
 * Use with n <= 15.
 */
static __rte_always_inline int
rte_strieq15_or_less(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t len)
{
  /* Yes, Virginia, it had better be unsigned */
  const ng_uint8_t *s1;
  const ng_uint8_t *s2;
  unsigned char c1, c2;
  
  if (!len)
    return 1;
  
  s1 = dst;
  s2 = src;
  
  do 
  {
    c1 = *s1++;
    c2 = *s2++;
    if (!c1 || !c2)
      break;
    if (c1 == c2)
      continue;
    c1 = ng_tolower(c1);
    c2 = ng_tolower(c2);
    if (c1 != c2)
      break;
  } 
  while (--len);
  
  return (int)c1 == (int)c2;
}

/**
 * Copy 16 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq16(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  __m128i xmm0,xmm1,xmm2,xmm3;
  xmm0 = _mm_loadu_si128((const __m128i *)(const void *)src);
  xmm1 = _mm_loadu_si128((const __m128i *)(const void *)dst);
  xmm2 = _mm_xor_si128(xmm0,xmm1);
  xmm3 = _mm_or_si128(xmm2, strieq_const__C.A128);
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  return _mm_cmpeq_epi64_mask(xmm3,strieq_const__C.A128)==0x3;
#else
  __m128i xmm4 = _mm_cmpeq_epi64(xmm3,strieq_const__C.A128);
  return _mm_movemask_epi8(xmm4)==0xffff;
#endif  
}

/**
 * Copy 32 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq32(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined RTE_MESTRIEQ_AVX
  __m256i ymm0,ymm1,ymm2,ymm3;
  ymm0 = _mm256_loadu_si256((const __m256i *)(const void *)src);
  ymm1 = _mm256_loadu_si256((const __m256i *)(const void *)dst);
  ymm2 = _mm256_xor_si256(ymm0,ymm1);
  ymm3 = _mm256_or_si256(ymm2, strieq_const__C.A256);
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  return _mm256_cmpeq_epi64_mask(ymm3,strieq_const__C.A256)==0xf;
#else
  __m256i ymm4 = _mm256_cmpeq_epi64(ymm3,strieq_const__C.A256);
  return _mm256_movemask_epi8(ymm4) == -1;
#endif
#else /* SSE implementation */
  int ret;
  ret = rte_strieq16(dst + 0, src + 0);
  if (!ret) return ret;
  return rte_strieq16(dst + 16, src + 16);
#endif
}

/**
 * Copy 64 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq64(const ng_uint8_t *dst, const ng_uint8_t *src)
{
#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
  __m512i zmm0,zmm1,zmm2,zmm3;

  zmm0 = _mm512_loadu_si512((const void *)src);
  zmm1 = _mm512_loadu_si512((const void *)dst);
  zmm2 = _mm512_xor_si512(zmm0,zmm1);
  zmm3 = _mm512_or_si512(zmm2, strieq_const__C.A512);
  return _mm512_cmpeq_epi64_mask(zmm3, strieq_const__C.A512) == 0xff;
#else /* AVX2, AVX & SSE implementation */
  int ret;
  ret = rte_strieq32(dst + 0 * 32, src + 0 * 32);
  if (!ret) return ret;
  return rte_strieq32(dst + 1 * 32, src + 1 * 32);
#endif
}

/**
 * Copy 128 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq128(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  int ret;
  rte_prefetch0(dst + 64);
  rte_prefetch0(src + 64);
  ret = rte_strieq64(dst + 0 * 64, src + 0 * 64);
  if (!ret) return ret;
  return rte_strieq64(dst + 1 * 64, src + 1 * 64);
}

/**
 * Copy 256 bytes from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq256(const ng_uint8_t *dst, const ng_uint8_t *src)
{
  int ret;
  rte_prefetch0(dst + 128);
  rte_prefetch0(src + 128);
  ret = rte_strieq128(dst + 0 * 128, src + 0 * 128);
  if (!ret) return ret;
  return rte_strieq128(dst + 1 * 128, src + 1 * 128);
}

#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512

/**
 * AVX512 implementation below
 */

#define ALIGNMENT_MASK 0x3F

#define __m512_LOAD_STRIEQ(i) do {                                         \
  zmm0 = _mm512_loadu_si512((const void *)(src + (i << 6)));               \
  zmm1 = _mm512_loadu_si512((const void *)(dst + (i << 6)));               \
  zmm2 = _mm512_xor_si512(zmm0,zmm1);                                      \
  zmm3 = _mm512_or_si512(zmm2, strieq_const__C.A512);                      \
  if (_mm512_cmpeq_epi64_mask(zmm3, strieq_const__C.A512)!=0xff) return 0; \
}while(0)

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq128blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1, zmm2, zmm3;

  while (n >= 128) 
  {
    n -= 128;
    rte_prefetch0(dst + 128);
    rte_prefetch0(src + 128);
    __m512_LOAD_STRIEQ(0);
    __m512_LOAD_STRIEQ(1);
    src += 128;
    dst += 128;
  }

  return 1;
}

/**
 * Copy 512-byte blocks from one location to another,
 * locations should not overlap.
 */
static inline int
rte_strieq512blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m512i zmm0, zmm1, zmm2, zmm3;

  while (n >= 512) 
  {
    n -= 512;
    rte_prefetch0(dst + 512);
    rte_prefetch0(src + 512);
    __m512_LOAD_STRIEQ(0);
    __m512_LOAD_STRIEQ(1);
    __m512_LOAD_STRIEQ(2);
    __m512_LOAD_STRIEQ(3);
    __m512_LOAD_STRIEQ(4);
    __m512_LOAD_STRIEQ(5);
    __m512_LOAD_STRIEQ(6);
    __m512_LOAD_STRIEQ(7);
    src += 512;
    dst += 512;
  }

  return 1;
}

static __rte_always_inline int
rte_strieq_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  int ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_strieq15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    ret = rte_strieq32(dst, src);
    return ret;
  }
  
  if (n <= 32) 
  {
    rte_prefetch0(dst - 16 + n);
    rte_prefetch0(src - 16 + n);
    ret = rte_strieq16(dst, src);
    if (!ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    ret = rte_strieq16(dst - 16 + n, src - 16 + n);
    return ret;
  }
  
  if (__rte_constant(n) && n == 64) 
  {
    ret = rte_strieq64(dst, src);
    return ret;
  }
  
  if (n <= 64) 
  {
    rte_prefetch0(dst - 32 + n);
    rte_prefetch0(src - 32 + n);
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
    ret = rte_strieq32(dst - 32 + n, src - 32 + n);
    return ret;
  }
  
  if (n <= 512) 
  {
    if (n >= 256) 
    {
      n -= 256;
      rte_prefetch0(dst + 256);
      rte_prefetch0(src + 256);
      ret = rte_strieq256(dst, src);
      if (!ret) return ret;
      src += 256;
      dst += 256;
    }
    
    if (n >= 128) 
    {
      n -= 128;
      rte_prefetch0(dst + 128);
      rte_prefetch0(src + 128);
      ret = rte_strieq128(dst, src);
      if (!ret) return ret;
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK63:
    if (n > 64) 
    {
      rte_prefetch0(dst - 64 + n);
      rte_prefetch0(src - 64 + n);
      ret = rte_strieq64(dst, src);
      if (!ret) return ret;
      ret = rte_strieq64(dst - 64 + n, src - 64 + n);
      return ret;
    }
    
    if (n > 0)
    {  
      rte_prefetch0(dst - 64 + n);
      rte_prefetch0(src - 64 + n);
      return rte_strieq64(dst - 64 + n, src - 64 + n);
    }
    
    return 1;
  }

  /**
   * Make store aligned when copy size exceeds 512 bytes
   */
  dstofss = ((ng_uintptr_t)dst & 0x3F);
  if (dstofss > 0) 
  {
    dstofss = 64 - dstofss;
    n -= dstofss;
    rte_prefetch0(dst + dstofss);
    rte_prefetch0(src + dstofss);
    ret = rte_strieq64(dst, src);
    if (!ret) return ret;
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 512-byte blocks.
   * Use copy block function for better instruction order control,
   * which is important when load is unaligned.
   */
  ret = rte_strieq512blocks(dst, src, n);
  if (!ret) return ret;
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
    ret = rte_strieq128blocks(dst, src, n);
    if (!ret) return ret;
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

#elif defined RTE_MESTRIEQ_AVX

/**
 * AVX implementation below
 */

#define ALIGNMENT_MASK 0x1F

#if defined __AVX512F__ && defined RTE_MEMCPY_AVX512
#define __m256_LOAD_STRIEQ(i) do {                                       \
  ymm0 = _mm256_loadu_si256((const void *)(src + (i << 5)));             \
  ymm1 = _mm256_loadu_si256((const void *)(dst + (i << 5)));             \
  ymm2 = _mm256_xor_si256(ymm0,ymm1);                                    \
  ymm3 = _mm256_or_si256(ymm2, strieq_const__C.A256);                    \
  if (_mm256_cmpeq_epi64_mask(ymm3,strieq_const__C.A256)!=0xf) return 0; \
}while(0)
#else
#define __m256_LOAD_STRIEQ(i) do {                                       \
  ymm0 = _mm256_loadu_si256((const void *)(src + (i << 5)));             \
  ymm1 = _mm256_loadu_si256((const void *)(dst + (i << 5)));             \
  ymm2 = _mm256_xor_si256(ymm0,ymm1);                                    \
  ymm3 = _mm256_or_si256(ymm2, strieq_const__C.A256);                    \
  __m256i ymm4 = _mm256_cmpeq_epi64(ymm3, strieq_const__C.A256);         \
  if (_mm256_movemask_epi8(ymm4)!=-1) return 0;                          \
}while(0)
#endif

/**
 * Copy 128-byte blocks from one location to another,
 * locations should not overlap.
 */
static __rte_always_inline int
rte_strieq128blocks(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m256i ymm0, ymm1, ymm2, ymm3;

  while (n >= 128) 
  {
    n -= 128;
    rte_prefetch0(dst + 128);
    rte_prefetch0(src + 128);
    __m256_LOAD_STRIEQ(0);
    __m256_LOAD_STRIEQ(1);
    __m256_LOAD_STRIEQ(2);
    __m256_LOAD_STRIEQ(3);
    src += 128;
    dst += 128;
  }

  return 1;
}

static __rte_always_inline int
rte_strieq_generic(const void *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  int ret;
  ng_size_t dstofss;
  ng_size_t bits;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_strieq15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 256 bytes
   */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_strieq32(dst, src);
  }
  
  if (n <= 32) 
  {
    rte_prefetch0(dst - 16 + n);
    rte_prefetch0(src - 16 + n);
    ret = rte_strieq16(dst, src);
    if (!ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    if (!ret) return ret;
    return rte_strieq16(dst - 16 + n, src - 16 + n);
  }
  
  if (n <= 64) 
  {
    rte_prefetch0(dst - 32 + n);
    rte_prefetch0(src - 32 + n);
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
    ret = rte_strieq32(dst - 32 + n, src - 32 + n);
    return ret;
  }
  
  if (n <= 256) 
  {
    if (n >= 128) 
    {
      n -= 128;
      rte_prefetch0(dst + 128);
      rte_prefetch0(src + 128);
      ret = rte_strieq128(dst, src);
      if (!ret) return ret;
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK31:
    if (n >= 64) 
    {
      rte_prefetch0(dst + 64);
      rte_prefetch0(src + 64);
      n -= 64;
      ret = rte_strieq64(dst, src);
      if (!ret) return ret;
      src += 64;
      dst += 64;
    }
    
    if (n > 32) 
    {
      rte_prefetch0(dst - 32 + n);
      rte_prefetch0(src - 32 + n);
      ret = rte_strieq32(dst, src);
      if (!ret) return ret;
      ret = rte_strieq32(dst - 32 + n, src - 32 + n);
      return ret;
    }
    
    if (n > 0) 
    {
      rte_prefetch0(dst - 32 + n);
      rte_prefetch0(src - 32 + n);
      return rte_strieq32(dst - 32 + n, src - 32 + n);
    }
    
    return 1;
  }

  /**
   * Make store aligned when copy size exceeds 256 bytes
   */
  dstofss = (ng_uintptr_t)dst & 0x1F;
  if (dstofss > 0) 
  {
    dstofss = 32 - dstofss;
    rte_prefetch0(dst + dstofss);
    rte_prefetch0(src + dstofss);
    n -= dstofss;
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
    src += dstofss;
    dst += dstofss;
  }

  /**
   * Copy 128-byte blocks
   */
  ret = rte_strieq128blocks(dst, src, n);
  if (!ret) return ret;
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
#define STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm0,xmm1,xmm0,offset) do {   \
  mm1 = _mm_xor_si128(ymm0,_mm_alignr_epi8(xmm1, xmm0, offset));     \
  mm2 = _mm_or_si128(mm1, strieq_const__C.A128);                     \
  if (_mm_cmpeq_epi64_mask(mm2,strieq_const__C.A128)!=0x3) return 0; \
}while(0)
#elif defined __SSE4_1__
#define STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm0,xmm1,xmm0,offset) do {   \
  mm1 = _mm_xor_si128(ymm0,_mm_alignr_epi8(xmm1, xmm0, offset));     \
  mm2 = _mm_or_si128(mm1, strieq_const__C.A128);                     \
  __m128i mm3 = _mm_cmpeq_epi64(mm2,strieq_const__C.A128);           \
  if (_mm_movemask_epi8(mm3)!=0xffff) return 0;                      \
}while(0)
#else
#define STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm0,xmm1,xmm0,offset) do {   \
  mm1 = _mm_xor_si128(ymm0,_mm_alignr_epi8(xmm1, xmm0, offset));     \
  mm2 = _mm_or_si128(mm1, strieq_const__C.A128);                     \
  __m128i mm3 = _mm_cmpeq_epi32(mm2,strieq_const__C.A128);           \
  if (_mm_movemask_epi8(mm3)!=0xffff) return 0;                      \
}while(0)
#endif
#define STRIEQUNALIGNED_LEFT47_IMM(dst, src, len, offset)                                               \
{                                                                                                       \
  ng_size_t tmp;                                                                                        \
  while (len >= 128 + 16 - offset) {                                                                    \
    len -= 128;                                                                                         \
    rte_prefetch0(dst + 128);                                                                           \
    rte_prefetch0(src + 128);                                                                           \
    xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0));                          \
    xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 16));                         \
    xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 32));                         \
    xmm3 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 48));                         \
    xmm4 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 64));                         \
    xmm5 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 80));                         \
    xmm6 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 96));                         \
    xmm7 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 112));                        \
    xmm8 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 128));                        \
    ymm0 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 0));                                   \
    ymm1 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 16));                                  \
    ymm2 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 32));                                  \
    ymm3 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 48));                                  \
    ymm4 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 64));                                  \
    ymm5 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 80));                                  \
    ymm6 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 96));                                  \
    ymm7 = _mm_loadu_si128((const __m128i *)(const void *)(dst + 112));                                 \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm0, xmm1, xmm0, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm1, xmm2, xmm1, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm2, xmm3, xmm2, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm3, xmm4, xmm3, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm4, xmm5, xmm4, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm5, xmm6, xmm5, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm6, xmm7, xmm6, offset);                                           \
    STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm7, xmm8, xmm7, offset);                                           \
    src += 128;                                                                                         \
    dst += 128;                                                                                         \
  }                                                                                                     \
  tmp = len;                                                                                            \
  len = ((len - 16 + offset) & 127) + 16 - offset;                                                      \
  tmp -= len;                                                                                           \
  src += tmp;                                                                                           \
  dst += tmp;                                                                                           \
  if (len >= 32 + 16 - offset) {                                                                        \
    while (len >= 32 + 16 - offset) {                                                                   \
      len -= 32;                                                                                        \
      rte_prefetch0(dst + 32);                                                                          \
      rte_prefetch0(src + 32);                                                                          \
      xmm0 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 0));                        \
      xmm1 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 16));                       \
      xmm2 = _mm_loadu_si128((const __m128i *)(const void *)(src - offset + 32));                       \
      ymm0 = _mm_loadu_si128((__m128i *)(void *)((ng_uint8_t *)dst + 0));                              \
      ymm1 = _mm_loadu_si128((__m128i *)(void *)((ng_uint8_t *)dst + 16));                             \
      STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm0, xmm1, xmm0, offset);                                         \
      STRIEQUNALIGNED_LEFT47_IMM_XOR(ymm1, xmm2, xmm1, offset);                                         \
      src += 32;                                                                                        \
      dst += 32;                                                                                        \
    }                                                                                                   \
    tmp = len;                                                                                          \
    len = ((len - 16 + offset) & 31) + 16 - offset;                                                     \
    tmp -= len;                                                                                         \
    src += tmp;                                                                                         \
    dst += tmp;                                                                                         \
  }                                                                                                     \
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
 * - __m128i <xmm0> ~ <xmm8> used in STRIEQUNALIGNED_LEFT47_IMM must be pre-defined
 */
#define STRIEQUNALIGNED_LEFT47(dst, src, len, offset)                   \
{                                                                       \
    switch (offset) {                                                   \
    case 0x01: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x01); break;    \
    case 0x02: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x02); break;    \
    case 0x03: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x03); break;    \
    case 0x04: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x04); break;    \
    case 0x05: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x05); break;    \
    case 0x06: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x06); break;    \
    case 0x07: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x07); break;    \
    case 0x08: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x08); break;    \
    case 0x09: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x09); break;    \
    case 0x0A: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0A); break;    \
    case 0x0B: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0B); break;    \
    case 0x0C: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0C); break;    \
    case 0x0D: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0D); break;    \
    case 0x0E: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0E); break;    \
    case 0x0F: STRIEQUNALIGNED_LEFT47_IMM(dst, src, n, 0x0F); break;    \
    default:;                                                           \
    }                                                                   \
}

static __rte_always_inline int
rte_strieq_generic(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  __m128i mm1,mm2;
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
  __m128i ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
  int ret;
  ng_size_t dstofss;
  ng_size_t srcofs;

  /**
   * Copy less than 16 bytes
   */
  if (n < 16) 
  {
    return rte_strieq15_or_less(dst, src, n);
  }

  /**
   * Fast way when copy size doesn't exceed 512 bytes
   */
  if (n <= 32) 
  {
    rte_prefetch0(dst - 16 + n);
    rte_prefetch0(src - 16 + n);
    ret = rte_strieq16(dst, src);
    if (!ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_strieq16(dst - 16 + n, src - 16 + n);
  }
  
  if (n <= 64) 
  {
    rte_prefetch0(dst + 32);
    rte_prefetch0(src + 32);
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
    if (n > 48)
    {  
      rte_prefetch0(dst - 16 + n);
      rte_prefetch0(src - 16 + n);
      ret = rte_strieq16(dst + 32, src + 32);
      if (!ret) return ret;
    }
    return rte_strieq16(dst - 16 + n, src - 16 + n);
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
      ret = rte_strieq128(dst, src);
      if (!ret) return ret;
      ret = rte_strieq128(dst + 128, src + 128);
      if (!ret) return ret;
      src += 256;
      dst += 256;
    }
    
COPY_BLOCK_255_BACK15:
    if (n >= 128) 
    {
      rte_prefetch0(dst + 128);
      rte_prefetch0(src + 128);
      n -= 128;
      ret = rte_strieq128(dst, src);
      if (!ret) return ret;
      src += 128;
      dst += 128;
    }
    
COPY_BLOCK_128_BACK15:
    if (n >= 64) 
    {
      rte_prefetch0(dst + 64);
      rte_prefetch0(src + 64);
      n -= 64;
      ret = rte_strieq64(dst, src);
      if (!ret) return ret;
      src += 64;
      dst += 64;
    }
    
COPY_BLOCK_64_BACK15:
    if (n >= 32) 
    {
      rte_prefetch0(dst + 32);
      rte_prefetch0(src + 32);
      n -= 32;
      ret = rte_strieq32(dst, src);
      if (!ret) return ret;
      src += 32;
      dst += 32;
    }
    
    if (n > 16) 
    {
      ret = rte_strieq16(dst, src);
      if (!ret) return ret;
      return rte_strieq16(dst - 16 + n, src - 16 + n);
    }
    
    if (n > 0) 
    {
      return rte_strieq16(dst - 16 + n, src - 16 + n);
    }
    
    return 1;
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
    rte_prefetch0(dst + dstofss);
    rte_prefetch0(src + dstofss);
    n -= dstofss;
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
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
      rte_prefetch0(dst + 256);
      rte_prefetch0(src + 256);
      ret = rte_strieq256(dst, src);
      if (!ret) return ret;
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
  STRIEQUNALIGNED_LEFT47(dst, src, n, srcofs);

  /**
   * Copy whatever left
   */
  goto COPY_BLOCK_64_BACK15;
}

#endif /* __AVX512F__ */

static __rte_always_inline int
rte_strieq_aligned(const ng_uint8_t *dst, 
  const ng_uint8_t *src, ng_size_t n)
{
  int ret;
  
  /* Copy size < 16 bytes */
  if (n < 16) 
  {
    return rte_strieq15_or_less(dst, src, n);
  }

  /* Copy 16 <= size <= 32 bytes */
  if (__rte_constant(n) && n == 32) 
  {
    return rte_strieq32(dst, src);
  }
  
  if (n <= 32) 
  {
    rte_prefetch0(dst - 16 + n);
    rte_prefetch0(src - 16 + n);
    ret = rte_strieq16(dst, src);
    if (!ret) return ret;
    if (__rte_constant(n) && n == 16)
      return ret; /* avoid (harmless) duplicate copy */
    return rte_strieq16(dst - 16 + n, src - 16 + n);
  }

  /* Copy 32 < size <= 64 bytes */
  if (__rte_constant(n) && n == 64) 
  {
    return rte_strieq64(dst, src);
  }
  
  if (n <= 64) 
  {
    rte_prefetch0(dst - 32 + n);
    rte_prefetch0(src - 32 + n);
    ret = rte_strieq32(dst, src);
    if (!ret) return ret;
    return rte_strieq32(dst - 32 + n, src - 32 + n);
  }

  /* Copy 64 bytes blocks */
  for (; n > 64; n -= 64) 
  {
    rte_prefetch0(dst + 64);
    rte_prefetch0(src + 64);
    ret = rte_strieq64(dst, src);
    if (!ret) return ret;
    dst += 64;
    src += 64;
  }

  /* Copy whatever left */
  return rte_strieq64(dst - 64 + n, src - 64 + n);
}

static __rte_always_inline int
rte_strieq(const void *dst, const void *src, ng_size_t n)
{
  rte_prefetch0(&strieq_const__C);
  if (!(((ng_uintptr_t)dst | (ng_uintptr_t)src) & ALIGNMENT_MASK))
    return rte_strieq_aligned((const ng_uint8_t *)dst, 
              (const ng_uint8_t *)src, n);
  else
    return rte_strieq_generic((const ng_uint8_t *)dst, 
              (const ng_uint8_t *)src, n);
}

#undef ALIGNMENT_MASK

#if defined(RTE_TOOLCHAIN_GCC) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define ng_strnocasecmp(dst, src, n) (!rte_strieq(dst, src, n))

#else

#ifdef WIN32
#define ng_strnocasecmp(dst, src, n) ng_local_strncasecmp(dst, src, n)
#else
#define ng_strnocasecmp(dst, src, n) strncasecmp(dst, src, n)
#endif

#endif

extern int memchr_test(void);
extern int memrchr_test(void);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_STRIEQ_X86_64_H_ */
