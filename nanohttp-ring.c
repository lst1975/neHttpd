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
#include "nanohttp-ring.h"
#include "nanohttp-mem.h"

int 
ng_singlerw_ring_init(ng_singlerw_ring_s *ring, NG_U32 size)
{ 
  ring->mask  = size - 1;
  ring->size  = size;
  ring->__W   = 0;
  ring->__R   = 0;
  ring->array = NULL;
  ng_atomic_set(ring->__N, 0);

  if (!size || !ng_is_power_of_2(size))
    return -1;
  
  ring->array = (void **)ng_srw_malloc(size*sizeof(void *));
  if (ring->array == NULL)
    return -1;

  return 0;
}

void 
ng_singlerw_ring_free(ng_singlerw_ring_s *ring)
{ 
  if (ring->array != NULL)
  {
    ng_srw_free(ring->array);
    ring->array = NULL;
  }
  return;
}

#define RING_LOG(t, fmt, ...) NG_RING_DEBUG("%d"fmt,LOG_ ## t, __VA_ARGS__)

/* mask of all valid flag values to ring_create() */
#define RING_F_MASK (RING_F_SP_ENQ | RING_F_SC_DEQ | RING_F_EXACT_SZ)

/* by default set head/tail distance as 1/8 of ring capacity */
#define HTD_MAX_DEF  8

/* return the size of memory occupied by a ring */
ssize_t
rte_ring_get_memsize_elem(unsigned int esize, unsigned int count)
{
  ssize_t sz;

  /* Check if element size is a multiple of 4B */
  if (esize % 4 != 0) {
    RING_LOG(ERR, "%s", "element size is not a multiple of 4");

    return -EINVAL;
  }

  /* count must be a power of 2 */
  if ((!POWEROF2(count)) || (count > RTE_RING_SZ_MASK )) {
    RING_LOG(ERR,
      "Requested number of elements is invalid, must be power of 2, and not exceed %u",
      RTE_RING_SZ_MASK);

    return -EINVAL;
  }

  sz = sizeof(ng_ring_s) + (ssize_t)count * esize;
  sz = RTE_ALIGN(sz, RTE_CACHE_LINE_SIZE);
  return sz;
}

/* return the size of memory occupied by a ring */
ssize_t
rte_ring_get_memsize(unsigned int count)
{
  return rte_ring_get_memsize_elem(sizeof(void *), count);
}

/*
 * internal helper function to reset prod/cons head-tail values.
 */
static void
reset_headtail(void *p)
{
  struct rte_ring_headtail *ht;

  ht = p;

  switch (ht->sync_type) {
  case RTE_RING_SYNC_MT:
  case RTE_RING_SYNC_ST:
    ht->head = 0;
    ht->tail = 0;
    break;
  default:
    /* unknown sync mode */
    RTE_ASSERT(0);
  }
}

void
rte_ring_reset(ng_ring_s *r)
{
  reset_headtail(&r->prod);
  reset_headtail(&r->cons);
}

/*
 * helper function, calculates sync_type values for prod and cons
 * based on input flags. Returns zero at success or negative
 * errno value otherwise.
 */
static int
get_sync_type(ng_uint32_t flags, enum rte_ring_sync_type *prod_st,
  enum rte_ring_sync_type *cons_st)
{
  static const ng_uint32_t prod_st_flags =
    (RING_F_SP_ENQ);
  static const ng_uint32_t cons_st_flags =
    (RING_F_SC_DEQ);

  switch (flags & prod_st_flags) {
  case 0:
    *prod_st = RTE_RING_SYNC_MT;
    break;
  case RING_F_SP_ENQ:
    *prod_st = RTE_RING_SYNC_ST;
    break;
  default:
    return -EINVAL;
  }

  switch (flags & cons_st_flags) {
  case 0:
    *cons_st = RTE_RING_SYNC_MT;
    break;
  case RING_F_SC_DEQ:
    *cons_st = RTE_RING_SYNC_ST;
    break;
  default:
    return -EINVAL;
  }

