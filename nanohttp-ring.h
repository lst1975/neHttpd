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
#ifndef __nanohttp_ring_h
#define __nanohttp_ring_h

#include "nanohttp-config.h"
#include "nanohttp-defs.h"
#include "nanohttp-logging.h"

#ifdef __KERNEL__
#define __LN "\n"
#define NG_RING_DEBUG(x,...) kprintf(x,__VA_ARGS__)
#else
#define __LN ""
#define NG_RING_DEBUG(x,...) log_debug(x,__VA_ARGS__)
#endif

#if __NG_RING_DEBUG
#define NG_RING_DEBUG_RING(x,...) NG_RING_DEBUG(x,__VA_ARGS__)
#else
#define NG_RING_DEBUG_RING(x,...) HTTPD_UNUSED(x)
#endif

#ifdef __KERNEL__
#include <linux/compiler.h>
#include <linux/atomic.h>
#include <asm/barrier.h>
//--------------------- wordsize ----------------------------------------------
#define ng_srw_malloc(x) kmalloc(x,GFP_KERNEL)
#define ng_srw_free(x) kfree(x)
#define ng_atomic_read(x) atomic_read(x) 
#define ng_atomic_inc(x)  atomic_inc(x) 
#define ng_atomic_dec(x)  atomic_dec(x) 
#define ng_atomic_set(x,n)  atomic_set(x,n) 
typedef u32 NG_U32;
#define ng_assert(x) BUG_ON(!(x))
#define RTE_ATOMIC(t) atomic_t
#else
#define ng_assert(x) NG_ASSERT(x)
#define ng_srw_malloc(x) ng_malloc(x)
#define ng_srw_free(x) ng_free(x)
typedef ng_uint32_t NG_U32;
/* We provide internal macro here to allow conditional expansion
 * in the body of the per-arch rte_atomic_thread_fence inline functions.
 */
#define ng_atomic_inc(x)    __atomic_add_fetch(&(x), 1, __ATOMIC_RELAXED)
#define ng_atomic_dec(x)    __atomic_add_fetch(&(x), -1, __ATOMIC_RELAXED)
#define ng_atomic_read(x)   __atomic_load_n(&(x), __ATOMIC_RELAXED)
#define ng_atomic_set(x,n)  __atomic_store_n(&(x), n, __ATOMIC_RELAXED)
#endif

#define NG_ATOMIC_T(t) RTE_ATOMIC(t)

struct ng_singleRW_ring
{
  void **array;
  NG_ATOMIC_T(NG_U32) __N;
  volatile NG_U32 __R;
  volatile NG_U32 __W;
  NG_U32 size;
  NG_U32 mask;
};
typedef struct ng_singleRW_ring ng_singlerw_ring_s;

int ng_singlerw_ring_init(ng_singlerw_ring_s *ring, NG_U32 size);
void ng_singlerw_ring_free(ng_singlerw_ring_s *ring);

static inline void *
ng_singlerw_ring_get(ng_singlerw_ring_s *ring)
{
  void *elem;
  int count = ng_atomic_read(ring->__N);
  if (count == 0)
    return NULL;

  ng_smp_mb();
  elem = ring->array[ring->__R & ring->mask];
  ring->__R++;
  NG_RING_DEBUG_RING("  R:%u"__LN,  ring->__R & ring->mask);
  ng_smp_mb();

  ng_atomic_dec(ring->__N);
  NG_RING_DEBUG_RING("  count:%u"__LN,  ng_atomic_read(ring->__N));
  return elem;
};

static inline int 
ng_singlerw_ring_set(ng_singlerw_ring_s *ring, void *elem)
{
  int count = ng_atomic_read(ring->__N);
  if (count > ring->mask)
    return -1;
  
  ng_smp_mb();
  ring->array[ring->__W & ring->mask] = elem;
  ring->__W++;
  NG_RING_DEBUG_RING("  W:%u"__LN,  ring->__W & ring->mask);
  ng_smp_mb();

  ng_atomic_inc(ring->__N);
  NG_RING_DEBUG_RING("  count:%u"__LN,  ng_atomic_read(ring->__N));
  return 0;
};

#if __NHTTP_NO_LOGGING
#define ng_singlerw_ring_dump(r) NG_UNUSED(r)
#else
static inline void 
ng_singlerw_ring_dump(ng_singlerw_ring_s *ring)
{
  int count = ng_atomic_read(ring->__N);
  NG_RING_DEBUG("%s","NG-RING: 1 consumers/1 producers."__LN);
  NG_RING_DEBUG("  count:%u"__LN,      count & ring->mask);
  NG_RING_DEBUG("   size:%u"__LN,  ring->size);
  NG_RING_DEBUG("      R:%u"__LN,  ring->__R & ring->mask);
  NG_RING_DEBUG("      W:%u"__LN,  ring->__W & ring->mask);
  return;
};
#endif

#define rte_set_errno(n) HTTPD_UNUSED(0)
#define RTE_SET_USED(x) HTTPD_UNUSED(x)
#define __rte_always_inline inline __attribute__((always_inline))
#define RTE_MEMZONE_NAMESIZE 64
#define RTE_TAILQ_RING_NAME "RTE_RING"
#define __rte_aligned(a) __attribute__((__aligned__(a)))
typedef ng_uint64_t unaligned_uint64_t __rte_aligned(1);
#define RTE_CACHE_GUARD_LINES 1

static __rte_always_inline void
rte_wait_until_equal_32(volatile ng_uint32_t *addr, ng_uint32_t expected,
    rte_memory_order memorder)
{
  ng_assert(memorder == rte_memory_order_acquire || memorder == rte_memory_order_relaxed);

  while (rte_atomic_load_explicit((volatile __rte_atomic ng_uint32_t *)addr, memorder)
      != expected)
    rte_pause();
}

/** enqueue/dequeue behavior types */
enum rte_ring_queue_behavior {
  /** Enq/Deq a fixed number of items from a ring */
  RTE_RING_QUEUE_FIXED = 0,
  /** Enq/Deq as many items as possible from ring */
  RTE_RING_QUEUE_VARIABLE
};

#define RTE_RING_MZ_PREFIX "RG_"
/** The maximum length of a ring name. */
#define RTE_RING_NAMESIZE (RTE_MEMZONE_NAMESIZE - \
         sizeof(RTE_RING_MZ_PREFIX) + 1)

/** prod/cons sync types */
enum rte_ring_sync_type {
  RTE_RING_SYNC_MT,     /**< multi-thread safe (default mode) */
  RTE_RING_SYNC_ST,     /**< single thread only */
};

