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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>  // For PATH_MAX

#include "nanohttp-logging.h"
#include "nanohttp-server.h"
#include "nanohttp-json.h"
#include "nanohttp-file.h"
#include "nanohttp-form.h"
#include "nanohttp-urlencode.h"
#include "nanohttp-base64.h"
#include "nanohttp-user.h"
#include "nanohttp-ring.h"
#include "nanohttp-data.h"

static int
simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{
  httpd_user_t *auth_user;
  log_debug("logging in user=\"%s\" password=\"%s\"\n", user, password);

  auth_user = nanohttp_users_match(user, strlen(user), password, strlen(password));
  if (auth_user == NULL) {
    log_error("authenticate failed\n");
    return 0;
  }

  req->userLevel = auth_user->type;
  return 1;
}

/**********************************************************************************
*  ADD JSON FILE HERE
**********************************************************************************/
#define ___(x) { .cptr=x, .len=sizeof(x)-1 }
static const httpd_buf_t __TEST_DEV_FILE  =___("/device.json");
#undef ___
static void
default_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  char buf[REQUEST_MAX_PATH_SIZE+256+1];

  if (BUF_isequal(&__TEST_DEV_FILE, req->path, req->path_len))
  {
    httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    http_output_stream_write_string(conn->out,
      "{"
        "'shortAddr':10,"
        "'channel':12"
      "}");
  }
  else
  {
    snprintf(buf, sizeof buf, "Resource \"%s\" not found", req->path);
    httpd_send_not_found(conn, buf);
  }
  return;
}

static void
headers_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  hpair_t *walker;
  char buf[512];
  size_t len;

  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Request headers</title>"
      "</head>"
      "<body>"
        "<h1>Request headers</h1>"
        "<ul>");

  for (walker=req->header; walker; walker=walker->next)
  {
    len = snprintf(buf, 512, "<li>%s: %s</li>", walker->key, walker->value);
    http_output_stream_write(conn->out, (unsigned char *)buf, len);
  }

  http_output_stream_write_string(conn->out,
        "</ul>"
      "</body>"
    "</html>");
}

static void
mime_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_not_implemented(conn, "mime_service");
}

/*"Content-Disposition: form-data; name=\"File\"; filename=\"nanoHttpd-2.10.tar.xz\"\r\n
   Content-Type: application/x-xz\r\n\r\ny7zXZ"*/
static size_t 
__multipart_cb_headers_complete(multipartparser *p)
{
  if (p->arg != NULL)
  {
    return 0;
  }
  else
  {
    char *file = NULL;

    if (p->value_length)
    {
      if (p->field_length == 19 && !strncmp("Content-Disposition",p->field,19))
      {
        char *f = strstr(p->value,"filename");
        char *end = p->value + p->value_length;

        if (f != NULL)
        {
          for (f += 8;f < end;f++)
          {
            if (*f == '"')
              break;
          }
          if (f < end)
          {
            char *e;
            f++;
            while(f < end && isspace((int)*f))
            {
              f++;
            }
            e = memchr(f, '"', end - f);
            if (e != NULL)
            {
              while(e > f && isspace((int)e[-1]))
              {
                e--;
              }
              if (e > f && e - f < PATH_MAX)
              {
                char *buf = NULL;
                buf = http_malloc(PATH_MAX);
                if (buf == NULL)
                {
                  log_error("Failed to malloc temporary buffer.");
                  return -1;
                }
                snprintf(buf,PATH_MAX,"%s/%.*s","uploads",(int)(e-f), f);
                file = buf;
              }
            }
          }
        }
      }
    }

    if (file == NULL)
    {
      log_error("Try to open the file uploads/gw.bin for writing.");
      if (p->arg == NULL)
        log_error("Not able to open the file uploads/gw.bin for writing.");
    }
    else
    {
      log_error("Try to open the file %s for writing.", file);
      p->arg = nanohttp_file_open_for_write(file);
      http_free(file);
      if (p->arg == NULL)
        log_error("Not able to open the file %s for writing.", file);
    }
    
    if (p->arg == NULL)
    {
      return -1;
    }
    
    return 0;
  }
}