  return 0;
}
/**
 * Combines 32b inputs most significant set bits into the least
 * significant bits to construct a value with the same MSBs as x
 * but all 1's under it.
 *
 * @param x
 *    The integer whose MSBs need to be combined with its LSBs
 * @return
 *    The combined value.
 */
static inline ng_uint32_t
rte_combine32ms1b(ng_uint32_t x)
{
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return x;
}

/**
 * Aligns input parameter to the next power of 2
 *
 * @param x
 *   The integer value to align
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline ng_uint32_t
rte_align32pow2(ng_uint32_t x)
{
  x--;
  x = rte_combine32ms1b(x);

  return x + 1;
}

int
rte_ring_init(ng_ring_s *r, const char *name, ng_size_t name_len, 
  unsigned int count, unsigned int flags)
{
  int ret;

  /* compilation-time checks */
  RTE_BUILD_BUG_ON((sizeof(ng_ring_s) &
        RTE_CACHE_LINE_MASK) != 0);
  RTE_BUILD_BUG_ON((offsetof(ng_ring_s, cons) &
        RTE_CACHE_LINE_MASK) != 0);
  RTE_BUILD_BUG_ON((offsetof(ng_ring_s, prod) &
        RTE_CACHE_LINE_MASK) != 0);

  /* future proof flags, only allow supported values */
  if (flags & ~RING_F_MASK) {
    RING_LOG(ERR,
      "Unsupported flags requested %#x", flags);
    return -EINVAL;
  }

  /* init the ring structure */
  ng_memset(r, 0, sizeof(*r));
  
  if (name_len + 1 >= sizeof(r->name))
    return -ENAMETOOLONG;
  ng_memcpy(r->name, name, name_len);
  r->name[name_len] = '\0';
  
  r->flags = flags;
  ret = get_sync_type(flags, &r->prod.sync_type, &r->cons.sync_type);
  if (ret != 0)
    return ret;

  if (flags & RING_F_EXACT_SZ) {
    r->size = rte_align32pow2(count + 1);
    r->mask = r->size - 1;
    r->capacity = count;
  } else {
    if ((!POWEROF2(count)) || (count > RTE_RING_SZ_MASK)) {
      RING_LOG(ERR,
        "Requested size is invalid, must be power of 2, and not exceed the size limit %u",
        RTE_RING_SZ_MASK);
      return -EINVAL;
    }
    r->size = count;
    r->mask = count - 1;
    r->capacity = r->mask;
  }

  return 0;
}

/* create the ring for a given element size */
ng_ring_s *
rte_ring_create_elem(const char *name, ng_size_t name_len, 
  unsigned int esize, unsigned int count, unsigned int flags)
{
  ng_ring_s *r;
  ssize_t ring_size;
  const unsigned int requested_count = count;

  /* for an exact size ring, round up from count to a power of two */
  if (flags & RING_F_EXACT_SZ)
    count = rte_align32pow2(count + 1);

  ring_size = rte_ring_get_memsize_elem(esize, count);
  if (ring_size < 0) {
    rte_set_errno(-ring_size);
    return NULL;
  }

  /* reserve a memory zone for this ring. If we can't get rte_config or
   * we are secondary process, the memzone_reserve function will set
   * rte_errno for us appropriately - hence no check in this function
   */
  r = (ng_ring_s *)os_malloc(ring_size);
  if (r != NULL) {
    /* no need to check return value here, we already checked the
     * arguments above */
    rte_ring_init(r, name, name_len, requested_count, flags);
    r->memzone = NULL;
  } else {
    r = NULL;
    RING_LOG(ERR, "%s", "Cannot reserve memory");
  }

  return r;
}