/**
 * structures to hold a pair of head/tail values and other metadata.
 * Depending on sync_type format of that structure might be different,
 * but offset for *sync_type* and *tail* values should remain the same.
 */
struct rte_ring_headtail {
  volatile RTE_ATOMIC(ng_uint32_t) head;      /**< prod/consumer head. */
  volatile RTE_ATOMIC(ng_uint32_t) tail;      /**< prod/consumer tail. */
  union {
    /** sync type of prod/cons */
    enum rte_ring_sync_type sync_type;
    /** deprecated -  True if single prod/cons */
    ng_uint32_t single;
  };
};

/**
 * An RTE ring structure.
 *
 * The producer and the consumer have a head and a tail index. The particularity
 * of these index is that they are not between 0 and size(ring)-1. These indexes
 * are between 0 and 2^32 -1, and we mask their value when we access the ring[]
 * field. Thanks to this assumption, we can do subtractions between 2 index
 * values in a modulo-32bit base: that's why the overflow of the indexes is not
 * a problem.
 */
struct _rte_ring {
  alignas(RTE_CACHE_LINE_SIZE) char name[RTE_RING_NAMESIZE];
  /**< Name of the ring. */
  int flags;               /**< Flags supplied at creation. */
  const void *memzone;
      /**< Memzone, if any, containing the rte_ring */
  ng_uint32_t size;           /**< Size of ring. */
  ng_uint32_t mask;           /**< Mask (size-1) of ring. */
  ng_uint32_t capacity;       /**< Usable size of ring */

  RTE_CACHE_GUARD;

  /** Ring producer status. */
  struct rte_ring_headtail prod;

  RTE_CACHE_GUARD;

  /** Ring consumer status. */
  struct rte_ring_headtail cons;

  RTE_CACHE_GUARD;
};

typedef struct _rte_ring ng_ring_s;

#define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
#define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */
/**
 * Ring is to hold exactly requested number of entries.
 * Without this flag set, the ring size requested must be a power of 2, and the
 * usable space will be that size - 1. With the flag, the requested size will
 * be rounded up to the next power of two, but the usable space will be exactly
 * that requested. Worst case, if a power-of-2 size is requested, half the
 * ring space will be wasted.
 */
#define RING_F_EXACT_SZ 0x0004
#define RTE_RING_SZ_MASK  (0x7fffffffU) /**< Ring size mask */

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wstringop-overread"
#endif
#endif

/**
 * 128-bit integer structure.
 */
struct __rte_aligned(16) _rte_int128{
  union {
    ng_uint64_t val[2];
#if defined(__LP64__) || defined(_LP64)
    __extension__ __int128 int128;
#endif
  };
};
typedef struct _rte_int128 rte_int128_t;

static __rte_always_inline void
__rte_ring_enqueue_elems_32(ng_ring_s *r, const ng_uint32_t size,
    ng_uint32_t idx, const void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  ng_uint32_t *ring = (ng_uint32_t *)&r[1];
  const ng_uint32_t *obj = (const ng_uint32_t *)obj_table;
  if (likely(idx + n <= size)) {
    for (i = 0; i < (n & ~0x7); i += 8, idx += 8) {
      ring[idx] = obj[i];
      ring[idx + 1] = obj[i + 1];
      ring[idx + 2] = obj[i + 2];
      ring[idx + 3] = obj[i + 3];
      ring[idx + 4] = obj[i + 4];
      ring[idx + 5] = obj[i + 5];
      ring[idx + 6] = obj[i + 6];
      ring[idx + 7] = obj[i + 7];
    }
    switch (n & 0x7) {
    case 7:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 6:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 5:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 4:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 3:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 2:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 1:
      ring[idx++] = obj[i++]; /* fallthrough */
    }
  } else {
    for (i = 0; idx < size; i++, idx++)
      ring[idx] = obj[i];
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      ring[idx] = obj[i];
  }
}

static __rte_always_inline void
__rte_ring_enqueue_elems_64(ng_ring_s *r, ng_uint32_t prod_head,
    const void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  const ng_uint32_t size = r->size;
  ng_uint32_t idx = prod_head & r->mask;
  ng_uint64_t *ring = (ng_uint64_t *)&r[1];
  const unaligned_uint64_t *obj = (const unaligned_uint64_t *)obj_table;
  if (likely(idx + n <= size)) {
    for (i = 0; i < (n & ~0x3); i += 4, idx += 4) {
      ring[idx] = obj[i];
      ring[idx + 1] = obj[i + 1];
      ring[idx + 2] = obj[i + 2];
      ring[idx + 3] = obj[i + 3];
    }
    switch (n & 0x3) {
    case 3:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 2:
      ring[idx++] = obj[i++]; /* fallthrough */
    case 1:
      ring[idx++] = obj[i++];
    }
  } else {
    for (i = 0; idx < size; i++, idx++)
      ring[idx] = obj[i];
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      ring[idx] = obj[i];
  }
}

static __rte_always_inline void
__rte_ring_enqueue_elems_128(ng_ring_s *r, ng_uint32_t prod_head,
    const void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  const ng_uint32_t size = r->size;
  ng_uint32_t idx = prod_head & r->mask;
  rte_int128_t *ring = (rte_int128_t *)&r[1];
  const rte_int128_t *obj = (const rte_int128_t *)obj_table;
  if (likely(idx + n <= size)) 
  {
    for (i = 0; i < (n & ~0x1); i += 2, idx += 2)
      ng_memcpy((void *)(ring + idx),
        (const void *)(obj + i), 32);
    switch (n & 0x1) {
    case 1:
      ng_memcpy((void *)(ring + idx),
        (const void *)(obj + i), 16);
    }
  } 
  else 
  {
    for (i = 0; idx < size; i++, idx++)
      ng_memcpy((void *)(ring + idx),
        (const void *)(obj + i), 16);
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      ng_memcpy((void *)(ring + idx),
        (const void *)(obj + i), 16);
  }
}

/* the actual enqueue of elements on the ring.
 * Placed here since identical code needed in both
 * single and multi producer enqueue functions.
 */
