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
#ifndef __nanohttp_endian_h
#define __nanohttp_endian_h

/*
 * Compile-time endianness detection
 */
#define RTE_BIG_ENDIAN    1
#define RTE_LITTLE_ENDIAN 2
#if defined __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif /* __BYTE_ORDER__ */
#elif defined __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif /* __BYTE_ORDER */
#elif defined __BIG_ENDIAN__
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif defined __LITTLE_ENDIAN__
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#elif defined RTE_TOOLCHAIN_MSVC
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif
#if !defined(RTE_BYTE_ORDER)
#error Unknown endianness.
#endif

#include "nanohttp-types.h"

#if (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64))
/*
 * An architecture-optimized byte swap for a 64-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap64().
 */
/* 64-bit mode */
static inline uint64_t rte_arch_bswap64(uint64_t _x)
{
	uint64_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}
#define RTE_ARCH_BSWAP16 0
#define RTE_ARCH_BSWAP32 0
#define RTE_ARCH_BSWAP64 1
#elif (defined(__i386__) || defined(__i386))
/*
 * An architecture-optimized byte swap for a 16-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap16().
 */
static inline uint16_t rte_arch_bswap16(uint16_t _x)
{
	uint16_t x = _x;
	asm volatile ("xchgb %b[x1],%h[x2]"
		      : [x1] "=Q" (x)
		      : [x2] "0" (x)
		      );
	return x;
}

/*
 * An architecture-optimized byte swap for a 32-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap32().
 */
static inline uint32_t rte_arch_bswap32(uint32_t _x)
{
	uint32_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}
/*
 * An architecture-optimized byte swap for a 64-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap64().
 */
/* Compat./Leg. mode */
static inline uint64_t rte_arch_bswap64(uint64_t x)
{
	uint64_t ret = 0;
	ret |= ((uint64_t)rte_arch_bswap32(x & 0xffffffffUL) << 32);
	ret |= ((uint64_t)rte_arch_bswap32((x >> 32) & 0xffffffffUL));
	return ret;
}
#define RTE_ARCH_BSWAP16 1
#define RTE_ARCH_BSWAP32 1
#define RTE_ARCH_BSWAP64 1
#else
#define RTE_ARCH_BSWAP16 0
#define RTE_ARCH_BSWAP32 0
#define RTE_ARCH_BSWAP64 0
#endif

/*
 * The following types should be used when handling values according to a
 * specific byte ordering, which may differ from that of the host CPU.
 *
 * Libraries, public APIs and applications are encouraged to use them for
 * documentation purposes.
 */
typedef ng_uint16_t rte_be16_t; /**< 16-bit big-endian value. */
typedef ng_uint32_t rte_be32_t; /**< 32-bit big-endian value. */
typedef ng_uint64_t rte_be64_t; /**< 64-bit big-endian value. */
typedef ng_uint16_t rte_le16_t; /**< 16-bit little-endian value. */
typedef ng_uint32_t rte_le32_t; /**< 32-bit little-endian value. */
typedef ng_uint64_t rte_le64_t; /**< 64-bit little-endian value. */

#define RTE_STATIC_BSWAP16(v) \
	((((ng_uint16_t)(v) & UINT16_C(0x00ff)) << 8) | \
	 (((ng_uint16_t)(v) & UINT16_C(0xff00)) >> 8))

#define RTE_STATIC_BSWAP32(v) \
	((((ng_uint32_t)(v) & UINT32_C(0x000000ff)) << 24) | \
	 (((ng_uint32_t)(v) & UINT32_C(0x0000ff00)) <<  8) | \
	 (((ng_uint32_t)(v) & UINT32_C(0x00ff0000)) >>  8) | \
	 (((ng_uint32_t)(v) & UINT32_C(0xff000000)) >> 24))

#define RTE_STATIC_BSWAP64(v) \
	((((ng_uint64_t)(v) & UINT64_C(0x00000000000000ff)) << 56) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x000000000000ff00)) << 40) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x0000000000ff0000)) << 24) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x00000000ff000000)) <<  8) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x000000ff00000000)) >>  8) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x0000ff0000000000)) >> 24) | \
	 (((ng_uint64_t)(v) & UINT64_C(0x00ff000000000000)) >> 40) | \
	 (((ng_uint64_t)(v) & UINT64_C(0xff00000000000000)) >> 56))

/*
 * An internal function to swap bytes in a 16-bit value.
 *
 * It is used by rte_bswap16() when the value is constant. Do not use
 * this function directly; rte_bswap16() is preferred.
 */
static inline ng_uint16_t
rte_constant_bswap16(ng_uint16_t x)
{
#if RTE_ARCH_BSWAP16
  return (ng_uint64_t)rte_arch_bswap16(x);
#else
	return (ng_uint16_t)RTE_STATIC_BSWAP16(x);
#endif
}

/*
 * An internal function to swap bytes in a 32-bit value.
 *
 * It is used by rte_bswap32() when the value is constant. Do not use
 * this function directly; rte_bswap32() is preferred.
 */
static inline ng_uint32_t
rte_constant_bswap32(ng_uint32_t x)
{
#if RTE_ARCH_BSWAP32
  return (ng_uint64_t)rte_arch_bswap32(x);
#else
	return (ng_uint32_t)RTE_STATIC_BSWAP32(x);
#endif
}

/*
 * An internal function to swap bytes of a 64-bit value.
 *
 * It is used by rte_bswap64() when the value is constant. Do not use
 * this function directly; rte_bswap64() is preferred.
 */
static inline ng_uint64_t
rte_constant_bswap64(ng_uint64_t x)
{
#if RTE_ARCH_BSWAP64
  return (ng_uint64_t)rte_arch_bswap64(x);
#else
  return (ng_uint64_t)RTE_STATIC_BSWAP64(x);
#endif
}

