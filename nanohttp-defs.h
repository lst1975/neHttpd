/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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

#include <string.h>

#define HTTPD_UNUSED(x) (void)(x)

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

/** Force alignment to cache line. */
#define __rte_cache_aligned __rte_aligned(RTE_CACHE_LINE_SIZE)

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

#define HSERVER_ERROR			            (1100)
#define HSERVER_ERROR_MALLOC		      (HSERVER_ERROR + 1)
#define HSERVER_ERROR_SYSTEM		      (HSERVER_ERROR + 2)
#define HSERVER_ERROR_INVAL 		      (HSERVER_ERROR + 3)
#define HSERVER_ERROR_2SHORT		      (HSERVER_ERROR + 4)

#define RTE_MIN(x,y) ((x)<(y) ? (x) : (y))

#endif
