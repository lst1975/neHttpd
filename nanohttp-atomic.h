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
 *                         https://github.com/lst1975/ngRTOS
 *                         https://github.com/lst1975/neHttpd
 **************************************************************************************
 */
#ifndef __nanohttp_atomic_h
#define __nanohttp_atomic_h

#ifdef RTE_ENABLE_STDATOMIC
#ifndef _MSC_VER
#ifdef __STDC_NO_ATOMICS__
#error enable_stdatomic=true but atomics not supported by toolchain
#endif
#endif

#include <stdatomic.h>

/* RTE_ATOMIC(type) is provided for use as a type specifier
 * permitting designation of an rte atomic type.
 */
#define RTE_ATOMIC(type) _Atomic(type)

/* __rte_atomic is provided for type qualification permitting
 * designation of an rte atomic qualified type-name.
 */
#define __rte_atomic _Atomic

/* The memory order is an enumerated type in C11. */
typedef memory_order rte_memory_order;

#define rte_memory_order_relaxed memory_order_relaxed
#ifdef __ATOMIC_RELAXED
static_assert(rte_memory_order_relaxed == __ATOMIC_RELAXED,
	"rte_memory_order_relaxed == __ATOMIC_RELAXED");
#endif

#define rte_memory_order_consume memory_order_consume
#ifdef __ATOMIC_CONSUME
static_assert(rte_memory_order_consume == __ATOMIC_CONSUME,
	"rte_memory_order_consume == __ATOMIC_CONSUME");
#endif

#define rte_memory_order_acquire memory_order_acquire
#ifdef __ATOMIC_ACQUIRE
static_assert(rte_memory_order_acquire == __ATOMIC_ACQUIRE,
	"rte_memory_order_acquire == __ATOMIC_ACQUIRE");
#endif

#define rte_memory_order_release memory_order_release
#ifdef __ATOMIC_RELEASE
static_assert(rte_memory_order_release == __ATOMIC_RELEASE,
	"rte_memory_order_release == __ATOMIC_RELEASE");
#endif

#define rte_memory_order_acq_rel memory_order_acq_rel
#ifdef __ATOMIC_ACQ_REL
static_assert(rte_memory_order_acq_rel == __ATOMIC_ACQ_REL,
	"rte_memory_order_acq_rel == __ATOMIC_ACQ_REL");
#endif

#define rte_memory_order_seq_cst memory_order_seq_cst
#ifdef __ATOMIC_SEQ_CST
static_assert(rte_memory_order_seq_cst == __ATOMIC_SEQ_CST,
	"rte_memory_order_seq_cst == __ATOMIC_SEQ_CST");
#endif

#define rte_atomic_load_explicit(ptr, memorder) \
	atomic_load_explicit(ptr, memorder)

#define rte_atomic_store_explicit(ptr, val, memorder) \
	atomic_store_explicit(ptr, val, memorder)

#define rte_atomic_exchange_explicit(ptr, val, memorder) \
	atomic_exchange_explicit(ptr, val, memorder)

#define rte_atomic_compare_exchange_strong_explicit(ptr, expected, desired, \
		succ_memorder, fail_memorder) \
	atomic_compare_exchange_strong_explicit(ptr, expected, desired, \
		succ_memorder, fail_memorder)

#define rte_atomic_compare_exchange_weak_explicit(ptr, expected, desired, \
		succ_memorder, fail_memorder) \
	atomic_compare_exchange_weak_explicit(ptr, expected, desired, \
		succ_memorder, fail_memorder)

#define rte_atomic_fetch_add_explicit(ptr, val, memorder) \
	atomic_fetch_add_explicit(ptr, val, memorder)

#define rte_atomic_fetch_sub_explicit(ptr, val, memorder) \
	atomic_fetch_sub_explicit(ptr, val, memorder)

#define rte_atomic_fetch_and_explicit(ptr, val, memorder) \
	atomic_fetch_and_explicit(ptr, val, memorder)

#define rte_atomic_fetch_xor_explicit(ptr, val, memorder) \
	atomic_fetch_xor_explicit(ptr, val, memorder)

#define rte_atomic_fetch_or_explicit(ptr, val, memorder) \
	atomic_fetch_or_explicit(ptr, val, memorder)