/* ARM architecture is bi-endian (both big and little). */
#if RTE_BYTE_ORDER == RTE_LITTLE_ENDIAN

#define rte_cpu_to_le_16(x) (x)
#define rte_cpu_to_le_32(x) (x)
#define rte_cpu_to_le_64(x) (x)

#define rte_cpu_to_be_16(x) rte_constant_bswap16(x)
#define rte_cpu_to_be_32(x) rte_constant_bswap32(x)
#define rte_cpu_to_be_64(x) rte_constant_bswap64(x)

#define rte_le_to_cpu_16(x) (x)
#define rte_le_to_cpu_32(x) (x)
#define rte_le_to_cpu_64(x) (x)

#define rte_be_to_cpu_16(x) rte_constant_bswap16(x)
#define rte_be_to_cpu_32(x) rte_constant_bswap32(x)
#define rte_be_to_cpu_64(x) rte_constant_bswap64(x)

#else /* RTE_BIG_ENDIAN */

#define rte_cpu_to_le_16(x) rte_constant_bswap16(x)
#define rte_cpu_to_le_32(x) rte_constant_bswap32(x)
#define rte_cpu_to_le_64(x) rte_constant_bswap64(x)

#define rte_cpu_to_be_16(x) (x)
#define rte_cpu_to_be_32(x) (x)
#define rte_cpu_to_be_64(x) (x)

#define rte_le_to_cpu_16(x) rte_constant_bswap16(x)
#define rte_le_to_cpu_32(x) rte_constant_bswap32(x)
#define rte_le_to_cpu_64(x) rte_constant_bswap64(x)

#define rte_be_to_cpu_16(x) (x)
#define rte_be_to_cpu_32(x) (x)
#define rte_be_to_cpu_64(x) (x)
#endif

#ifdef RTE_FORCE_INTRINSICS
#if RTE_TOOLCHAIN_MSVC
#define rte_bswap16(x) _byteswap_ushort(x)
#define rte_bswap32(x) _byteswap_ulong(x)
#define rte_bswap64(x) _byteswap_uint64(x)
#else
#define rte_bswap16(x) __builtin_bswap16(x)
#define rte_bswap32(x) __builtin_bswap32(x)
#define rte_bswap64(x) __builtin_bswap64(x)
#endif
#else
#define rte_bswap16(x) rte_constant_bswap16(x)
#define rte_bswap32(x) rte_constant_bswap32(x)
#define rte_bswap64(x) rte_constant_bswap64(x)
#endif

#define ng_ntohs  rte_be_to_cpu_16
#define ng_ntohl  rte_be_to_cpu_32
#define ng_ntohll rte_be_to_cpu_64
#define ng_htons  rte_cpu_to_be_16
#define ng_htonl  rte_cpu_to_be_32
#define ng_htonll rte_cpu_to_be_64

/*
 * These macros are functionally similar to rte_cpu_to_(be|le)(16|32|64)(),
 * they take values in host CPU order and return them converted to the
 * intended endianness.
 *
 * They resolve at compilation time to integer constants which can safely be
 * used with static initializers, since those cannot involve function calls.
 *
 * On the other hand, they are not as optimized as their rte_cpu_to_*()
 * counterparts, therefore applications should refrain from using them on
 * variable values, particularly inside performance-sensitive code.
 */
#if RTE_BYTE_ORDER == RTE_BIG_ENDIAN
#define RTE_BE16(v) (rte_be16_t)(v)
#define RTE_BE32(v) (rte_be32_t)(v)
#define RTE_BE64(v) (rte_be64_t)(v)
#define RTE_LE16(v) (rte_le16_t)(RTE_STATIC_BSWAP16(v))
#define RTE_LE32(v) (rte_le32_t)(RTE_STATIC_BSWAP32(v))
#define RTE_LE64(v) (rte_le64_t)(RTE_STATIC_BSWAP64(v))
#elif RTE_BYTE_ORDER == RTE_LITTLE_ENDIAN
#define RTE_BE16(v) (rte_be16_t)(RTE_STATIC_BSWAP16(v))
#define RTE_BE32(v) (rte_be32_t)(RTE_STATIC_BSWAP32(v))
#define RTE_BE64(v) (rte_be64_t)(RTE_STATIC_BSWAP64(v))
#define RTE_LE16(v) (rte_le16_t)(v)
#define RTE_LE32(v) (rte_le32_t)(v)
#define RTE_LE64(v) (rte_le64_t)(v)
#else
#error Unsupported endianness.
#endif

static __ng_inline__ ng_uint32_t 
ng_hash_string(const char *s, ng_size_t length) 
{
  ng_uint32_t h=0;
  for (int i = 0; i < length; i++) 
  {
    /* h = 31 * h + s[i]; */
    h = ((h << 5) + h) + s[i];
  }
  return h;
}

#define NG_NET_U32_HASH(u32t) \
  ((u32t) ^ ((u32t) >> 7) ^ ((u32t) >> 17))

static __ng_inline__ ng_uint32_t 
ng_hash_ipv4(void *ip) 
{
  ng_uint32_t u = *(ng_uint32_t *)ip;
  return NG_NET_U32_HASH(u);
}
  
static __ng_inline__ ng_uint32_t 
ng_hash_ipv6(void *ip) 
{
  ng_uint32_t *u = (ng_uint32_t *)ip;
  return  NG_NET_U32_HASH(u[0]) 
        ^ NG_NET_U32_HASH(u[1]) 
        ^ NG_NET_U32_HASH(u[2]) 
        ^ NG_NET_U32_HASH(u[3]); 
}

#endif
