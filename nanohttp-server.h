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
/******************************************************************
 *  $Id: nanohttp-server.h,v 1.40 2007/11/04 06:57:32 m0gg Exp $
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
#ifndef __nanohttp_server_h
#define __nanohttp_server_h

/* XXX: Clean up nanohttp to make this unnecessary */
#ifdef __NHTTP_INTERNAL
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-response.h"
#endif

/** @page nanohttp_server_page Howto write an HTTP server
 *
 * @section server_sec Table of contents
 * 
 * - @ref nanohttp_server_init_sec
 * - @ref nanohttp_server_service_sec
 * - @ref nanohttp_server_running_sec
 * - @ref nanohttp_server_cleanup_sec
 * - @ref nanohttp_server_function_sec
 *
 * @section nanohttp_server_init_sec Server initialization
 *
 * @code
 * int main(int argc, char **argv)
 * {
 *   herror_t status;
 *   nanohttp_set_level(NANOHTTP_LOG_INFO);
 *
 *   if (httpd_init(argc, argv))
 *   {
 *     ng_fprintf(stderr, "Cannot init httpd\n");
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_service_sec Service registration
 *
 * @code
 *   if ((status = httpd_register("/", root_service)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot register service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register_secure("/secure", secure_service, simple_authenticator)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot register secure service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register("/headers", headers_service)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot register headers service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register_default("/error", default_service)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot register default service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_running_sec Running the server
 *
 * @code
 *   if ((status = httpd_run()) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_cleanup_sec Server cleanup
 *
 * @code
 *   httpd_destroy();
 *
 *   exit(0);
 * }
 * @endcode
 *
 * @section nanohttp_server_function_sec Sample service function
 *
 * @code
 * static void headers_service(httpd_conn_s *conn, struct hrequest_t *req)
 * {
 *   hpair_s *walker;
 *
 *   httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
 *   http_output_stream_write_string(conn->out,
 *     "<html>"
 *       "<head>"
 *         "<title>Request headers</title>"
 *       "</head>"
 *       "<body>"
 *         "<h1>Request headers</h1>"
 *         "<ul>");
 *
 *   for (walker=req->header; walker; walker=walker->next)
 *   {
 *     http_output_stream_write_string(conn->out, "<li>");
 *     http_output_stream_write_string(conn->out, walker->key);
 *     http_output_stream_write_string(conn->out, " = ");
 *     http_output_stream_write_string(conn->out, walker->value);
 *     http_output_stream_write_string(conn->out, "</li>");
 *   }
 *
 *   http_output_stream_write_string(conn->out,
 *         "</ul>"
 *       "</body>"
 *     "</html>");
 *
 *   return;
 * }
 * @endcode
 *
 */

/** @file nanohttp-server.h HTTP server interface
 *
 * @defgroup NANOHTTP nanoHTTP
 */
/**@{*/

/** @defgroup NAONHTTP_CMDLINE_FLAGS Command line flags
 */
/**@{*/
/** @defgroup NANOHTTP_SERVER_CMDLINE_FLAGS Server command line flags
 */
/**@{*/


#define NHTTPD_ARG_PORT	         "-NHTTPport"        /**< Port the server is listening on */
#define NHTTPD_ARG_TERMSIG       "-NHTTPtsig"        /**< Signal which may used to terminate the server */
#define NHTTPD_ARG_MAXCONN       "-NHTTPmaxconn"     /**< Maximum number of simultanous connections */
#define NHTTPD_ARG_MAXCONN_PEND  "-NHTTPmaxconnPend" /**< Maximum number of pending connections for listen*/
#define NHTTPD_ARG_TIMEOUT       "-NHTTPtimeout"     /**< Timeout on reads */
#define NHTTPD_ARG_DAEMONIZE     "-NHTTPDaemon"      /**< Run as a daemon */
#define NHTTPD_ARG_LOGLEVEL      "-NHTTPLoglevel"    /**< log level */
#define NHTTPD_ARG_HELP          "--help"            /**< Run as a daemon */