static __rte_always_inline void
__rte_ring_enqueue_elems(ng_ring_s *r, ng_uint32_t prod_head,
    const void *obj_table, ng_uint32_t esize, ng_uint32_t num)
{
  /* 8B and 16B copies implemented individually to retain
   * the current performance.
   */
  if (esize == 8)
    __rte_ring_enqueue_elems_64(r, prod_head, obj_table, num);
  else if (esize == 16)
    __rte_ring_enqueue_elems_128(r, prod_head, obj_table, num);
  else 
  {
    ng_uint32_t idx, scale, nr_idx, nr_num, nr_size;

    /* Normalize to ng_uint32_t */
    scale = esize / sizeof(ng_uint32_t);
    nr_num = num * scale;
    idx = prod_head & r->mask;
    nr_idx = idx * scale;
    nr_size = r->size * scale;
    __rte_ring_enqueue_elems_32(r, nr_size, nr_idx,
        obj_table, nr_num);
  }
}

static __rte_always_inline void
__rte_ring_dequeue_elems_32(ng_ring_s *r, const ng_uint32_t size,
    ng_uint32_t idx, void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  ng_uint32_t *ring = (ng_uint32_t *)&r[1];
  ng_uint32_t *obj = (ng_uint32_t *)obj_table;
  if (likely(idx + n <= size)) 
  {
    for (i = 0; i < (n & ~0x7); i += 8, idx += 8) 
    {
      obj[i] = ring[idx];
      obj[i + 1] = ring[idx + 1];
      obj[i + 2] = ring[idx + 2];
      obj[i + 3] = ring[idx + 3];
      obj[i + 4] = ring[idx + 4];
      obj[i + 5] = ring[idx + 5];
      obj[i + 6] = ring[idx + 6];
      obj[i + 7] = ring[idx + 7];
    }
    switch (n & 0x7) 
    {
    case 7:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 6:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 5:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 4:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 3:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 2:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 1:
      obj[i++] = ring[idx++]; /* fallthrough */
    }
  }
  else 
  {
    for (i = 0; idx < size; i++, idx++)
      obj[i] = ring[idx];
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      obj[i] = ring[idx];
  }
}

static __rte_always_inline void
__rte_ring_dequeue_elems_64(ng_ring_s *r, ng_uint32_t cons_head,
    void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  const ng_uint32_t size = r->size;
  ng_uint32_t idx = cons_head & r->mask;
  ng_uint64_t *ring = (ng_uint64_t *)&r[1];
  unaligned_uint64_t *obj = (unaligned_uint64_t *)obj_table;
  if (likely(idx + n <= size)) {
    for (i = 0; i < (n & ~0x3); i += 4, idx += 4) {
      obj[i] = ring[idx];
      obj[i + 1] = ring[idx + 1];
      obj[i + 2] = ring[idx + 2];
      obj[i + 3] = ring[idx + 3];
    }
    switch (n & 0x3) {
    case 3:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 2:
      obj[i++] = ring[idx++]; /* fallthrough */
    case 1:
      obj[i++] = ring[idx++]; /* fallthrough */
    }
  } else {
    for (i = 0; idx < size; i++, idx++)
      obj[i] = ring[idx];
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      obj[i] = ring[idx];
  }
}

static __rte_always_inline void
__rte_ring_dequeue_elems_128(ng_ring_s *r, ng_uint32_t cons_head,
    void *obj_table, ng_uint32_t n)
{
  unsigned int i;
  const ng_uint32_t size = r->size;
  ng_uint32_t idx = cons_head & r->mask;
  rte_int128_t *ring = (rte_int128_t *)&r[1];
  rte_int128_t *obj = (rte_int128_t *)obj_table;
  if (likely(idx + n <= size)) {
    for (i = 0; i < (n & ~0x1); i += 2, idx += 2)
      ng_memcpy((void *)(obj + i), (void *)(ring + idx), 32);
    switch (n & 0x1) {
    case 1:
      ng_memcpy((void *)(obj + i), (void *)(ring + idx), 16);
    }
  } else {
    for (i = 0; idx < size; i++, idx++)
      ng_memcpy((void *)(obj + i), (void *)(ring + idx), 16);
    /* Start at the beginning */
    for (idx = 0; i < n; i++, idx++)
      ng_memcpy((void *)(obj + i), (void *)(ring + idx), 16);
  }
}

/* the actual dequeue of elements from the ring.
 * Placed here since identical code needed in both
 * single and multi producer enqueue functions.
 */
static __rte_always_inline void
__rte_ring_dequeue_elems(ng_ring_s *r, ng_uint32_t cons_head,
    void *obj_table, ng_uint32_t esize, ng_uint32_t num)
{
  /* 8B and 16B copies implemented individually to retain
   * the current performance.
   */
  if (esize == 8)
    __rte_ring_dequeue_elems_64(r, cons_head, obj_table, num);
  else if (esize == 16)
    __rte_ring_dequeue_elems_128(r, cons_head, obj_table, num);
  else {
    ng_uint32_t idx, scale, nr_idx, nr_num, nr_size;

    /* Normalize to ng_uint32_t */
    scale = esize / sizeof(ng_uint32_t);
    nr_num = num * scale;
    idx = cons_head & r->mask;
    nr_idx = idx * scale;
    nr_size = r->size * scale;
    __rte_ring_dequeue_elems_32(r, nr_size, nr_idx,
        obj_table, nr_num);
  }
}

static __rte_always_inline void
__rte_ring_update_tail(struct rte_ring_headtail *ht, ng_uint32_t old_val,
    ng_uint32_t new_val, ng_uint32_t single, ng_uint32_t enqueue)
{
  if (enqueue)
    rte_smp_wmb();
  else
    rte_smp_rmb();
  /*
   * If there are other enqueues/dequeues in progress that preceded us,
   * we need to wait for them to complete
   */
  if (!single)
    rte_wait_until_equal_32((volatile ng_uint32_t *)(ng_uintptr_t)&ht->tail, old_val,
      rte_memory_order_relaxed);

  ht->tail = new_val;
}

/**
 * @internal This function updates the producer head for enqueue
 *
 * @param r
 *   A pointer to the ring structure
 * @param is_sp
 *   Indicates whether multi-producer path is needed or not
 * @param n
 *   The number of elements we will want to enqueue, i.e. how far should the
 *   head be moved
 * @param behavior
 *   RTE_RING_QUEUE_FIXED:    Enqueue a fixed number of items from a ring
 *   RTE_RING_QUEUE_VARIABLE: Enqueue as many items as possible from ring
 * @param old_head
 *   Returns head value as it was before the move, i.e. where enqueue starts
 * @param new_head
 *   Returns the current/new head value i.e. where enqueue finishes
 * @param free_entries
 *   Returns the amount of ng_free space in the ring BEFORE head was moved
 * @return
 *   Actual number of objects enqueued.
 *   If behavior == RTE_RING_QUEUE_FIXED, this will be 0 or n only.
 */