static size_t
__multipart_cb_data(multipartparser *p, const char* data, size_t size)
{
  return nanohttp_file_write(p->arg, data, size) != size;
}

static size_t
__multipart_cb_header_field(multipartparser *p, const char* data, size_t size)
{
  if (p->content_disposition_parsed)
    return 0;

  p->field_length+=snprintf(p->field+p->field_length, 
    sizeof(p->field)-p->field_length, "%.*s", (int)size, data);
  return 0;
}

static size_t
__multipart_cb_header_value(multipartparser *p, const char* data, size_t size)
{
  if (p->field_length == 19 && !strncmp("Content-Disposition",p->field,19))
  {
    p->content_disposition_parsed = 1;
  }
  p->value_length+=snprintf(p->value+p->value_length, 
    sizeof(p->value)-p->value_length, "%.*s", (int)size, data);
  return 0;
}

static herror_t
__post_internal_error(httpd_conn_t *conn, herror_t r)
{
  herror_t sr;
  log_error("%s", herror_message(r));
  sr = httpd_send_header(conn, 500, HTTP_STATUS_500_REASON_PHRASE);
  if (sr != H_OK) 
  {
    herror_release(sr);
    return r;
  }
  sr = http_output_stream_write_string(conn->out, herror_message(r));
  herror_release(sr);
  return r;
}

static herror_t
__post_service(httpd_conn_t *conn, struct hrequest_t *req, 
  const char *file)
{
  herror_t r;
  if (req->method == HTTP_REQUEST_POST)
  {
    hpair_t *pair;
    content_type_t *ct;
    multipartparser p;
    multipartparser_callbacks settings;
    multipartparser_callbacks_init(&settings);
    settings.on_data = __multipart_cb_data;
    settings.on_header_field = __multipart_cb_header_field;
    settings.on_header_value = __multipart_cb_header_value;
    settings.on_headers_complete = __multipart_cb_headers_complete;

    ct = req->content_type;
    if (ct == NULL
      || ct->type_len != 19
      || memcmp(ct->type, "multipart/form-data", 19))
    {
      hrequest_free_data(req);
      log_warn("Bad POST data format, must 'multipart/form-data' for %s", req->path);
      return httpd_send_not_implemented(conn, "post_service");
    }

    pair = ct->params;
    if (pair == NULL
      || pair->key_len != 8
      || memcmp(pair->key, "boundary", 8)
      || pair->value == NULL
      || !pair->value_len)
    {
      hrequest_free_data(req);
      r = herror_new("post_service", FILE_ERROR_READ, 
        "Bad POST data format, must 'multipart/form-data' for %s", req->path);
      return __post_internal_error(conn, r);
    }

    if (multipartparser_init(&p, NULL, pair->value, pair->value_len))
    {
      hrequest_free_data(req);
      r = herror_new("post_service", GENERAL_ERROR, 
        "Failed to do multipartparser for 'multipart/form-data'");
      return __post_internal_error(conn, r);
    }
    
    while (http_input_stream_is_ready(req->in))
    {
      char *buffer;
      size_t len;
      if (!req->data.len)
      {
        len = http_input_stream_read(req->in, req->data.ptr, req->data.size);
        if (len == -1)
        {
          hrequest_free_data(req);
          nanohttp_file_close(p.arg);
          r = herror_new("post_service", FILE_ERROR_READ, 
            "Failed to read stream %s", req->path);
          return __post_internal_error(conn, r);
        }
        buffer = req->data.buf;
      }
      else
      {
        buffer = req->data.p;
        len = req->data.len;
        req->data.len = 0;
        req->in->received = len;
      }
      
      if (len != multipartparser_execute(&p, &settings, buffer, len))
      {
        hrequest_free_data(req);
        nanohttp_file_close(p.arg);
        r = herror_new("post_service", FILE_ERROR_WRITE, 
          "Failed to read stream %s", req->path);
        return __post_internal_error(conn, r);
      }
    }

    hrequest_free_data(req);
    nanohttp_file_close(p.arg);
    return httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  }
  else
  {
    hrequest_free_data(req);
    return httpd_send_not_implemented(conn, "post_service");
  }
}

