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
/** @file nanohttp-request.c HTTP request handling */
/******************************************************************
*  $Id: nanohttp-request.c,v 1.21 2007/11/03 22:40:11 m0gg Exp $
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

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-mime.h"
#include "nanohttp-request.h"

static struct hrequest_t *
hrequest_new(void)
{
  struct hrequest_t *req;
 
  if (!(req = (struct hrequest_t *)http_malloc(sizeof(*req))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  if (!(req->statistics = (struct request_statistics *)
    http_malloc(sizeof(*req->statistics))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    http_free(req);
    return NULL;
  }

  if (gettimeofday(&(req->statistics->time), NULL) < 0)
    log_error("gettimeofday failed (%s)", strerror(errno));

  req->method = HTTP_REQUEST_GET;
  req->version = HTTP_1_1;
  req->query = NULL;
  req->header = NULL;
  req->in = NULL;
  req->attachments = NULL;
  req->content_type = NULL;

  return req;
}

static struct hrequest_t *
_hrequest_parse_header(char *data, size_t len)
{
  struct hrequest_t *req;
  hpair_t *hpair = NULL, *qpair = NULL, *tmppair = NULL;

  char *tmp;
  char *result;
  char *key;
  char *end;
  char *opt_key;
  char *opt_value;
  int firstline = 1;

  req = hrequest_new();
  if (req == NULL)
  {
    log_error("hrequest_new failed.");
    goto clean0;
  }
  tmp = data;
  end = data + len;
  
  for (;;)
  {
    if (end - tmp < 2)
    {
      if (end == tmp)
        break;
      if (tmp[0] != '\n')
      {
        log_error("Bad header.");
        goto clean1;
      }
    }
    result = memchr(tmp, '\n', end - tmp);
    if (result == NULL)
      break;
    if (*--result!='\r')
      continue;
    
    key = tmp;
    tmp = result + 2;
    
    if (firstline)
    {
      int key_len;
      char *t;
      firstline = 0;

      /* parse [GET|POST] [PATH] [SPEC] */
      t = result;
      result = (char *) memchr(key, ' ', t - key);
      if (result == NULL)
        break;
      
      /* save method (get or post) */
      key_len = result - key;
      if (key_len == 4 && !memcmp(key, "POST", 4))
        req->method = HTTP_REQUEST_POST;
      else if (key_len == 3 && !memcmp(key, "GET", 3))
        req->method = HTTP_REQUEST_GET;
      else if (key_len == 7 && !memcmp(key, "OPTIONS", 7))
        req->method = HTTP_REQUEST_OPTIONS;
      else if (key_len == 4 && !memcmp(key, "HEAD", 4))
        req->method = HTTP_REQUEST_HEAD;
      else if (key_len == 3 && !memcmp(key, "PUT", 3))
        req->method = HTTP_REQUEST_PUT;
      else if (key_len == 6 && !memcmp(key, "DELETE", 6))
        req->method = HTTP_REQUEST_DELETE;
      else if (key_len == 5 && !memcmp(key, "TRACE", 5))
        req->method = HTTP_REQUEST_TRACE;
      else if (key_len == 7 && !memcmp(key, "CONNECT", 7))
        req->method = HTTP_REQUEST_CONNECT;
      else
        req->method = HTTP_REQUEST_UNKOWN;

      if (req->method > HTTP_REQUEST_GET)
      {
        log_error("Not supported request method (%.*s)", 
          key_len, key);
        goto clean1;
      }

      /* below is key the path and tmp2 the spec */
      result++;
      key = (char *)memchr(result, ' ', t - result);
      /* save version */
      if (key == NULL)
      {
        log_error("Not supported http version (%.*s)", key_len, key);
        goto clean1;
      }

      req->version = -1;
      /* req->spec = (char *) http_malloc(strlen(tmp2) + 1); 
        strcpy(req->spec, tmp2); */
      if (t - key - 1 == 8)
      {
        if (*(uint64_t *)(key + 1) == *(const uint64_t *)"HTTP/1.0")
          req->version = HTTP_1_0;
        else if (*(uint64_t *)(key + 1) == *(const uint64_t *)"HTTP/1.1")
          req->version = HTTP_1_1;
      }
      if (req->version == -1)
      {
        log_error("Not supported http version (%.*s)", 
          t - key - 1, key + 1);
        goto clean1;
      }
      
      /* 
       * parse and save path+query parse:
       * /path/of/target?key1=value1&key2=value2...
       */
      opt_key = memchr(result, '?', key-result);

      /* save path */
      /* req->path = (char *) http_malloc(strlen(key) + 1); */
      req->path_len = opt_key == NULL ? key-result : opt_key - result;
      memcpy(req->path, result, RTE_MIN(req->path_len, sizeof(req->path)-1));
      req->path[req->path_len] = '\0';

      /* parse options */
      result = opt_key;
      t = key;
      if (result != NULL)
      {
        for (;result;)
        {
          result++;
          opt_key = result;
          key = memchr(result, '&', t - result);
          if (key == NULL)
          {
            key = t;
            result = NULL;
          }
          else
          {
            result = key;
          }
          
          opt_value = memchr(opt_key, '=', key - opt_key);

          /* create option pair */
          if (!(tmppair = (hpair_t *)http_malloc(sizeof(hpair_t))))
          {
            log_error("http_malloc failed (%s)", strerror(errno));
            goto clean1;
          }

          /* fill hpairnode_t struct */
          tmppair->next = NULL;
          tmppair->value_len = opt_value == NULL ? 0 : key - opt_value - 1;
          tmppair->key_len = opt_value == NULL ? key - opt_key : opt_value - opt_key;
          tmppair->key = http_strdup_size(opt_key, tmppair->key_len);
          if (opt_value)
            tmppair->value = http_strdup_size(opt_value+1, tmppair->value_len);
          
          if (req->query == NULL)
          {
            req->query = tmppair;
          }
          else
          {
            qpair->next = tmppair;
           }
          qpair = tmppair;
        }
      }
    }
    else
    {
      if (result - key == 0)
        continue;
      tmppair = hpairnode_parse(key, result-key, ':', NULL);
      if (tmppair == NULL)
      {
        log_error("hpairnode_parse failed.");
        goto clean1;
      }
        
      if (req->header == NULL)
      {
        req->header = hpair = tmppair;
      }
      else
      {
        hpair->next = tmppair;
        hpair = tmppair;
      }
    }
  }

  /* Check Content-type */
  tmppair = hpairnode_get_ignore_case_len(req->header, 
                      HEADER_CONTENT_TYPE, 12);
  if (tmppair != NULL)
  {
    req->content_type = content_type_new(tmppair->value, tmppair->value_len);
    if (req->content_type == NULL)
    {
      log_error("content_type_new failed.");
      goto clean1;
    }
  }
  return req;
  
