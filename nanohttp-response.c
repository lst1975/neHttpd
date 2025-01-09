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
/** @file nanohttp-response.c HTTP response handling */
/******************************************************************
*  $Id: nanohttp-response.c,v 1.19 2007/11/03 22:40:11 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003-2004  Ferhat Ayaz
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#include "nanohttp-config.h"

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
#include "nanohttp-response.h"
#include "nanohttp-header.h"

static hresponse_t *
_hresponse_new(void)
{
  hresponse_t *res;

  if (!(res = (hresponse_t *) http_malloc(sizeof(hresponse_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  res->version = HTTP_1_1;
  res->errcode = -1;
  res->desc   = NULL;
  res->header = NULL;
  res->in = NULL;
  res->content_type = NULL;
  res->attachments = NULL;
  res->data.buf = NULL;
  res->data.len = 0;
  return res;
}

static hresponse_t *
_hresponse_parse_header(const char *buffer, size_t len)
{
  hpair_t *pair;
  hresponse_t *res;
  const char *s1, *str, *end, *endp;

  /* create response object */
  res = _hresponse_new();
  if (res == NULL)
  {
    log_error("_hresponse_new Failed");
    goto clean0;
  }

  /* *** parse spec *** */
  /* [HTTP/1.1 | 1.2] [CODE] [DESC] */

  end = buffer + len;
  
  /* stage 1: HTTP spec */
  str = memchr(buffer, ' ', len);
  if (str == NULL)
  {
    log_error("Parse error reading HTTP spec");
    goto clean1;
  }

  res->version = -1;
  if (str - buffer == 8)
  {
    if (*(uint64_t *)buffer == *(const uint64_t *)"HTTP/1.0")
      res->version = HTTP_1_0;
    else if (*(uint64_t *)buffer == *(const uint64_t *)"HTTP/1.1")
      res->version = HTTP_1_1;
  }
  if (res->version == -1)
  {
    log_error("Not supported http version (%.*s)", 
      str - buffer, buffer);
    goto clean1;
  }

  /* stage 2: http code */
  s1 = str+1;
  str = memchr(s1, ' ', end - s1);
  if (str == NULL)
  {
    log_error("Parse error reading HTTP code");
    goto clean1;
  }
  res->errcode = ng_atoi32(str, str-s1, &endp);
  if (res->errcode < 0 || endp != str)
  {
    log_error("Invalid HTTP code");
    goto clean1;
  }

  /* stage 3: description text */
  s1 = str+1;
  str = memchr(s1, '\r', end - s1);
  if (unlikely(str == NULL || str + 1 >= end || str[1] != '\n'))
  {
    log_error("Parse error reading HTTP description");
    goto clean1;
  }

  /*	res->desc = (char *) http_malloc(strlen(str) + 1);*/
  res->desc_len = str - s1;
  res->desc = http_strdup_size(str, res->desc_len);
  if (res->desc == NULL)
  {
    log_error("Parse error reading HTTP description");
    goto clean1;
  }

  s1 = str + 2;
  /* *** parse header *** */
  /* [key]: [value] */
  for (;;)
  {
    /* check if header ends without body */
    str = memchr(s1, '\r', end - s1);
    if (str == NULL)
    {
      break;
    }
    /* check also for end of header */
    if (unlikely(str + 1 >= end || str[1] != '\n'))
    {
      log_error("Parse error HTTP header, need LF after CR.");
      goto clean1;
    }

    pair = hpairnode_parse(s1, str - s1, ':', NULL);
    if (pair == NULL)
    {
      log_error("hpairnode_parse failed.");
      goto clean1;
    }
    pair->next  = res->header;
    res->header = pair;
  }

  /* Check Content-type */
  pair = hpairnode_get_ignore_case_len(res->header, 
            __HDR_BUF(HEADER_CONTENT_TYPE));
  if (pair != NULL)
  {
    res->content_type = content_type_new(pair->val.cptr, pair->val.len);
    if (res->content_type == NULL)
    {
      log_error("content_type_new failed.");
      goto clean1;
    }
  }
  
  /* return response object */
  return res;
  
clean1:
  hresponse_free(res);
clean0:
  return NULL;
}

herror_t
hresponse_new_from_socket(struct hsocket_t *sock, hresponse_t **out)
{
  size_t hdrlen;
  size_t rcvbytes;
  herror_t status;
  hresponse_t *res;
  struct attachments_t *mimeMessage;
  char *buffer;

  buffer = http_malloc(MAX_HEADER_SIZE + 1);
  if (buffer == NULL)
  {
    status = herror_new("hresponse_new_from_socket", GENERAL_ERROR, 
      "http_malloc failed.");
    goto clean0;
  }

read_header:                   /* for errorcode: 100 (continue) */
  /* Read header */
  status = http_header_recv(sock, buffer, MAX_HEADER_SIZE, &hdrlen, &rcvbytes);
  if (status != H_OK)
  {
    goto clean1;
  }

  /* Create response */
  res = _hresponse_parse_header(buffer, hdrlen);
  if (res == NULL)
  {
    log_error("Header parse error");
    status = herror_new("hresponse_new_from_socket",
                      GENERAL_HEADER_PARSE_ERROR,
                      "Can not parse response header");
    goto clean1;
  }

  res->data.buf  = buffer;
  res->data.p    = buffer + hdrlen;
  res->data.len  = rcvbytes - hdrlen;
  res->data.size = MAX_HEADER_SIZE + 1;
  hpairnode_dump_deep(res->header);

  /* Chec for Errorcode: 100 (continue) */
  if (res->errcode == 100)
  {
    hresponse_free(res);
    goto read_header;
  }

  /* Create input stream */
  /* Check for MIME message */
  if ((res->content_type &&
       !strcmp(res->content_type->type, "multipart/related")))
  {
    status = mime_get_attachments(res->content_type, res->in, &mimeMessage);
    if (status != H_OK)
    {
      /* TODO (#1#): Handle error */
      goto clean2;
    }
    else
    {
      res->attachments = mimeMessage;
      res->in = http_input_stream_new_from_file(mimeMessage->root_part->filename);
    }
  }
  else
  {
    res->in = http_input_stream_new(sock, res->header);
  }

  if (res->in == NULL)
  {
    status = herror_new("hresponse_new_from_socket", GENERAL_ERROR, 
      "http_input_stream_new failed.");
    goto clean2;
  }

  ng_free_data_buffer(&res->data);
  *out = res;
  return H_OK;
  
clean2:  
  hresponse_free(res);
  return status;
clean1:  
  http_free(buffer);
clean0:  
  return status;
}

void
hresponse_free(hresponse_t *res)
{
  if (res)
  {
    if (res->desc)
      http_free(res->desc);
    
    if (res->header)
      hpairnode_free_deep(res->header);

    if (res->in)
      http_input_stream_free(res->in);

    if (res->content_type)
      content_type_free(res->content_type);

    if (res->attachments)
      attachments_free(res->attachments);

    ng_free_data_buffer(&res->data);
    http_free(res);
  }
  return;
}