static void
post_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = __post_service(conn, req, "config/gw.bin");
  herror_release(r);
}

static herror_t
__root_service_read(void *arg, const char *buf, size_t length)
{
  httpd_conn_t *conn=(httpd_conn_t *)arg;
  return http_output_stream_write(conn->out, (const unsigned char *)buf, length);
}

/*
"<html>"
  "<head>"
    "<title>nanoHTTP server examples</title>"
  "</head>"
  "<body>"
    "<h1>nanoHTTP server examples</h1>"
    "<ul>"
      "<li><a href=\"/\">Simple service</a></li>"
      "<li><a href=\"/secure\">Secure service</a> (try: bob/builder)</li>"
      "<li><a href=\"/headers\">Request headers</a></li>"
        "<li><a href=\"/mime\">MIME service</a></li>"
        "<li>"
        "<form action=\"/post\" method=\"post\" enctype=\"multipart/form-data\">"
          "<fieldset>"
            "<legend>Upload file</legend>"
            "<input name=\"Text\" type=\"text\" value=\"test-field\"? "
            "<input name=\"File\" type=\"file\" accept=\"text/\*\"> "
            "<input value=\"Submit\" type=\"submit\">"
          "</fieldset>"
        "</form>"
        "</li>"
      "<li><a href=\"/not_existent\">The default service</a></li>"
      "<li><a href=\"/nhttp\">Admin page</a> (try -NHTTPDadmin on the command line)</li>"
    "</ul>"
  "</body>"
"</html>");
*/

static herror_t
__send_menu_js(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = H_OK;
  do
  {
    if (req->userLevel == _N_http_user_type_ADMIN)
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_ADMIN);
    else if (req->userLevel == _N_http_user_type_SUPER)
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_SUPER);
    else
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_GUEST);
    if (r != H_OK) break;
  } 
  while(0);
  
  return r;
}
static herror_t
__send_root_html(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = H_OK;

  do
  {
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    if (r != H_OK) break;

    r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL1);
    if (r != H_OK) break;

    r = __send_menu_js(conn, req);
    if (r != H_OK) break;
    
    r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL2);
    if (r != H_OK) break;

    r = http_output_stream_write(conn->out, 
                (const unsigned char *)"<body>", 
                6);
    if (r != H_OK) break;
    r = http_output_stream_write(conn->out, 
                (const unsigned char *)__nanohttp_index_html_body, 
                sizeof(__nanohttp_index_html_body)-1);
    if (r != H_OK) break;
    r = http_output_stream_write(conn->out, 
                (const unsigned char *)"</body></html>", 
                14);
  } 
  while(0);

  return r;
}

static void
secure_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

  r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  if (r == H_OK)
  {
    log_info("secure_service received, redirect to index.html");
    do {
      r = http_output_stream_write(conn->out, 
                (const unsigned char *)__nanohttp_index_html_body, 
                sizeof(__nanohttp_index_html_body)-1);
      if (r != H_OK) break;
      r = __send_menu_js(conn, req);
      if (r != H_OK) break;
    }
    while (0);
  }
  else
  {
    r = http_output_stream_write(conn->out,
                (const unsigned char *)__nanohttp_secure_html,
                sizeof(__nanohttp_secure_html)-1);
  }

  herror_release(r);
  return;
}

extern const char *nanohttp_index_html_head;