clean1:
  hrequest_free(req);
clean0:
  return NULL;
}

void
hrequest_free_data(struct hrequest_t *req)
{
  if (req->data.buf != NULL)
  {
    http_free(req->data.buf);
    req->data.buf = NULL;
  }
}

void
hrequest_free(struct hrequest_t * req)
{
  if (req == NULL)
    return;

  hpairnode_free_deep(req->header);
  hpairnode_free_deep(req->query);

  if (req->in)
    http_input_stream_free(req->in);

  if (req->content_type)
    content_type_free(req->content_type);

  if (req->attachments)
    attachments_free(req->attachments);

  if (req->statistics)
    http_free(req->statistics);

  hrequest_free_data(req);
  http_free(req);

  return;
}

herror_t
hrequest_new_from_socket(struct hsocket_t *sock, struct hrequest_t **out)
{
  size_t hdrlen;
  size_t rcvbytes;
  herror_t status = H_OK;
  char *buffer;
  content_type_t *ct;
  struct hrequest_t *req;
  struct attachments_t *mimeMessage;

  buffer = http_malloc(MAX_HEADER_SIZE + 1);
  if (buffer == NULL)
  {
    status = herror_new("hrequest_new_from_socket", GENERAL_ERROR, 
      "http_malloc failed.");
    goto clean0;
  }
  
  if ((status = http_header_recv(sock, buffer, MAX_HEADER_SIZE, &hdrlen, &rcvbytes)) != H_OK)
  {
    goto clean1;
  }

  /* Create response */
  req = _hrequest_parse_header(buffer, hdrlen);
  if (req == NULL)
  {
    status = herror_new("hrequest_new_from_socket", GENERAL_HEADER_PARSE_ERROR, 
      "_hrequest_parse_header failed.");
    goto clean1;
  }

  req->data.buf  = buffer;
  req->data.p    = buffer + hdrlen;
  req->data.len  = rcvbytes - hdrlen;
  req->data.size = MAX_HEADER_SIZE + 1;
  hpairnode_dump_deep(req->header);
  
  /* Check for MIME message */
  ct = req->content_type;
  if ((ct != NULL && ct->type_len == 17 && !memcmp(ct->type, "multipart/related", 17)))
  {
    status = mime_get_attachments(req->content_type, req->in, &mimeMessage);
    if (status != H_OK)
    {
      /* TODO (#1#): Handle error */
      goto clean2;
    }
    else
    {
      req->attachments = mimeMessage;
      req->in =
        http_input_stream_new_from_file(mimeMessage->root_part->filename);
    }
  }
  else
  {
    /* Create input stream */
    req->in = http_input_stream_new(sock, req->header);
  }

  if (req->in == NULL)
  {
    status = herror_new("hrequest_new_from_socket", GENERAL_ERROR, 
      "http_input_stream_new failed.");
    goto clean2;
  }

  if (ct == NULL || ct->type_len != 19 
    || memcmp(ct->type, "multipart/form-data", 19))
  {
    hrequest_free_data(req);
  }
  
  *out = req;
  return H_OK;
  
clean2:  
  hrequest_free(req);
  return status;
clean1:  
  http_free(buffer);
clean0:  
  return status;
}