static __rte_always_inline unsigned int
__rte_ring_move_prod_head(ng_ring_s *r, unsigned int is_sp,
    unsigned int n, enum rte_ring_queue_behavior behavior,
    ng_uint32_t *old_head, ng_uint32_t *new_head,
    ng_uint32_t *free_entries)
{
  const ng_uint32_t capacity = r->capacity;
  unsigned int max = n;
  int success;

  do {
    /* Reset n to the initial burst count */
    n = max;

    *old_head = r->prod.head;

    /* add rmb barrier to avoid load/load reorder in weak
     * memory model. It is noop on x86
     */
    rte_smp_rmb();

    /*
     *  The subtraction is done between two unsigned 32bits value
     * (the result is always modulo 32 bits even if we have
     * *old_head > cons_tail). So 'free_entries' is always between 0
     * and capacity (which is < size).
     */
    *free_entries = (capacity + r->cons.tail - *old_head);

    /* check that we have enough room in ring */
    if (unlikely(n > *free_entries))
      n = (behavior == RTE_RING_QUEUE_FIXED) ?
          0 : *free_entries;

    if (n == 0)
      return 0;

    *new_head = *old_head + n;
    if (is_sp) {
      r->prod.head = *new_head;
      success = 1;
    } else
      success = rte_atomic32_cmpset((ng_uint32_t *)(ng_uintptr_t)&r->prod.head,
          *old_head, *new_head);
  } while (unlikely(success == 0));
  return n;
}

/**
 * @internal This function updates the consumer head for dequeue
 *
 * @param r
 *   A pointer to the ring structure
 * @param is_sc
 *   Indicates whether multi-consumer path is needed or not
 * @param n
 *   The number of elements we will want to dequeue, i.e. how far should the
 *   head be moved
 * @param behavior
 *   RTE_RING_QUEUE_FIXED:    Dequeue a fixed number of items from a ring
 *   RTE_RING_QUEUE_VARIABLE: Dequeue as many items as possible from ring
 * @param old_head
 *   Returns head value as it was before the move, i.e. where dequeue starts
 * @param new_head
 *   Returns the current/new head value i.e. where dequeue finishes
 * @param entries
 *   Returns the number of entries in the ring BEFORE head was moved
 * @return
 *   - Actual number of objects dequeued.
 *     If behavior == RTE_RING_QUEUE_FIXED, this will be 0 or n only.
 */
static __rte_always_inline unsigned int
__rte_ring_move_cons_head(ng_ring_s *r, unsigned int is_sc,
    unsigned int n, enum rte_ring_queue_behavior behavior,
    ng_uint32_t *old_head, ng_uint32_t *new_head,
    ng_uint32_t *entries)
{
  unsigned int max = n;
  int success;

  /* move cons.head atomically */
  do {
    /* Restore n as it may change every loop */
    n = max;

    *old_head = r->cons.head;

    /* add rmb barrier to avoid load/load reorder in weak
     * memory model. It is noop on x86
     */
    rte_smp_rmb();

    /* The subtraction is done between two unsigned 32bits value
     * (the result is always modulo 32 bits even if we have
     * cons_head > prod_tail). So 'entries' is always between 0
     * and size(ring)-1.
     */
    *entries = (r->prod.tail - *old_head);

    /* Set the actual entries for dequeue */
    if (n > *entries)
      n = (behavior == RTE_RING_QUEUE_FIXED) ? 0 : *entries;

    if (unlikely(n == 0))
      return 0;

    *new_head = *old_head + n;
    if (is_sc) {
      r->cons.head = *new_head;
      rte_smp_rmb();
      success = 1;
    } else {
      success = rte_atomic32_cmpset((ng_uint32_t *)(ng_uintptr_t)&r->cons.head,
          *old_head, *new_head);
    }
  } while (unlikely(success == 0));
  return n;
}

/**
 * @internal Enqueue several objects on the ring
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param behavior
 *   RTE_RING_QUEUE_FIXED:    Enqueue a fixed number of items from a ring
 *   RTE_RING_QUEUE_VARIABLE: Enqueue as many items as possible from ring
 * @param is_sp
 *   Indicates whether to use single producer or multi-producer head update
 * @param free_space
 *   returns the amount of space after the enqueue operation has finished
 * @return
 *   Actual number of objects enqueued.
 *   If behavior == RTE_RING_QUEUE_FIXED, this will be 0 or n only.
 */
static __rte_always_inline unsigned int
__rte_ring_do_enqueue_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n,
    enum rte_ring_queue_behavior behavior, unsigned int is_sp,
    unsigned int *free_space)
{
  ng_uint32_t prod_head, prod_next;
  ng_uint32_t free_entries;

  n = __rte_ring_move_prod_head(r, is_sp, n, behavior,
      &prod_head, &prod_next, &free_entries);
  if (n == 0)
    goto end;

  __rte_ring_enqueue_elems(r, prod_head, obj_table, esize, n);

  __rte_ring_update_tail(&r->prod, prod_head, prod_next, is_sp, 1);
end:
  if (free_space != NULL)
    *free_space = free_entries - n;
  return n;
}

/**
 * @internal Dequeue several objects from the ring
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to pull from the ring.
 * @param behavior
 *   RTE_RING_QUEUE_FIXED:    Dequeue a fixed number of items from a ring
 *   RTE_RING_QUEUE_VARIABLE: Dequeue as many items as possible from ring
 * @param is_sc
 *   Indicates whether to use single consumer or multi-consumer head update
 * @param available
 *   returns the number of remaining ring entries after the dequeue has finished
 * @return
 *   - Actual number of objects dequeued.
 *     If behavior == RTE_RING_QUEUE_FIXED, this will be 0 or n only.
 */
static __rte_always_inline unsigned int
__rte_ring_do_dequeue_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n,
    enum rte_ring_queue_behavior behavior, unsigned int is_sc,
    unsigned int *available)
{
  ng_uint32_t cons_head, cons_next=0;
  ng_uint32_t entries;

  n = __rte_ring_move_cons_head(r, (int)is_sc, n, behavior,
      &cons_head, &cons_next, &entries);
  if (n == 0)
    goto end;

  __rte_ring_dequeue_elems(r, cons_head, obj_table, esize, n);

  __rte_ring_update_tail(&r->cons, cons_head, cons_next, is_sc, 0);

end:
  if (available != NULL)
    *available = entries - n;
  return n;
}