#define HTTP_INIT_PARSE_RESULT_OK     0
#define HTTP_INIT_PARSE_RESULT_DAEMON 1
#define HTTP_INIT_PARSE_RESULT_HELP   2
#define HTTP_INIT_PARSE_RESULT_ERR    3

/**@}*/
/**@}*/

/** @defgroup NANOHTTP_CLIENT Client */

/** @defgroup NANOHTTP_SERVER Server */
/**@{*/

struct _httpd_conn
{
  hsocket_s *sock;
  char content_type[25];
  http_output_stream_s *out;
  ng_list_head_s header;
}
;
typedef struct _httpd_conn httpd_conn_s;

/** Service callback function for a nanoHTTP service. */
typedef void (*httpd_service_f) (httpd_conn_s *conn, hrequest_s *req);

/** Authentication callback function for a nanoHTTP service. */
typedef int (*httpd_auth_f) (hrequest_s *req, 
                    const ng_block_s *user, const ng_block_s *pass);

#define __NHTTP_USE_STAT_RWLOCK 0
#ifdef __NHTTP_INTERNAL
/** Service statistics per nanoHTTP service. */
struct service_statistics
{
#if __NHTTP_USE_STAT_RWLOCK
  volatile ng_uint64_t requests;
  volatile ng_uint64_t bytes_transmitted;
  volatile ng_uint64_t bytes_received;
  volatile ng_uint64_t time;
  pthread_rwlock_t lock;
#else
  rte_atomic64_t requests;
  rte_atomic64_t bytes_transmitted;
  rte_atomic64_t bytes_received;
  rte_atomic64_t time;
  char lock[0];
#endif
};
typedef struct service_statistics service_statistics_s;
#endif

#if __NHTTP_USE_STAT_RWLOCK
#define STAT_u64_read(a)  (a)
#define STAT_u64_add(a,i) (a) += i
#define STAT_u64_set(a,i) (a) = i
#define STAT_u64_inc(a)   (a)++
#define stat_pthread_rwlock_init(l,a) pthread_rwlock_init(l, a)
#define stat_pthread_rwlock_rdlock(l) pthread_rwlock_rdlock(l)
#define stat_pthread_rwlock_unlock(l) pthread_rwlock_unlock(l)
#define stat_pthread_rwlock_wrlock(l) pthread_rwlock_wrlock(l)
#else
#define STAT_u64_read(a) rte_atomic64_read(&(a))
#define STAT_u64_add(a,i) rte_atomic64_add(&(a), i)
#define STAT_u64_set(a,i) rte_atomic64_set(&(a), i)
#define STAT_u64_inc(a) rte_atomic64_inc(&(a))
#define stat_pthread_rwlock_init(l,a) NG_UNUSED(l);NG_UNUSED(a)
#define stat_pthread_rwlock_rdlock(l) NG_UNUSED(l)
#define stat_pthread_rwlock_unlock(l) NG_UNUSED(l)
#define stat_pthread_rwlock_wrlock(l) NG_UNUSED(l)
#endif

/** @defgroup NANOHTTP_SERVICE_STATUS Service status
 */ 
/**@{*/

/**
 * @see hservice_t
 * @see http://www.w3.org/TR/wslc/
 */
#define NHTTPD_SERVICE_DOWN	0

/**
 * @see hservice_t
 * @see http://www.w3.org/TR/wslc/
 */
#define NHTTPD_SERVICE_UP	1
/**@}*/

/** Service representation object
 */
typedef struct tag_hservice
{
  ng_block_s name;                  /**< Path where service is connected */
  ng_block_s context;               /**< Path where service is connected */
  int status;                       /**< Current status of this service */
  httpd_service_f func;             /**< Service function */
  httpd_auth_f auth;                /**< Authentication function */
  ng_list_head_s link;              /**< Next service in service list */
#ifdef __NHTTP_INTERNAL
  service_statistics_s statistics;  /**< Service statistics */
#endif
}
hservice_t;