#define rte_atomic_fetch_nand_explicit(ptr, val, memorder) \
	atomic_fetch_nand_explicit(ptr, val, memorder)

#define rte_atomic_flag_test_and_set_explicit(ptr, memorder) \
	atomic_flag_test_and_set_explicit(ptr, memorder)

#define rte_atomic_flag_clear_explicit(ptr, memorder) \
	atomic_flag_clear_explicit(ptr, memorder)

/* We provide internal macro here to allow conditional expansion
 * in the body of the per-arch rte_atomic_thread_fence inline functions.
 */
#define __rte_atomic_thread_fence(memorder) \
	atomic_thread_fence(memorder)

#else /* !RTE_ENABLE_STDATOMIC */

#define RTE_ATOMIC(type) type
  
#define __rte_atomic
  
/* The memory order is an integer type in GCC built-ins,
 * not an enumerated type like in C11.
 */
typedef int rte_memory_order;

#define rte_memory_order_relaxed __ATOMIC_RELAXED
#define rte_memory_order_consume __ATOMIC_CONSUME
#define rte_memory_order_acquire __ATOMIC_ACQUIRE
#define rte_memory_order_release __ATOMIC_RELEASE
#define rte_memory_order_acq_rel __ATOMIC_ACQ_REL
#define rte_memory_order_seq_cst __ATOMIC_SEQ_CST
  
#define rte_atomic_load_explicit(ptr, memorder) \
    __atomic_load_n(ptr, memorder)
  
#define rte_atomic_store_explicit(ptr, val, memorder) \
    __atomic_store_n(ptr, val, memorder)
  
#define rte_atomic_exchange_explicit(ptr, val, memorder) \
    __atomic_exchange_n(ptr, val, memorder)
  
#define rte_atomic_compare_exchange_strong_explicit(ptr, expected, desired, \
      succ_memorder, fail_memorder) \
    __atomic_compare_exchange_n(ptr, expected, desired, 0, \
      succ_memorder, fail_memorder)
  
#define rte_atomic_compare_exchange_weak_explicit(ptr, expected, desired, \
      succ_memorder, fail_memorder) \
    __atomic_compare_exchange_n(ptr, expected, desired, 1, \
      succ_memorder, fail_memorder)
  
#define rte_atomic_fetch_add_explicit(ptr, val, memorder) \
    __atomic_fetch_add(ptr, val, memorder)
  
#define rte_atomic_fetch_sub_explicit(ptr, val, memorder) \
    __atomic_fetch_sub(ptr, val, memorder)
  
#define rte_atomic_fetch_and_explicit(ptr, val, memorder) \
    __atomic_fetch_and(ptr, val, memorder)
  
#define rte_atomic_fetch_xor_explicit(ptr, val, memorder) \
    __atomic_fetch_xor(ptr, val, memorder)
  
#define rte_atomic_fetch_or_explicit(ptr, val, memorder) \
    __atomic_fetch_or(ptr, val, memorder)
  
#define rte_atomic_fetch_nand_explicit(ptr, val, memorder) \
    __atomic_fetch_nand(ptr, val, memorder)
  
#define rte_atomic_flag_test_and_set_explicit(ptr, memorder) \
    __atomic_test_and_set(ptr, memorder)
  
#define rte_atomic_flag_clear_explicit(ptr, memorder) \
    __atomic_clear(ptr, memorder)
  
  /* We provide internal macro here to allow conditional expansion
   * in the body of the per-arch rte_atomic_thread_fence inline functions.
   */
#define __rte_atomic_thread_fence(memorder) \
    __atomic_thread_fence(memorder)

#endif

