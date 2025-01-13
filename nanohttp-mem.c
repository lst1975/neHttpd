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
#include "nanohttp-config.h"
#include "nanohttp-mem.h"
#include "nanohttp-list.h"
#include "nanohttp-ring.h"
#include "nanohttp-server.h"

#ifdef WIN32
#include <Windows.h>
#endif

struct _mentry {
  ng_list_head_s link;
#if __NHTTP_MEM_DEBUG  
  const char *file;
  uint64_t line:16;
#endif  
  uint64_t iscache:1;
  uint64_t isinq:1;
  uint64_t size:46;
}__rte_cache_aligned;

typedef struct _mentry http_mentry_s;

#define TSN_MEM_Cache_Max 1024
#define TSN_MEM_ENTRY_SIZE 2048

#ifdef __KERNEL__
#define TSN_ASSERT(x) BUG_ON(!(x))
#else
#include <assert.h>
#define TSN_ASSERT(x) assert(x)
#endif

#define TSN_RESERVED_HEADROOM 128

#if !__NHTTP_USE_NATIVE_MEM
static uint8_t *msg_array_ptr;
static uint8_t *msg_array_end;
static uint8_t msg_cache_array[(TSN_MEM_Cache_Max-1)*TSN_MEM_ENTRY_SIZE]={0};
struct rte_ring *__http_mem_ring = NULL;

#if __NHTTP_MEM_DEBUG  
#ifdef WIN32
typedef HANDLE MUTEXT_T;
#else
typedef pthread_mutex_t MUTEXT_T;
#endif
static ng_list_head_s msg_malloced_list;
static MUTEXT_T msg_mutex_lock;
#endif  

static void __http_malloced_list_enq(http_mentry_s *e)
{
#if __NHTTP_MEM_DEBUG  
  httpd_enter_mutex(&msg_mutex_lock);
  e->isinq = 1;
  ng_list_add_tail(&e->link, &msg_malloced_list);
  httpd_leave_mutex(&msg_mutex_lock);
#else
  HTTPD_UNUSED(e);
#endif
}

static void __http_malloced_list_deq(http_mentry_s *e)
{
#if __NHTTP_MEM_DEBUG  
  if (e->isinq)
  {
    httpd_enter_mutex(&msg_mutex_lock);
    ng_list_del(&e->link);
    httpd_leave_mutex(&msg_mutex_lock);
  }
#else
  HTTPD_UNUSED(e);
#endif
}
#endif

#if __NHTTP_MEM_DEBUG  
#define __P2M(p) ((uint8_t *)(p)+sizeof(http_mentry_s))
#define __M2P(p) ((uint8_t *)(p)-sizeof(http_mentry_s))
#else
#define __P2M(p) (p)
#define __M2P(p) (p)
#endif

int http_memcache_init(void)
{
#if __NHTTP_USE_NATIVE_MEM
  log_info("[OK]: http_memcache_init.");
  return 0;
#else
  uint8_t *p;

  RTE_BUILD_BUG_ON(!POWEROF2(RTE_CACHE_LINE_SIZE));
  RTE_BUILD_BUG_ON(RTE_CACHE_LINE_SIZE < 32);
  RTE_BUILD_BUG_ON(sizeof(http_mentry_s) != RTE_CACHE_LINE_SIZE);

#if __NHTTP_MEM_DEBUG  
  int err;
  ng_INIT_LIST_HEAD(&msg_malloced_list);
  if (0 > (err = httpd_create_mutex(&msg_mutex_lock)))
  {
    log_fatal("pthread_spin_init failed.");
    goto clean0;
  }
#endif
  
  __http_mem_ring = rte_ring_create("httm_mem", TSN_MEM_Cache_Max, 0);
  if (__http_mem_ring == NULL)
  {
    log_fatal("ng_singlerw_ring_init failed.");
    goto clean1;
  }

  rte_ring_dump(__http_mem_ring);
  msg_array_ptr = &msg_cache_array[0]; 
  msg_array_end = msg_cache_array + sizeof(msg_cache_array); 
  p = msg_array_ptr; 

  while (p < msg_array_end)
  {
#if __NHTTP_MEM_DEBUG  
    http_mentry_s *e = (http_mentry_s *)p;
    e->file = NULL;
    e->line = 0;
#endif
    if (rte_ring_mp_enqueue(__http_mem_ring, p) < 0)
    {
      log_fatal("rte_ring_mp_enqueue failed.");
      goto clean1;
    }
    p += TSN_MEM_ENTRY_SIZE;
  }

  log_info("[OK]: http_memcache_init.");
  return 0;
  
clean1:
#if __NHTTP_MEM_DEBUG  
  httpd_destroy_mutex(&msg_mutex_lock);
clean0:
#endif
  return -1;
#endif
}