#ifdef __cplusplus
extern "C"
{
#endif

/** This function initializes the nanoHTTP server.
 *
 * @see httpd_destroy()
 */
extern herror_t httpd_init(int argc, char *argv[]);

/** This function destroys the nanoHTTP server.
 *
 * @see httpd_init()
 */
extern void httpd_destroy(void);

/** This function executes the nanoHTTP server in an endless loop.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_run(void);

/** This function registers a service routine.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register(const ng_block_s *context, 
  httpd_service_f service, const ng_block_s *service_name);

/** This function registers a service routing which is secured by
 * a password (HTTP basic authentication).
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_secure(const ng_block_s *context, 
  httpd_service_f service, httpd_auth_f auth, 
  const ng_block_s *service_name);

/** This function registers a service routing which is executed if
 * no matching service is found.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_default(const ng_block_s *context, 
  httpd_service_f service);

/** This function registers a serivce routing which is executed if
 * no matching service is found, it is protected by a password.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_default_secure(
  const ng_block_s *context, httpd_service_f service, 
  httpd_auth_f auth);

/** This function returns the port the service is listening on.
 *
 * @return The port the service is listening on.
 */
extern short httpd_get_port(void);

/** This function returns the timeout which is used for read's.
 *
 * @return The read timeout used.
 */
extern int httpd_get_timeout(void);

/** This function sets the timeout which is used for read.
 */
extern void httpd_set_timeout(int secs);

/** This function returns a string representation of the underlying
 * protocol used.
 *
 * @return "http" or "https"
 */
extern const char *httpd_get_protocol(void);

/** This function returns the actual connection count.
 */
extern int httpd_get_conncount(void);

int httpd_create_mutex(void *mutex);
void httpd_destroy_mutex(void *mutex);
int httpd_enter_mutex(void *mutex);
void httpd_leave_mutex(void *mutex);

extern ng_list_head_s *httpd_get_services(void);
extern hservice_t *httpd_find_service(const char *name, int context_len);

extern int httpd_enable_service(hservice_t *service);
extern int httpd_disable_service(hservice_t *service);

static inline void
httpd_response_set_content_type(httpd_conn_s *res, const char *content_type, int len)
{
  len = RTE_MIN(sizeof(res->content_type)-1, len);
  ng_memcpy(res->content_type, content_type, len);
  res->content_type[len] = 0;
  return;
}

extern herror_t httpd_send_header(httpd_conn_s *res, int code);

/** This function sends a minimalistic HTML error document with HTTP
 * status 400.
 *
 * @see HTTP_STATUS_400_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t 
httpd_send_bad_request(httpd_conn_s *conn, const char *msg);

/** This function sends a minimalistc HTML error document with HTTP
 * status 401.
 *
 * @see HTTP_STATUS_401_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t 
httpd_send_unauthorized(httpd_conn_s *conn, const char *realm);

/** This function sends a minimalistic HTML error document with HTTP
 * status 404.
 *
 * @see HTTP_STATUS_404_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t 
httpd_send_not_found(httpd_conn_s *conn, const char *msg);

/** This function sends a minimalistic HTML error document with HTTP
 * status 500.
 *
 * @see HTTP_STATUS_500_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t 
httpd_send_internal_error(httpd_conn_s * conn, const char *msg);

/** This functions sends a minimalistic HTML error document with HTTP
 * status 501.
 *
 * @see HTTP_STATUS_501_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_not_implemented(httpd_conn_s *conn, const char *msg);

extern int httpd_parse_arguments(int argc, char **argv);

extern ng_block_s __nanohttp_html[];
#define nanohttp_index_html_head_JS_MENU_GUEST &__nanohttp_html[0]
#define nanohttp_index_html_head_JS_MENU_ADMIN &__nanohttp_html[1]
#define nanohttp_index_html_head_JS_MENU_SUPER &__nanohttp_html[2]
#define nanohttp_index_html_head_DECL1         &__nanohttp_html[3]
#define nanohttp_index_html_head_DECL2         &__nanohttp_html[4]
#define nanohttp_index_html_head_LOGIN         &__nanohttp_html[5]

extern void nanohttpd_stop_running(void);

#ifdef __cplusplus
}
#endif

/**@}*/
/**@}*/

#endif
