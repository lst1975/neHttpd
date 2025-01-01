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
 
  if (!(req = (struct hrequest_t *) http_malloc(sizeof(struct hrequest_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  if (!(req->statistics = (struct request_statistics *)http_malloc(sizeof(struct request_statistics))))
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
_hrequest_parse_header(char *data)
{
  struct hrequest_t *req;
  hpair_t *hpair = NULL, *qpair = NULL, *tmppair = NULL;

  char *tmp;
  char *tmp2;
  char *saveptr;
  char *saveptr2;
  char *saveptr3;
  char *result;
  char *key;
  char *opt_key;
  char *opt_value;
  int firstline = 1;

  req = hrequest_new();
  tmp = data;

  for (;;)
  {
    result = (char *) strtok_r(tmp, "\r\n", &saveptr);
    tmp = saveptr;

    if (result == NULL)
      break;

    if (firstline)
    {
      firstline = 0;
      tmp2 = result;

      /* parse [GET|POST] [PATH] [SPEC] */
      key = (char *) strtok_r(tmp2, " ", &saveptr2);

      /* save method (get or post) */
      tmp2 = saveptr2;
      if (key != NULL)
      {
        if (!strcmp(key, "POST"))
          req->method = HTTP_REQUEST_POST;
        else if (!strcmp(key, "GET"))
          req->method = HTTP_REQUEST_GET;
      	else if (!strcmp(key, "OPTIONS"))
                req->method = HTTP_REQUEST_OPTIONS;
      	else if (!strcmp(key, "HEAD"))
          req->method = HTTP_REQUEST_HEAD;
        else if (!strcmp(key, "PUT"))
          req->method = HTTP_REQUEST_PUT;
        else if (!strcmp(key, "DELETE"))
          req->method = HTTP_REQUEST_DELETE;
        else if (!strcmp(key, "TRACE"))
          req->method = HTTP_REQUEST_TRACE;
        else if (!strcmp(key, "CONNECT"))
          req->method = HTTP_REQUEST_CONNECT;
        else
          req->method = HTTP_REQUEST_UNKOWN;
      }
      /* below is key the path and tmp2 the spec */
      key = (char *) strtok_r(tmp2, " ", &saveptr2);

      /* save version */
      tmp2 = saveptr2;
      if (tmp2 != NULL)
      {
        /* req->spec = (char *) http_malloc(strlen(tmp2) + 1); strcpy(req->spec,
           tmp2); */
        if (!strcmp(tmp2, "HTTP/1.0"))
          req->version = HTTP_1_0;
        else
          req->version = HTTP_1_1;
      }
      /* 
       * parse and save path+query parse:
       * /path/of/target?key1=value1&key2=value2...
       */

      if (key != NULL)
      {
        tmp2 = key;
        key = (char *) strtok_r(tmp2, "?", &saveptr2);
        tmp2 = saveptr2;

        /* save path */
        /* req->path = (char *) http_malloc(strlen(key) + 1); */
        strncpy(req->path, key, REQUEST_MAX_PATH_SIZE-1);

        /* parse options */
        for (;;)
        {
          key = (char *) strtok_r(tmp2, "&", &saveptr2);
          tmp2 = saveptr2;

          if (key == NULL)
            break;

          opt_key = (char *) strtok_r(key, "=", &saveptr3);
          opt_value = saveptr3;

          if (opt_value == NULL)
            opt_value = "";

          /* create option pair */
          if (opt_key != NULL)
          {
            if (!(tmppair = (hpair_t *) http_malloc(sizeof(hpair_t))))
            {
              log_error("http_malloc failed (%s)", strerror(errno));
              return NULL;
            }

            if (req->query == NULL)
            {
              req->query = qpair = tmppair;
            }
            else
            {
              qpair->next = tmppair;
              qpair = tmppair;
            }

            /* fill hpairnode_t struct */
            qpair->next = NULL;
            qpair->key = http_strdup(opt_key);
            qpair->value = http_strdup(opt_value);
          }
        }
      }
    }
    else
    {
      tmppair = hpairnode_parse(result, ":", NULL);

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
  tmp = hpairnode_get_ignore_case(req->header, HEADER_CONTENT_TYPE);
  if (tmp != NULL)
    req->content_type = content_type_new(tmp);

  return req;
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

  http_free(req);

  return;
}

herror_t
hrequest_new_from_socket(struct hsocket_t *sock, struct hrequest_t ** out)
{
  herror_t status;
  struct hrequest_t *req;
  char buffer[MAX_HEADER_SIZE + 1];
  struct attachments_t *mimeMessage;

  if ((status = http_header_recv(sock, buffer, MAX_HEADER_SIZE)) != H_OK)
  {
    return status;
  }

  /* Create response */
  req = _hrequest_parse_header(buffer);

  /* Create input stream */
  req->in = http_input_stream_new(sock, req->header);

  /* Check for MIME message */
  if ((req->content_type &&
       !strcmp(req->content_type->type, "multipart/related")))
  {
    status = mime_get_attachments(req->content_type, req->in, &mimeMessage);
    if (status != H_OK)
    {
      /* TODO (#1#): Handle error */
      hrequest_free(req);
      return status;
    }
    else
    {
      req->attachments = mimeMessage;
      req->in =
        http_input_stream_new_from_file(mimeMessage->root_part->filename);
    }
  }

  *out = req;
  return H_OK;
}
