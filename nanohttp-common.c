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
/** @file nanohttp-common.c Common functions and definitions */
/******************************************************************
*  $Id: nanohttp-common.c,v 1.39 2007/11/03 22:40:10 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
*
* This library is ng_free software; you can redistribute it and/or
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
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-defs.h"
#include "nanohttp-common.h"
#include "nanohttp-ctype.h"
#include "nanohttp-header.h"

hpair_s *
hpairnode_new(const ng_block_s *key, const ng_block_s *val, 
  ng_list_head_s *head)
{
  hpair_s *pair;

  log_verbose("new pair ('%pS','%pS').", key, val);

  pair = (hpair_s *)ng_malloc(sizeof(hpair_s));
  if (pair == NULL)
  {
    log_error("ng_malloc failed.");
    goto clean0;
  }

  if (0 > ng_dup_data_block(&pair->key, key, ng_FALSE))
  {
    log_error("ng_dup_data_block key failed.");
    goto clean1;
  }

  if (0 > ng_dup_data_block(&pair->val, val, ng_FALSE))
  {
    log_error("ng_dup_data_block val failed.");
    goto clean2;
  }

  if (head != NULL)
    ng_list_add_tail(&pair->link, head);
  
  return pair;

clean2:
  ng_free_data_block(&pair->key);
clean1:  
  ng_free(pair);
clean0:  
  return NULL;
}

hpair_s *
hpairnode_parse(const char *str, int _size, char delim, 
  ng_list_head_s *head)
{
  ng_block_s key;
  ng_block_s val;
  const char *value;
  int size = _size ? _size : ng_strlen(str);
  
  if (str == NULL || !_size)
  {
    log_error("bad header key.");
    return NULL;
  }
  
  value = ng_memchr(str, delim, size);
  if (value != NULL)
  {
    ng_block_set(&key, str, value - str);
    /* skip white space */
    for (value++; __ng_isspace((int)*value); value++) ;
    ng_block_set(&val, value, size - (value - str));
  }
  else
  {
    ng_block_set(&key, str, size);
    ng_block_init(&val);
  }

  return hpairnode_new(&key, &val, head);
}

hpair_s *
hpairnode_copy(const hpair_s *src, ng_list_head_s *head)
{
  if (src == NULL)
    return NULL;

  return hpairnode_new(&src->key, &src->val, head);
}

int
hpairnode_copy_deep(const ng_list_head_s *src, 
  ng_list_head_s *dst)
{
  hpair_s *pair;

  if (src == NULL)
    return -1;

  ng_list_for_each_entry(pair, hpair_s, src, link)
  {
    if (NULL == hpairnode_copy(pair, dst))
      return -1;
  }

  return 0;
}

void
hpairnode_dump(const hpair_s *pair)
{
  if (pair == NULL)
  {
    log_verbose("(NULL)[]");
    return;
  }
  log_verbose("%pS: %pS.", &pair->key, &pair->val);

  return;
}

void
__hpairnode_dump_deep(const ng_list_head_s *head)
{
  const hpair_s *p;

  log_verbose("-- BEGIN dump_deep hpair_s --");
  ng_list_for_each_entry(p, hpair_s, head, link)
  {
    hpairnode_dump(p);
  }
  log_verbose("-- END dump_deep hpair_s --");

  return;
}

void
hpairnode_free(hpair_s *pair)
{
  if (pair == NULL)
    return;

  ng_free_data_block(&pair->key);
  ng_free_data_block(&pair->val);
  ng_free(pair);

  return;
}

void
hpairnode_free_deep(ng_list_head_s *head)
{
  while (!ng_list_empty(head))
  {
    hpair_s *pair;
    pair = ng_list_first_entry(head,hpair_s,link);
    assert(pair != NULL);
    ng_list_del(&pair->link);
    hpairnode_free(pair);
  }

  return;
}

hpair_s *
hpairnode_get_ignore_case(ng_list_head_s *head, 
  const char *key, int len)
{
  if (key == NULL || !len)
  {
    log_error("key is NULL.");
    return NULL;
  }

  hpair_s *pair;
  const ng_block_s b={.cptr=key, .len = len};
  
  ng_list_for_each_entry(pair, hpair_s, head, link)
  {
    if (ng_block_isequal_nocase(&pair->key, &b))
    {
      return pair;
    }
  }

  return NULL;
}

hpair_s *
hpairnode_get(ng_list_head_s *head, const ng_block_s *key)
{
  if (key == NULL)
  {
    log_error("key is NULL.");
    return NULL;
  }

  hpair_s *pair;
  ng_list_for_each_entry(pair, hpair_s, head, link)
  {
    if (ng_block_isequal__(&pair->key, key))
    {
      return pair;
    }
  }

  return NULL;
}

/**
 *
 * Adds a header element (key, value) pair.
 *
 * @return 0 on success, -1 on failure.
 *
 * @see httpc_set_header
 * @see httpc_add_headers
 * @see http_general_header_fields
 * @see http_request_header_fields
 *
 */
