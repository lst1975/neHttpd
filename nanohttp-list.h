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
#ifndef  __ng_LIST_h__
#define  __ng_LIST_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "nanohttp-defs.h"

#define ng_WRITE_ONCE(a,v)        ((a) = (v))
#define ng_READ_ONCE(v)           (v)

#ifndef NG_LIST_POISON1
#if defined(__LP64__) || defined(__LP64) || defined(__amd64) 
#define NG_LIST_POISON1 ((ng_list_head_s *)(ng_uintptr_t)0xabababababababab)
#else
#define NG_LIST_POISON1 ((ng_list_head_s *)(ng_uintptr_t)0xabababab)
#endif
#endif

#ifndef NG_LIST_POISON2
#if defined(__LP64__) || defined(__LP64) || defined(__amd64)
#define NG_LIST_POISON2 ((ng_list_head_s *)(ng_uintptr_t)0xcdcdcdcdcdcdcdcd)
#else
#define NG_LIST_POISON2 ((ng_list_head_s *)(ng_uintptr_t)0xcdcdcdcd)
#endif
#endif

typedef struct ng_list_head ng_list_head_s;
struct ng_list_head {
  ng_list_head_s *next;
  ng_list_head_s *prev;
};
/* SPDX-License-Identifier: GPL-2.0 */

#define __ng_config_UseLIST_Poison 0

#if __ng_config_UseLIST_Poison  
#define __ng_list_poison_entry(entry) {  \
  (entry)->next = NG_LIST_POISON1; \
  (entry)->prev = NG_LIST_POISON2; \
}
#else
#define __ng_list_poison_entry(entry) HTTPD_UNUSED(entry)
#endif  

/*
 * Circular doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define NG_LIST_HEAD_INIT(name)  { &(name), &(name) }
#define NG_LIST_ENTRY_INIT() { NG_LIST_POISON1, NG_LIST_POISON1 }

#define NG_LIST_HEAD(name) \
  ng_list_head_s name = NG_LIST_HEAD_INIT(name)

/**
 * INIT_LIST_HEAD - Initialize a list_head structure
 * @list: list_head structure to be initialized.
 *
 * Initializes the list_head to point to itself.  If it is a list header,
 * the result is an empty list.
 */
