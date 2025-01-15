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
#ifndef __nanohttp_prefetch_h
#define __nanohttp_prefetch_h

/*/////////////////////////////////////////////////////////////////////////////////
                                      aarch64
/////////////////////////////////////////////////////////////////////////////////*/
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
/*/////////////////////////////////////////////////////////////////////////////////
                                      __arm__
/////////////////////////////////////////////////////////////////////////////////*/
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
/*/////////////////////////////////////////////////////////////////////////////////
                                      x86
/////////////////////////////////////////////////////////////////////////////////*/
#elif defined(__i386__) || defined(__x86_64__)
#ifdef RTE_TOOLCHAIN_MSVC
#include <emmintrin.h>
#endif
static inline void rte_prefetch0(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(ng_uintptr_t)p, _MM_HINT_T0);
#else
	asm volatile ("prefetcht0 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch1(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(ng_uintptr_t)p, _MM_HINT_T1);
#else
	asm volatile ("prefetcht1 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch2(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(ng_uintptr_t)p, _MM_HINT_T2);
#else
	asm volatile ("prefetcht2 %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
#ifdef RTE_TOOLCHAIN_MSVC
	_mm_prefetch((const char *)(ng_uintptr_t)p, _MM_HINT_NTA);
#else
	asm volatile ("prefetchnta %[p]" : : [p] "m" (*(const volatile char *)p));
#endif
}
/*/////////////////////////////////////////////////////////////////////////////////
                                      __powerpc64__
/////////////////////////////////////////////////////////////////////////////////*/
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
/*/////////////////////////////////////////////////////////////////////////////////
                                      __riscv__
/////////////////////////////////////////////////////////////////////////////////*/
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
/*/////////////////////////////////////////////////////////////////////////////////
                                      __loongarch__
/////////////////////////////////////////////////////////////////////////////////*/
#elif defined(__loongarch__)
static inline void rte_prefetch0(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 3);
}

static inline void rte_prefetch1(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 2);
}

static inline void rte_prefetch2(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 1);
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
/*/////////////////////////////////////////////////////////////////////////////////
                                      ¿
/////////////////////////////////////////////////////////////////////////////////*/
#else
static inline void rte_prefetch0(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 3);
}

static inline void rte_prefetch1(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 2);
}

static inline void rte_prefetch2(const volatile void *p)
{
	__builtin_prefetch((const void *)(ng_uintptr_t)p, 0, 1);
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}
#endif

#endif
