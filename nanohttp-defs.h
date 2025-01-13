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
#ifndef __nanohttp_defs_h
#define __nanohttp_defs_h

#include "nanohttp-config.h"

#include <stdint.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdatomic.h>

#include "nanohttp-lfq.h"

#define RTE_FORCE_INTRINSICS 1
#define fallthrough() (void)(0)

#define ng_FALSE 0
#define ng_TRUE  1

#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
#endif

#ifdef WIN32
typedef uint64_t ng_time_t;
#else
typedef uint64_t ng_time_t;
#endif
typedef unsigned long ng_ulong_t;
typedef int ng_bool_t;

#include "nanohttp-vsprintf.h"
#include "nanohttp-utils.h"

#define ng_snprintf   __ng_snprintf
#define ng_vsnprintf  __ng_vsnprintf
#define ng_fprintf    __ng_fprintf
#define ng_vfprintf   __ng_vfprintf
#define ng_strstr     strstr

#ifdef WIN32
#define ng_strnocasecmp ng_local_strncasecmp
#else
#define ng_strnocasecmp strncasecmp
#endif

#define ng_strncpy(s1,s2,l) strncpy(s1,s2,l)
#define ng_strcmp(s1,s2) strcmp(s1,s2)
#define ng_strlen(s) strlen(s)
#define ng_memcpy(d,s,l) memcpy(d,s,l)
#define ng_memcmp(d,s,l) memcmp(d,s,l)
#define ng_memset(d,s,l) memset(d,s,l)
#define ng_memchr(d,c,l) memchr(d,c,l)
#define ng_bzero(s,l) ng_memset(s, 0, l)

/* Workaround for toolchain issues with missing C11 macro in FreeBSD */
#if !defined(static_assert) && !defined(__cplusplus)
#define	static_assert	_Static_assert
#endif

#ifdef WIN32
#define RTE_DEFINE_PER_LCORE(type, name)			\
	__declspec(thread) type per_lcore_##name

#define RTE_DECLARE_PER_LCORE(type, name)			\
	extern __declspec(thread) type per_lcore_##name
#else
/**
 * Macro to define a per lcore variable "var" of type "type", don't
 * use keywords like "static" or "volatile" in type, just prefix the
 * whole macro.
 */
#define RTE_DEFINE_PER_LCORE(type, name)			\
	__thread type per_lcore_##name

/**
 * Macro to declare an extern per lcore variable "var" of type "type"
 */
#define RTE_DECLARE_PER_LCORE(type, name)			\
	extern __thread type per_lcore_##name
#endif

/**
 * Read/write the per-lcore variable value
 */
#define RTE_PER_LCORE(name) (per_lcore_##name)

/**
 * Triggers an error at compilation time if the condition is true.
 *
 * The do { } while(0) exists to workaround a bug in clang (#55821)
 * where it would not handle _Static_assert in a switch case.
 */
#define RTE_BUILD_BUG_ON(condition) do { static_assert(!(condition), #condition); } while (0)

#if defined(_MSC_VER)
#define RTE_TOOLCHAIN_MSVC
#endif

#if defined(__linux__)
#define RTE_ENV_LINUX
#define RTE_EXEC_ENV_LINUX
#endif

#define NG_ITEMS(ar) (sizeof(ar)/sizeof((ar)[0]))
#ifndef RTE_ASSERT
#define RTE_ASSERT assert
#endif
#define NG_ASSERT RTE_ASSERT

typedef int ng_int_t;
typedef unsigned int ng_uint_t;
typedef int int_t;
typedef size_t ng_size_t;

#define rte_memory_order_relaxed __ATOMIC_RELAXED
#define rte_memory_order_consume __ATOMIC_CONSUME
#define rte_memory_order_acquire __ATOMIC_ACQUIRE
#define rte_memory_order_release __ATOMIC_RELEASE
#define rte_memory_order_acq_rel __ATOMIC_ACQ_REL
#define rte_memory_order_seq_cst __ATOMIC_SEQ_CST
#define rte_atomic_thread_fence(x) __atomic_thread_fence(x)