int
hpairnode_add_header(ng_list_head_s *header, const ng_block_s *key, 
  const ng_block_s *val)
{
  if (0 > hpairnode_new(key, val, header))
  {
    log_fatal("hpairnode_new_len failed.");
    return -1;
  }

  return 0;
}

/**
 *
 * Adds a list of (key, value) pairs.
 *
 * @see httpc_set_header
 * @see httpc_add_header
 * @see http_general_header_fields
 * @see http_request_header_fields
 *
 */
int
hpairnode_add_header_list(ng_list_head_s *header, 
  const ng_list_head_s *values)
{
  hpair_s *p;
  
  ng_list_for_each_entry(p,hpair_s,values,link)
  {
    if (0 > hpairnode_add_header(header, &p->key, &p->val))
    {
      log_fatal("hpairnode_add_header_list failed.");
      return -1;
    }
  }

  return 0;
}

/**
 *
 * Sets header element (key,value) pair.
 *
 * @return 0 on success or failure (yeah!), 1 if a (key,value) pair was replaced.
 *
 * @see httpc_add_header
 * @see httpc_add_headers
 * @see http_general_header_fields
 * @see http_request_header_fields
 *
 */
int
hpairnode_set_header(ng_list_head_s *head, const ng_block_s *key, 
  const ng_block_s *val)
{
  hpair_s *p;

  p = hpairnode_get(head, key);
  if (p != NULL)
  {
    if (val->len)
    {
      void *data = ng_malloc(val->len);
      if (data == NULL)
      {
        log_fatal("ng_malloc for value failed.");
        return -1;
      }
      ng_memcpy(data, val->data, val->len);
      ng_free(p->val.data);
      p->val.data = data;
      p->val.len  = val->len;
    }
  }
  else
  {
    p = hpairnode_new(key, val, head);
    if (p == NULL)
    {
      log_fatal("hpairnode_new failed.");
      return -1;
    }
  }

  return 0;
}

int
hpairnode_set_header_list(ng_list_head_s *dst, const ng_list_head_s *src)
{
  hpair_s *p;
  
  ng_list_for_each_entry(p, hpair_s, src, link)
  {
    if (0 > hpairnode_set_header(dst, &p->key, &p->val))
    {
      log_fatal("hpairnode_add_header_list failed.");
      return -1;
    }
  }

  return 0;
}

static inline int 
__rm_ctsp(const char *content_type_str, int i, int len)
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
content_type_print(content_type_s *ct)
{
  if (ct != NULL)
  {
    hpair_s *pair;
    log_verbose(" ++++++ Parsed Content-Type :");
    log_verbose(" Content-Type : %pS", &ct->type);
    ng_list_for_each_entry(pair,hpair_s,&ct->params,link)
    {
      log_verbose("    %pS = %pS", &pair->key, &pair->val);
    }
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
content_type_s *
content_type_new(const char *content_type_str, int len)
{
  hpair_s *pair = NULL;
  content_type_s *ct = NULL;
  char ch;
  ng_block_s key;
  ng_block_s val;
  int inQuote, mode, i, c;
  
  /* 0: searching ';' 1: process key 2: process value */

  /* Create object */
  ct = (content_type_s *)ng_malloc(sizeof(*ct));
  if (ct == NULL)
  {
    log_fatal("Failed to malloc content_type_s.");
    goto clean0;
  }

  i           = 0;
  c           = 0;
  inQuote     = 0;
  mode        = 0;
  ng_block_init(&ct->type);
  ng_INIT_LIST_HEAD(&ct->params);
  ng_block_init(&key);
  ng_block_init(&val);
  
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
        ct->type.len  = c;
        ct->type.data = ng_strdup_size(content_type_str, c);
        if (ct->type.data == NULL)
        {
          log_fatal("Failed to malloc content_type_s->type.");
          goto clean1;
        }
        
        c = 0;
        mode = 1;
        i = __rm_ctsp(content_type_str, i, len);
        key.cptr = content_type_str+i;
        if (i == len)
          i++;
      }
      else
      {
        c++;
      }
      break;

    case 1:
      if (ch == '=')
      {
        key.len = c;
        c = 0;
        mode = 2;
        i = __rm_ctsp(content_type_str, i, len);
        val.cptr = content_type_str+i;
      }
      else if (ch != ' ' && ch != '\t' && ch != '\r')
      {
        c++;
      }
      else if (c)
      {
        log_warn("Bad char in content_type_s->Key %02X.", ch);
        goto clean1;
      }
      break;

    case 2:
      if ((ch == ';' || ch == ' ' || ch == '\t') && !inQuote)
      {
        val.len = c;
        pair = hpairnode_new(&key, &val, &ct->params);
        if (pair == NULL)
        {
          log_fatal("hpairnode_new failed.");
          goto clean1;
        }

        c    = 0;
        mode = 1;
        i = __rm_ctsp(content_type_str, i, len);
        key.cptr = content_type_str+i;
        key.len  = val.len = 0;
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
            val.cptr++;
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
            val.cptr++;
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
  content_type_s *ct;
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
content_type_free(content_type_s * ct)
{
  if (!ct)
    return;

  ng_free_data_block(&ct->type);
  hpairnode_free_deep(&ct->params);
  ng_free(ct);
  return;
}