/* create the ring */
ng_ring_s *
rte_ring_create(const char *name, ng_size_t name_len, 
  unsigned int count, unsigned int flags)
{
  return rte_ring_create_elem(name, name_len, sizeof(void *), count, flags);
}

/* os_free the ring */
void
rte_ring_free(ng_ring_s *r)
{
  if (r == NULL)
    return;
  os_free(r);
}

/* dump the status of the ring on the console */
void
rte_ring_dump(const ng_ring_s *r)
{
  NG_RING_DEBUG("RTE-RING: n consumers/n producers <%s>@%p"__LN, r->name, r);
  NG_RING_DEBUG("  flags=%x" __LN, r->flags);
  NG_RING_DEBUG("  size=%"PRIu32 __LN, r->size);
  NG_RING_DEBUG("  capacity=%"PRIu32 __LN, r->capacity);
  NG_RING_DEBUG("  ct=%"PRIu32 __LN, r->cons.tail);
  NG_RING_DEBUG("  ch=%"PRIu32 __LN, r->cons.head);
  NG_RING_DEBUG("  pt=%"PRIu32 __LN, r->prod.tail);
  NG_RING_DEBUG("  ph=%"PRIu32 __LN, r->prod.head);
  NG_RING_DEBUG("  used=%u" __LN, rte_ring_count(r));
  NG_RING_DEBUG("  avail=%u" __LN, rte_ring_free_count(r));
}


#if __NG_RING_DEBUG
#include <pthread.h>

static NG_ATOMIC_T(NG_U32) malloc_count;
static NG_ATOMIC_T(NG_U32) __running;
static ng_singlerw_ring_s __ring;
static int __stopped = 0;
ng_ring_s *__test_rte_ring = NULL;

struct ng_thread_pool{
  ng_uint32_t count;
  ng_uint32_t alive:31;
  ng_uint32_t not_stopped:1;
  pthread_t *pool;
};
typedef struct ng_thread_pool ng_thread_pool_s;

typedef void * (*ng_thread_routine_f)(void *);

static void *
thread_pool_reader_burst(void *arg)
{
  ng_thread_pool_s *p = (ng_thread_pool_s *)arg;
  while (p->not_stopped)
  {
    int i,n;
    void *data[16];
    n = rte_ring_mc_dequeue_burst(__test_rte_ring, data, 16, NULL);
    if (n > 0)
    {
      for (i=0;i<n;i++)
      {
        ng_atomic_dec(&malloc_count);
        ng_smp_wmb();
        ng_free(data[i]);
      }
    }
  }
  return NULL;
}

static void *
thread_pool_writer_burst(void *arg)
{
  ng_thread_pool_s *p = (ng_thread_pool_s *)arg;
  while (p->not_stopped)
  {
    int i,n;
    void *data[16];
    for (i=0;i<16;i++)
    {
      data[i] = ng_malloc(256);
      if (data[i] == NULL)
        break;
      ng_atomic_inc(&malloc_count);
    }
    n = rte_ring_mp_enqueue_burst(__test_rte_ring, data, i, NULL);
    if (n < i)
    {
      for (;n<i;n++)
      {
        ng_atomic_dec(&malloc_count);
        ng_smp_wmb();
        ng_free(data[n]);
      }
    }
  }
  return NULL;
}

static ng_thread_routine_f __pool_get_routine(int i)
{
  return (i&1) ? thread_pool_writer_burst : thread_pool_reader_burst;
}

void
ng_thread_pool_free(ng_thread_pool_s *p)
{
  p->not_stopped = 0;
  for (int i=0;i<p->alive;i++)
  {
    pthread_join(p->pool[i],NULL);
  }
  ng_free(p);
}