/**
 * Compiler barrier.
 *
 * Guarantees that operation reordering does not occur at compile time
 * for operations directly before and after the barrier.
 */
#if defined(_MSC_VER)
#define rte_compiler_barrier() _ReadWriteBarrier()
#else
#define	rte_compiler_barrier() do {		\
	asm volatile ("" : : : "memory");	\
} while(0)
#endif

/**
 * Force a function to be inlined
 */
#if defined(_MSC_VER)
#define __rte_always_inline inline
#else
#define __rte_always_inline inline __attribute__((always_inline))
#endif
#define __ng_inline__ __rte_always_inline
/**
 * Force a function to be noinlined
 */
#define __rte_noinline __attribute__((noinline))

#define HTTPD_UNUSED(x) (void)(x)
#define NG_UNUSED(x) HTTPD_UNUSED(x)
typedef int ng_result_t;

/**
 * Check if a branch is likely to be taken.
 *
 * This compiler builtin allows the developer to indicate if a branch is
 * likely to be taken. Example:
 *
 *   if (likely(x > 1))
 *      do_stuff();
 */
#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif /* likely */

/**
 * Check if a branch is unlikely to be taken.
 *
 * This compiler builtin allows the developer to indicate if a branch is
 * unlikely to be taken. Example:
 *
 *   if (unlikely(x < 1))
 *      do_stuff();
 */
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif /* unlikely */

#ifndef offsetof
/** Return the offset of a field in a structure. */
#define offsetof(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)
#endif

/**
 * short definition to mark a function parameter unused
 */
#if defined(_MSC_VER)
#define __rte_unused
#else
#define __rte_unused __attribute__((__unused__))
#endif

/**
 * Return pointer to the wrapping struct instance.
 *
 * Example:
 *
 *  struct wrapper {
 *      ...
 *      struct child c;
 *      ...
 *  };
 *
 *  struct child *x = obtain(...);
 *  struct wrapper *w = container_of(x, struct wrapper, c);
 */
#ifndef container_of
#if defined(_MSC_VER)
#define container_of(ptr, type, member) \
			((type *)((uintptr_t)(ptr) - offsetof(type, member)))
#else
#define container_of(ptr, type, member)	__extension__ ({		\
			const typeof(((type *)0)->member) *_ptr = (ptr); \
			__rte_unused type *_target_ptr =	\
				(type *)(ptr);				\
			(type *)(((uintptr_t)_ptr) - offsetof(type, member)); \
		})
#endif
#endif

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no
 * bigger than the first parameter. Second parameter must be a
 * power-of-two value.
 */
#define RTE_ALIGN_FLOOR(val, align) \
  (typeof(val))((val) & (~((typeof(val))((align) - 1))))

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no lower
 * than the first parameter. Second parameter must be a power-of-two
 * value.
 */
#define RTE_ALIGN_CEIL(val, align) \
  RTE_ALIGN_FLOOR(((val) + ((typeof(val)) (align) - 1)), align)

/**
 * Macro to align a value to a given power-of-two. The resultant
 * value will be of the same type as the first parameter, and
 * will be no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 * This function is the same as RTE_ALIGN_CEIL
 */
#define RTE_ALIGN(val, align) RTE_ALIGN_CEIL(val, align)

/**
 * Macro to align a value to the multiple of given value. The resultant
 * value will be of the same type as the first parameter and will be no lower
 * than the first parameter.
 */
#define RTE_ALIGN_MUL_CEIL(v, mul) \
	((((v) + (typeof(v))(mul) - 1) / ((typeof(v))(mul))) * (typeof(v))(mul))

/**
 * Macro to align a value to the multiple of given value. The resultant
 * value will be of the same type as the first parameter and will be no higher
 * than the first parameter.
 */
