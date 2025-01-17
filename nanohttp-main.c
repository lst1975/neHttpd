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
#include "nanohttp-config.h"
#include "nanohttp-logging.h"
#include "nanohttp-json.h"
#include "nanohttp-file.h"
#include "nanohttp-urlencode.h"
#include "nanohttp-base64.h"
#include "nanohttp-user.h"
#include "nanohttp-ring.h"
#include "nanohttp-data.h"
#include "nanohttp-socket.h"
#include "nanohttp-system.h"
#include "nanohttp-signal.h"
#include "nanohttp-header.h"
#include "nanohttp-vsprintf.h"
#include "nanohttp-url.h"
#include "nanohttp-const.h"
#include "nanohttp-code.h"
#include "nanohttp-mime.h"
#include "nanohttp-server.h"

static int
simple_authenticator(hrequest_s *req, const ng_block_s *user, 
  const ng_block_s *password)
{
  httpd_user_t *auth_user;
  log_debug("logging in user=\"%.*s\" password=\"%.*s\"\n", 
    user->len, user->cptr, password->len, password->cptr);

  auth_user = nanohttp_users_match(user->buf, user->len, password->buf, password->len);
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
static const ng_buffer_s __TEST_DEV_FILE  =___("/device.json");
#undef ___
static void
default_service(httpd_conn_s *conn, hrequest_s *req)
{
  char buf[REQUEST_MAX_PATH_SIZE+256+1];

  if (ng_block_isequal__(&__TEST_DEV_FILE, &req->path))
  {
    httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
    http_output_stream_write_string(conn->out,
      "{"
        "'shortAddr':10,"
        "'channel':12"
      "}");
  }
  else
  {
    ng_snprintf(buf, sizeof buf, "Resource \"%pS\" not found", &req->path);
    httpd_send_not_found(conn, buf);
  }
  return;
}

static void
headers_service(httpd_conn_s *conn, hrequest_s *req)
{
  hpair_s *walker;
  herror_t status;

  status = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
  if (status != H_OK) goto clean0;
  
  status = http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Request headers</title>"
      "</head>"
      "<body>"
        "<h1>Request headers</h1>"
        "<ul>");
  if (status != H_OK) goto clean0;

  ng_list_for_each_entry(walker,hpair_s,&req->header,link)
  {
    status = http_output_stream_write_printf(conn->out, 
      "<li>%pS: %pS</li>", &walker->key, &walker->val);
    if (status != H_OK) goto clean0;
  }

  status = http_output_stream_write_string(conn->out,
        "</ul>"
      "</body>"
    "</html>");
  if (status != H_OK) goto clean0;

clean0:
  herror_release(status);
}

static void
mime_service(httpd_conn_s *conn, hrequest_s *req)
{
  httpd_send_not_implemented(conn, "mime_service");
}