ng_thread_pool_s *
ng_thread_pool_create(int count, ng_thread_routine_f (*get_routine)(int))
{
  ng_thread_pool_s *p;

  p = (ng_thread_pool_s *)ng_malloc(sizeof(*p)+count*sizeof(pthread_t));
  if (p == NULL) {
    log_fatal("failed to ng_malloc ng_thread_pool_s."__LN);
    goto cleanup0;
  }
  p->count = count;
  p->alive = 0;
  p->not_stopped = 1;
  p->pool = (pthread_t *)(p+1);

  for (;p->alive<count;p->alive++)
  {
    int ret = pthread_create(&p->pool[p->alive], NULL,
      get_routine(p->alive), p);
    if (ret != 0) {
      log_fatal("pthread_create failed."__LN);
      goto cleanup1;
    }
  }

  return p;
  
cleanup1:
  ng_thread_pool_free(p);
cleanup0:
  return NULL;
}

static void *
thread_start_1(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    void *data = ng_malloc(sizeof(ng_singlerw_ring_s));
    if (data != NULL)
    {
      ng_atomic_inc(&malloc_count);
      ng_smp_mb();
      if (0 > ng_singlerw_ring_set(&__ring, data))
      {
        ng_atomic_dec(&malloc_count);
        ng_free(data);
      }
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void *
thread_start_2(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    void *data = ng_singlerw_ring_get(&__ring);
    if (data != NULL)
    {
      ng_atomic_dec(&malloc_count);
      ng_smp_wmb();
      ng_free(data);
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void *
thread_start_reader(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    void *data;
    if (!rte_ring_mc_dequeue(__test_rte_ring, &data))
    {
      ng_atomic_dec(&malloc_count);
      ng_smp_wmb();
      ng_free(data);
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void *
thread_start_writer(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    void *data = ng_malloc(sizeof(ng_singlerw_ring_s));
    if (data != NULL)
    {
      ng_atomic_inc(&malloc_count);
      ng_smp_mb();
      if (0 > rte_ring_mp_enqueue(__test_rte_ring, data))
      {
        ng_atomic_dec(&malloc_count);
        ng_free(data);
      }
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void *
thread_start_reader_burst(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    int i,n;
    void *data[16];
    n = rte_ring_mc_dequeue_burst(__test_rte_ring, data, 16, NULL);
    if (n > 0)
    {
      for (i=0;i<n;i++)
      {
        ng_atomic_dec(&malloc_count);
        ng_smp_wmb();
        ng_free(data[i]);
      }
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void *
thread_start_writer_burst(void *arg)
{
  ng_atomic_inc(&__running);
  while (!__stopped)
  {
    int i,n;
    void *data[16];
    for (i=0;i<16;i++)
    {
      data[i] = ng_malloc(256);
      if (data[i] == NULL)
        break;
      ng_atomic_inc(&malloc_count);
    }
    n = rte_ring_mp_enqueue_burst(__test_rte_ring, data, i, NULL);
    if (n < i)
    {
      for (;n<i;n++)
      {
        ng_atomic_dec(&malloc_count);
        ng_smp_wmb();
        ng_free(data[n]);
      }
    }
  }
  ng_atomic_dec(&__running);
  return NULL;
}

static void __test_ring(void)
{
  int ret;
  char c;
  pthread_t t1,t2,t3,t4,t5,t6,t7,t8;
  ng_thread_pool_s *test_thread_pool;

  if (0 > ng_singlerw_ring_init(&__ring, 1024))
  {
    log_fatal("ng_singlerw_ring_init failed."__LN);
    goto cleanup0;
  }

  __test_rte_ring = rte_ring_create(CONST_STR_ARG("ring_test"), 1024, 0);
  if (__test_rte_ring == NULL)
  {
    log_fatal("ng_singlerw_ring_init failed."__LN);
    goto cleanup1;
  }

  ng_singlerw_ring_dump(&__ring);
  
  ret = pthread_create((pthread_t *)&t1, NULL,
    thread_start_1, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup2;
  }
  
  ret = pthread_create((pthread_t *)&t2, NULL,
    thread_start_2, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup3;
  }
  
  ret = pthread_create((pthread_t *)&t3, NULL,
    thread_start_reader, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup4;
  }
  
  ret = pthread_create((pthread_t *)&t4, NULL,
    thread_start_reader, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup5;
  }
  
  ret = pthread_create((pthread_t *)&t5, NULL,
    thread_start_writer, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup6;
  }
  
  ret = pthread_create((pthread_t *)&t6, NULL,
    thread_start_writer, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup7;
  }
  
  ret = pthread_create((pthread_t *)&t7, NULL,
    thread_start_reader_burst, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup8;
  }
  
  ret = pthread_create((pthread_t *)&t8, NULL,
    thread_start_writer_burst, NULL);
  if (ret != 0) {
    log_fatal("pthread_create failed."__LN);
    goto cleanup9;
  }

  test_thread_pool = ng_thread_pool_create(24, __pool_get_routine);
  if (test_thread_pool == NULL)
  {
    log_fatal("pthread_pool_create failed."__LN);
    goto cleanup10;
  }
  
  while (1)
  {
    c = getc(stdin);
    if (c == 'd' || c == 'r')
    {
      NG_U32 count = ng_atomic_read(&malloc_count);
      log_info("%s"__LN, "#######################################################################");
      ng_singlerw_ring_dump(&__ring);
      log_info("%s"__LN, "**********************************************************************");
      rte_ring_dump(__test_rte_ring);
      log_info("%s"__LN, "**********************************************************************");
      log_info("memory malloced: %u"__LN, count);
      log_info("thread count   : %u"__LN, ng_atomic_read(&__running) + test_thread_pool->alive);
      log_info("%s\n"__LN, "**********************************************************************");
    }
    else if (c == 's')
    {
      __stopped = 1;
      pthread_join(t1,NULL);
      pthread_join(t2,NULL);
      pthread_join(t3,NULL);
      pthread_join(t4,NULL);
      pthread_join(t5,NULL);
      pthread_join(t6,NULL);
      pthread_join(t7,NULL);
      pthread_join(t8,NULL);
      ng_smp_mb();
      while (ng_atomic_read(&__running)){};
      ng_thread_pool_free(test_thread_pool);
      break;
    }
  }
  
  return;
  
cleanup10:
  pthread_cancel(t8);
cleanup9:
  pthread_cancel(t7);
cleanup8:
  pthread_cancel(t6);
cleanup7:
  pthread_cancel(t5);
cleanup6:
  pthread_cancel(t4);
cleanup5:
  pthread_cancel(t3);
cleanup4:
  pthread_cancel(t2);
cleanup3:
  pthread_cancel(t1);
cleanup2:
  rte_ring_free(__test_rte_ring);
cleanup1:
  ng_singlerw_ring_free(&__ring);
cleanup0:
  log_fatal("Init failed!"__LN);
  return;
}

void test_ring(void)
{
  void *data;
  NG_U32 count;

  log_info("'d': dump."__LN);
  log_info("'s': exit."__LN);

  ng_atomic_set(&malloc_count, 0);
  ng_atomic_set(&__running, 0);
  
  __test_ring();
  
  while (ng_atomic_read(&__ring.__N))
  {
    data = ng_singlerw_ring_get(&__ring);
    if (data != NULL)
    {
      ng_atomic_dec(&malloc_count);
      ng_free(data);
    }
  }
  while (!rte_ring_dequeue(__test_rte_ring, &data))
  {
    ng_atomic_dec(&malloc_count);
    ng_free(data);
  }
  
  count = ng_atomic_read(&malloc_count);
  if (count)
  {
    log_error("memory leaked: %u."__LN, count);
  }
  else
  {
    log_error("memory malloced: %u."__LN, count);
  }
  
  ng_singlerw_ring_free(&__ring);
  rte_ring_free(__test_rte_ring);

  http_memcache_free();
  return;
}

#endif