#if defined(__GNUC__)
#if (GCC_VERSION >= 120000)
#pragma GCC diagnostic pop
#endif
#endif

/**
 * Calculate the memory size needed for a ring with given element size
 *
 * This function returns the number of bytes needed for a ring, given
 * the number of elements in it and the size of the element. This value
 * is the sum of the size of the structure rte_ring and the size of the
 * memory needed for storing the elements. The value is aligned to a cache
 * line size.
 *
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 * @param count
 *   The number of elements in the ring (must be a power of 2).
 * @return
 *   - The memory size needed for the ring on success.
 *   - -EINVAL - esize is not a multiple of 4 or count provided is not a
 *     power of 2.
 */
ssize_t rte_ring_get_memsize_elem(unsigned int esize, unsigned int count);

/**
 * Create a new ring named *name* that stores elements with given size.
 *
 * This function uses ``memzone_reserve()`` to allocate memory. Then it
 * calls rte_ring_init() to initialize an empty ring.
 *
 * The new ring size is set to *count*, which must be a power of
 * two. Water marking is disabled by default. The real usable ring size
 * is *count-1* instead of *count* to differentiate a full ring from an
 * empty ring.
 *
 * The ring is added in RTE_TAILQ_RING list.
 *
 * @param name
 *   The name of the ring.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 * @param count
 *   The number of elements in the ring (must be a power of 2).
 * @param socket_id
 *   The *socket_id* argument is the socket identifier in case of
 *   NUMA. The value can be *SOCKET_ID_ANY* if there is no NUMA
 *   constraint for the reserved zone.
 * @param flags
 *   An OR of the following:
 *   - One of mutually exclusive flags that define producer behavior:
 *      - RING_F_SP_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "single-producer".
 *      - RING_F_MP_RTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer RTS mode".
 *      - RING_F_MP_HTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer HTS mode".
 *     If none of these flags is set, then default "multi-producer"
 *     behavior is selected.
 *   - One of mutually exclusive flags that define consumer behavior:
 *      - RING_F_SC_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "single-consumer". Otherwise, it is "multi-consumers".
 *      - RING_F_MC_RTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer RTS mode".
 *      - RING_F_MC_HTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer HTS mode".
 *     If none of these flags is set, then default "multi-consumer"
 *     behavior is selected.
 * @return
 *   On success, the pointer to the new allocated ring. NULL on error with
 *    rte_errno set appropriately. Possible errno values include:
 *    - E_RTE_NO_CONFIG - function could not get pointer to rte_config structure
 *    - EINVAL - esize is not a multiple of 4 or count provided is not a
 *     power of 2.
 *    - ENOSPC - the maximum number of memzones has already been allocated
 *    - EEXIST - a memzone with the same name already exists
 *    - ENOMEM - no appropriate memory area found in which to create memzone
 */
ng_ring_s *rte_ring_create_elem(const char *name, ng_size_t name_len, 
  unsigned int esize, unsigned int count, unsigned int flags);

/**
 * Enqueue several objects on the ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_mp_enqueue_bulk_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  return __rte_ring_do_enqueue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_FIXED, RTE_RING_SYNC_MT, free_space);
}

/**
 * Enqueue several objects on a ring
 *
 * @warning This API is NOT multi-producers safe
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_sp_enqueue_bulk_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  return __rte_ring_do_enqueue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_FIXED, RTE_RING_SYNC_ST, free_space);
}

/**
 * Enqueue several objects on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version depending on the default behavior that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_enqueue_bulk_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  switch (r->prod.sync_type) {
  case RTE_RING_SYNC_MT:
    return rte_ring_mp_enqueue_bulk_elem(r, obj_table, esize, n,
      free_space);
  case RTE_RING_SYNC_ST:
    return rte_ring_sp_enqueue_bulk_elem(r, obj_table, esize, n,
      free_space);
  }

  /* valid ring should never reach this point */
  RTE_ASSERT(0);
  if (free_space != NULL)
    *free_space = 0;
  return 0;
}

/**
 * Enqueue one object on a ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_mp_enqueue_elem(ng_ring_s *r, void *obj, unsigned int esize)
{
  return rte_ring_mp_enqueue_bulk_elem(r, obj, esize, 1, NULL) ? 0 :
                -ENOBUFS;
}

/**
 * Enqueue one object on a ring
 *
 * @warning This API is NOT multi-producers safe
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_sp_enqueue_elem(ng_ring_s *r, void *obj, unsigned int esize)
{
  return rte_ring_sp_enqueue_bulk_elem(r, obj, esize, 1, NULL) ? 0 :
                -ENOBUFS;
}

/**
 * Enqueue one object on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_enqueue_elem(ng_ring_s *r, void *obj, unsigned int esize)
{
  return rte_ring_enqueue_bulk_elem(r, obj, esize, 1, NULL) ? 0 :
                -ENOBUFS;
}

/**
 * Dequeue several objects from a ring (multi-consumers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_mc_dequeue_bulk_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  return __rte_ring_do_dequeue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_FIXED, RTE_RING_SYNC_MT, available);
}

/**
 * Dequeue several objects from a ring (NOT multi-consumers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table,
 *   must be strictly positive.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_sc_dequeue_bulk_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  return __rte_ring_do_dequeue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_FIXED, RTE_RING_SYNC_ST, available);
}

/**
 * Dequeue several objects from a ring.
 *
 * This function calls the multi-consumers or the single-consumer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_dequeue_bulk_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  switch (r->cons.sync_type) {
  case RTE_RING_SYNC_MT:
    return rte_ring_mc_dequeue_bulk_elem(r, obj_table, esize, n,
      available);
  case RTE_RING_SYNC_ST:
    return rte_ring_sc_dequeue_bulk_elem(r, obj_table, esize, n,
      available);
  }

  /* valid ring should never reach this point */
  RTE_ASSERT(0);
  if (available != NULL)
    *available = 0;
  return 0;
}