#define RTE_ALIGN_MUL_FLOOR(v, mul) \
	(((v) / ((typeof(v))(mul))) * (typeof(v))(mul))

/**
 * Macro to align value to the nearest multiple of the given value.
 * The resultant value might be greater than or less than the first parameter
 * whichever difference is the lowest.
 */
#define RTE_ALIGN_MUL_NEAR(v, mul)				\
	__extension__ ({					\
		typeof(v) ceil = RTE_ALIGN_MUL_CEIL(v, mul);	\
		typeof(v) floor = RTE_ALIGN_MUL_FLOOR(v, mul);	\
		(ceil - (v)) > ((v) - floor) ? floor : ceil;	\
	})

#define RTE_CACHE_LINE_SIZE 64

/**
 * Force type alignment
 *
 * This macro should be used when alignment of a struct or union type
 * is required. For toolchain compatibility it should appear between
 * the {struct,union} keyword and tag. e.g.
 *
 *   struct __rte_aligned(8) tag { ... };
 *
 * If alignment of an object/variable is required then this macro should
 * not be used, instead prefer C11 alignas(a).
 */
#if defined(_MSC_VER)
#define __rte_aligned(a) __declspec(align(a))
#else
#define __rte_aligned(a) __attribute__((__aligned__(a)))
#endif
/** Force alignment to cache line. */
#define __rte_cache_aligned __rte_aligned(RTE_CACHE_LINE_SIZE)
#define __ng_cache_aligned __rte_cache_aligned
#define __ng_cache_align_as alignas(RTE_CACHE_LINE_SIZE)

/** Cache line size in terms of log2 */
#if RTE_CACHE_LINE_SIZE == 64
#define RTE_CACHE_LINE_SIZE_LOG2 6
#elif RTE_CACHE_LINE_SIZE == 128
#define RTE_CACHE_LINE_SIZE_LOG2 7
#else
#error "Unsupported cache line size"
#endif

/** Minimum Cache line size. */
#define RTE_CACHE_LINE_MIN_SIZE 64

/** Force minimum cache line alignment. */
#define __rte_cache_min_aligned __rte_aligned(RTE_CACHE_LINE_MIN_SIZE)

#define _RTE_CACHE_GUARD_HELPER2(unique) \
  alignas(RTE_CACHE_LINE_SIZE) \
  char cache_guard_ ## unique[RTE_CACHE_LINE_SIZE * RTE_CACHE_GUARD_LINES]
#define _RTE_CACHE_GUARD_HELPER1(unique) _RTE_CACHE_GUARD_HELPER2(unique)
/**
 * Empty cache lines, to guard against false sharing-like effects
 * on systems with a next-N-lines hardware prefetcher.
 *
 * Use as spacing between data accessed by different lcores,
 * to prevent cache thrashing on hardware with speculative prefetching.
 */
#define RTE_CACHE_GUARD _RTE_CACHE_GUARD_HELPER1(__COUNTER__)
/**
 * Triggers an error at compilation time if the condition is true.
 *
 * The do { } while(0) exists to workaround a bug in clang (#55821)
 * where it would not handle _Static_assert in a switch case.
 */
#define RTE_BUILD_BUG_ON(condition) do { static_assert(!(condition), #condition); } while (0)

/*********** Cache line related macros ********/

/** Cache line mask. */
#define RTE_CACHE_LINE_MASK (RTE_CACHE_LINE_SIZE-1)

/** Return the first cache-aligned value greater or equal to size. */
#define RTE_CACHE_LINE_ROUNDUP(size) RTE_ALIGN_CEIL(size, RTE_CACHE_LINE_SIZE)

#define HSERVER_ERROR			            (1100)
#define HSERVER_ERROR_MALLOC		      (HSERVER_ERROR + 1)
#define HSERVER_ERROR_SYSTEM		      (HSERVER_ERROR + 2)
#define HSERVER_ERROR_INVAL 		      (HSERVER_ERROR + 3)
#define HSERVER_ERROR_2SHORT		      (HSERVER_ERROR + 4)