/*------------------------- 16 bit atomic operations -------------------------*/

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile ng_int16_t cnt; /**< An internal counter value. */
} rte_atomic16_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC16_INIT(val) { (val) }

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 16-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
rte_atomic16_cmpset(volatile ng_uint16_t *dst, ng_uint16_t exp, ng_uint16_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

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
static inline ng_uint16_t
rte_atomic16_exchange(volatile ng_uint16_t *dst, ng_uint16_t val)
{
	return rte_atomic_exchange_explicit(dst, val, rte_memory_order_seq_cst);
}

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic16_init(rte_atomic16_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically read a 16-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline ng_int16_t
rte_atomic16_read(const rte_atomic16_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 16-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
rte_atomic16_set(rte_atomic16_t *v, ng_int16_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 16-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
rte_atomic16_add(rte_atomic16_t *v, ng_int16_t inc)
{
	rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, inc,
		rte_memory_order_seq_cst);
}

/**
 * Atomically subtract a 16-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
rte_atomic16_sub(rte_atomic16_t *v, ng_int16_t dec)
{
	rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, dec,
		rte_memory_order_seq_cst);
}

/**
 * Atomically increment a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic16_inc(rte_atomic16_t *v)
{
	rte_atomic16_add(v, 1);
}

/**
 * Atomically decrement a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic16_dec(rte_atomic16_t *v)
{
	rte_atomic16_sub(v, 1);
}

/**
 * Atomically add a 16-bit value to a counter and return the result.
 *
 * Atomically adds the 16-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline ng_int16_t
rte_atomic16_add_return(rte_atomic16_t *v, ng_int16_t inc)
{
	return rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, inc,
		rte_memory_order_seq_cst) + inc;
}

/**
 * Atomically subtract a 16-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 16-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline ng_int16_t
rte_atomic16_sub_return(rte_atomic16_t *v, ng_int16_t dec)
{
	return rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, dec,
		rte_memory_order_seq_cst) - dec;
}

/**
 * Atomically increment a 16-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the increment operation is 0; false otherwise.
 */
static inline int rte_atomic16_inc_and_test(rte_atomic16_t *v)
{
	return rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, 1,
		rte_memory_order_seq_cst) + 1 == 0;
}

/**
 * Atomically decrement a 16-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the decrement operation is 0; false otherwise.
 */
static inline int rte_atomic16_dec_and_test(rte_atomic16_t *v)
{
	return rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int16_t *)&v->cnt, 1,
		rte_memory_order_seq_cst) - 1 == 0;
}

