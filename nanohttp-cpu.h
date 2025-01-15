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
#ifndef __nanohttp_cpu_h
#define __nanohttp_cpu_h

#ifdef __KERNEL__
#include <linux/compiler.h>
#include <linux/atomic.h>
#include <asm/barrier.h>
#define ng_smp_mb() smp_mb()
#define ng_smp_wmb() smp_wmb()
#define ng_smp_rmb() smp_rmb()
#define ng_barrier() barrier()
#else
/* We provide internal macro here to allow conditional expansion
 * in the body of the per-arch rte_atomic_thread_fence inline functions.
 */
#define ng_smp_mb()  __atomic_thread_fence(__ATOMIC_ACQ_REL)
#define ng_smp_wmb() __atomic_thread_fence(__ATOMIC_RELEASE)
#define ng_smp_rmb() __atomic_thread_fence(__ATOMIC_CONSUME)
#define ng_barrier() HTTPD_UNUSED(0)
#endif

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

/* The memory order is an integer type in GCC built-ins,
 * not an enumerated type like in C11.
 */
typedef int rte_memory_order;

#if defined(aarch64)
#define rte_mb() asm volatile("dmb osh" : : : "memory")
#define rte_wmb() asm volatile("dmb oshst" : : : "memory")
#define rte_rmb() asm volatile("dmb oshld" : : : "memory")
#define rte_smp_mb() asm volatile("dmb ish" : : : "memory")
#define rte_smp_wmb() asm volatile("dmb ishst" : : : "memory")
#define rte_smp_rmb() asm volatile("dmb ishld" : : : "memory")
static inline void rte_pause(void)
{
}
#elif defined(__arm__)
#define  rte_mb()  __sync_synchronize()
#define  rte_wmb() do { asm volatile ("dmb st" : : : "memory"); } while (0)
#define  rte_rmb() __sync_synchronize()
#define rte_smp_mb() rte_mb()
#define rte_smp_wmb() rte_wmb()
#define rte_smp_rmb() rte_rmb()
static inline void rte_pause(void)
{
  asm volatile("yield" ::: "memory");
}
#elif defined(__i386__) || defined(__x86_64__)
#include <emmintrin.h>
#define  rte_mb() _mm_mfence()
#define  rte_wmb() _mm_sfence()
#define  rte_rmb() _mm_lfence()
#define rte_smp_wmb() rte_compiler_barrier()
#define rte_smp_rmb() rte_compiler_barrier()
static inline void rte_pause(void)
{
  _mm_pause();
}
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__)
#define  rte_mb()  asm volatile("sync" : : : "memory")
#define  rte_wmb() asm volatile("sync" : : : "memory")
#define  rte_rmb() asm volatile("sync" : : : "memory")
#define rte_smp_mb() rte_mb()
#define rte_smp_wmb() rte_wmb()
#define rte_smp_rmb() rte_rmb()
static inline void rte_pause(void)
{
  /* Set hardware multi-threading low priority */
  asm volatile("or 1,1,1");
  /* Set hardware multi-threading medium priority */
  asm volatile("or 2,2,2");
  rte_compiler_barrier();
}
#elif defined(__riscv__) || defined(__riscv) || defined(__RISCV64__) || defined(__riscv64__)
#define rte_mb()  asm volatile("fence rw, rw" : : : "memory")
#define rte_wmb()  asm volatile("fence w, w" : : : "memory")
#define rte_rmb()  asm volatile("fence r, r" : : : "memory")
#define rte_smp_mb()  rte_mb()
#define rte_smp_wmb()  rte_wmb()
#define rte_smp_rmb()  rte_rmb()
static inline void rte_pause(void)
{
  /* Insert pause hint directly to be compatible with old compilers.
   * This will work even on platforms without Zihintpause extension
   * because this is a FENCE hint instruction which evaluates to NOP.
   */
  asm volatile(".int 0x0100000F" : : : "memory");
}
#elif defined(__loongarch__)
#define rte_mb()  do { asm volatile("dbar 0":::"memory"); } while (0)
#define rte_wmb()  rte_mb()
#define rte_rmb()  rte_mb()
#define rte_smp_mb()  rte_mb()
#define rte_smp_wmb()  rte_mb()
#define rte_smp_rmb()  rte_mb()
static inline void rte_pause(void)
{
}
#else
#define rte_smp_mb()  ng_smp_mb()
#define rte_smp_wmb()  ng_smp_wmb()
#define rte_smp_rmb()  ng_smp_rmb()
static inline void rte_pause(void)
{
}
#endif

#endif