#define NG_INT16_STR_LEN_MAX   (sizeof("-32767") - 1)
#define NG_INT32_STR_LEN_MAX   (sizeof("-2147483648") - 1)
#define NG_INT64_STR_LEN_MAX   (sizeof("-9223372036854775808") - 1)
#define NG_UINT16_STR_LEN_MAX  (sizeof("65535") - 1)
#define NG_UINT32_STR_LEN_MAX  (sizeof("4294967295") - 1)
#define NG_UINT64_STR_LEN_MAX  (sizeof("18446744073709551615") - 1)
#define NG_INET_ADDRSTRLEN     (sizeof("111.112.113.114") - 1)
#define NG_INET6_ADDRSTRLEN    (sizeof("0001:0002:0003:0004:0005:ffff:111.112.113.114") - 1)

#define NG_IS_64BIT_SYS defined(__64BIT__) || defined(_LP64) || defined(__LP64__) || defined(_WIN64) ||\
  defined(__x86_64__) || defined(_M_X64) ||\
  defined(__ia64) || defined(_M_IA64) ||\
  defined(__aarch64__) ||\
  defined(__mips64) ||\
  defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) ||\
  defined(__s390x__) || defined(__RISCV64__) || defined(__riscv64__)

#if NG_IS_64BIT_SYS
#define NG_SIZET_STR_LEN_MAX NG_UINT64_STR_LEN_MAX
#define __NG_BITS_PER_LONG 64
#else
#define NG_SIZET_STR_LEN_MAX NG_UINT32_STR_LEN_MAX
#define __NG_BITS_PER_LONG 32
#endif
//--------------------- wordsize ----------------------------------------------
#ifndef __WORDSIZE
#define __WORDSIZE __NG_BITS_PER_LONG
#endif

/* -1.79769313486231570E+308 -4.94065645841246544E-324 1.79769313486231570E+308 */
#define NG_DOUBLE_STR_LEN_MAX     (sizeof("-1.79769313486231570E+308") - 1)
/* 1.175494351e-38F 3.402823466e+038F */
#define NG_FLOAT_STR_LEN_MAX      (sizeof("3.402823466e+038") - 1)

#define RTE_MIN(x,y) ((x)<(y) ? (x) : (y))

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

/*
 * The following types should be used when handling values according to a
 * specific byte ordering, which may differ from that of the host CPU.
 *
 * Libraries, public APIs and applications are encouraged to use them for
 * documentation purposes.
 */
typedef uint16_t rte_be16_t; /**< 16-bit big-endian value. */
typedef uint32_t rte_be32_t; /**< 32-bit big-endian value. */
typedef uint64_t rte_be64_t; /**< 64-bit big-endian value. */
typedef uint16_t rte_le16_t; /**< 16-bit little-endian value. */
typedef uint32_t rte_le32_t; /**< 32-bit little-endian value. */
typedef uint64_t rte_le64_t; /**< 64-bit little-endian value. */

#ifndef UINT64_C
#define UINT64_C(c) c ## ull
#endif
#ifndef UINT32_C
#define UINT32_C(c) c ## u
#endif
#ifndef UINT16_C
#define UINT16_C(c) c
#endif

#define RTE_STATIC_BSWAP16(v) \
	((((uint16_t)(v) & UINT16_C(0x00ff)) << 8) | \
	 (((uint16_t)(v) & UINT16_C(0xff00)) >> 8))

#define RTE_STATIC_BSWAP32(v) \
	((((uint32_t)(v) & UINT32_C(0x000000ff)) << 24) | \
	 (((uint32_t)(v) & UINT32_C(0x0000ff00)) <<  8) | \
	 (((uint32_t)(v) & UINT32_C(0x00ff0000)) >>  8) | \
	 (((uint32_t)(v) & UINT32_C(0xff000000)) >> 24))

