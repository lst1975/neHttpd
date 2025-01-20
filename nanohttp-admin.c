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
/** @file nanohttp-admin.c Administrator application */
/******************************************************************
*  $Id: nanohttp-admin.c,v 1.13 2007/11/03 22:40:10 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
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
#include "nanohttp-error.h"
#include "nanohttp-logging.h"
#include "nanohttp-common.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-server.h"
#include "nanohttp-code.h"
#include "nanohttp-admin.h"

static void
_httpd_admin_send_title(httpd_conn_s *conn, const char *title)
{
  httpd_send_header(conn, HTTP_RESPONSE_CODE_200_OK);

  http_output_stream_write_string(conn->out,
   "<html>"
     "<head>");
  http_output_stream_write_string(conn->out,
       "<style>"
         ".logo {"
         " color: #005177;"
         " background-color: transparent;"
         " font-family: Calligraphic, arial, sans-serif;"
         " font-size: 36px;"
         "}"
       "</style>");
  http_output_stream_write_string(conn->out,
     "</head>"
     "<body>"
       "<span class=\"logo\">nhttpd</span> ");
  http_output_stream_write_string(conn->out, title);
  http_output_stream_write_string(conn->out, "<hr />");
}

static inline void
_httpd_admin_send_footer(httpd_conn_s *conn)
{
  http_output_stream_write_string(conn->out,
        "<hr />"
        "<a href=\"" NHTTPD_ADMIN_CONTEXT "\">Admin page</a> "
      	"<a href=\"http://csoap.sf.net/\">cSOAP Home</a>"
      "</body>"
    "</html>");
}

static void
_httpd_admin_list_services(httpd_conn_s *conn)
{
  char buffer[1024];
  hservice_t *node;

  _httpd_admin_send_title(conn, "Available services");

  http_output_stream_write_string(conn->out, "<ul>");
  for (node = httpd_get_services(); node; node = node->next)
  {
    switch (node->status)
    {
      case NHTTPD_SERVICE_DOWN:
        ng_snprintf(buffer, sizeof(buffer),
          "<li>"
            "<a href=\"%s\">%s</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_ACTIVATE_SERVICE "=%s\">[Activate]</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a>"
          "</li>",
          node->context, node->context, node->context, node->context);
        break;
      case NHTTPD_SERVICE_UP:
      default:
        ng_snprintf(buffer, sizeof(buffer),
          "<li>"
            "<a href=\"%s\">%s</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_PASSIVATE_SERVICE "=%s\">[Passivate]</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a> "
          "</li>",
          node->context, node->context, node->context, node->context);
        break;
    }
    http_output_stream_write_string(conn->out, buffer);
  }
  http_output_stream_write_string(conn->out, "</ul>");
  
  _httpd_admin_send_footer(conn);
}

#ifdef __NHTTP_INTERNAL
static void
_httpd_admin_list_statistics(httpd_conn_s *conn, 
  const char *service_name, int service_name_len)
{
  char buffer[1024];
  hservice_t *service;
  int n;
  
  log_verbose("Client requested statistics for \"%.*s\".", 
    service_name_len, service_name);

  ng_snprintf(buffer, sizeof(buffer), 
    "Listing statistics for service <b>%.*s</b>", 
    service_name_len, service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!service_name 
    || !(service = httpd_find_service(service_name, service_name_len)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");
    _httpd_admin_send_footer(conn);
    return;
  }

  stat_pthread_rwlock_rdlock(&(service->statistics.lock));
  ng_uint64_t secs = STAT_u64_read(service->statistics.time);
  n = ng_snprintf(buffer, sizeof(buffer), 
                  "<ul>"
                    "<li>Requests served: %"PRIu64"</li>"
                    "<li>Bytes read: %"PRIu64"</li>"
                    "<li>Bytes sent: %"PRIu64"</li>"
                    "<li>Time used: %li second%s</li>"
                  "</ul>",
                  STAT_u64_read(service->statistics.requests),
                  STAT_u64_read(service->statistics.bytes_received),
                  STAT_u64_read(service->statistics.bytes_transmitted),
                  secs, secs ? "s":"");
  stat_pthread_rwlock_unlock(&(service->statistics.lock));

  http_output_stream_write(conn->out, (unsigned char *)buffer, n);
  _httpd_admin_send_footer(conn);
}
#endif

static void
_httpd_admin_enable_service(httpd_conn_s *conn, 
  const char *service_name, int service_name_len)
{
  hservice_t *service;
  char buffer[1024];

  ng_snprintf(buffer, sizeof(buffer), "Activating service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!(service = httpd_find_service(service_name, service_name_len)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");

    _httpd_admin_send_footer(conn);
    return;   
  }

  httpd_enable_service(service);

  http_output_stream_write_string(conn->out,
    "<p>"
      "Service is up"
    "</p>");

  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_disable_service(httpd_conn_s *conn, 
  const char *service_name, int service_name_len)
{
  hservice_t *service;
  char buffer[1024];

  ng_snprintf(buffer, sizeof(buffer), "Passivating service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!(service = httpd_find_service(service_name, service_name_len)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");
    _httpd_admin_send_footer(conn);
    return;
  }

  httpd_disable_service(service);

  http_output_stream_write_string(conn->out,
    "<p>"
      "Service is down"
    "</p>");
  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_set_loglevel(httpd_conn_s *conn, const char *loglevel)
{
  nanohttp_loglevel_t old;
  char buffer[256];
  char *tmp;

  if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_OFF_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_OFF);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_VERBOSE_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_DEBUG_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_DEBUG);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_INFO_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_INFO);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_WARN_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_WARN);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_ERROR_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_ERROR);
  }
  else if (ng_strcmp(loglevel, NANOHTTP_LOG_LEVEL_FATAL_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_FATAL);
  }
  else
  {
    old = nanohttp_log_get_loglevel();
    loglevel = NANOHTTP_LOG_LEVEL_UNKNOWN_STRING;
    log_error("unknown loglevel requested");
  }

  switch (old)
  {
    case NANOHTTP_LOG_OFF:
      tmp = NANOHTTP_LOG_LEVEL_OFF_STRING;
      break;
    case NANOHTTP_LOG_VERBOSE:
      tmp = NANOHTTP_LOG_LEVEL_VERBOSE_STRING;
      break;
    case NANOHTTP_LOG_DEBUG:
      tmp = NANOHTTP_LOG_LEVEL_DEBUG_STRING;
      break;
    case NANOHTTP_LOG_INFO:
      tmp = NANOHTTP_LOG_LEVEL_INFO_STRING;
      break;
    case NANOHTTP_LOG_WARN:
      tmp = NANOHTTP_LOG_LEVEL_WARN_STRING;
      break;
    case NANOHTTP_LOG_ERROR:
      tmp = NANOHTTP_LOG_LEVEL_ERROR_STRING;
      break;
    case NANOHTTP_LOG_FATAL:
      tmp = NANOHTTP_LOG_LEVEL_FATAL_STRING;
      break;
    default:
      tmp = NANOHTTP_LOG_LEVEL_UNKNOWN_STRING;
      break;
  }

  _httpd_admin_send_title(conn, "Adjusting loglevel");

  ng_snprintf(buffer, sizeof(buffer), "<p>Switching from %s to %s</p>", tmp, loglevel);
  http_output_stream_write_string(conn->out, buffer);

  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_handle_get(httpd_conn_s *conn, hrequest_s *req)
{
  hpair_s *param;
#define ___(X) (X), sizeof(X)-1
  if ((param = hpairnode_get_ignore_case(&req->query, 
        ___(NHTTPD_ADMIN_QUERY_SERVICES))))
  {
    _httpd_admin_list_services(conn);
  }
#ifdef __NHTTP_INTERNAL
  else if ((param = hpairnode_get_ignore_case(&req->query, 
        ___(NHTTPD_ADMIN_QUERY_STATISTICS))))
  {
    _httpd_admin_list_statistics(conn, param->val.cptr, param->val.len);
  }
#endif
  else if ((param = hpairnode_get_ignore_case(&req->query, 
        ___(NHTTPD_ADMIN_QUERY_ACTIVATE_SERVICE))))
  {
    _httpd_admin_enable_service(conn, param->val.cptr, param->val.len);
  }
  else if ((param = hpairnode_get_ignore_case(&req->query, 
        ___(NHTTPD_ADMIN_QUERY_PASSIVATE_SERVICE))))
  {
    _httpd_admin_disable_service(conn, param->val.cptr, param->val.len);
  }
  else if ((param = hpairnode_get_ignore_case(&req->query, 
    ___(NHTTPD_ADMIN_QUERY_SET_LOGLEVEL))))
  {
    _httpd_admin_set_loglevel(conn, param->val.cptr);
  }
  else
  {
    _httpd_admin_send_title(conn, "Welcome to the admin page");

    http_output_stream_write_string(conn->out,
      "<ul>"
        "<li>"
          "<a href=\"?" NHTTPD_ADMIN_QUERY_SERVICES "\">Services</a>"
        "</li>"
        "<li>"
          "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "\">Statistics</a>"
        "</li>"
      	"<li>Set loglevel: "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_OFF_STRING "\">OFF</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_VERBOSE_STRING "\">VERBOSE</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_DEBUG_STRING "\">DEBUG</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_INFO_STRING "\">INFO</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_WARN_STRING "\">WARN</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_ERROR_STRING "\">ERROR</a> "
      	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_FATAL_STRING "\">FATAL</a> "
      	"</li>"
      "</ul>");

    _httpd_admin_send_footer(conn);
  }
#undef ___
}

static void
_httpd_admin_entry(httpd_conn_s *conn, hrequest_s *req)
{
  if (req->method == HTTP_REQUEST_METHOD_GET)
  {
    _httpd_admin_handle_get(conn, req);
  }
  else
  {
    httpd_send_header(conn, HTTP_RESPONSE_CODE_501_Not_Implemented);
    http_output_stream_write_string(conn->out,
      "<html>"
        "<head>"
        "</head>"
      "<body>"
        "<h1>Sorry!</h1>"
        "<hr />"
        "<div>Only GET method is implemented now. Use your browser.</div>"
      "</body>"
    "</html>");
  }
}

herror_t
httpd_admin_init_args(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++)
  {
    if (!ng_strcmp(argv[i], NHTTPD_ARG_ENABLE_ADMIN))
    {
      httpd_register(NHTTPD_ADMIN_CONTEXT, 
        sizeof(NHTTPD_ADMIN_CONTEXT)-1, _httpd_admin_entry, "ADMIN", 5);
      break;
    }
  }

  return H_OK;
}