static inline void ng_INIT_LIST_HEAD(ng_list_head_s *list)
{
  ng_WRITE_ONCE(list->next, list);
  ng_WRITE_ONCE(list->prev, list);
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __ng_list_add(ng_list_head_s *_new,
            ng_list_head_s *prev,
            ng_list_head_s *next)
{
  next->prev = _new;
  _new->next = next;
  _new->prev = prev;
  ng_WRITE_ONCE(prev->next, _new);
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void ng_list_add_head(ng_list_head_s *_new, ng_list_head_s *head)
{
  __ng_list_add(_new, head, head->next);
}
#define ng_list_add ng_list_add_head 

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void ng_list_add_tail(ng_list_head_s *_new, ng_list_head_s *head)
{
  __ng_list_add(_new, head->prev, head);
}

/**
 * list_add_before - add a new entry before the existing element in one list
 * @new: new entry to be added
 * @el: the existing element in one list to add it before
 *
 * Insert a new entry before the specified element.
 */
static inline void ng_list_add_before(ng_list_head_s *_new, ng_list_head_s *el)
{
  __ng_list_add(_new, el->prev, el);
}

/**
 * list_add_before - add a new entry after the existing element in one list
 * @new: new entry to be added
 * @el: the existing element in one list to add it after
 *
 * Insert a new entry after the specified element.
 */
static inline void ng_list_add_after(ng_list_head_s *_new, ng_list_head_s *el)
{
  __ng_list_add(_new, el, el->next);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int ng_list_empty(const ng_list_head_s *head)
{
  return ng_READ_ONCE(head->next) == head;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __ng_list_del(ng_list_head_s * prev, ng_list_head_s * next)
{
  next->prev = prev;
  ng_WRITE_ONCE(prev->next, next);
}

static inline void __ng_list_del_entry(ng_list_head_s *entry)
{
  __ng_list_del(entry->prev, entry->next);
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void ng_list_del(ng_list_head_s *entry)
{
  __ng_list_del_entry(entry);
  __ng_list_poison_entry(entry);
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void ng_list_replace(ng_list_head_s *old,
        ng_list_head_s *_new)
{
  _new->next = old->next;
  _new->next->prev = _new;
  _new->prev = old->prev;
  _new->prev->next = _new;
}

/**
 * list_replace_init - replace old entry by new one and initialize the old one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void ng_list_replace_init(ng_list_head_s *old,
             ng_list_head_s *_new)
{
  ng_list_replace(old, _new);
  ng_INIT_LIST_HEAD(old);
}

/**
 * list_swap - replace entry1 with entry2 and re-add entry1 at entry2's position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
static inline void ng_list_swap(ng_list_head_s *entry1,
           ng_list_head_s *entry2)
{
  ng_list_head_s *pos = entry2->prev;

  ng_list_del(entry2);
  ng_list_replace(entry1, entry2);
  if (pos == entry1)
    pos = entry2;
  ng_list_add(entry1, pos);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void ng_list_del_init(ng_list_head_s *entry)
{
  __ng_list_del_entry(entry);
  ng_INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void ng_list_move(ng_list_head_s *list, ng_list_head_s *head)
{
  __ng_list_del_entry(list);
  ng_list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void ng_list_move_tail(ng_list_head_s *list,
          ng_list_head_s *head)
{
  __ng_list_del_entry(list);
  ng_list_add_tail(list, head);
}

/**
 * list_bulk_move_tail - move a subsection of a list to its tail
 * @head: the head that will follow our entry
 * @first: first entry to move
 * @last: last entry to move, can be the same as first
 *
 * Move all entries between @first and including @last before @head.
 * All three entries must belong to the same linked list.
 */
static inline void ng_list_bulk_move_tail(ng_list_head_s *head,
               ng_list_head_s *first,
               ng_list_head_s *last)
{
  first->prev->next = last->next;
  last->next->prev = first->prev;

  head->prev->next = first;
  first->prev = head->prev;

  last->next = head;
  head->prev = last;
}

/**
 * list_is_first -- tests whether @list is the first entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int ng_list_is_first(const ng_list_head_s *list, const ng_list_head_s *head)
{
  return list->prev == head;
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int ng_list_is_last(const ng_list_head_s *list, const ng_list_head_s *head)
{
  return list->next == head;
}

/**
 * list_is_head - tests whether @list is the list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int ng_list_is_head(const ng_list_head_s *list, const ng_list_head_s *head)
{
  return list == head;
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void ng_list_rotate_left(ng_list_head_s *head)
{
  ng_list_head_s *first;

  if (!ng_list_empty(head)) {
    first = head->next;
    ng_list_move_tail(first, head);
  }
}

/**
 * list_rotate_to_front() - Rotate list to specific item.
 * @list: The desired new front of the list.
 * @head: The head of the list.
 *
 * Rotates list so that @list becomes the new front of the list.
 */
static inline void ng_list_rotate_to_front(ng_list_head_s *list,
          ng_list_head_s *head)
{
  /*
   * Deletes the list head from the list denoted by @head and
   * places it as the tail of @list, this effectively rotates the
   * list so that @list is at the front.
   */
  ng_list_move_tail(head, list);
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int ng_list_is_singular(const ng_list_head_s *head)
{
  return !ng_list_empty(head) && (head->next == head->prev);
}

static inline void __ng_list_cut_position(ng_list_head_s *list,
    ng_list_head_s *head, ng_list_head_s *entry)
{
  ng_list_head_s *new_first = entry->next;
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry;
  entry->next = list;
  head->next = new_first;
  new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *  and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void ng_list_cut_position(ng_list_head_s *list,
    ng_list_head_s *head, ng_list_head_s *entry)
{
  if (ng_list_empty(head))
    return;
  if (ng_list_is_singular(head) && !ng_list_is_head(entry, head) && (entry != head->next))
    return;
  if (ng_list_is_head(entry, head))
    ng_INIT_LIST_HEAD(list);
  else
    __ng_list_cut_position(list, head, entry);
}

/**
 * list_cut_before - cut a list into two, before given entry
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *
 * This helper moves the initial part of @head, up to but
 * excluding @entry, from @head to @list.  You should pass
 * in @entry an element you know is on @head.  @list should
 * be an empty list or a list you do not care about losing
 * its data.
 * If @entry == @head, all entries on @head are moved to
 * @list.
 */
static inline void ng_list_cut_before(ng_list_head_s *list,
           ng_list_head_s *head,
           ng_list_head_s *entry)
{
  if (head->next == entry) {
    ng_INIT_LIST_HEAD(list);
    return;
  }
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry->prev;
  list->prev->next = list;
  head->next = entry;
  entry->prev = head;
}

static inline void __ng_list_splice(const ng_list_head_s *list,
         ng_list_head_s *prev,
         ng_list_head_s *next)
{
  ng_list_head_s *first = list->next;
  ng_list_head_s *last = list->prev;

  first->prev = prev;
  prev->next = first;

  last->next = next;
  next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void ng_list_splice(const ng_list_head_s *list,
        ng_list_head_s *head)
{
  if (!ng_list_empty(list))
    __ng_list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void ng_list_splice_tail(ng_list_head_s *list,
        ng_list_head_s *head)
{
  if (!ng_list_empty(list))
    __ng_list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void ng_list_splice_init(ng_list_head_s *list,
            ng_list_head_s *head)
{
  if (!ng_list_empty(list)) {
    __ng_list_splice(list, head, head->next);
    ng_INIT_LIST_HEAD(list);
  }
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void ng_list_splice_tail_init(ng_list_head_s *list,
           ng_list_head_s *head)
{
  if (!ng_list_empty(list)) {
    __ng_list_splice(list, head->prev, head);
    ng_INIT_LIST_HEAD(list);
  }
}

/**
 * list_entry - get the struct for this entry
 * @ptr:  the &list_head_s pointer.
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_entry(ptr, type, member) \
  container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:  the list head to take the element from.
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define ng_list_first_entry(ptr, type, member) \
  ng_list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:  the list head to take the element from.
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define ng_list_last_entry(ptr, type, member) \
  ng_list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:  the list head to take the element from.
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define ng_list_first_entry_or_null(ptr, type, member) ({ \
  ng_list_head_s *head__ = (ptr); \
  ng_list_head_s *pos__ = ng_READ_ONCE(head__->next); \
  pos__ != head__ ? ng_list_entry(pos__, type, member) : NULL; \
})

/**
 * ng_list_get_from_head - remove one element from list
 * @head:  the list head to take the element from.
 * @ptr:  the type * to cursor
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_get_from_head(head, ptr, type, member) do { \
  ptr = ng_list_first_entry_or_null(head, type, member); \
  if (ptr != NULL) ng_list_del(&(ptr)->member); \
} while(0)
  
/**
 * ng_list_get_from_head - remove one element from list
 * @head:  the list head to take the element from.
 * @ptr:  the type * to cursor
 * @type:  the type of the struct this is embedded in.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_get_from_tail(head, ptr, type, member) do { \
    if (!ng_list_empty(head)) {  \
      (ptr) = ng_list_last_entry(head, type, member); \
      NG_ASSERT(ptr != NULL); \
      ng_list_del(&(ptr)->member); \
    }  \
    else \
      (ptr) = NULL; \
  } while(0)

/**
 * list_next_entry - get the next element in list
 * @pos:  the type * to cursor
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_next_entry(pos, type, member) \
  ng_list_entry((pos)->member.next, type, member)

/**
 * list_next_entry_circular - get the next element in list
 * @pos:  the type * to cursor.
 * @head:  the list head to take the element from.
 * @member:  the name of the list_head within the struct.
 *
 * Wraparound if pos is the last element (return the first element).
 * Note, that list is expected to be not empty.
 */
#define ng_list_next_entry_circular(pos, type, head, member) \
  (ng_list_is_last(&(pos)->member, head) ? \
  ng_list_first_entry(head, type, member) : ng_list_next_entry(pos, type,member))

/**
 * list_prev_entry - get the prev element in list
 * @pos:  the type * to cursor
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_prev_entry(pos, type, member) \
  ng_list_entry((pos)->member.prev, type, member)

/**
 * list_prev_entry_circular - get the prev element in list
 * @pos:  the type * to cursor.
 * @head:  the list head to take the element from.
 * @member:  the name of the list_head within the struct.
 *
 * Wraparound if pos is the first element (return the last element).
 * Note, that list is expected to be not empty.
 */
#define ng_list_prev_entry_circular(pos, type,  head, member) \
  (ng_list_is_first(&(pos)->member, head) ? \
  ng_list_last_entry(head, type, member) : ng_list_prev_entry(pos, type, member))

/**
 * list_for_each  -  iterate over a list
 * @pos:  the &list_head_s to use as a loop cursor.
 * @head:  the head for your list.
 */
#define ng_list_for_each(pos, head) \
  for (pos = (head)->next; !ng_list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_continue - continue iteration over a list
 * @pos:  the &list_head_s to use as a loop cursor.
 * @head:  the head for your list.
 *
 * Continue to iterate over a list, continuing after the current position.
 */
#define ng_list_for_each_continue(pos, head) \
  for (pos = pos->next; !ng_list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_prev  -  iterate over a list backwards
 * @pos:  the &list_head_s to use as a loop cursor.
 * @head:  the head for your list.
 */
#define ng_list_for_each_prev(pos, head) \
  for (pos = (head)->prev; !ng_list_is_head(pos, (head)); pos = pos->prev)

/**
 * list_entry_is_head - test if the entry points to the head of the list
 * @pos:  the type * to cursor
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_entry_is_head(pos, head, member)        \
  (&pos->member == (head))

/**
 * list_for_each_entry  -  iterate over list of given type
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_for_each_entry(pos, type,  head, member)        \
  for (pos = ng_list_first_entry(head, type, member);  \
       !ng_list_entry_is_head(pos, head, member);      \
       pos = ng_list_next_entry(pos, type, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:  the type * to use as a loop cursor.
 * @n:    another type * to use as temporary storage
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ng_list_for_each_entry_safe(pos, type, n, head, member)      \
  for (pos = ng_list_first_entry(head, type, member),  \
    n = ng_list_next_entry(pos, type, member);     \
       !ng_list_entry_is_head(pos, head, member);      \
       pos = n, n = ng_list_next_entry(n, type, member))
       
/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 */
#define ng_list_for_each_entry_reverse(pos, type,  head, member)      \
  for (pos = ng_list_last_entry(head, type, member);    \
       !ng_list_entry_is_head(pos, head, member);       \
       pos = ng_list_prev_entry(pos, type, member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:  the type * to use as a start point
 * @head:  the head of the list
 * @member:  the name of the list_head within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define ng_list_prepare_entry(pos, type,  head, member) \
  ((pos) ? : ng_list_entry(head, type, member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define ng_list_for_each_entry_continue(pos, type,  head, member)     \
  for (pos = ng_list_next_entry(pos, type, member);      \
       !ng_list_entry_is_head(pos, head, member);      \
       pos = ng_list_next_entry(pos, type, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define ng_list_for_each_entry_continue_reverse(pos, type,  head, member)    \
  for (pos = ng_list_prev_entry(pos, type, member);      \
       !ng_list_entry_is_head(pos, head, member);      \
       pos = ng_list_prev_entry(pos, type, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define ng_list_for_each_entry_from(pos, type, head, member)       \
  for (; !ng_list_entry_is_head(pos, head, member);      \
       pos = ng_list_next_entry(pos, type, member))

/**
 * list_for_each_entry_from_reverse - iterate backwards over list of given type
 *                                    from the current point
 * @pos:  the type * to use as a loop cursor.
 * @head:  the head for your list.
 * @member:  the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, continuing from current position.
 */
#define ng_list_for_each_entry_from_reverse(pos, type, head, member)    \
  for (; !ng_list_entry_is_head(pos, head, member);      \
       pos = ng_list_prev_entry(pos, type, member))
    
#define ng_list_cmp_add_before(ele, type, head, member, cmp, data) \
      do { \
        ng_list_head_s *c;  \
        ng_list_for_each(c, head) \
        { \
          type *t = ng_list_entry(c,type,member); \
          if (cmp(t, ele, data)) \
            break; \
        } \
        ng_list_add_before(&(ele)->member, c); \
      } while(0)
    
#define list_cmp_add_after(ele, type, head, member, cmp, data) \
      do { \
        list_head_s *c;  \
        list_for_each(c, head) \
        { \
          type *t = list_entry(c,type,member); \
          if (cmp(t, ele, data)) \
            break; \
        } \
        ng_list_add_after(&(ele)->member, c); \
      } while(0)

static inline ng_list_head_s *ng_list_first(ng_list_head_s *head)
{
  return head->next;
}
          
#ifdef __cplusplus
          }
#endif

#endif