#define RTE_STATIC_BSWAP64(v) \
	((((uint64_t)(v) & UINT64_C(0x00000000000000ff)) << 56) | \
	 (((uint64_t)(v) & UINT64_C(0x000000000000ff00)) << 40) | \
	 (((uint64_t)(v) & UINT64_C(0x0000000000ff0000)) << 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00000000ff000000)) <<  8) | \
	 (((uint64_t)(v) & UINT64_C(0x000000ff00000000)) >>  8) | \
	 (((uint64_t)(v) & UINT64_C(0x0000ff0000000000)) >> 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00ff000000000000)) >> 40) | \
	 (((uint64_t)(v) & UINT64_C(0xff00000000000000)) >> 56))

/*
 * An internal function to swap bytes in a 16-bit value.
 *
 * It is used by rte_bswap16() when the value is constant. Do not use
 * this function directly; rte_bswap16() is preferred.
 */
static inline uint16_t
rte_constant_bswap16(uint16_t x)
{
	return (uint16_t)RTE_STATIC_BSWAP16(x);
}

/*
 * An internal function to swap bytes in a 32-bit value.
 *
 * It is used by rte_bswap32() when the value is constant. Do not use
 * this function directly; rte_bswap32() is preferred.
 */
static inline uint32_t
rte_constant_bswap32(uint32_t x)
{
	return (uint32_t)RTE_STATIC_BSWAP32(x);
}

/*
 * An internal function to swap bytes of a 64-bit value.
 *
 * It is used by rte_bswap64() when the value is constant. Do not use
 * this function directly; rte_bswap64() is preferred.
 */
static inline uint64_t
rte_constant_bswap64(uint64_t x)
{
	return (uint64_t)RTE_STATIC_BSWAP64(x);
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
#if defined(_MSC_VER)
#define rte_bswap16(x) _byteswap_ushort(x)

#define rte_bswap32(x) _byteswap_ulong(x)

#define rte_bswap64(x) _byteswap_uint64(x)
#else
#define rte_bswap16(x) __builtin_bswap16(x)

#define rte_bswap32(x) __builtin_bswap32(x)

#define rte_bswap64(x) __builtin_bswap64(x)
#endif
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

static __ng_inline__ uint32_t 
ng_hash_string(const char *s, size_t length) 
{
  uint32_t h=0;
  for (int i = 0; i < length; i++) 
  {
    /* h = 31 * h + s[i]; */
    h = ((h << 5) + h) + s[i];
  }
  return h;
}

#define NG_NET_U32_HASH(u32t) \
  ((u32t) ^ ((u32t) >> 7) ^ ((u32t) >> 17))

static __ng_inline__ uint32_t 
ng_hash_ipv4(void *ip) 
{
  uint32_t u = *(uint32_t *)ip;
  return NG_NET_U32_HASH(u);
}
  
static __ng_inline__ uint32_t 
ng_hash_ipv6(void *ip) 
{
  uint32_t *u = (uint32_t *)ip;
  return  NG_NET_U32_HASH(u[0]) 
        ^ NG_NET_U32_HASH(u[1]) 
        ^ NG_NET_U32_HASH(u[2]) 
        ^ NG_NET_U32_HASH(u[3]); 
}

/* RTE_ATOMIC(type) is provided for use as a type specifier
 * permitting designation of an rte atomic type.
 */
#define std_RTE_ATOMIC(type) _Atomic(type)

/* __std_rte_atomic is provided for type qualification permitting
 * designation of an rte atomic qualified type-name.
 */
#define __std_rte_atomic _Atomic

/**
 * An atomic compare and set function used by the mutex functions.
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 64-bit words)
 *
 * @param dst
 *   The destination into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
rte_atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

#define rte_atomic_load_explicit(ptr, memorder) \
  __atomic_load_n(ptr, memorder)

#define rte_atomic_store_explicit(ptr, val, memorder) \
  __atomic_store_n(ptr, val, memorder)
  
#define rte_atomic_compare_exchange_strong_explicit(ptr, expected, desired, \
    succ_memorder, fail_memorder) \
  __atomic_compare_exchange_n(ptr, expected, desired, 0, \
    succ_memorder, fail_memorder)

#define rte_atomic_fetch_sub_explicit(ptr, val, memorder) \
  __atomic_fetch_sub(ptr, val, memorder)
  
#define rte_atomic_fetch_add_explicit(ptr, val, memorder) \
  __atomic_fetch_add(ptr, val, memorder)
    
#define rte_atomic_exchange_explicit(ptr, val, memorder) \
	atomic_exchange_explicit(ptr, val, memorder)

/**
 * Atomic exchange.
 *
 * (atomic) equivalent to:
 *   ret = *dst
 *   *dst = val;
 *   return ret;
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param val
 *   The new value.
 * @return
 *   The original value at that location
 */
static inline uint64_t
rte_atomic64_exchange(volatile uint64_t *dst, uint64_t val)
{
	return rte_atomic_exchange_explicit(dst, val, rte_memory_order_seq_cst);
}

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int64_t cnt;  /**< Internal counter value. */
} rte_atomic64_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC64_INIT(val) { (val) }

