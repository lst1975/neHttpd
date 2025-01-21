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
/** @file nanohttp-client.c nanoHTTP client interface */
/******************************************************************
*  $Id: nanohttp-client.c,v 1.54 2007/11/03 22:40:10 m0gg Exp $
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#include "nanohttp-config.h"
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-response.h"
#include "nanohttp-base64.h"
#include "nanohttp-url.h"
#include "nanohttp-client.h"
#include "nanohttp-time.h"
#include "nanohttp-header.h"
#include "nanohttp-system.h"

herror_t
httpc_init(int argc, char **argv)
{
  return hsocket_module_init(argc, argv);
}

void
httpc_destroy(void)
{
  hsocket_module_destroy();

  return;
}

httpc_conn_s *httpc_new(void)
{
  static int counter = 10000;
  herror_t status;
  httpc_conn_s *res;
 
  if (!(res = (httpc_conn_s *) ng_malloc(sizeof(httpc_conn_s))))
  {
    log_error("ng_malloc failed %m.", ng_errno);
    goto clean0;
  }

  if (!(res->sock = (hsocket_s *)ng_malloc(sizeof(hsocket_s))))
  {
    log_error("ng_malloc failed %m.", ng_errno);
    goto clean1;
  }

  if (ng_url_init(&res->url) < 0)
  {
    log_error("ng_malloc failed %m.", ng_errno);
    goto clean2;
  }

  if ((status = hsocket_init(res->sock)) != H_OK)
  {
    herror_log(status);
    log_warn("hsocket_init failed.");
    goto clean3;
  }

  res->version = HTTP_VERSION_1_1;
  res->out = NULL;
  res->id = counter++;
  ng_INIT_LIST_HEAD(&res->header);
  
  return res;

clean3:
  ng_url_free(&res->url);
clean2:
  ng_free(res->sock);
clean1:
  ng_free(res);
clean0:
  return NULL;
}

void
httpc_free(httpc_conn_s * conn)
{
  if (conn == NULL)
    return;

  hpairnode_free_deep(&conn->header);

  if (conn->out != NULL)
  {
    http_output_stream_free(conn->out);
    conn->out = NULL;
  }

  hsocket_free(conn->sock);
  ng_url_free(&conn->url);

  if (conn->sock)
    ng_free(conn->sock);
  ng_free(conn);

  return;
}

void
httpc_close_free(httpc_conn_s * conn)
{
  if (conn == NULL)
    return;

  hsocket_close(conn->sock);
  httpc_free(conn);

  return;
}

static int
_httpc_set_basic_authorization_header(httpc_conn_s *conn, 
  const ng_block_s *key, const ng_block_s *user, const ng_block_s *password)
{
  int inlen, outlen, len;
  unsigned char *p, *o;
  ng_block_s in, out;

  /* XXX: do we need this really? */
  inlen  = user->len + password->len + 1;
  outlen = B64_ENCLEN(inlen);

  p = (unsigned char *)ng_malloc(6+inlen+1+outlen);
  if (p == NULL)
  {
    log_fatal("Malloc failed.");
    return 0;
  }
  ng_memcpy(p, user->ptr, user->len);
  p[user->len]=':';
  ng_memcpy(p+user->len+1, password->ptr, password->len);
  
  o = p + inlen + 7;
  in.ptr = p;
  in.len = inlen;
  out.ptr = o;
  len = b64Encode(&in, &out);
  ng_memcpy(o - 6, "Basic ", 6);
  out.len = len + 6;
  out.ptr = o - 6;
  
  if (0 > hpairnode_set_header(&conn->header, key, &out))
  {
    ng_free(p);
    log_fatal("httpc_set_header failed.");
    return -1;
  }
  
  ng_free(p);
  return 0;
}

int
httpc_set_basic_authorization(httpc_conn_s *conn, 
  const ng_block_s *user, const ng_block_s *password)
{
  return _httpc_set_basic_authorization_header(conn, 
    &__HDR_BUF__(HEADER_AUTHORIZATION), user, password);
}

int
httpc_set_basic_proxy_authorization(httpc_conn_s *conn, 
  const ng_block_s *user, const ng_block_s *password)
{
  return _httpc_set_basic_authorization_header(conn, 
    &__HDR_BUF__(HEADER_PROXY_AUTHORIZATION), user, password);
}

static int
httpc_header_set_date(httpc_conn_s *conn)
{
  char buffer[32];
  ng_block_s date;
  date.len = ng_http_date_s(buffer, 32);
  date.buf = buffer;
  return hpairnode_set_header(&conn->header, &__HDR_BUF__(HEADER_DATE), &date);
}

/*--------------------------------------------------
FUNCTION: httpc_send_header
DESC: Sends the current header information stored
in conn through conn->sock.
----------------------------------------------------*/
herror_t
httpc_send_header(httpc_conn_s *conn)
{
  hpair_s *walker;
  ng_list_for_each_entry(walker,hpair_s,&conn->header,link)
  {
    if (walker->key.len && walker->val.len)
    {
      herror_t status;
      status = hsocket_send_string(conn->sock, "%pS: %pS\r\n", 
        &walker->key, &walker->val);
      if (status != H_OK)
        return status;
    }
  }

  return hsocket_send(conn->sock, (const unsigned char *)"\r\n", 2);
}