#define _(x) _nanoConfig_HTTPD_DATA_SERVICE x
#define __(x) { .cptr=_(x), .len=sizeof(_(x))-1 }
#define ___(x) { .cptr=x, .len=sizeof(x)-1 }
static const httpd_buf_t __SYS_FILE=__("system.json");
static const httpd_buf_t __DEV_FILE=__("device.json");
static const httpd_buf_t __USR_FILE=__("users.json");
static const httpd_buf_t __MIB_FILE=__("setmib.json");
static const httpd_buf_t __ADD_FILE=__("add.json");
static const httpd_buf_t __DEL_FILE=__("del.json");
static const httpd_buf_t __SAV_FILE=__("save.json");
static const httpd_buf_t __TMP_USR_FILE  =__("templateUser.json");
static const httpd_buf_t __TMP_INT_FILE  =__("templateInterface.json");
static const httpd_buf_t __FAVOR_FILE =___("/favicon.ico");
static const httpd_buf_t __ROOT_FILE  =___("/");
#undef ___
#undef __
#undef _

static void
root_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = NULL;

  if (strstr(req->path, "..") != NULL)
  {
    return default_service(conn, req);
  }
  else
  {
    if (BUF_isequal(&__ROOT_FILE, req->path, req->path_len))
    {
      r = __send_root_html(conn, req);
    }
    else if (BUF_isequal(&__FAVOR_FILE, req->path, req->path_len))
    {
      unsigned char *bf=NULL;
      do
      {
        int len;
        bf = (unsigned char *)http_malloc(B64_DECLEN(sizeof(favorICON)-1)+1);
        if (bf == NULL) break;
#if __configUseStreamBase64
        ng_buffer_s in, out;
        in.cptr = favorICON;
        in.len  = sizeof(favorICON)-1;
        out.ptr = bf;
        len = b64Decode(&in, &out);
        if (len < 0)
        {
          log_error("b64Decode failed");
          break;
        }
#else      
        len = base64_decode_string((const unsigned char *)favorICON, bf);
#endif
        if (httpd_set_header(conn, HEADER_CONTENT_TYPE, "image/png")) break;

        r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
        if (r != NULL) break;
        r = http_output_stream_write(conn->out, bf, len);
        if (r != NULL) break;
      } 
      while(0);
      
      if (bf != NULL)
        http_free(bf);
    }
    else if (!BUF_isequal(&__MIB_FILE, req->path, req->path_len)
      && !BUF_isequal(&__ADD_FILE, req->path, req->path_len)
      && !BUF_isequal(&__DEL_FILE, req->path, req->path_len)
      && !BUF_isequal(&__SAV_FILE, req->path, req->path_len)
#if !__NHTTP_TEST      
      && !BUF_isequal(&__DEV_FILE, req->path, req->path_len)
      && !BUF_isequal(&__USR_FILE, req->path, req->path_len)
      && !BUF_isequal(&__SYS_FILE, req->path, req->path_len)
#endif      
      )
    {
      log_debug("Try to open the file %s.", req->path);

      void *file = nanohttp_file_open_for_read(req->path+1);
      if (file == NULL)
      {
        // If the file does not exist
        log_error("Not able to open the file %s for reading.", req->path);
        r = httpd_send_header(conn, 404, HTTP_STATUS_404_REASON_PHRASE);
      }
      else
      {
        r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
        if (r == NULL)
        {
          r = nanohttp_file_read(file, __root_service_read, conn);
          if (r != NULL)
          {
            herror_release(r);
            r = http_output_stream_write_printf(conn->out, 
                  "Failed to readfile %s: %s", req->path, herror_message(r));
          }
        }
        nanohttp_file_close(file);
      }
    }
    else
    {
      return default_service(conn, req);
    }
  }
  
  herror_release(r);
}

#define ___CFG_TEST "{\"err\":0}"

#define ___USER_PFX_STR "2.0."
#define ___USER_PFX_LEN 4 

#define __CFG_ret0(x) "{\"id\":%d,\"err\":[{\"id\":\"" x "\",\"reason\":\""
#define CFG_ret1 __CFG_ret0("%.*s")
#define CFG_ret3 "\"}]}"
#define CFG_RET0(msg) __CFG_ret0("") msg CFG_ret3
#define CFG_RET1(msg) CFG_ret1 msg CFG_ret3

static void
data_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