static herror_t
__post_internal_error(httpd_conn_s *conn, herror_t r)
{
  herror_t sr;

  herror_log(r);
  sr = httpd_send_header(conn, HTTP_RESPONSE_CODE_500_Internal_Server_Error);
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
__post_service(httpd_conn_s *conn, hrequest_s *req, 
  const char *file)
{
  herror_t r;
  
  if (req->method == HTTP_REQUEST_METHOD_POST)
  {
    mime_parser_s p;

    r = multipartparser_init(&p, req, req->content_type);
    if (r != H_OK)
    {
      log_error("multipartparser_init failed (%s).", herror_message(r));
      return __post_internal_error(conn, r);
    }

    r = multipart_get_attachment(&p, req->in);
    if (r != H_OK)
    {
      log_error("multipart_get_attachment failed (%s).", herror_message(r));
      return __post_internal_error(conn, r);
    }
    
    return httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
  }
  else
  {
    return httpd_send_not_implemented(conn, "post_service");
  }
}

static void
post_service(httpd_conn_s *conn, hrequest_s *req)
{
  herror_t r = __post_service(conn, req, "config/gw.bin");
  if (r != H_OK)
  {
    herror_log(r);
    herror_release(r);
  }
}

static herror_t
__root_service_read(void *arg, const char *buf, ng_size_t length)
{
  httpd_conn_s *conn=(httpd_conn_s *)arg;
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
__send_menu_js(httpd_conn_s *conn, hrequest_s *req)
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
__send_root_html(httpd_conn_s *conn, hrequest_s *req)
{
  herror_t r = H_OK;

  do
  {
    r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
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
secure_service(httpd_conn_s *conn, hrequest_s *req)
{
  herror_t r;

  r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
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
static const ng_block_s __SYS_FILE=__("system.json");
static const ng_block_s __DEV_FILE=__("device.json");
static const ng_block_s __USR_FILE=__("users.json");
static const ng_block_s __MIB_FILE=__("setmib.json");
static const ng_block_s __ADD_FILE=__("add.json");
static const ng_block_s __DEL_FILE=__("del.json");
static const ng_block_s __SAV_FILE=__("save.json");
static const ng_block_s __TEM_FILE=__("template.json");
static const ng_block_s __TMP_USR_FILE  =__("templateUser.json");
static const ng_block_s __TMP_INT_FILE  =__("templateInterface.json");
static const ng_block_s __FAVOR_FILE =___("/favicon.ico");
static const ng_block_s __ROOT_FILE  =___("/");
#undef ___
#undef __
#undef _

static void
root_service(httpd_conn_s *conn, hrequest_s *req)
{
  herror_t r = NULL;

  if (strstr(req->path.buf, "..") != NULL)
  {
    return default_service(conn, req);
  }
  else
  {
    if (ng_block_isequal__(&__ROOT_FILE, &req->path))
    {
      r = __send_root_html(conn, req);
    }
    else if (ng_block_isequal__(&__FAVOR_FILE, &req->path))
    {
      unsigned char *bf=NULL;
      do
      {
        int len = B64_DECLEN(sizeof(favorICON)-1)+1;
        bf = (unsigned char *)ng_malloc(len);
        if (bf == NULL) 
        {
          log_error("ng_malloc failed");
          break;
        }
#if __configUseStreamBase64
        len = b64Decode_with_len(favorICON, sizeof(favorICON)-1, (char *)bf, len);
        if (len < 0)
        {
          log_error("b64Decode failed");
          break;
        }
#else      
        len = base64_decode_string((const unsigned char *)favorICON, bf);
#endif
        if (hpairnode_set_header(&conn->header, 
          &__HDR_BUF__(HEADER_CONTENT_TYPE), 
          ng_http_get_mime_type(&cstr_icon))) 
        {
          log_error("httpd_set_header failed");
          break;
        }
        r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
        if (r != NULL) break;
        r = http_output_stream_write(conn->out, bf, len);
        if (r != NULL) break;
      } 
      while(0);
      
      if (bf != NULL)
        ng_free(bf);
    }
    else if (!ng_block_isequal__(&__MIB_FILE, &req->path)
          && !ng_block_isequal__(&__ADD_FILE, &req->path)
          && !ng_block_isequal__(&__DEL_FILE, &req->path)
          && !ng_block_isequal__(&__SAV_FILE, &req->path)
#if !__NHTTP_TEST      
          && !ng_block_isequal__(&__DEV_FILE, &req->path)
          && !ng_block_isequal__(&__USR_FILE, &req->path)
          && !ng_block_isequal__(&__SYS_FILE, &req->path)
#endif      
      )
    {
      log_debug("Try to open the file %pS.", &req->path);

      void *file = nanohttp_file_open_for_read(req->path.cptr+1, 
        req->path.len - 1);
      if (file == NULL)
      {
        // If the file does not exist
        log_error("Not able to open the file %pS for reading.", &req->path);
        r = httpd_send_header(conn, HTTP_RESPONSE_CODE_404_Not_Found);
      }
      else
      {
        r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
        if (r == NULL)
        {
          r = nanohttp_file_read_callback(file, __root_service_read, conn);
          if (r != NULL)
          {
            herror_release(r);
            r = http_output_stream_write_printf(conn->out, 
                  "Failed to readfile %pS: %s", 
                  &req->path, herror_message(r));
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
#define CFG_ret1 __CFG_ret0("%pS")
#define CFG_ret3 "\"}]}"
#define CFG_RET0(msg) __CFG_ret0("") msg CFG_ret3
#define CFG_RET1(msg) CFG_ret1 msg CFG_ret3

static void
data_service(httpd_conn_s *conn, hrequest_s *req)
{
  herror_t r;

#if __NHTTP_TEST      
  if (ng_block_isequal__(&__SYS_FILE, &req->path)
   || ng_block_isequal__(&__DEV_FILE, &req->path)
   || ng_block_isequal__(&__USR_FILE, &req->path))
  {
    return root_service(conn, req);
  }
#endif

  if (!ng_list_empty(&req->query))
  {
    char buf[126]={0};
    int n, len, id;
    unsigned char *query;
    hpair_s *data;
    JSONPair_s *pair,*p;
    JSONStatus_e result;
    ng_buffer_s in;

    data = hpairnode_get(&req->query, &cstr_data);
    if (data == NULL)
    {
      log_error("No query data.");
      goto finished;
    }
    
    log_debug("try to decode %pS\n", data);
    
    /* decode_url malloced new data */
    if (0 > decode_url(&in, data->val.ptr, data->val.len))
    {
      log_error("Failed to parse query");
      goto finished;
    }
    
    log_debug("decoded query is : %.*s", in.len, in.cptr);

    len = B64_DECLEN(in.len) + 1;
    query = (unsigned char *)ng_malloc(len);
    if (query == NULL)
    {
      ng_free(in.ptr);
      log_error("Failed to ng_malloc key");
      goto finished;
    }
    
    {
#if __configUseStreamBase64
      len = b64Decode_with_len(in.buf, in.len, (char *)query, len);
      if (len < 0)
      {
        ng_free(in.ptr);
        log_error("b64Decode failed");
        goto finished;
      }
#else      
      len = base64_decode_string(in.ptr, query);
#endif
      ng_free(in.ptr);
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
    
    if (ng_block_isequal__(&__MIB_FILE, &req->path))
    {
      // Process del operation
      int err, usrLen;

      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }

      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONObject)
      {
        // Process set operation
        n = ng_snprintf(buf, sizeof buf, CFG_RET0("Mailformed Request."), id);
        goto finished;
      }
      
      for (p=p->children; p != NULL; p = p->siblings)
      {
        if (p->key.len >= ___USER_PFX_LEN 
          && !ng_memcmp(p->key.cptr, ___USER_PFX_STR, ___USER_PFX_LEN))
        {
          const char *colon;
          
          if (req->userLevel != _N_http_user_type_SUPER)
          {
            // Process set operation
            n = ng_snprintf(buf, sizeof buf, 
                    CFG_RET0("Authorization Failed."), id);
            goto finished;
          }
          
          colon = ng_memchr(p->key.cptr, ':', p->key.len);
          if (colon != NULL && colon - p->key.cptr > ___USER_PFX_LEN)
          {
            const char *usr;
            
            usr = colon + 1;
            usrLen = p->key.len - (usr - p->key.cptr) - 2;
            if ((usrLen < _N_http_user_NAME_MINLEN
              || usrLen > _N_http_user_NAME_MAXLEN))
            {
              n = ng_snprintf(buf, sizeof buf, 
                    CFG_RET1("Invalid username length."), 
                    id, 
                    &p->key);
              goto finished;
            }
            if (!ng_memcmp(p->key.cptr+p->key.len-2, ".0", 2))
            {
              n = ng_snprintf(buf, sizeof buf, 
                    CFG_RET1("This user does not exists."), 
                    id, 
                    &p->key);
            }
            else 
            {
              if (!ng_memcmp(p->key.cptr+p->key.len-2, ".1", 2))
              {
                err = nanohttp_users_update(usr, usrLen, 
                        p->val.cptr, p->val.len, NULL, 0);
              }
              else if (!ng_memcmp(p->key.cptr+p->key.len-2, ".2", 2))
              {
                err = nanohttp_users_update(usr, usrLen, 
                        NULL, 0, p->val.cptr, p->val.len);
              }
              else
              {
                err = _N_http_user_error_SYS;
              }
              switch (err)
              {
                case _N_http_user_error_NONE:
                  n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
                  break;
                case _N_http_user_error_PERM:
                  n = ng_snprintf(buf, sizeof buf, 
                        CFG_RET1("Superuser can not be deleted."), 
                        id, 
                        &p->key);
                  goto finished;
                case _N_http_user_error_EXIST:
                  n = ng_snprintf(buf, sizeof buf, 
                        CFG_RET1("This user does not exists."), 
                        id, 
                        &p->key);
                  goto finished;
                case _N_http_user_error_VNAME:
                  n = ng_snprintf(buf, sizeof buf, 
                        CFG_RET1("The length of username is invalid: [%d,%d]."), 
                        id, 
                        &p->key,
                        _N_http_user_NAME_MINLEN, _N_http_user_NAME_MAXLEN);
                  goto finished;
                case _N_http_user_error_INVAL:
                  n = ng_snprintf(buf, sizeof buf, 
                        CFG_RET1("Invalid account type."), 
                        id, 
                        &p->key);
                  goto finished;
                case _N_http_user_error_SYS:
                default:
                  n = ng_snprintf(buf, sizeof buf, 
                        CFG_RET1("System error."), 
                        id, 
                        &p->key);
                  goto finished;
              }
            }
          }
          else
          {
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("Invalid parameter."), 
                  id, 
                  &p->key);
            goto finished;
          }
        }
        else
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("For testing! received error message from our product."), 
                  id, 
                  &p->key);
          goto finished;
        }
      }
    }
    else if (ng_block_isequal__(&__SAV_FILE, &req->path))
    {
      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Process save operation
      n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }
    else if (ng_block_isequal__(&__ADD_FILE, &req->path))
    {
      int err;
      JSONPair_s *usr, *pwd, *type;
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
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        if (!usr || !pwd)
        {
          if (usr)
            n = ng_snprintf(buf, sizeof buf, CFG_RET1("No password."), id, 
                  &usr->key);
          else
            n = ng_snprintf(buf, sizeof buf, CFG_RET0("No username."), id);
          goto finished;
        }
        err = nanohttp_users_add(
          usr->val.cptr, 
          usr->val.len,
          pwd->val.cptr, 
          pwd->val.len,
          type ? type->val.cptr : NULL,
          type ? type->val.len : 0
          );
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_PERM:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("Superuser can not be deleted."), 
                  id, 
                  &usr->key);
            goto finished;
          case _N_http_user_error_EXIST:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("This user already exists."), 
                  id, 
                  &usr->key);
            goto finished;
          case _N_http_user_error_VNAME:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  &usr->key, 
                  _N_http_user_NAME_MINLEN, 
                  _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_VPSWD:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  &pwd->key, 
                  _N_http_user_PSWD_MINLEN, 
                  _N_http_user_PSWD_MAXLEN);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = ng_snprintf(buf, sizeof buf, CFG_RET1("System error."), id, 
                  &usr->key);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), 
            id);
          goto finished;
        }
        n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (ng_block_isequal__(&__DEL_FILE, &req->path))
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

      if (p->val.len >= ___USER_PFX_LEN
        && !ng_memcmp(p->val.cptr, ___USER_PFX_STR, ___USER_PFX_LEN))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        colon = ng_memchr(p->val.cptr, ':', p->val.len);
        if (colon == NULL || colon-p->val.cptr < ___USER_PFX_LEN + 1)
        {
          n = ng_snprintf(buf, sizeof buf, 
                CFG_RET1("This user does not exists."), 
                id, 
                &p->val);
          goto finished;
        }
        usr = colon + 1;
        usrLen = p->val.len - (usr - p->val.cptr);
        if ((usrLen < _N_http_user_NAME_MINLEN
          || usrLen > _N_http_user_NAME_MAXLEN))
        {
          n = ng_snprintf(buf, sizeof buf, 
                CFG_RET1("Invalid username length."), 
                id, 
                &p->val);
          goto finished;
        }
        
        err = nanohttp_users_del(usr, usrLen);
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_EXIST:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("This user does not exists."), 
                  id, 
                  &p->val);
            goto finished;
          case _N_http_user_error_VNAME:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("The length of username is invalid: [%d,%d]."), 
                  id, 
                  &p->val,
                  _N_http_user_NAME_MINLEN, 
                  _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_PERM:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("Superuser can not be deleted."), 
                  id, 
                  &p->val);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = ng_snprintf(buf, sizeof buf, 
                  CFG_RET1("System error."), 
                  id, 
                  &p->val);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (ng_block_isequal__(&__TEM_FILE, &req->path))
    {
      // Generate template.json
      p = json_find_bykey(pair, "value", 5);
      if (p == NULL || p->jsonType != JSONString)
      {
        goto finished;
      }
      if (p->val.len == 3 && !ng_memcmp(p->val.cptr,"2.0",3))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        if (ng_dup_data_block(&req->path, &__TMP_USR_FILE, ng_TRUE) < 0)
        {
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Malloc Failed."), id);
          goto finished;
        }
      }
      else if (p->val.len > 2 && !ng_memcmp(p->val.cptr, "1.", 2))
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        if (ng_dup_data_block(&req->path, &__TMP_INT_FILE, ng_TRUE) < 0)
        {
          n = ng_snprintf(buf, sizeof buf, CFG_RET0("Malloc Failed."), id);
          goto finished;
        }
      }
      else
      {
        n = ng_snprintf(buf, sizeof buf, CFG_RET0("Bad Request."), id);
        goto finished;
      }
      
      ng_free(query);
      json_pairs_free(pair);
      root_service(conn, req);
      return;
    }
    else if (ng_block_isequal__(&__USR_FILE, &req->path))
    {
      if (req->userLevel != _N_http_user_type_SUPER)
      {
        // Process set operation
        n = ng_snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Generate template.json
      n = ng_snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }

finished:  
    ng_free(query);
    json_pairs_free(pair);
    r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
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
    if (ng_block_isequal__(&__SYS_FILE, &req->path))
    {
      // Generate system.json
      r = httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);
    }
    else
    {
      r = httpd_send_header(conn, HTTP_RESPONSE_CODE_204_No_Content);
    }
    herror_release(r);
    return;
  }
}