/**
 * Dequeue one object from a ring (multi-consumers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to the object that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success; objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue; no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_mc_dequeue_elem(ng_ring_s *r, void *obj_p,
        unsigned int esize)
{
  return rte_ring_mc_dequeue_bulk_elem(r, obj_p, esize, 1, NULL)  ? 0 :
                -ENOENT;
}

/**
 * Dequeue one object from a ring (NOT multi-consumers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to the object that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success; objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue, no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_sc_dequeue_elem(ng_ring_s *r, void *obj_p,
        unsigned int esize)
{
  return rte_ring_sc_dequeue_bulk_elem(r, obj_p, esize, 1, NULL) ? 0 :
                -ENOENT;
}

/**
 * Dequeue one object from a ring.
 *
 * This function calls the multi-consumers or the single-consumer
 * version depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to the object that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @return
 *   - 0: Success, objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue, no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_dequeue_elem(ng_ring_s *r, void *obj_p, unsigned int esize)
{
  return rte_ring_dequeue_bulk_elem(r, obj_p, esize, 1, NULL) ? 0 :
                -ENOENT;
}

/**
 * Enqueue several objects on the ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_mp_enqueue_burst_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  return __rte_ring_do_enqueue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_VARIABLE, RTE_RING_SYNC_MT, free_space);
}

/**
 * Enqueue several objects on a ring
 *
 * @warning This API is NOT multi-producers safe
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_sp_enqueue_burst_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  return __rte_ring_do_enqueue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_VARIABLE, RTE_RING_SYNC_ST, free_space);
}

/**
 * Enqueue several objects on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version depending on the default behavior that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_enqueue_burst_elem(ng_ring_s *r, const void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *free_space)
{
  switch (r->prod.sync_type) {
  case RTE_RING_SYNC_MT:
    return rte_ring_mp_enqueue_burst_elem(r, obj_table, esize, n,
      free_space);
  case RTE_RING_SYNC_ST:
    return rte_ring_sp_enqueue_burst_elem(r, obj_table, esize, n,
      free_space);
  }

  /* valid ring should never reach this point */
  RTE_ASSERT(0);
  if (free_space != NULL)
    *free_space = 0;
  return 0;
}

/**
 * Dequeue several objects from a ring (multi-consumers safe). When the request
 * objects are more than the available objects, only dequeue the actual number
 * of objects
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - n: Actual number of objects dequeued, 0 if ring is empty
 */
static __rte_always_inline unsigned int
rte_ring_mc_dequeue_burst_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  return __rte_ring_do_dequeue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_VARIABLE, RTE_RING_SYNC_MT, available);
}

/**
 * Dequeue several objects from a ring (NOT multi-consumers safe).When the
 * request objects are more than the available objects, only dequeue the
 * actual number of objects
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - n: Actual number of objects dequeued, 0 if ring is empty
 */
static __rte_always_inline unsigned int
rte_ring_sc_dequeue_burst_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  return __rte_ring_do_dequeue_elem(r, obj_table, esize, n,
      RTE_RING_QUEUE_VARIABLE, RTE_RING_SYNC_ST, available);
}

/**
 * Dequeue multiple objects from a ring up to a maximum number.
 *
 * This function calls the multi-consumers or the single-consumer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of objects that will be filled.
 * @param esize
 *   The size of ring element, in bytes. It must be a multiple of 4.
 *   This must be the same value used while creating the ring. Otherwise
 *   the results are undefined.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - Number of objects dequeued
 */
static __rte_always_inline unsigned int
rte_ring_dequeue_burst_elem(ng_ring_s *r, void *obj_table,
    unsigned int esize, unsigned int n, unsigned int *available)
{
  switch (r->cons.sync_type) {
  case RTE_RING_SYNC_MT:
    return rte_ring_mc_dequeue_burst_elem(r, obj_table, esize, n,
      available);
  case RTE_RING_SYNC_ST:
    return rte_ring_sc_dequeue_burst_elem(r, obj_table, esize, n,
      available);
  }

  /* valid ring should never reach this point */
  RTE_ASSERT(0);
  if (available != NULL)
    *available = 0;
  return 0;
}


/**
 * Calculate the memory size needed for a ring
 *
 * This function returns the number of bytes needed for a ring, given
 * the number of elements in it. This value is the sum of the size of
 * the structure rte_ring and the size of the memory needed by the
 * objects pointers. The value is aligned to a cache line size.
 *
 * @param count
 *   The number of elements in the ring (must be a power of 2).
 * @return
 *   - The memory size needed for the ring on success.
 *   - -EINVAL if count is not a power of 2.
 */
ssize_t rte_ring_get_memsize(unsigned int count);

/**
 * Initialize a ring structure.
 *
 * Initialize a ring structure in memory pointed by "r". The size of the
 * memory area must be large enough to store the ring structure and the
 * object table. It is advised to use rte_ring_get_memsize() to get the
 * appropriate size.
 *
 * The ring size is set to *count*, which must be a power of two.
 * The real usable ring size is *count-1* instead of *count* to
 * differentiate a full ring from an empty ring.
 *
 * The ring is not added in RTE_TAILQ_RING global list. Indeed, the
 * memory given by the caller may not be shareable among dpdk
 * processes.
 *
 * @param r
 *   The pointer to the ring structure followed by the objects table.
 * @param name
 *   The name of the ring.
 * @param count
 *   The number of elements in the ring (must be a power of 2,
 *   unless RING_F_EXACT_SZ is set in flags).
 * @param flags
 *   An OR of the following:
 *   - One of mutually exclusive flags that define producer behavior:
 *      - RING_F_SP_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "single-producer".
 *      - RING_F_MP_RTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer RTS mode".
 *      - RING_F_MP_HTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer HTS mode".
 *     If none of these flags is set, then default "multi-producer"
 *     behavior is selected.
 *   - One of mutually exclusive flags that define consumer behavior:
 *      - RING_F_SC_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "single-consumer". Otherwise, it is "multi-consumers".
 *      - RING_F_MC_RTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer RTS mode".
 *      - RING_F_MC_HTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer HTS mode".
 *     If none of these flags is set, then default "multi-consumer"
 *     behavior is selected.
 *   - RING_F_EXACT_SZ: If this flag is set, the ring will hold exactly the
 *     requested number of entries, and the requested size will be rounded up
 *     to the next power of two, but the usable space will be exactly that
 *     requested. Worst case, if a power-of-2 size is requested, half the
 *     ring space will be wasted.
 *     Without this flag set, the ring size requested must be a power of 2,
 *     and the usable space will be that size - 1.
 * @return
 *   0 on success, or a negative value on error.
 */
int rte_ring_init(ng_ring_s *r, const char *name, ng_size_t name_len,
  unsigned int count, unsigned int flags);