/**
 * Atomically test and set a 16-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int rte_atomic16_test_and_set(rte_atomic16_t *v)
{
	return rte_atomic16_cmpset((volatile ng_uint16_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 16-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void rte_atomic16_clear(rte_atomic16_t *v)
{
	v->cnt = 0;
}

/*------------------------- 32 bit atomic operations -------------------------*/

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile ng_int32_t cnt; /**< An internal counter value. */
} rte_atomic32_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC32_INIT(val) { (val) }

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic32_init(rte_atomic32_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically read a 32-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline ng_int32_t
rte_atomic32_read(const rte_atomic32_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 32-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
rte_atomic32_set(rte_atomic32_t *v, ng_int32_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 32-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
rte_atomic32_add(rte_atomic32_t *v, ng_int32_t inc)
{
	rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, inc,
		rte_memory_order_seq_cst);
}

/**
 * Atomically subtract a 32-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
rte_atomic32_sub(rte_atomic32_t *v, ng_int32_t dec)
{
	rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, dec,
		rte_memory_order_seq_cst);
}

/**
 * Atomically increment a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic32_inc(rte_atomic32_t *v)
{
	rte_atomic32_add(v, 1);
}

/**
 * Atomically decrement a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
rte_atomic32_dec(rte_atomic32_t *v)
{
	rte_atomic32_sub(v,1);
}

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 32-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
rte_atomic32_cmpset(volatile ng_uint32_t *dst, ng_uint32_t exp, ng_uint32_t src)
{
  return __sync_bool_compare_and_swap(dst, exp, src);
}

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
static inline ng_uint32_t
rte_atomic32_exchange(volatile ng_uint32_t *dst, ng_uint32_t val)
{
	return rte_atomic_exchange_explicit(dst, val, rte_memory_order_seq_cst);
}

/**
 * Atomically add a 32-bit value to a counter and return the result.
 *
 * Atomically adds the 32-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline ng_int32_t
rte_atomic32_add_return(rte_atomic32_t *v, ng_int32_t inc)
{
	return rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, inc,
		rte_memory_order_seq_cst) + inc;
}

/**
 * Atomically subtract a 32-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 32-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline ng_int32_t
rte_atomic32_sub_return(rte_atomic32_t *v, ng_int32_t dec)
{
	return rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, dec,
		rte_memory_order_seq_cst) - dec;
}

/**
 * Atomically increment a 32-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the increment operation is 0; false otherwise.
 */
static inline int rte_atomic32_inc_and_test(rte_atomic32_t *v)
{
	return rte_atomic_fetch_add_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, 1,
		rte_memory_order_seq_cst) + 1 == 0;
}

/**
 * Atomically decrement a 32-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the decrement operation is 0; false otherwise.
 */
static inline int rte_atomic32_dec_and_test(rte_atomic32_t *v)
{
	return rte_atomic_fetch_sub_explicit((volatile __rte_atomic ng_int32_t *)&v->cnt, 1,
		rte_memory_order_seq_cst) - 1 == 0;
}

/**
 * Atomically set a 32-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void rte_atomic32_clear(rte_atomic32_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically test and set a 32-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int rte_atomic32_test_and_set(rte_atomic32_t *v)
{
	return rte_atomic32_cmpset((volatile ng_uint32_t *)&v->cnt, 0, 1);
}

/*------------------------- 64 bit atomic operations -------------------------*/

/**
 * The atomic counter structure.
 */
typedef struct {
  volatile ng_int64_t cnt;  /**< Internal counter value. */
} rte_atomic64_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC64_INIT(val) { (val) }

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
rte_atomic64_cmpset(volatile ng_uint64_t *dst, ng_uint64_t exp, ng_uint64_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

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
static inline ng_uint64_t
rte_atomic64_exchange(volatile ng_uint64_t *dst, ng_uint64_t val)
{
  return rte_atomic_exchange_explicit(dst, val, rte_memory_order_seq_cst);
}

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
  ng_uint64_t tmp;

  while (success == 0) {
    tmp = v->cnt;
    success = rte_atomic64_cmpset(
      (volatile ng_uint64_t *)&v->cnt, tmp, 0);
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
static inline ng_int64_t
rte_atomic64_read(rte_atomic64_t *v)
{
#if NG_IS_64BIT_SYS
  return v->cnt;
#else
  int success = 0;
  ng_uint64_t tmp;

  while (success == 0) {
    tmp = v->cnt;
    /* replace the value by itself */
    success = rte_atomic64_cmpset(
      (volatile ng_uint64_t *)&v->cnt, tmp, tmp);
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
rte_atomic64_set(rte_atomic64_t *v, ng_int64_t new_value)
{
#if NG_IS_64BIT_SYS
  v->cnt = new_value;
#else
  int success = 0;
  ng_uint64_t tmp;

  while (success == 0) {
    tmp = v->cnt;
    success = rte_atomic64_cmpset(
      (volatile ng_uint64_t *)&v->cnt, tmp, new_value);
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
rte_atomic64_add(rte_atomic64_t *v, ng_int64_t inc)
{
  rte_atomic_fetch_add_explicit(
    (volatile __rte_atomic ng_int64_t *)&v->cnt, inc,
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
rte_atomic64_sub(rte_atomic64_t *v, ng_int64_t dec)
{
  rte_atomic_fetch_sub_explicit(
    (volatile __rte_atomic ng_int64_t *)&v->cnt, dec,
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
static inline ng_int64_t
rte_atomic64_add_return(rte_atomic64_t *v, ng_int64_t inc)
{
  return rte_atomic_fetch_add_explicit(
    (volatile __rte_atomic ng_int64_t *)&v->cnt, inc,
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
static inline ng_int64_t
rte_atomic64_sub_return(rte_atomic64_t *v, ng_int64_t dec)
{
  return rte_atomic_fetch_sub_explicit(
    (volatile __rte_atomic ng_int64_t *)&v->cnt, dec,
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
static inline int 
rte_atomic64_inc_and_test(rte_atomic64_t *v)
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
static inline int 
rte_atomic64_dec_and_test(rte_atomic64_t *v)
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
static inline int 
rte_atomic64_test_and_set(rte_atomic64_t *v)
{
  return rte_atomic64_cmpset((volatile ng_uint64_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 64-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void 
rte_atomic64_clear(rte_atomic64_t *v)
{
  rte_atomic64_set(v, 0);
}

#endif