#if __NHTTP_TEST      
  if (BUF_isequal(&__SYS_FILE, req->path, req->path_len)
    || BUF_isequal(&__DEV_FILE, req->path, req->path_len)
    || BUF_isequal(&__USR_FILE, req->path, req->path_len))
  {
    return root_service(conn, req);
  }
#endif

  if (req->query != NULL)
  {
    char buf[126]={0};
    int n, len, id;
    unsigned char *query;
    unsigned char *data = NULL;
    JSONPair_t *pair,*p;
    JSONStatus_t result;
    httpd_buf_t in;

    data = (unsigned char *)hpairnode_get(req->query, "data");
    if (data == NULL)
    {
      log_error("No query data");
      goto finished;
    }
    
    log_debug("try to decode %s\n",(char *)data);
    
    /* decode_url malloced new data */
    if (0 > decode_url(&in, (const uint8_t*)data, strlen((char *)data)))
    {
      log_error("Failed to parse query");
      goto finished;
    }
    
    log_debug("decoded query is : %.*s", in.len, in.cptr);

    query = (unsigned char *)http_malloc(B64_DECLEN(in.len) + 1);
    if (query == NULL)
    {
      http_free(in.ptr);
      log_error("Failed to http_malloc key");
      goto finished;
    }
    
    {
#if __configUseStreamBase64
      ng_buffer_s out;
      out.ptr = query;
      len = b64Decode(&in, &out);
      if (len < 0)
      {
        http_free(in.ptr);
        log_error("b64Decode failed");
        goto finished;
      }
#else      
      len = base64_decode_string(in.ptr, query);
#endif
      http_free(in.ptr);
      log_debug("decoded query is : %s", query);
    }

    result = json_parse(&pair, (const char *)query, len);
    if (result != JSONSuccess)
    {
      log_error("Failed to parse json");
      goto finished;
    }

    p = json_find_bykey(pair, "id", 2);
    if (p == NULL || p->jsonType != JSONNumber)
    {
      log_error("Bad id in json");
      goto finished;
    }
    id = (int)p->vint;
    
    if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"setmib.json", req->path))
    {
      // Process del operation
      int err, usrLen;

      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }

      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONObject)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Mailformed Request."), id);
        goto finished;
      }
      
      for (p=p->children; p != NULL; p = p->siblings)
      {
        if (p->keyLength >= ___USER_PFX_LEN 
          && !memcmp(p->key, ___USER_PFX_STR, ___USER_PFX_LEN))
        {
          const char *colon;
          
          if (req->userLevel != _N_http_user_type_SUPER)
          {
            // Process set operation
            n = snprintf(buf, sizeof buf, 
                    CFG_RET0("Authorization Failed."), id);
            goto finished;
          }
          
          colon = memchr(p->key, ':', p->keyLength);
          if (colon != NULL && colon - p->key > ___USER_PFX_LEN)
          {
            const char *usr;
            
            usr = colon + 1;
            usrLen = p->keyLength - (usr - p->key) - 2;
            if ((usrLen < _N_http_user_NAME_MINLEN
              || usrLen > _N_http_user_NAME_MAXLEN))
            {
              n = snprintf(buf, sizeof buf, 
                    CFG_RET1("Invalid username length."), 
                    id, 
                    p->keyLength, 
                    p->key);
              goto finished;
            }
            if (!memcmp(p->key+p->keyLength-2, ".0", 2))
            {
              n = snprintf(buf, sizeof buf, 
                    CFG_RET1("This user does not exists."), 
                    id, 
                    p->keyLength, 
                    p->key);
            }
            else 
            {
              if (!memcmp(p->key+p->keyLength-2, ".1", 2))
              {
                err = nanohttp_users_update(usr, usrLen, 
                        p->value, p->valueLength, NULL, 0);
              }
              else if (!memcmp(p->key+p->keyLength-2, ".2", 2))
              {
                err = nanohttp_users_update(usr, usrLen, 
                        NULL, 0, p->value, p->valueLength);
              }
              else
              {
                err = _N_http_user_error_SYS;
              }
              switch (err)
              {
                case _N_http_user_error_NONE:
                  n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
                  break;
                case _N_http_user_error_PERM:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("Superuser can not be deleted."), 
                        id, 
                        (int)p->keyLength, 
                        p->key);
                  goto finished;
                case _N_http_user_error_EXIST:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("This user does not exists."), 
                        id, 
                        p->keyLength, 
                        p->key);
                  goto finished;
                case _N_http_user_error_VNAME:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("The length of username is invalid: [%d,%d]."), 
                        id, 
                        p->keyLength, 
                        p->key,
                        _N_http_user_NAME_MINLEN, _N_http_user_NAME_MAXLEN);
                  goto finished;
                case _N_http_user_error_INVAL:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("Invalid account type."), 
                        id, 
                        p->keyLength, 
                        p->key);
                  goto finished;
                case _N_http_user_error_SYS:
                default:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("System error."), 
                        id, 
                        p->keyLength, 
                        p->key);
                  goto finished;
              }
            }
          }
          else
          {
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("Invalid parameter."), 
                  id, 
                  p->keyLength, 
                  p->key);
            goto finished;
          }
        }
        else
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, 
                  CFG_RET1("For testing received error message from our product."), 
                  id, 
                  p->keyLength, 
                  p->key);
        }
      }
    }
    else if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"save.json", req->path))
    {
      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Process save operation
      n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }
    else if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"add.json", req->path))
    {
      int err;
      JSONPair_t *usr, *pwd, *type;
      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONObject)
      {
        goto finished;
      }
      usr = json_find_bykey_head_tail(p->children,  
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.0", 3);
      pwd = json_find_bykey_head_tail(p->children,  
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.1", 3);
      type = json_find_bykey_head_tail(p->children, 
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.2", 3);
      if (usr != NULL || pwd != NULL)
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        if (!usr || !pwd)
        {
          if (usr)
            n = snprintf(buf, sizeof buf, CFG_RET1("No password."), id, 
                  (int)usr->keyLength-1, usr->key);
          else
            n = snprintf(buf, sizeof buf, CFG_RET0("No username."), id);
          goto finished;
        }
        err = nanohttp_users_add(
          usr->value, 
          usr->valueLength,
          pwd->value, 
          pwd->valueLength,
          type ? type->value : NULL,
          type ? type->valueLength : 0
          );
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_PERM:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("Superuser can not be deleted."), 
                  id, 
                  (int)usr->keyLength, 
                  usr->key);
            goto finished;
          case _N_http_user_error_EXIST:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("This user already exists."), 
                  id, 
                  (int)usr->keyLength, 
                  usr->key);
            goto finished;
          case _N_http_user_error_VNAME:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  (int)usr->keyLength, 
                  usr->key, 
                  _N_http_user_NAME_MINLEN, 
                  _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_VPSWD:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  (int)pwd->keyLength, 
                  pwd->key, 
                  _N_http_user_PSWD_MINLEN, 
                  _N_http_user_PSWD_MAXLEN);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = snprintf(buf, sizeof buf, CFG_RET1("System error."), id, 
                  (int)usr->keyLength, usr->key);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), 
            id);
          goto finished;
        }
        n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"del.json", req->path))
    {
      // Process del operation
      int err, usrLen;
      const char *colon, *usr;
      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONString)
      {
        goto finished;
      }

      if (!memcmp(p->value, ___USER_PFX_STR, ___USER_PFX_LEN))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        colon = memchr(p->value, ':', p->valueLength);
        if (colon == NULL || colon-p->value < ___USER_PFX_LEN + 1)
        {
          n = snprintf(buf, sizeof buf, 
                CFG_RET1("This user does not exists."), 
                id, 
                (int)p->valueLength, 
                p->value);
          goto finished;
        }
        usr = colon + 1;
        usrLen = p->valueLength - (usr - p->value);
        if ((usrLen < _N_http_user_NAME_MINLEN
          || usrLen > _N_http_user_NAME_MAXLEN))
        {
          n = snprintf(buf, sizeof buf, 
                CFG_RET1("Invalid username length."), 
                id, 
                (int)p->valueLength, 
                p->value);
          goto finished;
        }
        
        err = nanohttp_users_del(usr, usrLen);
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_EXIST:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("This user does not exists."), 
                  id, 
                  (int)p->valueLength, 
                  p->value);
            goto finished;
          case _N_http_user_error_VNAME:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  (int)p->valueLength, 
                  p->value,
                  _N_http_user_NAME_MINLEN, 
                  _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_PERM:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("Superuser can not be deleted."), 
                  id, 
                  (int)p->valueLength, 
                  p->value);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = snprintf(buf, sizeof buf, 
                  CFG_RET1("System error."), 
                  id, 
                  (int)p->valueLength, 
                  p->value);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"template.json", req->path))
    {
      // Generate template.json
      p = json_find_bykey(pair, "value", 5);
      if (p == NULL || p->jsonType != JSONString)
      {
        goto finished;
      }
      if (p->valueLength == 3 && !strncmp(p->value,"2.0",3))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        if (req->path) http_free(req->path);
        req->path = http_strdup_size(__TMP_USR_FILE.buf,__TMP_USR_FILE.len);
        if (req->path == NULL)
        {
          n = snprintf(buf, sizeof buf, CFG_RET0("Malloc Failed."), id);
          goto finished;
        }
      }
      else if (p->valueLength > 2 && !memcmp(p->value,"1.",2))
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        if (req->path) http_free(req->path);
        req->path = http_strdup_size(__TMP_INT_FILE.buf,__TMP_INT_FILE.len);
        if (req->path == NULL)
        {
          n = snprintf(buf, sizeof buf, CFG_RET0("Malloc Failed."), id);
          goto finished;
        }
      }
      else
      {
        n = snprintf(buf, sizeof buf, CFG_RET0("Bad Request."), id);
        goto finished;
      }
      
      http_free(query);
      json_pairs_free(pair);
      root_service(conn, req);
      return;
    }
    else if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"users.json", req->path))
    {
      if (req->userLevel != _N_http_user_type_SUPER)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Generate template.json
      n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }

finished:  
    http_free(query);
    json_pairs_free(pair);
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    herror_release(r);
    if (*buf)
    {
      r = http_output_stream_write(conn->out, (unsigned char *)buf, n);
      herror_release(r);
    }
    return;
  }
  else
  {
    if (!strcmp(_nanoConfig_HTTPD_DATA_SERVICE"system.json", req->path))
    {
      // Generate system.json
      r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    }
    else
    {
      r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
    }
    herror_release(r);
    return;
  }
}

static void
file_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  return root_service(conn, req);
}

void json_show( const char * json,
           size_t length )
{
  size_t start = 0, next = 0;
  JSONPair_t pair = { 0 };
  JSONStatus_t result;

  result = JSON_Validate( json, length );
  if( result == JSONSuccess )
  {
    result = JSON_Iterate( json, length, &start, &next, &pair );
  }

  while( result == JSONSuccess )
  {
    if( pair.key != NULL )
    {
      printf( "key: %.*s\t", ( int ) pair.keyLength, pair.key );
    }

    printf( "value: (%s) %.*s\n", json_type2str(pair.jsonType),
            ( int ) pair.valueLength, pair.value );

    result = JSON_Iterate( json, length, &start, &next, &pair );
  }
}

#if __NHTTP_DEBUG
static int json_test(void)
{
  // Variables used in this example.
  JSONStatus_t result;
  char buffer[] = "{\"foo\":\"abc\",\"bar\":{\"foo\":\"xyz\"},"
    "\"ar\":[{\"a1\":0,\"a2\":true},{\"a1\":1,\"a2\":false}]}";
  size_t bufferLength = sizeof( buffer ) - 1;
  char queryKey[] = "bar.foo";
  size_t queryKeyLength = sizeof( queryKey ) - 1;
  char * value;
  size_t valueLength;

  // Calling JSON_Validate() is not necessary if the document is 
  // guaranteed to be valid.
  result = JSON_Validate( buffer, bufferLength );

  if( result == JSONSuccess )
  {
    result = JSON_Search( buffer, bufferLength, queryKey, queryKeyLength,
                          &value, &valueLength );
  }

  if( result == JSONSuccess )
  {
    // The pointer "value" will point to a location in the "buffer".
    char save = value[ valueLength ];
    // After saving the character, set it to a null byte for printing.
    value[ valueLength ] = '\0';
    // "Found: bar.foo -> xyz" will be printed.
    printf( "Found: %s -> %s\n", queryKey, value );
    // Restore the original character.
    value[ valueLength ] = save;
  }
  
  json_show(buffer,strlen(buffer));

  printf("\n\n");
  JSONPair_t *p;
  result = json_parse(&p, buffer,bufferLength);

  //json_print(p,0,"  ");
  //printf("\n\n");
  //json_print(p,-1,"  ");
  //printf("\n\n");

  char buf[1024];
  int m = json_cal_length(p,0,"  ");
  printf("%d=====\n\n",m);
  json_tostr(p,buf,sizeof(buf),0,"  ");
  printf("%lu=====\n\n",strlen(buf));
  printf("%s\n\n",buf);

  int n = json_cal_length(p,-1,"  ");
  printf("%d=====\n\n",n);
  json_tostr(p,buf,sizeof(buf),-1,"  ");
  printf("%lu=====\n\n",strlen(buf));
  printf("%s\n\n",buf);

  json_pairs_free(p);

  return 0;
}
#endif