/**
 * Create a new ring named *name* in memory.
 *
 * This function uses ``memzone_reserve()`` to allocate memory. Then it
 * calls rte_ring_init() to initialize an empty ring.
 *
 * The new ring size is set to *count*, which must be a power of two.
 * The real usable ring size is *count-1* instead of *count* to
 * differentiate a full ring from an empty ring.
 *
 * The ring is added in RTE_TAILQ_RING list.
 *
 * @param name
 *   The name of the ring.
 * @param count
 *   The size of the ring (must be a power of 2,
 *   unless RING_F_EXACT_SZ is set in flags).
 * @param socket_id
 *   The *socket_id* argument is the socket identifier in case of
 *   NUMA. The value can be *SOCKET_ID_ANY* if there is no NUMA
 *   constraint for the reserved zone.
 * @param flags
 *   An OR of the following:
 *   - One of mutually exclusive flags that define producer behavior:
 *      - RING_F_SP_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "single-producer".
 *      - RING_F_MP_RTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer RTS mode".
 *      - RING_F_MP_HTS_ENQ: If this flag is set, the default behavior when
 *        using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *        is "multi-producer HTS mode".
 *     If none of these flags is set, then default "multi-producer"
 *     behavior is selected.
 *   - One of mutually exclusive flags that define consumer behavior:
 *      - RING_F_SC_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "single-consumer". Otherwise, it is "multi-consumers".
 *      - RING_F_MC_RTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer RTS mode".
 *      - RING_F_MC_HTS_DEQ: If this flag is set, the default behavior when
 *        using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *        is "multi-consumer HTS mode".
 *     If none of these flags is set, then default "multi-consumer"
 *     behavior is selected.
 *   - RING_F_EXACT_SZ: If this flag is set, the ring will hold exactly the
 *     requested number of entries, and the requested size will be rounded up
 *     to the next power of two, but the usable space will be exactly that
 *     requested. Worst case, if a power-of-2 size is requested, half the
 *     ring space will be wasted.
 *     Without this flag set, the ring size requested must be a power of 2,
 *     and the usable space will be that size - 1.
 * @return
 *   On success, the pointer to the new allocated ring. NULL on error with
 *    rte_errno set appropriately. Possible errno values include:
 *    - E_RTE_NO_CONFIG - function could not get pointer to rte_config structure
 *    - EINVAL - count provided is not a power of 2
 *    - ENOSPC - the maximum number of memzones has already been allocated
 *    - EEXIST - a memzone with the same name already exists
 *    - ENOMEM - no appropriate memory area found in which to create memzone
 */
ng_ring_s *rte_ring_create(const char *name, ng_size_t name_len, 
  unsigned int count, unsigned int flags);

/**
 * De-allocate all memory used by the ring.
 *
 * @param r
 *   Ring to ng_free.
 *   If NULL then, the function does nothing.
 */
void rte_ring_free(ng_ring_s *r);

/**
 * Dump the status of the ring to a file.
 *
 * @param f
 *   A pointer to a file for output
 * @param r
 *   A pointer to the ring structure.
 */
void rte_ring_dump(const ng_ring_s *r);

/**
 * Enqueue several objects on the ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_mp_enqueue_bulk(ng_ring_s *r, void * const *obj_table,
       unsigned int n, unsigned int *free_space)
{
  return rte_ring_mp_enqueue_bulk_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Enqueue several objects on a ring (NOT multi-producers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_sp_enqueue_bulk(ng_ring_s *r, void * const *obj_table,
       unsigned int n, unsigned int *free_space)
{
  return rte_ring_sp_enqueue_bulk_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Enqueue several objects on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version depending on the default behavior that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   The number of objects enqueued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_enqueue_bulk(ng_ring_s *r, void * const *obj_table,
          unsigned int n, unsigned int *free_space)
{
  return rte_ring_enqueue_bulk_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Enqueue one object on a ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_mp_enqueue(ng_ring_s *r, void *obj)
{
  return rte_ring_mp_enqueue_elem(r, &obj, sizeof(void *));
}

/**
 * Enqueue one object on a ring (NOT multi-producers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_sp_enqueue(ng_ring_s *r, void *obj)
{
  return rte_ring_sp_enqueue_elem(r, &obj, sizeof(void *));
}

/**
 * Enqueue one object on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj
 *   A pointer to the object to be added.
 * @return
 *   - 0: Success; objects enqueued.
 *   - -ENOBUFS: Not enough room in the ring to enqueue; no object is enqueued.
 */
static __rte_always_inline int
rte_ring_enqueue(ng_ring_s *r, void *obj)
{
  return rte_ring_enqueue_elem(r, &obj, sizeof(void *));
}

/**
 * Dequeue several objects from a ring (multi-consumers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_mc_dequeue_bulk(ng_ring_s *r, void **obj_table,
    unsigned int n, unsigned int *available)
{
  return rte_ring_mc_dequeue_bulk_elem(r, obj_table, sizeof(void *),
      n, available);
}

/**
 * Dequeue several objects from a ring (NOT multi-consumers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table,
 *   must be strictly positive.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_sc_dequeue_bulk(ng_ring_s *r, void **obj_table,
    unsigned int n, unsigned int *available)
{
  return rte_ring_sc_dequeue_bulk_elem(r, obj_table, sizeof(void *),
      n, available);
}

/**
 * Dequeue several objects from a ring.
 *
 * This function calls the multi-consumers or the single-consumer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   The number of objects dequeued, either 0 or n
 */
static __rte_always_inline unsigned int
rte_ring_dequeue_bulk(ng_ring_s *r, void **obj_table, unsigned int n,
    unsigned int *available)
{
  return rte_ring_dequeue_bulk_elem(r, obj_table, sizeof(void *),
      n, available);
}

/**
 * Dequeue one object from a ring (multi-consumers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to a void * pointer (object) that will be filled.
 * @return
 *   - 0: Success; objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue; no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_mc_dequeue(ng_ring_s *r, void **obj_p)
{
  return rte_ring_mc_dequeue_elem(r, obj_p, sizeof(void *));
}

/**
 * Dequeue one object from a ring (NOT multi-consumers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to a void * pointer (object) that will be filled.
 * @return
 *   - 0: Success; objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue, no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_sc_dequeue(ng_ring_s *r, void **obj_p)
{
  return rte_ring_sc_dequeue_elem(r, obj_p, sizeof(void *));
}

/**
 * Dequeue one object from a ring.
 *
 * This function calls the multi-consumers or the single-consumer
 * version depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_p
 *   A pointer to a void * pointer (object) that will be filled.
 * @return
 *   - 0: Success, objects dequeued.
 *   - -ENOENT: Not enough entries in the ring to dequeue, no object is
 *     dequeued.
 */