/*--------------------------------------------------
FUNCTION: httpc_talk_to_server
DESC: This function is the heart of the httpc
module. It will send the request and process the
response.

Here the parameters:

method:
the request method. This can be HTTP_REQUEST_POST and
HTTP_REQUEST_GET.

conn:
the connection object (created with httpc_new())

urlstr:
the complete url in string format.
http://<host>:<port>/<context>
where <port> is not mendatory.

start_cb:
a callback function, which will be called when
the response header is completely arrives.

cb:
a callback function, which will be called everytime
when data arrives.

content_size:
size of content to send.
(only if method is HTTP_REQUEST_POST)

content:
the content data to send.
(only if method is HTTP_REQUEST_POST)

userdata:
a user define data, which will be passed to the
start_cb and cb callbacks as a parameter. This
can also be NULL.


If success, this function will return 0.
>0 otherwise.
----------------------------------------------------*/
static herror_t
_httpc_talk_to_server(hreq_method_e method, httpc_conn_s *conn, 
  const ng_block_s *urlstr)
{
  char *buffer;
  herror_t status;
  int len, ssl;

#define ___BUFSZ 4096
  buffer = (char *)ng_malloc(___BUFSZ);
  if (buffer == NULL)
  {
    log_fatal("Failed to malloc temp buffer.");
    status = herror_new("httpc_mime_begin", 
                      GENERAL_ERROR_MALLOC,
                      "Can malloc \"%d\" %m.", 
                      ___BUFSZ, 
                      ng_errno);
    goto clean0;
  }
  
  if (conn == NULL)
  {
    status = herror_new("httpc_talk_to_server", GENERAL_INVALID_PARAM, 
      "httpc_conn_s param is NULL");
    goto clean1;
  }

  /* Build request header */
  if (httpc_header_set_date(conn) < 0)
  {
    log_error("httpc_header_set_date failed.");
    status = herror_new("httpc_talk_to_server", GENERAL_ERROR, 
      "httpc_header_set_date failed.");
    goto clean1;
  }

  status = ng_url_parse(&conn->url, urlstr->cptr, urlstr->len);
  if (status != H_OK)
  {
    log_error("Cannot parse URL \"%pS\"", urlstr);
    goto clean1;
  }
  
  /* TODO (#1#): Check for HTTP protocol in URL */

  /* Set hostname */
  if (hpairnode_set_header(&conn->header, &__HDR_BUF__(HEADER_HOST), 
    &conn->url.host) < 0)
  {
    log_error("httpc_set_header failed.");
    status = herror_new("httpc_talk_to_server", GENERAL_ERROR, 
      "httpc_set_header failed.");
    goto clean1;
  }

  ssl = conn->url.protocol == NG_PROTOCOL_HTTPS ? 1 : 0;
  log_verbose("ssl = %i (%i %i).", ssl, conn->url.protocol, NG_PROTOCOL_HTTPS);

  /* Open connection */
  status = hsocket_open(conn->sock, conn->url.host.cptr, conn->url.port, ssl);
  if (status != H_OK)
  {
    herror_log(status);
    log_error("hsocket_open failed.");
    goto clean1;
  }

  switch(method)
  {
    case HTTP_REQUEST_METHOD_GET:
    case HTTP_REQUEST_METHOD_POST:
      len = ng_snprintf(buffer, ___BUFSZ, "%s %pS HTTP/%s\r\n", 
          method == HTTP_REQUEST_METHOD_GET ? "GET" : "POST",
          &conn->url.context, 
          (conn->version == HTTP_VERSION_1_0) ? "1.0" : "1.1");
      break;

    default:
      log_error("Unknown method type!");
      status = herror_new("httpc_talk_to_server",
        GENERAL_INVALID_PARAM,
        "hreq_method_e must be  HTTP_REQUEST_GET or HTTP_REQUEST_POST");
      goto clean2;
  }

  log_verbose("Sending request...");
  status = hsocket_send(conn->sock, (unsigned char *)buffer, len);
  if (status != H_OK)
  {
    herror_log(status);
    log_error("Cannot send request.");
    goto clean2;
  }

  log_verbose("Sending header...");
  status = httpc_send_header(conn);
  if (status != H_OK)
  {
    herror_log(status);
    log_error("Cannot send header.");
    goto clean2;
  }

  ng_free(buffer);
  return H_OK;

#undef ___BUFSZ
  
clean2:
  hsocket_close(conn->sock);
clean1:
  ng_free(buffer);
clean0:
  return status;
}

herror_t
httpc_get(httpc_conn_s *conn, hresponse_t **out, const ng_block_s *urlstr)
{
  herror_t status;

  if ((status = _httpc_talk_to_server(HTTP_REQUEST_METHOD_GET, conn, urlstr)) != H_OK)
  {
    herror_log(status);
    log_error("_httpc_talk_to_server failed.");
    return status;
  }

  if ((status = hresponse_new_from_socket(conn->sock, out)) != H_OK)
  {
    herror_log(status);
    log_error("hresponse_new_from_socket failed.");
    return status;
  }

  return H_OK;
}

herror_t
httpc_post_begin(httpc_conn_s *conn, const ng_block_s *url)
{
  herror_t status;

  if ((status = _httpc_talk_to_server(HTTP_REQUEST_METHOD_POST, conn, url)) != H_OK)
  {
    herror_log(status);
    log_error("_httpc_talk_to_server failed.");
    return status;
  }

  conn->out = http_output_stream_new(conn->sock, &conn->header);
  if (conn->out == NULL)
  {
    log_error("http_output_stream_new failed.");
    status = herror_new("httpc_post_begin", GENERAL_ERROR, 
      "http_output_stream_new failed.");
    return status;
  }

  return H_OK;
}

/*--------------------------------------------------
FUNCTION: httpc_post_begin
DESC: End a "POST" method and receive the response.
  You MUST call httpc_post_end() before!
----------------------------------------------------*/
herror_t
httpc_post_end(httpc_conn_s *conn, hresponse_t **out)
{
  herror_t status;

  if ((status = http_output_stream_flush(conn->out)) != H_OK)
    return status;

  if ((status = hresponse_new_from_socket(conn->sock, out)) != H_OK)
    return status;

  return H_OK;
}
