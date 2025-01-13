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
#ifndef __nanohttp_lfq_h
#define __nanohttp_lfq_h

/*
 * rculfqueue-static.h
 *
 * Userspace RCU library - Lock-Free RCU Queue
 *
 * Copyright 2010 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * TO BE INCLUDED ONLY IN LGPL-COMPATIBLE CODE. See rculfqueue.h for linking
 * dynamically with the userspace rcu library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef __cplusplus
extern "C" {
#endif

struct lfq_node {
	struct lfq_node *next; /* struct cds_lfq_node_rcu */
};
typedef struct lfq_node lfq_node_s;

struct lfq_queue {
	_Atomic(lfq_node_s *) head;
  _Atomic(lfq_node_s *) tail;
  lfq_node_s dummy;
};
typedef struct lfq_queue lfq_queue_s;

static inline void 
_lfq_node_init(lfq_node_s *node)
{
	node->next = NULL;
}

static inline void 
_lfq_init_rcu(lfq_queue_s *q)
{ 
  _lfq_node_init(&q->dummy);
  q->head = &q->dummy;
  q->tail = &q->dummy;
}

/*
 * The queue should be emptied before calling destroy.
 *
 * Return 0 on success, -1 if queue is not empty.
 */
static inline int 
_lfq_destroy(lfq_queue_s *q)
{
  lfq_node_s *old_head;
  lfq_node_s *old_tail;

  old_head  = atomic_load(&q->head);
  old_tail  = atomic_load(&q->tail);
  
  if (old_head != old_tail) 
  {
    return -1;
  } 
	return 0;
}

static inline void 
_lfq_enqueue_one(lfq_queue_s *q, lfq_node_s *new_node)
{
  lfq_node_s *null_node = NULL;

  new_node->next = NULL;
  
  while (1) 
  {
    lfq_node_s *old_tail = atomic_load(&q->tail);
    if (atomic_compare_exchange_weak(&old_tail->next, &null_node, new_node)) 
    {
      atomic_compare_exchange_weak(&q->tail, &old_tail, new_node);
      break;
    } 
    else 
    {
      atomic_compare_exchange_weak(&q->tail, &old_tail, old_tail->next);
    }
  }
}

static inline void 
_lfq_enqueue_list(lfq_queue_s *q, lfq_node_s *new_node, lfq_node_s *new_tail)
{
  lfq_node_s *null_node = NULL;

  while (1) 
  {
    lfq_node_s *old_tail = atomic_load(&q->tail);
    if (atomic_compare_exchange_weak(&old_tail->next, &null_node, new_node)) 
    {
      atomic_compare_exchange_weak(&q->tail, &old_tail, new_tail);
      break;
    } 
    else 
    {
      atomic_compare_exchange_weak(&q->tail, &old_tail, old_tail->next);
    }
  }
  
}

static inline lfq_node_s *
_lfq_dequeue_one(lfq_queue_s *q)
{
  while (1) 
  {
    lfq_node_s *old_head;
    lfq_node_s *old_tail;
    lfq_node_s *next_node;
    
    old_head  = atomic_load(&q->head);
    old_tail  = atomic_load(&q->tail);
    next_node = atomic_load(&old_head->next);
  
    if (old_head == old_tail) 
    {
      if (next_node == NULL) 
      {
        return NULL; // Queue is empty
      }
    } 
    else 
    {
      if (atomic_compare_exchange_weak(&q->head, &old_head, next_node)) 
      {
        return next_node; // Success
      }
    }
  }
}

static inline lfq_node_s *
_lfq_dequeue_list(lfq_queue_s *q)
{
  while (1) 
  {
    lfq_node_s *old_head;
    lfq_node_s *old_tail;
    lfq_node_s *next_node;
    
    old_head  = atomic_load(&q->head);
    old_tail  = atomic_load(&q->tail);
    next_node = atomic_load(&old_head->next);
  
    if (old_head == old_tail) 
    {
      if (next_node == NULL) 
      {
        return NULL; // Queue is empty
      }
    } 
    else 
    {
      if (atomic_compare_exchange_weak(&q->head, &old_head, old_tail)) 
      {
        return next_node; // Success
      }
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif /* _URCU_RCULFQUEUE_STATIC_H */