static void
file_service(httpd_conn_s *conn, hrequest_s *req)
{
  return root_service(conn, req);
}

void json_show( const char * json,
           ng_size_t length )
{
  ng_size_t start = 0, next = 0;
  JSONPair_s pair = { 0 };
  JSONStatus_e result;

  result = JSON_Validate(json, length);
  if (result == JSONSuccess)
  {
    result = JSON_Iterate(json, length, &start, &next, &pair);
  }

  while (result == JSONSuccess)
  {
    if (pair.key.cptr != NULL && pair.key.len)
    {
      printf("key: %pS\t", &pair.key);
    }

    printf("value: (%s) %pS\n", 
      json_type2str(pair.jsonType), &pair.val);

    result = JSON_Iterate(json, length, &start, &next, &pair);
  }
}

#if __NHTTP_DEBUG
static int json_test(void)
{
  // Variables used in this example.
  JSONStatus_e result;
  char buffer[] = "{\"foo\":\"abc\",\"bar\":{\"foo\":\"xyz\"},"
    "\"ar\":[{\"a1\":0,\"a2\":true},{\"a1\":1,\"a2\":false}]}";
  ng_size_t bufferLength = sizeof( buffer ) - 1;
  char queryKey[] = "bar.foo";
  ng_size_t queryKeyLength = sizeof( queryKey ) - 1;
  char * value;
  ng_size_t valueLength;

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
  
  json_show(buffer,ng_strlen(buffer));

  printf("\n\n");
  JSONPair_s *p;
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

void main_print_license(int daemonize)
{
  const char *p;
  const char *s = __HTTPD_LICENSE;
  int len = sizeof(__HTTPD_LICENSE)-1;
  const char *e = s + len;

  json_printer_default(NULL, "\n");
  while (1) {
    p = ng_memchr(s, '\n', len);
    if (p == NULL)
      break;
    if (daemonize)
      json_printer_default(NULL, "%.*s", p - s, s);
    else
      json_printer_default(NULL, "%.*s", p + 1 - s, s);
    s = p + 1;
    len = e - s;
  };
  json_printer_default(NULL, "\n");
  
  json_printer_default(NULL, "Ver: %s, running on OS: %s\n\n", 
    NG_VER_BUILD, ng_os_info.ng_os_version);
}

int
main(int argc, char **argv)
{
  RTE_BUILD_BUG_ON(sizeof(ng_uint64_t) != 8);
  RTE_BUILD_BUG_ON(sizeof(ng_uint32_t) != 4);
  RTE_BUILD_BUG_ON(sizeof(ng_uint16_t) != 2);
  RTE_BUILD_BUG_ON(sizeof(ng_uint8_t)  != 1);
  
  RTE_BUILD_BUG_ON(sizeof( ng_int64_t) != 8);
  RTE_BUILD_BUG_ON(sizeof( ng_int32_t) != 4);
  RTE_BUILD_BUG_ON(sizeof( ng_int16_t) != 2);
  RTE_BUILD_BUG_ON(sizeof( ng_int8_t)  != 1);
  
  int daemonize = 0, result; 
  herror_t status;

  nanohttp_log_set_loglevel(_nanoConfig_HTTPD_LOG_LEVEL);
    
  if (memchr_test()<0)
  {
    goto error0;
  }
  
  result = httpd_parse_arguments(argc, argv);
  if (result == HTTP_INIT_PARSE_RESULT_DAEMON)
  {
    nanohttp_log_set_logtype(NANOHTTP_LOG_SYSLOG);
    daemonize = 1;
    http_daemonize(1,0);
  }
  else if (result == HTTP_INIT_PARSE_RESULT_HELP)
  {
    fprintf(stdout, __nanohttp_help);
    return EXIT_SUCCESS;
  }
  else if (result == HTTP_INIT_PARSE_RESULT_ERR)
  {
    goto error0;
  }
  
  if (ng_os_init() != ng_ERR_NONE)
  {
    log_stderr("Cannot init OS\n");
    goto error0;
  }
    
  if (httpd_init(argc, argv))
  {
    log_stderr("Cannot init httpd\n");
    goto error1;
  }

#if __NHTTP_VSNPRINTF_DEBUG
  snprintf_test();
#endif

#if __NHTTP_URL_TEST
  ng_url_test();
#endif

#if __NG_RING_DEBUG
  test_ring();
#endif

#if __NHTTP_DEBUG
  test_encode_url();
  test_decode_url();
  json_test();
#endif
    
  if ((status = httpd_register_secure("/", 1, root_service, 
    simple_authenticator, "ROOT", 4)) != H_OK)
  {
    log_stderr("Cannot register service.");
    goto error2;
  }

  if ((status = httpd_register(_nanoConfig_HTTPD_FILE_SERVICE, 
    sizeof(_nanoConfig_HTTPD_FILE_SERVICE)-1,
    file_service, "FILE", 4)) != H_OK)
  {
    log_stderr("Cannot register service.");
    goto error2;
  }

  if ((status = httpd_register_secure("/secure", 7, secure_service, 
    simple_authenticator, "SECURE", 6)) != H_OK)
  {
    log_stderr("Cannot register secure service.");
    goto error2;
  }

  if ((status = httpd_register_secure("/headers", 8, headers_service, 
    simple_authenticator, "HEAD", 4)) != H_OK)
  {
    log_stderr("Cannot register headers service.");
    goto error2;
  }

  if ((status = httpd_register_secure("/mime", 5, mime_service, 
    simple_authenticator, "MIME", 4)) != H_OK)
  {
    log_stderr("Cannot register MIME service.");
    goto error2;
  }

  if ((status = httpd_register_secure("/post/wia.bin", 13, post_service, 
    simple_authenticator, "POST", 4)) != H_OK)
  {
    log_stderr("Cannot register POST service.");
    goto error2;
  }

  if ((status = httpd_register("/favicon.ico", 12, root_service, 
    "FAVICON", 7)) != H_OK)
  {
    log_stderr("Cannot register default service.");
    goto error2;
  }

  if ((status = httpd_register_secure(_nanoConfig_HTTPD_DATA_SERVICE, 
    sizeof(_nanoConfig_HTTPD_DATA_SERVICE)-1,
    data_service, simple_authenticator, "DATA", 4)) != H_OK)
  {
    log_stderr("Cannot register DATA service.");
    goto error2;
  }

  if ((status = httpd_register_default_secure("/error", 6, default_service, 
    simple_authenticator)) != H_OK)
  {
    log_stderr("Cannot register default service.");
    goto error2;
  }

  test_content_type();
  ng_os_dump(NULL, NULL);
  main_print_license(daemonize);

  log_print("neHTTPd is listening on PORT %d\n\n", httpd_get_port());

  if ((status = httpd_run()) != H_OK)
  {
    log_stderr("Cannot run httpd (%s)\n", herror_message(status));
    goto error2;
  }

  if (daemonize)
    closelog();
    
  httpd_destroy();
  ng_os_deinit();
  main_print_license(daemonize);

  return EXIT_SUCCESS;
    
error2:
  herror_log(status);
  herror_release(status);
error1:
  httpd_destroy();
  ng_os_deinit();
error0:
  if (daemonize)
    closelog();
  return EXIT_FAILURE;
}
