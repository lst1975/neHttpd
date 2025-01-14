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

#define RTE_FORCE_INTRINSICS 1
#define fallthrough() (void)(0)

#define ng_FALSE 0
#define ng_TRUE  1

#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
#endif

typedef unsigned long ng_ulong_t;
typedef int ng_bool_t;

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
			((type *)((ng_uintptr_t)(ptr) - offsetof(type, member)))
#else
#define container_of(ptr, type, member)	__extension__ ({		\
			const typeof(((type *)0)->member) *_ptr = (ptr); \
			__rte_unused type *_target_ptr =	\
				(type *)(ptr);				\
			(type *)(((ng_uintptr_t)_ptr) - offsetof(type, member)); \
		})
#endif
#endif

#define NG_IS_64BIT_SYS defined(__64BIT__) || defined(_LP64) || defined(__LP64__) || defined(_WIN64) ||\
  defined(__x86_64__) || defined(_M_X64) ||\
  defined(__ia64) || defined(_M_IA64) ||\
  defined(__aarch64__) ||\
  defined(__mips64) ||\
  defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) ||\
  defined(__s390x__) || defined(__RISCV64__) || defined(__riscv64__)
  
#define RTE_MIN(x,y) ((x)<(y) ? (x) : (y))

#define ng_snprintf   __ng_snprintf
#define ng_vsnprintf  __ng_vsnprintf
#define ng_fprintf    __ng_fprintf
#define ng_vfprintf   __ng_vfprintf
#define ng_strstr     strstr

#include "nanohttp-align.h"
#include "nanohttp-endian.h"
#include "nanohttp-atomic.h"
#include "nanohttp-prefetch.h"
#include "nanohttp-utils.h"
#include "nanohttp-lfq.h"
#include "nanohttp-vsprintf.h"

#endif