static __rte_always_inline int
rte_ring_dequeue(ng_ring_s *r, void **obj_p)
{
  return rte_ring_dequeue_elem(r, obj_p, sizeof(void *));
}

/**
 * Flush a ring.
 *
 * This function flush all the elements in a ring
 *
 * @warning
 * Make sure the ring is not in use while calling this function.
 *
 * @param r
 *   A pointer to the ring structure.
 */
void
rte_ring_reset(ng_ring_s *r);

/**
 * Return the number of entries in a ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   The number of entries in the ring.
 */
static inline unsigned int
rte_ring_count(const ng_ring_s *r)
{
  ng_uint32_t prod_tail = r->prod.tail;
  ng_uint32_t cons_tail = r->cons.tail;
  ng_uint32_t count = (prod_tail - cons_tail) & r->mask;
  return (count > r->capacity) ? r->capacity : count;
}

/**
 * Return the number of ng_free entries in a ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   The number of ng_free entries in the ring.
 */
static inline unsigned int
rte_ring_free_count(const ng_ring_s *r)
{
  return r->capacity - rte_ring_count(r);
}

/**
 * Test if a ring is full.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   - 1: The ring is full.
 *   - 0: The ring is not full.
 */
static inline int
rte_ring_full(const ng_ring_s *r)
{
  return rte_ring_free_count(r) == 0;
}

/**
 * Test if a ring is empty.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   - 1: The ring is empty.
 *   - 0: The ring is not empty.
 */
static inline int
rte_ring_empty(const ng_ring_s *r)
{
  ng_uint32_t prod_tail = r->prod.tail;
  ng_uint32_t cons_tail = r->cons.tail;
  return cons_tail == prod_tail;
}

/**
 * Return the size of the ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   The size of the data store used by the ring.
 *   NOTE: this is not the same as the usable space in the ring. To query that
 *   use ``rte_ring_get_capacity()``.
 */
static inline unsigned int
rte_ring_get_size(const ng_ring_s *r)
{
  return r->size;
}

/**
 * Return the number of elements which can be stored in the ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   The usable size of the ring.
 */
static inline unsigned int
rte_ring_get_capacity(const ng_ring_s *r)
{
  return r->capacity;
}

/**
 * Return sync type used by producer in the ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   Producer sync type value.
 */
static inline enum rte_ring_sync_type
rte_ring_get_prod_sync_type(const ng_ring_s *r)
{
  return r->prod.sync_type;
}

/**
 * Check is the ring for single producer.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   true if ring is SP, zero otherwise.
 */
static inline int
rte_ring_is_prod_single(const ng_ring_s *r)
{
  return (rte_ring_get_prod_sync_type(r) == RTE_RING_SYNC_ST);
}

/**
 * Return sync type used by consumer in the ring.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   Consumer sync type value.
 */
static inline enum rte_ring_sync_type
rte_ring_get_cons_sync_type(const ng_ring_s *r)
{
  return r->cons.sync_type;
}

/**
 * Check is the ring for single consumer.
 *
 * @param r
 *   A pointer to the ring structure.
 * @return
 *   true if ring is SC, zero otherwise.
 */
static inline int
rte_ring_is_cons_single(const ng_ring_s *r)
{
  return (rte_ring_get_cons_sync_type(r) == RTE_RING_SYNC_ST);
}

/**
 * Enqueue several objects on the ring (multi-producers safe).
 *
 * This function uses a "compare and set" instruction to move the
 * producer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_mp_enqueue_burst(ng_ring_s *r, 
  void * const *obj_table, unsigned int n, 
  unsigned int *free_space)
{
  return rte_ring_mp_enqueue_burst_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Enqueue several objects on a ring (NOT multi-producers safe).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_sp_enqueue_burst(ng_ring_s *r, void * const *obj_table,
       unsigned int n, unsigned int *free_space)
{
  return rte_ring_sp_enqueue_burst_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Enqueue several objects on a ring.
 *
 * This function calls the multi-producer or the single-producer
 * version depending on the default behavior that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects).
 * @param n
 *   The number of objects to add in the ring from the obj_table.
 * @param free_space
 *   if non-NULL, returns the amount of space in the ring after the
 *   enqueue operation has finished.
 * @return
 *   - n: Actual number of objects enqueued.
 */
static __rte_always_inline unsigned int
rte_ring_enqueue_burst(ng_ring_s *r, void * const *obj_table,
          unsigned int n, unsigned int *free_space)
{
  return rte_ring_enqueue_burst_elem(r, obj_table, sizeof(void *),
      n, free_space);
}

/**
 * Dequeue several objects from a ring (multi-consumers safe). When the request
 * objects are more than the available objects, only dequeue the actual number
 * of objects
 *
 * This function uses a "compare and set" instruction to move the
 * consumer index atomically.
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - n: Actual number of objects dequeued, 0 if ring is empty
 */
static __rte_always_inline unsigned int
rte_ring_mc_dequeue_burst(ng_ring_s *r, void **obj_table,
    unsigned int n, unsigned int *available)
{
  return rte_ring_mc_dequeue_burst_elem(r, obj_table, sizeof(void *),
      n, available);
}

/**
 * Dequeue several objects from a ring (NOT multi-consumers safe).When the
 * request objects are more than the available objects, only dequeue the
 * actual number of objects
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - n: Actual number of objects dequeued, 0 if ring is empty
 */
static __rte_always_inline unsigned int
rte_ring_sc_dequeue_burst(ng_ring_s *r, void **obj_table,
    unsigned int n, unsigned int *available)
{
  return rte_ring_sc_dequeue_burst_elem(r, obj_table, sizeof(void *),
      n, available);
}

/**
 * Dequeue multiple objects from a ring up to a maximum number.
 *
 * This function calls the multi-consumers or the single-consumer
 * version, depending on the default behaviour that was specified at
 * ring creation time (see flags).
 *
 * @param r
 *   A pointer to the ring structure.
 * @param obj_table
 *   A pointer to a table of void * pointers (objects) that will be filled.
 * @param n
 *   The number of objects to dequeue from the ring to the obj_table.
 * @param available
 *   If non-NULL, returns the number of remaining ring entries after the
 *   dequeue has finished.
 * @return
 *   - Number of objects dequeued
 */
static __rte_always_inline unsigned int
rte_ring_dequeue_burst(ng_ring_s *r, void **obj_table,
    unsigned int n, unsigned int *available)
{
  return rte_ring_dequeue_burst_elem(r, obj_table, sizeof(void *),
      n, available);
}

int test_ring(void);

#endif