#if !__NHTTP_USE_NATIVE_MEM
#if !__NHTTP_NO_LOGGING
#if __NHTTP_MEM_DEBUG  
static const char *file_base_name(const char *file)
{
  const char *fp = strrchr(file, '/');
  if (fp == NULL)
    fp = file;
  return fp;
}
#endif
#endif
#endif

void http_memcache_free(void)
{
#if __NHTTP_USE_NATIVE_MEM
  log_info("[OK]: http_memcache_free.");
  return;
#else
  void *p;
  
  while (!rte_ring_mc_dequeue(__http_mem_ring, &p))
  {
    if ((uint8_t *)p >= msg_array_ptr && (uint8_t *)p < msg_array_end)
      continue;
    os_free(p);
  }

#if __NHTTP_MEM_DEBUG  
  ng_list_head_s *list = &msg_malloced_list;
  if (!ng_list_empty(list))
  {
    log_fatal("!!!!!!!!!!!MEMORY LEAKED!!!!!!!!!!!!!!.");
    while (!ng_list_empty(list))
    {
      http_mentry_s *e;
      e = ng_list_first_entry(list,http_mentry_s,link);
      ng_list_del(&e->link);
      log_fatal("file:%s, line:%d", file_base_name(e->file), e->line);
      os_free(e);
    }
  }
#endif
  rte_ring_free(__http_mem_ring);
#if __NHTTP_MEM_DEBUG  
  httpd_destroy_mutex(&msg_mutex_lock);
#endif

  log_info("[OK]: http_memcache_free.");
#endif
}

void *__http_malloc(size_t size
#if __NHTTP_MEM_DEBUG  
  , const char *file, int line
#endif
)
{
#if __NHTTP_USE_NATIVE_MEM
#if __NHTTP_MEM_DEBUG  
  NG_UNUSED(file);
  NG_UNUSED(line);
#endif
  return os_malloc(size);
#else
  void *p;
  http_mentry_s *e;
  if (size + sizeof(http_mentry_s) < TSN_MEM_ENTRY_SIZE)
  {
    if (!rte_ring_mc_dequeue(__http_mem_ring, &p))
    {
      e = (http_mentry_s *)p;
#if __NHTTP_MEM_DEBUG  
      e->file = file;
      e->line = line;
#endif
      e->iscache = 1;
      return __P2M(p);
    }
  }

  size = RTE_ALIGN(size+sizeof(http_mentry_s), RTE_CACHE_LINE_SIZE);
  p = os_malloc(size);
  if (p == NULL)
  {
#if __NHTTP_MEM_DEBUG  
    log_fatal("Malloc failed, file:%s, line:%d", file_base_name(file), line);
#else
    log_fatal("Malloc failed");
#endif
    return NULL;
  }
  e = (http_mentry_s *)p;
  e->iscache = 0;
  e->size    = size;
  e->isinq   = 0;
#if __NHTTP_MEM_DEBUG  
  e->file = file;
  e->line = line;
#endif
  __http_malloced_list_enq(e);
  return __P2M(e);
#endif  
}

void __http_free(void *ptr)
{
#if __NHTTP_USE_NATIVE_MEM
  os_free(ptr);
#else  
  http_mentry_s *e = (http_mentry_s *)(__M2P(ptr));

  __http_malloced_list_deq(e);
  ng_smp_mb();

  if ((uint8_t *)e >= msg_array_ptr && (uint8_t *)e < msg_array_end)
  {
    rte_ring_mp_enqueue(__http_mem_ring, e);
    return;
  }

  os_free(e);
  return;
#endif  
}