void main_print_license(void)
{
  fprintf(stderr, "\n%s\n", __HTTPD_LICENSE);
}

int
main(int argc, char **argv)
{
  herror_t status;

  nanohttp_log_set_loglevel(_nanoConfig_HTTPD_LOG_LEVEL);
    
  if (httpd_init(argc, argv))
  {
    fprintf(stderr, "Cannot init httpd\n");
    goto error0;
  }

#if __NG_RING_DEBUG
  test_ring();
#endif

#if __NHTTP_DEBUG
  test_encode_url();
  test_decode_url();
  json_test();
#endif
    
  if ((status = httpd_register_secure("/", root_service, 
    simple_authenticator, "ROOT")) != H_OK)
  {
    fprintf(stderr, "Cannot register service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register(_nanoConfig_HTTPD_FILE_SERVICE, 
    file_service, "FILE")) != H_OK)
  {
    fprintf(stderr, "Cannot register service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_secure("/secure", secure_service, 
    simple_authenticator, "SECURE")) != H_OK)
  {
    fprintf(stderr, "Cannot register secure service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_secure("/headers", headers_service, 
    simple_authenticator, "HEAD")) != H_OK)
  {
    fprintf(stderr, "Cannot register headers service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_secure("/mime", mime_service, 
    simple_authenticator, "MIME")) != H_OK)
  {
    fprintf(stderr, "Cannot register MIME service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_secure("/post/wia.bin", post_service, 
    simple_authenticator, "POST")) != H_OK)
  {
    fprintf(stderr, "Cannot register POST service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register("/favicon.ico", root_service, 
    "FAVICON")) != H_OK)
  {
    fprintf(stderr, "Cannot register default service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_secure(_nanoConfig_HTTPD_DATA_SERVICE, 
    data_service, simple_authenticator, "DATA")) != H_OK)
  {
    fprintf(stderr, "Cannot register DATA service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  if ((status = httpd_register_default_secure("/error", default_service, 
    simple_authenticator)) != H_OK)
  {
    fprintf(stderr, "Cannot register default service (%s)\n", 
      herror_message(status));
    goto error1;
  }

  test_content_type();
  main_print_license();

  fprintf(stderr, "\nneHTTPd is listening on PORT %d\n\n\n", 
    _nanoConfig_HTTPD_PORT);

  if ((status = httpd_run()) != H_OK)
  {
    fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
    goto error1;
  }

  httpd_destroy();
  main_print_license();

  return 0;
    
error1:
  herror_release(status);
error0:
  httpd_destroy();
  return 1;
}