/**
 * Initialize the atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic64_init(rte_atomic64_t *v)
{
#if NG_IS_64BIT_SYS
	v->cnt = 0;
#else
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		success = rte_atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, 0);
	}
#endif
}

/**
 * Atomically read a 64-bit counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int64_t
rte_atomic64_read(rte_atomic64_t *v)
{
#if NG_IS_64BIT_SYS
	return v->cnt;
#else
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		/* replace the value by itself */
		success = rte_atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, tmp);
	}
	return tmp;
#endif
}

/**
 * Atomically set a 64-bit counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value of the counter.
 */
static inline void
rte_atomic64_set(rte_atomic64_t *v, int64_t new_value)
{
#if NG_IS_64BIT_SYS
	v->cnt = new_value;
#else
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		success = rte_atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, new_value);
	}
#endif
}

/**
 * Atomically add a 64-bit value to a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
rte_atomic64_add(rte_atomic64_t *v, int64_t inc)
{
	rte_atomic_fetch_add_explicit((volatile __std_rte_atomic int64_t *)&v->cnt, inc,
		rte_memory_order_seq_cst);
}

/**
 * Atomically subtract a 64-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
rte_atomic64_sub(rte_atomic64_t *v, int64_t dec)
{
	rte_atomic_fetch_sub_explicit((volatile __std_rte_atomic int64_t *)&v->cnt, dec,
		rte_memory_order_seq_cst);
}

/**
 * Atomically increment a 64-bit counter by one and test.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic64_inc(rte_atomic64_t *v)
{
	rte_atomic64_add(v, 1);
}

/**
 * Atomically decrement a 64-bit counter by one and test.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic64_dec(rte_atomic64_t *v)
{
	rte_atomic64_sub(v, 1);
}

/**
 * Add a 64-bit value to an atomic counter and return the result.
 *
 * Atomically adds the 64-bit value (inc) to the atomic counter (v) and
 * returns the value of v after the addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int64_t
rte_atomic64_add_return(rte_atomic64_t *v, int64_t inc)
{
	return rte_atomic_fetch_add_explicit((volatile __std_rte_atomic int64_t *)&v->cnt, inc,
		rte_memory_order_seq_cst) + inc;
}

/**
 * Subtract a 64-bit value from an atomic counter and return the result.
 *
 * Atomically subtracts the 64-bit value (dec) from the atomic counter (v)
 * and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int64_t
rte_atomic64_sub_return(rte_atomic64_t *v, int64_t dec)
{
	return rte_atomic_fetch_sub_explicit((volatile __std_rte_atomic int64_t *)&v->cnt, dec,
		rte_memory_order_seq_cst) - dec;
}

/**
 * Atomically increment a 64-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns
 * true if the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the addition is 0; false otherwise.
 */
