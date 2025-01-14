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
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-mime.h"
#include "nanohttp-request.h"
#include "nanohttp-time.h"
#include "nanohttp-header.h"
#include "nanohttp-system.h"

static hrequest_s *hrequest_new(void)
{
  hrequest_s *req;

  req = (hrequest_s *)http_malloc(sizeof(*req));
  if (req == NULL)
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    goto clean0;
  }

  req->method                       = HTTP_REQUEST_METHOD_UNKOWN;
  req->version                      = HTTP_VERSION_1_1;
  req->in                           = NULL;
  req->attachments                  = NULL;
  req->content_type                 = NULL;
  req->content_length               = -1;

#ifdef __NHTTP_INTERNAL
  req->statistics.time              = ng_get_time();
  req->statistics.bytes_transmitted = 0;
  req->statistics.bytes_received    = 0;
#endif

  ng_block_init(&req->path);
  ng_INIT_LIST_HEAD(&req->header);
  ng_INIT_LIST_HEAD(&req->query);
  
clean0:
  return req;
}

static hrequest_s *
_hrequest_parse_header(char *data, ng_size_t len)
{
  hpair_s *tmppair;
  hrequest_s *req;

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
    result = ng_memchr(tmp, '\n', end - tmp);
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
      result = (char *) ng_memchr(key, ' ', t - key);
      if (result == NULL)
        break;
      
      /* save method (get or post) */
      key_len = result - key;
      if (key_len == 3 && !ng_memcmp(key, "GET", 3))
        req->method = HTTP_REQUEST_METHOD_GET;
      else if (key_len == 4 && !ng_memcmp(key, "POST", 4))
        req->method = HTTP_REQUEST_METHOD_POST;
      else if (key_len == 7 && !ng_memcmp(key, "OPTIONS", 7))
        req->method = HTTP_REQUEST_METHOD_OPTIONS;
      else if (key_len == 4 && !ng_memcmp(key, "HEAD", 4))
        req->method = HTTP_REQUEST_METHOD_HEAD;
      else if (key_len == 3 && !ng_memcmp(key, "PUT", 3))
        req->method = HTTP_REQUEST_METHOD_PUT;
      else if (key_len == 6 && !ng_memcmp(key, "DELETE", 6))
        req->method = HTTP_REQUEST_METHOD_DELETE;
      else if (key_len == 5 && !ng_memcmp(key, "TRACE", 5))
        req->method = HTTP_REQUEST_METHOD_TRACE;
      else if (key_len == 7 && !ng_memcmp(key, "CONNECT", 7))
        req->method = HTTP_REQUEST_METHOD_CONNECT;
      else
        req->method = HTTP_REQUEST_METHOD_UNKOWN;

      if (req->method > HTTP_REQUEST_METHOD_GET)
      {
        log_error("Not supported request method (%.*s)", 
          key_len, key);
        goto clean1;
      }

      /* below is key the path and tmp2 the spec */
      result++;
      key = (char *)ng_memchr(result, ' ', t - result);
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
        if (*(ng_uint64_t *)(key + 1) == *(const ng_uint64_t *)"HTTP/1.0")
          req->version = HTTP_VERSION_1_0;
        else if (*(ng_uint64_t *)(key + 1) == *(const ng_uint64_t *)"HTTP/1.1")
          req->version = HTTP_VERSION_1_1;
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
      opt_key = ng_memchr(result, '?', key-result);

      /* save path */
      /* req->path = (char *) http_malloc(strlen(key) + 1); */
      req->path.len = opt_key == NULL ? key-result : opt_key - result;
      if (req->path.len >= REQUEST_MAX_PATH_SIZE - 1)
      {
        log_error("Request path length is too large (%d), MAX:", 
          req->path.len, REQUEST_MAX_PATH_SIZE);
        goto clean1;
      }
      req->path.buf = http_strdup_size(result, req->path.len);
      if (req->path.buf == NULL)
      {
        req->path.len = 0;
        log_error("Failed to malloc buffer for path (%s)", 
          os_strerror(ng_errno));
        goto clean1;
      }

      /* parse options */
      result = opt_key;
      t = key;
      if (result != NULL)
      {
        for (;result;)
        {
          ng_block_s k, v;
          hpair_s *tmppair;
          
          result++;
          opt_key = result;
          key = ng_memchr(result, '&', t - result);
          if (key == NULL)
          {
            key = t;
            result = NULL;
          }
          else
          {
            result = key;
          }
          
          opt_value = ng_memchr(opt_key, '=', key - opt_key);
          
          /* create option pair */
          if (!(tmppair = (hpair_s *)http_malloc(sizeof(hpair_s))))
          {
            log_error("http_malloc failed (%s)", os_strerror(ng_errno));
            goto clean1;
          }

          /* fill hpairnode_t struct */
          if (opt_value != NULL)
          {
            v.buf = opt_value+1;
            v.len = key - v.buf;
            k.len = opt_value - opt_key;
            k.buf = opt_key;
          }
          else
          {
            k.len = key - opt_key;
            k.buf = opt_key;
            v.buf = NULL;
            v.len = 0;
          }
          tmppair = hpairnode_new(&k, &v, &req->query);
          if (tmppair == NULL)
          {
            log_error("hpairnode_new failed (%s)", os_strerror(ng_errno));
            goto clean1;
          }
        }
      }
    }
    else
    {
      if (result - key == 0)
        continue;
      tmppair = hpairnode_parse(key, result-key, ':', &req->header);
      if (tmppair == NULL)
      {
        log_error("hpairnode_parse failed.");
        goto clean1;
      }
    }
  }

  /* Check Content-type */
  tmppair = hpairnode_get_ignore_case(&req->header, 
                      __HDR_BUF(HEADER_CONTENT_TYPE));
  if (tmppair != NULL)
  {
    req->content_type = content_type_new(tmppair->val.cptr, 
              tmppair->val.len);
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
hrequest_free(hrequest_s * req)
{
  if (req == NULL)
    return;

  hpairnode_free_deep(&req->header);
  hpairnode_free_deep(&req->query);

  if (req->in)
    http_input_stream_free(req->in);

  if (req->content_type)
    content_type_free(req->content_type);

  if (req->attachments)
    attachments_free((mime_attachment_s *)req->attachments);

  ng_free_data_block(&req->path);
  http_free(req);

  return;
}

herror_t
hrequest_new_from_socket(hsocket_s *sock, hrequest_s **out)
{
  ng_size_t hdrlen;
  ng_size_t rcvbytes;
  herror_t status;
  char *buffer;
  hrequest_s *req;
  ng_buffer_s data;

  buffer = http_malloc(MAX_HEADER_SIZE + 1);
  if (buffer == NULL)
  {
    status = herror_new("hrequest_new_from_socket", GENERAL_ERROR, 
      "http_malloc failed.");
    goto clean0;
  }
  
  if ((status = http_header_recv(sock, buffer, MAX_HEADER_SIZE, 
    &hdrlen, &rcvbytes)) != H_OK)
  {
    log_error("http_header_recv failed (%s).", herror_message(status));
    goto clean1;
  }

  /* Create response */
  req = _hrequest_parse_header(buffer, hdrlen);
  if (req == NULL)
  {
    status = herror_new("hrequest_new_from_socket", 
      GENERAL_HEADER_PARSE_ERROR, 
      "_hrequest_parse_header failed.");
    goto clean1;
  }

  data.buf  = buffer;
  data.p    = buffer + hdrlen;
  data.len  = rcvbytes - hdrlen;
  data.size = MAX_HEADER_SIZE + 1;
  
  hpairnode_dump_deep(&req->header);
  
  /* Create input stream */
  req->in = http_input_stream_new(sock, &req->header, &data);
  if (req->in == NULL)
  {
    status = herror_new("hrequest_new_from_socket", GENERAL_ERROR, 
      "http_input_stream_new failed.");
    goto clean2;
  }

  *out = req;
  return H_OK;
  
clean2:  
  hrequest_free(req);
clean1:  
  http_free(buffer);
clean0:  
  return status;
}
