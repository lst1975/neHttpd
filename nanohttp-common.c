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
/** @file nanohttp-common.c Common functions and definitions */
/******************************************************************
*  $Id: nanohttp-common.c,v 1.39 2007/11/03 22:40:10 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
*
* This library is http_free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA  02111-1307, USA.
*
* Email: ayaz@jprogrammer.net
******************************************************************/
#include "nanohttp-config.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-defs.h"
#include "nanohttp-common.h"
#include "nanohttp-ctype.h"
#include "nanohttp-header.h"

hpair_t *
hpairnode_new_len(const char *key, int keylen, 
  const char *value, int valuelen, hpair_t *next)
{
  hpair_t *pair;

  log_verbose("new pair ('%.*s','%.*s')", keylen, key, 
    valuelen, value);
  if (!(pair = (hpair_t *) http_malloc(sizeof(hpair_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  if (key != NULL)
  {
    pair->key.len = keylen;
    pair->key.buf = http_strdup_size(key, pair->key.len);
  }
  else
  {
    pair->key.buf = NULL;
    pair->key.len = 0;
  }

  if (value != NULL)
  {
    pair->val.len = valuelen;
    pair->val.buf = http_strdup_size(value, pair->val.len);
  }
  else
  {
    pair->val.buf = NULL;
    pair->val.len = 0;
  }

  pair->next = next;

  return pair;
}

hpair_t *
hpairnode_new(const char *key, const char *value, hpair_t *next)
{
  return hpairnode_new_len(key, SAFE_STRLEN(key), value, SAFE_STRLEN(value), next);
}

hpair_t *
hpairnode_parse(const char *str, int _size, char delim, hpair_t *next)
{
  hpair_t *pair;
  const char *value;
  int size = _size ? _size : strlen(str);
  
  if (str == NULL || !_size)
  {
    log_error("bad header key.");
    goto clean0;
  }
  
  pair = (hpair_t *)http_malloc(sizeof(hpair_t));
  if (pair == NULL)
  {
    log_error("http_malloc hpair_t failed.");
    goto clean0;
  }

  ng_block_init(&pair->key);
  ng_block_init(&pair->val);
  pair->next = next;

  value = ng_memchr(str, delim, size);
  pair->key.len = value == NULL ? size : value - str;
  pair->key.buf = http_strdup_size(str, pair->key.len);
  if (pair->key.buf == NULL)
  {
    log_error("http_malloc hpair_t->key failed.");
    goto clean1;
  }
  
  if (value != NULL)
  {
    /* skip white space */
    for (value++; __ng_isspace((int)*value); value++) ;
    pair->val.len = size - (value - str);
    if (pair->val.len)
    {
      pair->val.buf = http_strdup_size(value, pair->val.len);
      if (pair->val.buf == NULL)
      {
        log_error("http_malloc hpair_t->value failed.");
        goto clean1;
      }
    }
  }
  return pair;
  
clean1:
  hpairnode_free(pair);
clean0:
  return NULL;
}

hpair_t *
hpairnode_copy(const hpair_t * src)
{
  if (src == NULL)
    return NULL;

  return hpairnode_new_len(src->key.cptr, src->key.len, 
    src->val.cptr, src->val.len, NULL);
}

hpair_t *
hpairnode_copy_deep(const hpair_t * src)
{
  hpair_t *pair, *result, *next;

  if (src == NULL)
    return NULL;

  result = hpairnode_copy(src);

  next = src->next;
  pair = result;

  while (next != NULL)
  {
    pair->next = hpairnode_copy(next);
    pair = pair->next;
    next = next->next;
  }

  return result;
}

void
hpairnode_dump(const hpair_t * pair)
{
  if (pair == NULL)
  {
    log_verbose("(NULL)[]");
    return;
  }
  log_verbose("%pS: %pS", &pair->key, &pair->val);

  return;
}

void
__hpairnode_dump_deep(const hpair_t *pairs)
{
  const hpair_t *p;

  log_verbose("-- BEGIN dump_deep hpair_t --");
  for (p = pairs; p != NULL; p = p->next)
  {
    hpairnode_dump(p);
  }
  log_verbose("-- END dump_deep hpair_t --");

  return;
}

void
hpairnode_free(hpair_t * pair)
{
  if (pair == NULL)
    return;

  ng_free_data_block(&pair->key);
  ng_free_data_block(&pair->val);
  http_free(pair);

  return;
}


void
hpairnode_free_deep(hpair_t * pair)
{
  hpair_t *tmp;

  while (pair != NULL)
  {
    tmp = pair->next;
    hpairnode_free(pair);
    pair = tmp;
  }

  return;
}

hpair_t *
hpairnode_get_ignore_case_len(hpair_t *pair, 
  const char *key, int len)
{
  if (key == NULL || !len)
  {
    log_error("key is NULL");
    return NULL;
  }

  const ng_block_s b={.cptr=key, .len = len};
  
  while (pair != NULL)
  {
    if (ng_block_isequal_nocase(&pair->key, &b))
    {
      return pair;
    }
    pair = pair->next;
  }

  return NULL;
}

char *
hpairnode_get_ignore_case(hpair_t * pair, const char *key)
{
  hpair_t *p;
  p = hpairnode_get_ignore_case_len(pair, key, SAFE_STRLEN(key));
  if (p == NULL)
    return NULL;
  return p->val.buf;
}

hpair_t *
hpairnode_get_len(hpair_t *pair, const char *key, int len)
{
  if (key == NULL)
  {
    log_error("key is NULL");
    return NULL;
  }

  const ng_block_s b={.cptr=key, .len = len};
  
  while (pair != NULL)
  {
    if (ng_block_isequal__(&pair->key, &b))
    {
      return pair;
    }
    pair = pair->next;
  }

  return NULL;
}

char *
hpairnode_get(hpair_t *pair, const char *key)
{
  hpair_t *p;
  p = hpairnode_get_len(pair, key, SAFE_STRLEN(key));
  if (p == NULL)
    return NULL;
  return p->val.buf;
}

static inline int __rm_ctsp(const char *content_type_str, int i, int len)
{
  while (i < len)
  {
    int ch = content_type_str[i];
    if (ch == ' ' || ch == '\t' || ch == '\r')
      i++;
    else
      break;
  }
  return i;
}

void
content_type_print(content_type_t *ct)
{
  hpair_t *pair;
  if (ct != NULL)
  {
    log_verbose(" ++++++ Parsed Content-Type :");
    log_verbose(" Content-Type : %s", ct->type);
    for (pair =ct->params; pair; pair = pair->next)
      log_verbose("    %pS = %pS", &pair->key, &pair->val);
  }
}

/* Content-type multipart/form-data; boundary=----WebKitFormBoundaryn5yeffAJZX0Gx6OA"
   Content-Type: application/json
   Content-Type: text/plain; charset=utf-8
   Content-Type: text/html; charset=ISO-8859-1
   Content-Type: image/jpeg; quality=80
   Content-Type: application/json; version=1.0
   Content-Type: multipart/form-data; boundary=---011000010111000001101001
   
   Content-Type: multipart/related; boundary=\"the_boundary\"; type=\"application/json\"; start=\"json\"\r\n"
*/
content_type_t *
content_type_new(const char *content_type_str, int len)
{
  hpair_t *pair = NULL, *last = NULL;
  content_type_t *ct = NULL;
  char ch;
  const char *key=NULL, *value=NULL;
  int key_len, value_len;
  int inQuote = 0, i, c = 0;
  int mode = 0;
  
  /* 0: searching ';' 1: process key 2: process value */

  /* Create object */
  ct = (content_type_t *) http_malloc(sizeof(content_type_t));
  if (ct == NULL)
  {
    log_fatal("Failed to malloc content_type_t.");
    goto clean0;
  }
  ct->params = NULL;

  i = 0;
  key_len = value_len = 0;
  while (i <= len)
  {
    if (i == len)
      ch = ';';
    else
      ch = content_type_str[i++];
    
    switch (mode)
    {
    case 0:
      if (ch == ';' || ch == ' ' || ch == '\t')
      {
        ct->type_len = c;
        ct->type[c] = '\0';
        c = 0;
        mode = 1;
        i = __rm_ctsp(content_type_str, i, len);
        key = content_type_str+i;
        if (i == len)
          i++;
      }
      else
      {
        if (c >= sizeof(ct->type))
        {
          log_warn("content_type_t->Type is too small.");
          goto clean1;
        }
        if (ch != ' ' && ch != '\t' && ch != '\r')
          ct->type[c++] = ch;
        else if (c)
        {
          log_warn("Bad char in content_type_t->Type %02X.", ch);
          goto clean1;
        }
      }
      break;

    case 1:
      if (ch == '=')
      {
        key_len = c;
        c = 0;
        mode = 2;
        i = __rm_ctsp(content_type_str, i, len);
        value = content_type_str+i;
      }
      else if (ch != ' ' && ch != '\t' && ch != '\r')
      {
        c++;
      }
      else if (c)
      {
        log_warn("Bad char in content_type_t->Key %02X.", ch);
        goto clean1;
      }
      break;

    case 2:
      if ((ch == ';' || ch == ' ' || ch == '\t') && !inQuote)
      {
        value_len = c;
        pair = hpairnode_new_len(key, key_len, value, value_len, NULL);
        if (pair == NULL)
        {
          log_fatal("Failed hpairnode_new_len.");
          goto clean1;
        }
        if (ct->params == NULL)
          ct->params = pair;
        else
          last->next = pair;
        last = pair;

        c = 0;
        mode = 1;
        i = __rm_ctsp(content_type_str, i, len);
        key = content_type_str+i;
        key_len = value_len = 0;
        if (i == len)
          i++;
      }
      else if (ch == '"')
      {
        if (c==0)
          inQuote = 1;
        else if (i == len)
        {
          if (inQuote)
          {
            value++;
            c--;
            inQuote = 0;
          }
          else
          {
            c++;
          }
        }
        else if (content_type_str[i]==';' 
          || content_type_str[i]==' ' 
          || content_type_str[i] == '\t')
        {
          if (inQuote)
          {
            value++;
            c--;
            inQuote = 0;
          }
          else
          {
            c++;
          }
        }
        else
        {
          c++;
        }
      }
      else
      {
        c++;
      }
      break;
    }
  }
  return ct;

clean1:
  content_type_free(ct);
clean0: 
  return NULL;
}

void test_content_type(void)
{
#define __T1  "multipart/form-data; boundary=---011000010111000001101001"
#define __T2  "multipart/related; boundary=\"the_boundary\"; type=\"application/json\"; start=\"json\""
  content_type_t *ct;
  ct = content_type_new(__T1, sizeof(__T1)-1);
  if (ct != NULL)
  {
    content_type_print(ct);
    content_type_free(ct);
  }
  ct = content_type_new(__T2, sizeof(__T2)-1);
  if (ct != NULL)
  {
    content_type_print(ct);
    content_type_free(ct);
  }
}

void
content_type_free(content_type_t * ct)
{
  if (!ct)
    return;

  hpairnode_free_deep(ct->params);
  http_free(ct);
  return;
}

struct part_t *
part_new(const char *id, const char *filename, const char *content_type, 
  const char *transfer_encoding, struct part_t *next)
{
  int n;
  struct part_t *part;
  hpair_t *pair;
 
  if (!(part = (struct part_t *)http_malloc(sizeof(struct part_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    goto clean0;
  }

  part->header = NULL;
  part->deleteOnExit = 0;

  n = ng_snprintf(part->id, sizeof(part->id), "%s", id);
  pair = hpairnode_new_len(__HDR_BUF(HEADER_CONTENT_ID), 
            id, n, part->header);
  if (pair == NULL)
  {
    log_error("hpairnode_new_len failed.");
    goto clean1;
  }
  part->header = pair;
  
  part->next = next;
  ng_snprintf(part->filename, sizeof(part->filename), "%s", filename);

  /* TODO (#1#): encoding is always binary. implement also others! */
  /*  part->header = hpairnode_new(HEADER_CONTENT_TRANSFER_ENCODING, "binary", part->header);*/
  ng_snprintf(part->transfer_encoding, sizeof(part->transfer_encoding), "%s",
         transfer_encoding ? transfer_encoding : "binary");

  if (content_type)
  {
    n = ng_snprintf(part->content_type, sizeof(part->content_type), "%s", content_type);
    pair = hpairnode_new_len(__HDR_BUF(HEADER_CONTENT_TYPE), content_type, n, part->header);
    if (pair == NULL)
    {
      log_error("hpairnode_new_len failed.");
      goto clean1;
    }
    part->header = pair;
  }
  else
  {
    /* TODO (#1#): get content-type from mime type list */
    part->content_type[0] = '\0';
  }

  return part;
  
clean1:
  part_free(part);
clean0:
  return NULL;
}

void
part_free(struct part_t * part)
{
  if (part == NULL)
    return;

  if (part->deleteOnExit)
  {
    remove(part->filename);
  }

  hpairnode_free_deep(part->header);

  http_free(part);
}

struct attachments_t *
attachments_new(void)               /* should be used internally */
{
  struct attachments_t *attachments;
 
  if (!(attachments = (struct attachments_t *) http_malloc(sizeof(struct attachments_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  attachments->parts = NULL;
  attachments->last = NULL;
  attachments->root_part = NULL;

  return attachments;
}

void
attachments_add_part(struct attachments_t *attachments, struct part_t *part)
{
  if (!attachments)
    return;

  if (attachments->last)
    attachments->last->next = part;
  else
    attachments->parts = part;

  attachments->last = part;

  return;
}

/*
  Free a mime message 
*/
void
attachments_free(struct attachments_t *message)
{
  struct part_t *tmp, *part;

  if (!message)
    return;

  part = message->parts;
  while (part)
  {
    tmp = part->next;
    part_free(part);
    part = tmp;
  }

  if (message->root_part)
    part_free(message->root_part);

  http_free(message);

  return;
}