static inline int rte_atomic64_inc_and_test(rte_atomic64_t *v)
{
	return rte_atomic64_add_return(v, 1) == 0;
}

/**
 * Atomically decrement a 64-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after subtraction is 0; false otherwise.
 */
static inline int rte_atomic64_dec_and_test(rte_atomic64_t *v)
{
	return rte_atomic64_sub_return(v, 1) == 0;
}

/**
 * Atomically test and set a 64-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int rte_atomic64_test_and_set(rte_atomic64_t *v)
{
	return rte_atomic64_cmpset((volatile uint64_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 64-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void rte_atomic64_clear(rte_atomic64_t *v)
{
	rte_atomic64_set(v, 0);
}

#if defined(aarch64)
static inline void rte_prefetch0(const volatile void *p)
{
	asm volatile ("PRFM PLDL1KEEP, [%0]" : : "r" (p));
}

static inline void rte_prefetch1(const volatile void *p)
{
	asm volatile ("PRFM PLDL2KEEP, [%0]" : : "r" (p));
}

static inline void rte_prefetch2(const volatile void *p)
{
	asm volatile ("PRFM PLDL3KEEP, [%0]" : : "r" (p));
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	asm volatile ("PRFM PLDL1STRM, [%0]" : : "r" (p));
}
#elif defined(__arm__)
static inline void rte_prefetch0(const volatile void *p)
{
	asm volatile ("pld [%0]" : : "r" (p));
}

static inline void rte_prefetch1(const volatile void *p)
{
	asm volatile ("pld [%0]" : : "r" (p));
}

static inline void rte_prefetch2(const volatile void *p)
{
	asm volatile ("pld [%0]" : : "r" (p));
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#elif defined(__i386__) || defined(__x86_64__)
#ifdef RTE_TOOLCHAIN_MSVC
#include <emmintrin.h>
#endif
static inline void rte_prefetch0(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(uintptr_t)p, _MM_HINT_T0);
#else
	asm volatile ("prefetcht0 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch1(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(uintptr_t)p, _MM_HINT_T1);
#else
	asm volatile ("prefetcht1 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch2(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(uintptr_t)p, _MM_HINT_T2);
#else
	asm volatile ("prefetcht2 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(uintptr_t)p, _MM_HINT_NTA);
#else
	asm volatile ("prefetchnta %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__)
static inline void rte_prefetch0(const volatile void *p)
{
	asm volatile ("dcbt 0,%[p],0" : : [p] "r" (p));
}

static inline void rte_prefetch1(const volatile void *p)
{
	asm volatile ("dcbt 0,%[p],0" : : [p] "r" (p));
}

static inline void rte_prefetch2(const volatile void *p)
{
	asm volatile ("dcbt 0,%[p],0" : : [p] "r" (p));
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#elif defined(__riscv__) || defined(__riscv) || defined(__RISCV64__) || defined(__riscv64__)
static inline void rte_prefetch0(const volatile void *p)
{
	RTE_SET_USED(p);
}

static inline void rte_prefetch1(const volatile void *p)
{
	RTE_SET_USED(p);
}

static inline void rte_prefetch2(const volatile void *p)
{
	RTE_SET_USED(p);
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#elif defined(__loongarch__)
static inline void rte_prefetch0(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 3);
}

static inline void rte_prefetch1(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 2);
}

static inline void rte_prefetch2(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 1);
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#else
static inline void rte_prefetch0(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 3);
}

static inline void rte_prefetch1(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 2);
}

static inline void rte_prefetch2(const volatile void *p)
{
	__builtin_prefetch((const void *)(uintptr_t)p, 0, 1);
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#endif

#endif
