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
/** @file nanohttp-server.c HTTP server */
/******************************************************************
*  $Id: nanohttp-server.c,v 1.81 2007/11/03 22:40:12 m0gg Exp $
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

#ifdef HAVE_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <limits.h>  // For PATH_MAX

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-response.h"
#include "nanohttp-server.h"
#include "nanohttp-base64.h"
#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#endif
#include "nanohttp-ssl.h"
#else
static inline int hssl_enabled(void) { return 0; }
#endif
#include "nanohttp-admin.h"
#include "nanohttp-file.h"
#include "nanohttp-user.h"
#include "nanohttp-ring.h"
#include "nanohttp-data.h"

#ifndef timeradd
#define timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;	\
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#endif

#ifndef timersub
#define timersub(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif

typedef struct _conndata
{
  volatile int flag;
  struct hsocket_t sock;
#ifdef WIN32
  HANDLE tid;
#else
  pthread_t tid;
  pthread_attr_t attr;
#endif
}
conndata_t;

#define CONNECTION_FREE		0
#define CONNECTION_IN_USE	1

/**
 *
 * nanohttpd internally globals
 *
 */
static volatile int _httpd_run = 1;

static struct hsocket_t _httpd_socket4 = {.sock = HSOCKET_FREE, .ssl = NULL};
static struct hsocket_t _httpd_socket6 = {.sock = HSOCKET_FREE, .ssl = NULL};
static int _httpd_port = _nanoConfig_HTTPD_PORT;
static int _httpd_max_connections = _nanoConfig_HTTPD_MAX_CONNECTIONS;
static int _httpd_max_pending_connections = _nanoConfig_HTTPD_MAX_PENDING_CONNECTIONS;

static hservice_t *_httpd_services_default = NULL;
static hservice_t *_httpd_services_head = NULL;
static hservice_t *_httpd_services_tail = NULL;

static conndata_t *_httpd_connection = NULL;

#define __HTTP_USE_CONN_RING 1

#if __HTTP_USE_CONN_RING
static struct rte_ring *_httpd_connection_ring = NULL;
#endif

int nanohttpd_is_running(void)
{
  return _httpd_run;
}

#ifdef WIN32
#undef errno
#define errno GetLastError()
static DWORD _httpd_terminate_signal = CTRL_C_EVENT;
static int _httpd_max_idle = 120;
typedef HANDLE MUTEXT_T;
typedef HANDLE THREAD_T;
typedef DWORD SIGNAL_T;
typedef DWORD WINAPI (*HTTP_THREAD_EXEFUNC_T)(LPVOID arg);
#if !__HTTP_USE_CONN_RING
HANDLE _httpd_connection_lock = NULL;
#endif
#define strncasecmp(s1, s2, num) strncmp(s1, s2, num)
#define snprintf(buffer, num, s1, s2) sprintf(buffer, s1,s2)

#else

typedef pthread_mutex_t MUTEXT_T;
typedef pthread_t THREAD_T;
typedef void * (*HTTP_THREAD_EXEFUNC_T)(void *data);
typedef int SIGNAL_T;
static int _httpd_terminate_signal = SIGINT;
static sigset_t thrsigset;
#if !__HTTP_USE_CONN_RING
static pthread_mutex_t _httpd_connection_lock = PTHREAD_MUTEX_INITIALIZER;;
#endif
#endif

int httpd_create_mutex(MUTEXT_T *mutex)
{
  int err = 0;
#ifdef WIN32
  *mutex = CreateMutex(NULL, TRUE, NULL);
  if (*mutex == NULL)
    err = 1;
#else
  pthread_mutex_init(mutex, NULL);
#endif
  if (err)
    log_error("httpd_create_mutex failed");
  return err;
}
void httpd_destroy_mutex(MUTEXT_T *mutex)
{
#ifdef WIN32
  if (*mutex != NULL)
  {
    closeHandle(*mutex);
    *mutex = NULL;
  }
#else
  pthread_mutex_destroy(mutex);
#endif
}

void httpd_thread_join(THREAD_T *tid)
{
#ifndef WIN32
  pthread_join(*tid,NULL);
#else
  WaitForSingleObject(*tid, INFINITE);
#endif
}

void httpd_thread_kill(THREAD_T *tid)
{
#ifdef WIN32
#else
  pthread_kill(*tid, _httpd_terminate_signal);
#endif
}

void httpd_thread_init(THREAD_T *tid)
{
#ifndef WIN32
  memset(tid, 0, sizeof(*tid));
#else
  *tid = NULL;
#endif
}

#ifdef WIN32
BOOL WINAPI
_httpd_term(DWORD sig)
{
  /* log_debug ("Got signal %d", sig); */
  if (sig == _httpd_terminate_signal
    || sig == SIGTERM)
    _httpd_run = 0;

  return TRUE;
}

static void _httpd_sys_sleep(int secs)
{
  Sleep(secs*1000);

  return;
}
#else
static void
_httpd_term(int sig)
{
  log_debug("Got signal %d", sig);

  if (sig == _httpd_terminate_signal
    || sig == SIGTERM || sig == SIGABRT)
    _httpd_run = 0;

  return;
}

static inline void _httpd_sys_sleep(int secs)
{
  sleep(secs);

  return;
}
#endif

static void
_httpd_parse_arguments(int argc, char **argv)
{
  int i;
  
  for (i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i - 1], NHTTPD_ARG_PORT))
    {
      _httpd_port = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_TERMSIG))
    {
      _httpd_terminate_signal = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_MAXCONN))
    {
      _httpd_max_connections = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_MAXCONN_PEND))
    {
      _httpd_max_pending_connections = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_TIMEOUT))
    {
      hsocket_set_timeout(atoi(argv[i]));
    }
  }

  log_verbose("socket bind to port '%d'", _httpd_port);

  return;
}

static herror_t
_httpd_connection_slots_init(void)
{
  int i;
  herror_t status;

#if !__HTTP_USE_CONN_RING
  if (httpd_create_mutex(&_httpd_connection_lock))
  {
    status = herror_new("httpd_create_mutex",  GENERAL_ERROR,
                               "Failed to create mutex!");
    goto clean0;
  }
#endif

  _httpd_connection = (conndata_t *)
        http_calloc(_httpd_max_connections, sizeof(conndata_t));
  if (_httpd_connection == NULL)
  {
    status = herror_new("_httpd_connection_slots_init",
                               GENERAL_ERROR,
                               "Failed malloc _httpd_connection!");
    goto clean1;
  }
  
#if __HTTP_USE_CONN_RING
  struct rte_ring *ring;
  ring = rte_ring_create("http_conn", _httpd_max_connections, RING_F_EXACT_SZ);
  if (ring == NULL)
  {
    log_fatal("rte_ring_create failed.");
    status = herror_new("_httpd_connection_slots_init",
                               GENERAL_ERROR,
                               "Failed create ring for _httpd_connection!");
    goto clean2;
  }
  rte_ring_dump(ring);
  _httpd_connection_ring = ring;
#endif

  for (i = 0; i < _httpd_max_connections; i++)
  {
    conndata_t *c = &_httpd_connection[i];
    hsocket_init(&c->sock);
    httpd_thread_init(&c->tid);
#if __HTTP_USE_CONN_RING
    if (rte_ring_mp_enqueue(ring, c) < 0)
    {
      log_fatal("rte_ring_mp_enqueue failed.");
      status = herror_new("_httpd_connection_slots_init",
                                 GENERAL_ERROR,
                                 "Failed rte_ring_mp_enqueue!");
      goto clean3;
    }
#endif
  }

  log_info("[OK]");
  return H_OK;

#if __HTTP_USE_CONN_RING
clean3:
  rte_ring_free(_httpd_connection_ring);
  _httpd_connection_ring = NULL;
#endif
clean2:
  http_free(_httpd_connection);
  _httpd_connection = NULL;
clean1:
#if !__HTTP_USE_CONN_RING
  httpd_destroy_mutex(&_httpd_connection_lock);
clean0:
  log_info("[FAIL]");
#endif  
  return status;
}

static void
_httpd_connection_slots_free(void)
{
#if !__HTTP_USE_CONN_RING
  httpd_destroy_mutex(&_httpd_connection_lock);
#endif

  if (_httpd_connection)
  {
    for (int i=0;i<_httpd_max_connections;i++)
    {
      conndata_t *conn=&_httpd_connection[i];
      if (conn->flag == CONNECTION_IN_USE)
      {
        hsocket_close(&conn->sock);
      }
    }
    http_free(_httpd_connection);
  }
  
#if __HTTP_USE_CONN_RING
  int count = rte_ring_count(_httpd_connection_ring);
  while (count < _httpd_max_connections - 2){
    log_warn("Number of active connection thread is (%d)", count);
    _httpd_sys_sleep(1);
  };
  rte_ring_free(_httpd_connection_ring);
  _httpd_connection_ring = NULL;
#endif

  log_info("[OK]");
  return;
}

static herror_t
_httpd_register_builtin_services(int argc, char **argv)
{
  herror_t status;

  if ((status = httpd_admin_init_args(argc, argv)) != H_OK)
  {
    log_error("httpd_admin_init_args failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
httpd_init(int argc, char **argv)
{
  herror_t status;

  _httpd_parse_arguments(argc, argv);

  if (http_memcache_init() < 0)
  {
    log_error("Cannot init memcache.");
    return herror_new("http_memcache_init",HSOCKET_ERROR_INIT,"failed");
  }

  if ((status = nanohttp_dir_init(argv[0])) != H_OK)
  {
    log_error("nanohttp_dir_init failed (%s)", herror_message(status));
    return status;
  } 

  if (nanohttp_users_init() < 0)
  {
    log_error("Cannot init users.");
    return herror_new("nanohttp_users_init",HSOCKET_ERROR_INIT,"failed");
  }

  if ((status = hsocket_module_init(argc, argv)) != H_OK)
  {
    log_error("hsocket_modeule_init failed (%s)", herror_message(status));
    return status;
  } 

  if ((status = _httpd_connection_slots_init()) != H_OK)
  {
    log_error("hsocket_modeule_init failed (%s)", herror_message(status));
    return status;
  } 

  if ((status = _httpd_register_builtin_services(argc, argv)) != H_OK)
  {
    log_error("_httpd_register_builtin_services failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_init(&_httpd_socket4)) != H_OK)
  {
    log_error("hsocket_init failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_init(&_httpd_socket6)) != H_OK)
  {
    log_error("hsocket_init failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_bind(AF_INET, &_httpd_socket4, _httpd_port)) != H_OK)
  {
    log_error("hsocket_bind failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_bind(AF_INET6, &_httpd_socket6, _httpd_port)) != H_OK)
  {
    log_error("hsocket_bind failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
httpd_register_secure(const char *context, httpd_service func, 
  httpd_auth auth, const char *service_name)
{
  hservice_t *service;

  if (!(service = (hservice_t *) http_malloc(sizeof(hservice_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return herror_new("httpd_register_secure", 0, 
      "http_malloc failed (%s)", strerror(errno));
  }

  if (!(service->statistics = (struct service_statistics *)
    http_malloc(sizeof(struct service_statistics))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    http_free(service);
    return herror_new("httpd_register_secure", 0, 
      "http_malloc failed (%s)", strerror(errno));
  }    	
  memset(service->statistics, 0, sizeof(struct service_statistics));
  service->statistics->time.tv_sec = 0;
  service->statistics->time.tv_usec = 0;
  pthread_rwlock_init(&(service->statistics->lock), NULL);

  service->name = service_name;
  service->next = NULL;
  service->auth = auth;
  service->func = func;
  service->status = NHTTPD_SERVICE_UP;
  service->context = http_strdup(context);
  service->name_len = strlen(service_name);
  service->context_len = strlen(context);

  log_verbose("register service (%p) for \"%s\"", service, context);
  if (_httpd_services_head == NULL)
  {
    _httpd_services_head = _httpd_services_tail = service;
  }
  else
  {
    _httpd_services_tail->next = service;
    _httpd_services_tail = service;
  }

  return H_OK;
}

herror_t
httpd_register(const char *context, httpd_service service, 
  const char *service_name)
{
  return httpd_register_secure(context, service, NULL, service_name);
}

herror_t
httpd_register_default_secure(const char *context, 
  httpd_service service, httpd_auth auth)
{
  herror_t ret;

  ret = httpd_register_secure(context, service, auth, "DEFAULT");

  /* XXX: this is broken, but working */
  _httpd_services_default = _httpd_services_tail;

  return ret;
}

herror_t
httpd_register_default(const char *context, httpd_service service)
{
  return httpd_register_default_secure(context, service, NULL);
}

short
httpd_get_port(void)
{
  return _httpd_port;
}

int
httpd_get_timeout(void)
{
  return hsocket_get_timeout();
}

void
httpd_set_timeout(int secs)
{
  hsocket_set_timeout(secs);

  return;
}

const char *
httpd_get_protocol(void)
{
  return hssl_enabled() ? "https" : "http";
}

int
httpd_get_conncount(void)
{
#if __HTTP_USE_CONN_RING
  return rte_ring_free_count(_httpd_connection_ring);
#else
  int i, ret;
  for (ret = i = 0; i<_httpd_max_connections; i++)
  {
    if (_httpd_connection[i].flag == CONNECTION_IN_USE)
      ret++;
  }
  return ret;
#endif
}

hservice_t *
httpd_get_services(void)
{
  return _httpd_services_head;
}

static void
hservice_free(hservice_t * service)
{
  if (!service)
    return;

  if (service->statistics)
    http_free(service->statistics);

  http_free(service);

  log_verbose("unregister service \"%s\"", service->context);
  return;
}

int httpd_enable_service(hservice_t *service)
{
  int ret;

  ret = service->status;
  service->status = NHTTPD_SERVICE_UP;

  return ret;
}

int httpd_disable_service(hservice_t *service)
{
  int ret;
  
  ret = service->status;
  service->status = NHTTPD_SERVICE_DOWN;

  return ret;
}

hservice_t *
httpd_find_service(const char *context, int context_len)
{
  hservice_t *cur;

  for (cur = _httpd_services_head; cur; cur = cur->next)
  {
    if (cur->name_len == 4 && (*(uint32_t*)cur->name == *(const uint32_t*)"FILE"
      || *(uint32_t*)cur->name == *(const uint32_t*)"DATA"))
    {
      if (!memcmp(cur->context, context, cur->context_len))
        return cur;
    }
    else if (cur->context_len == context_len
        && !memcmp(cur->context, context, context_len))
      return cur;
  }

  return _httpd_services_default;
}

void
httpd_response_set_content_type(httpd_conn_t * res, const char *content_type)
{
  strncpy(res->content_type, content_type, sizeof(res->content_type)-1);

  return;
}

herror_t
httpd_send_header(httpd_conn_t * res, int code, const char *text)
{
  struct tm stm;
  time_t nw;
  char *header;
  hpair_t *cur;
  herror_t status = H_OK;
  httpd_buf_t b;
  int n;
  
#define __BUF_SZ 1024
#define __BUF BUF_CUR_PTR(&b), BUF_REMAIN(&b)
  header = (char *)http_malloc(__BUF_SZ);
  if (header == NULL)
  {
    log_fatal("Failed to malloc tempary buffer");
    status = herror_new("httpd_send_header", GENERAL_ERROR,
                      "Failed to malloc tempary buffer!");
    goto clean0;
  }

  BUF_SIZE_INIT(&b, header, __BUF_SZ);
  
  /* set status code */
  n = snprintf(__BUF, "HTTP/1.1 %d %s\r\n", code, text);
  BUF_GO(&b, n);
  
  /* set date */
  nw = time(NULL);
  localtime_r(&nw, &stm);
  n = strftime(__BUF, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", &stm);
  BUF_GO(&b, n);

  /* set content-type */
  /* 
   * if (res->content_type[0] == '\0') { strcat(header, "Content-Type:
   * text/html\r\n"); } else { sprintf(buffer, "Content-Type: %s\r\n",
   * res->content_type); strcat(header, buffer); }
   */

  /* set server name */
  n = snprintf(__BUF, "%s", "Server: nanoHTTP library\r\n");
  BUF_GO(&b, n);

  /* set _httpd_connection status */
  /* strcat (header, "Connection: close\r\n"); */

  /* add pairs */
  for (cur = res->header; cur; cur = cur->next)
  {
    n = snprintf(__BUF, "%s: %s\r\n", cur->key, cur->value);
    BUF_GO(&b, n);
  }

  /* set end of header */
  if (BUF_REMAIN(&b) > 1)
  {
    *(uint16_t*)BUF_CUR_PTR(&b) = *(const uint16_t*)"\r\n";
    BUF_GO(&b, 2);
  }
  else
  {
    log_warn("Tempary buffer size is too small: %d", __BUF_SZ);
    status = herror_new("httpd_send_header", GENERAL_ERROR,
                      "Tempary buffer size is too small: %d", 
                      __BUF_SZ);
    goto clean1;
  }

  /* send header */
  if ((status = hsocket_send(res->sock, b.ptr, b.len)) != H_OK)
    goto clean1;

#undef __BUF_SZ

  res->out = http_output_stream_new(res->sock, res->header);
  if (res->out == NULL)
  {
    log_fatal("Failed to malloc http_output_stream");
    status = herror_new("httpd_send_header", GENERAL_ERROR,
                      "Failed to malloc http_output_stream!");
    goto clean1;
  }

clean1:
  http_free(header);
clean0:
  return status;
}

static herror_t
_httpd_send_html_message(httpd_conn_t *conn, int reason, 
  const char *phrase, const char *msg)
{
  herror_t r;
  
  const char *tmpl =
    "<html>"
      "<head>"
        "<title>%s</title>"
      "</head>"
      "<body>"
        "<h3>%s</h3>"
      	"<hr/>"
        "<div>Message: '%s'</div>"
      "</body>"
    "</html>";
  char buf[4096];
  char slen[5];
  size_t len;

  len = snprintf(buf, 4096, tmpl, phrase, phrase, msg);
  snprintf(slen, 5, "%lu", len);
  httpd_set_header(conn, HEADER_CONTENT_LENGTH, slen);

  r = httpd_send_header(conn, reason, phrase);
  if (r != H_OK)
    return r;
  return http_output_stream_write(conn->out, (unsigned char *)buf, len);
}

herror_t
httpd_send_bad_request(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 400, HTTP_STATUS_400_REASON_PHRASE, msg);
}

#define _(x) { .cptr = x, .len = sizeof(x) -1}
httpd_buf_t __nanohttp_html[]={
  _(__nanohttp_index_html_head_JS_MENU_GUEST), 
  _(__nanohttp_index_html_head_JS_MENU_ADMIN), 
  _(__nanohttp_index_html_head_JS_MENU_SUPER), 
  _(__nanohttp_index_html_head_DECL1), 
  _(__nanohttp_index_html_head_DECL2), 
  _(__nanohttp_index_html_head_LOGIN), 
};
#undef _

herror_t
httpd_send_unauthorized(httpd_conn_t *conn, const char *realm)
{
  herror_t r;

  log_debug("httpd_send_unauthorized: building HTTP_STATUS_401_REASON_PHRASE.");

  if (1)
  {
    r = httpd_send_header(conn, 401, HTTP_STATUS_401_REASON_PHRASE);
    if (r != H_OK)
    {
      log_debug("httpd_send_header failed: %s.", herror_message(r));
      return r;
    }
    
    log_info("secure_service received, redirect to login.html");

    do {
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL1);
      if (r != H_OK) break;
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_GUEST);
      if (r != H_OK) break;
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL2);
      if (r != H_OK) break;
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_LOGIN);
    } 
    while (0);
    
    return r;
  }
  else
  {
    char buf[128];
    
    snprintf(buf, 128, "Basic realm=\"%s\"", realm);
    httpd_set_header(conn, HEADER_WWW_AUTHENTICATE, buf);
    
    r = httpd_send_header(conn, 401, HTTP_STATUS_401_REASON_PHRASE);
    if (r != H_OK)
    {
      log_debug("httpd_send_header failed: %s.", herror_message(r));
      return r;
    }
    return _httpd_send_html_message(conn, 401, 
      HTTP_STATUS_401_REASON_PHRASE, "Unauthorized request logged");
  }
}

herror_t
httpd_send_not_found(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 404, HTTP_STATUS_404_REASON_PHRASE, msg);
}

herror_t
httpd_send_internal_error(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 500, HTTP_STATUS_500_REASON_PHRASE, msg);
}

herror_t
httpd_send_not_implemented(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 501, HTTP_STATUS_501_REASON_PHRASE, msg);
}

static void
_httpd_request_print(struct hrequest_t * req)
{
  hpair_t *pair;
  const httpd_buf_t *level;

  log_verbose("++++++ Request +++++++++");
  log_verbose(" Method : '%s'",
               (req->method == HTTP_REQUEST_POST) ? "POST" : "GET");
  log_verbose(" Path   : '%s'", req->path);
  log_verbose(" Spec   : '%s'",
               (req->version == HTTP_1_0) ? "HTTP/1.0" : "HTTP/1.1");
  log_verbose(" ++++++ Parsed Query string :");

  for (pair = req->query; pair; pair = pair->next)
    log_verbose(" %s = '%s'", pair->key, pair->value);

  log_verbose(" ++++++ Parsed Header :");
  for (pair = req->header; pair; pair = pair->next)
    log_verbose(" %s = '%s'", pair->key, pair->value);

  log_verbose(" ++++++ Parsed Content-Type :");
  if (req->content_type != NULL)
  {
    pair = req->content_type->params;
    if (pair != NULL)
      log_verbose(" Content-Type : %s; %s", req->content_type->type, pair->value);
    else
      log_verbose(" Content-Type : %s", req->content_type->type);
  }
  log_verbose("++++++++++++++++++++++++");

  level = __nanohttp_level2string(req->userLevel);
  if (level != NULL)
    log_verbose(" User Level : %.*s", (int)level->len, level->cptr);
  log_verbose(" Connection : %p", req->conn);

  return;
}

httpd_conn_t *
httpd_new(struct hsocket_t * sock)
{
  httpd_conn_t *conn;

  if (!(conn = (httpd_conn_t *) http_malloc(sizeof(httpd_conn_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }
  conn->sock = sock;
  conn->out = NULL;
  conn->content_type[0] = '\0';
  conn->header = NULL;

  return conn;
}

void
httpd_free(httpd_conn_t * conn)
{
  if (!conn)
    return;

  if (conn->out)
    http_output_stream_free(conn->out);

  if (conn->header)
    hpairnode_free_deep(conn->header);

  http_free(conn);

  return;
}

static int
_httpd_decode_authorization(const char *_value, char **user, char **pass)
{
  unsigned char *tmp, *tmp2;
  size_t len, inlen;
  const char *value = _value;

  inlen = strlen(value);
  len = inlen * 2;
  if (!(tmp = (unsigned char *)http_calloc(1, len)))
  {
    log_error("http_calloc failed (%s)", strerror(errno));
    return -1;
  }

  value = strstr(_value, " ") + 1;

  log_verbose("Authorization (base64) = \"%s\"", value);

#if __configUseStreamBase64
  ng_buffer_s in, out;
  in.cptr = value;
  in.len  = inlen - (value - _value);
  out.ptr = tmp;
  if (b64Decode(&in, &out) < 0)
  {
    log_error("b64Decode failed");
    http_free(tmp);
    return -1;
  }
#else
  base64_decode_string((const unsigned char *)value, tmp);
#endif

  log_verbose("Authorization (ascii) = \"%s\"", tmp);

  if ((tmp2 = (unsigned char *)strstr((char *)tmp, ":")))
  {
    *tmp2++ = '\0';
    *pass = http_strdup((char *)tmp2);
  }
  else
  {
    *pass = http_strdup("");
  }
  *user = http_strdup((char *)tmp);

  http_free(tmp);

  return 0;
}

static int
_httpd_authenticate_request(struct hrequest_t * req, httpd_auth auth)
{
  char *user, *pass;
  hpair_t *authorization_pair;
  int ret;

  if (!auth)
    return 1;

  authorization_pair = hpairnode_get_ignore_case_len(req->header, 
    HEADER_AUTHORIZATION, sizeof(HEADER_AUTHORIZATION)-1);
  if (authorization_pair == NULL)
  {
    log_debug("\"%s\" header not set", HEADER_AUTHORIZATION);
    return 0;
  }

  if (_httpd_decode_authorization(authorization_pair->value, &user, &pass))
  {
    log_error("httpd_base64_decode_failed");
    return 0;
  }

  if ((ret = auth(req, user, pass)))
    log_debug("Access granted for user=\"%s\"", user);
  else
    log_info("Authentication failed for user=\"%s\"", user);

  http_free(user);
  http_free(pass);

  return ret;
}

#ifdef WIN32
static unsigned _stdcall
httpd_session_main(void *data)
#else
static void *
httpd_session_main(void *data)
#endif
{
  struct hrequest_t *req;
  conndata_t *conn;
  httpd_conn_t *rconn;
  hservice_t *service;
  herror_t status;
  struct timeval start, end, duration;
  int done;

  if (gettimeofday(&start, NULL) == -1)
    log_error("gettimeofday failed (%s)", strerror(errno));

  conn = (conndata_t *) data;

  log_verbose("starting new httpd session on socket %d", conn->sock);

  rconn = httpd_new(&(conn->sock));

  done = 0;
  while (!done)
  {
    log_verbose("starting HTTP request on socket %d (%p)", conn->sock, conn->sock.sock);

    if ((status = hrequest_new_from_socket(&(conn->sock), &req)) != H_OK)
    {
      int code;

      switch ((code = herror_code(status)))
      {
#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_SSL_H
        case HSSL_ERROR_SSLCLOSE:
#endif			
#endif			
        case HSOCKET_ERROR_RECEIVE:
          log_error("hrequest_new_from_socket failed (%s)", herror_message(status));
          break;
        default:
          httpd_send_bad_request(rconn, herror_message(status));
          break;
      }
      herror_release(status);
      done = 1;
    }
    else
    {
      char buffer[REQUEST_MAX_PATH_SIZE+256+1];
      hpair_t *conn_pair;

      req->conn = conn;
      req->userLevel = _N_http_user_type_NONE;
      _httpd_request_print(req);
      
      conn_pair = hpairnode_get_ignore_case_len(req->header, 
        HEADER_CONNECTION, sizeof(HEADER_CONNECTION)-1);
      if (conn_pair != NULL && 
        (conn_pair->value_len == 6 && !strncasecmp(conn_pair->value, "close", 6)))
        done = 1;

      if (!done)
        done = req->version == HTTP_1_0 ? 1 : 0;

      if ((service = httpd_find_service(req->path, req->path_len)))
      {
        log_verbose("service '%s' for '%s'(%s) found", service->context, req->path, service->name);

      	if (service->status == NHTTPD_SERVICE_UP)
      	{
          pthread_rwlock_wrlock(&(service->statistics->lock));
          service->statistics->requests++;
          pthread_rwlock_unlock(&(service->statistics->lock));

          if (service->auth == NULL || _httpd_authenticate_request(req, service->auth))
          {
            if (service->func != NULL)
            {
              service->func(rconn, req);

              if (gettimeofday(&end, NULL) == -1)
                log_error("gettimeofday failed (%s)", strerror(errno));
              timersub(&end, &start, &duration);

              pthread_rwlock_wrlock(&(service->statistics->lock));
              service->statistics->bytes_received += rconn->sock->bytes_received;
              service->statistics->bytes_transmitted += rconn->sock->bytes_transmitted;
              timeradd(&(service->statistics->time), &duration, &(service->statistics->time));
              pthread_rwlock_unlock(&(service->statistics->lock));

              if (rconn->out && rconn->out->type == HTTP_TRANSFER_CONNECTION_CLOSE)
              {
                log_verbose("Connection close requested");
                done = 1;
              }
            }
            else
            {
              snprintf(buffer, sizeof(buffer), 
                "service '%s' is not registered properly (service function is NULL)", req->path);
              log_verbose("%s", buffer);
              httpd_send_not_implemented(rconn, buffer);
            }
      	  }
          else
          {
            httpd_send_unauthorized(rconn, req->path);
            done = 1;
          }
        }
        else
        {
          snprintf(buffer, sizeof(buffer), "service for '%s' is disabled", req->path);
          log_verbose("%s", buffer);
          httpd_send_internal_error(rconn, buffer);
        }
      }
      else
      {
        snprintf(buffer, sizeof(buffer), "no service for '%s' found", req->path);
        log_verbose("%s", buffer);
      	httpd_send_not_implemented(rconn, buffer);
        done = 1;
      }
      hrequest_free(req);
    }
  }

  httpd_free(rconn);

  hsocket_close(&(conn->sock));

#ifdef WIN32
  CloseHandle((HANDLE) conn->tid);
#else
  pthread_attr_destroy(&(conn->attr));
#endif

  conn->flag = CONNECTION_FREE;
#if __HTTP_USE_CONN_RING
  if (rte_ring_mp_enqueue(_httpd_connection_ring, conn) < 0)
  {
    log_fatal("rte_ring_mp_enqueue failed.");
  }
#endif

  log_debug("Connection used is: %d", httpd_get_conncount());

#ifdef WIN32
  _endthread();
  return 0;
#else
  /* pthread_exits automagically */
  return NULL;
#endif
}

int
httpd_set_header(httpd_conn_t * conn, const char *key, const char *value)
{
  hpair_t *p;

  if (conn == NULL)
  {
    log_warn("Connection object is NULL");
    return 0;
  }

  for (p = conn->header; p; p = p->next)
  {
    if (p->key && !strcmp(p->key, key))
    {
      http_free(p->value);
      p->value = http_strdup(value);
      return 1;
    }
  }

  conn->header = hpairnode_new(key, value, conn->header);

  return 0;
}

void
httpd_set_headers(httpd_conn_t * conn, hpair_t * header)
{
  while (header)
  {
    httpd_set_header(conn, header->key, header->value);
    header = header->next;
  }
  return;
}

int
httpd_add_header(httpd_conn_t * conn, const char *key, const char *value)
{
  if (!conn)
  {
    log_warn("Connection object is NULL");
    return 0;
  }

  conn->header = hpairnode_new(key, value, conn->header);

  return 1;
}

void
httpd_add_headers(httpd_conn_t * conn, const hpair_t * values)
{
  if (!conn)
  {
    log_warn("Connection object is NULL");
    return;
  }

  while (values)
  {
    httpd_add_header(conn, values->key, values->value);
    values = values->next;
  }
  return;
}

/*
 * -----------------------------------------------------
 * FUNCTION: _httpd_register_signal_handler
 * -----------------------------------------------------
 */
static void
_httpd_register_signal_handler(SIGNAL_T sig)
{

#ifndef WIN32
  sigemptyset(&thrsigset);
  sigaddset(&thrsigset, SIGALRM);
#endif

  log_verbose("registering termination signal handler (SIGNAL:%d)",
               sig);
#ifdef WIN32
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE) _httpd_term, TRUE) == FALSE)
  {
    log_error("Unable to install console event handler!");
  }

#else
  signal(sig, _httpd_term);
#endif

  return;
}

static conndata_t *
_httpd_wait_for_empty_conn(void)
{
#if !__HTTP_USE_CONN_RING
  int i;

#ifdef WIN32
  WaitForSingleObject(_httpd_connection_lock, INFINITE);
#else
  pthread_mutex_lock(&_httpd_connection_lock);
#endif

  for (i = 0;; i++)
  {
    if (!nanohttpd_is_running())
    {
#ifdef WIN32
      ReleaseMutex(_httpd_connection_lock);
#else
      pthread_mutex_unlock(&_httpd_connection_lock);
#endif
      return NULL;
    }

    if (i >= _httpd_max_connections)
    {
      _httpd_sys_sleep(1);
      i = -1;
    }
    else if (_httpd_connection[i].flag == CONNECTION_FREE)
    {
      _httpd_connection[i].flag = CONNECTION_IN_USE;
      break;
    }
  }

#ifdef WIN32
  ReleaseMutex(_httpd_connection_lock);
#else
  pthread_mutex_unlock(&_httpd_connection_lock);
#endif

  return &_httpd_connection[i];
#else
  while (1)
  {
    conndata_t *conn;
    if (!rte_ring_mc_dequeue(_httpd_connection_ring, (void **)&conn))
      return conn;
  };
#endif
}

static void
_httpd_start_thread(conndata_t *conn)
{
  int err;

#ifdef WIN32
  conn->tid =
    (HANDLE) _beginthreadex(NULL, 65535, httpd_session_main, conn, 0, &err);
#else
  pthread_attr_init(&(conn->attr));

#ifdef PTHREAD_CREATE_DETACHED
  pthread_attr_setdetachstate(&(conn->attr), PTHREAD_CREATE_DETACHED);
#endif

  pthread_sigmask(SIG_BLOCK, &thrsigset, NULL);
  if ((err =
       pthread_create(&(conn->tid), &(conn->attr), httpd_session_main, conn)))
    log_error("pthread_create failed (%s)", strerror(err));
#endif

  return;
}

#if __NHTTP_USE_EPOLL
static inline herror_t
__httpd_run(struct hsocket_t *sock, const char *name)
{
  conndata_t *conn;
  herror_t err;
  struct epoll_event event;
  
  log_verbose("starting run routine: %s", name);

  if ((err = hsocket_listen(sock, _httpd_max_pending_connections)) != H_OK)
  {
    log_error("hsocket_listen failed (%s)", herror_message(err));
    return err;
  }

  if ((err = hsocket_epoll_create(sock)) != H_OK)
  {
    log_error("hsocket_epoll_create failed (%s)", herror_message(err));
    return err;
  }
  
  if ((err = hsocket_epoll_ctl(sock->ep, sock->sock, 
    &sock->event, EPOLL_CTL_ADD, EPOLLIN|EPOLLRDHUP|EPOLLERR)) != H_OK)
  {
    log_error("hsocket_epoll_ctl failed (%s)", herror_message(err));
    return err;
  }
  
  while (nanohttpd_is_running())
  {
    conn = _httpd_wait_for_empty_conn();
    if (!nanohttpd_is_running())
      break;

    /* Wait for a socket to accept */
    while (nanohttpd_is_running())
    {
      /* select socket descriptor with proper timeout */
      int n = epoll_wait(sock->ep, &event, 1, 1000);
      switch (n)
      {
      case 0:
        /* descriptor is not ready */
        break;
      case -1:
        log_warn("Socket %d epoll_wait error: (%s)", 
          sock, strerror(errno));
        /* got a signal? */
        break;
      case 1:
        if (event.events & (EPOLLRDHUP|EPOLLERR))
          continue;
        /* no nothing */
        if ((event.events & EPOLLIN) 
          && event.data.fd == sock->sock)
          break;
        // FALLTHROUGH;
      default:
        log_fatal("Socket %d unknown error: (%s)", 
          sock, strerror(errno));
        break;
      }

      if (n == 1 && (event.events & EPOLLIN)) 
        break;
    }

    /* check signal status */
    if (!nanohttpd_is_running())
      break;
    
    err = hsocket_accept(sock, &(conn->sock));
    if (err != H_OK)
    {
      log_error("hsocket_accept failed (%s)", herror_message(err));
    
      hsocket_close(&(conn->sock));
    
      continue;
    }
    
    _httpd_start_thread(conn);
  }

  return 0;
}
#else
static inline herror_t
__httpd_run(struct hsocket_t *sock, const char *name)
{
  struct timeval timeout;
  conndata_t *conn;
  herror_t err;
  fd_set fds;

  log_verbose("starting run routine: %s", name);

  if ((err = hsocket_listen(sock, _httpd_max_pending_connections)) != H_OK)
  {
    log_error("hsocket_listen failed (%s)", herror_message(err));
    return err;
  }

  while (nanohttpd_is_running())
  {
    conn = _httpd_wait_for_empty_conn();
    if (!nanohttpd_is_running())
      break;

    /* Wait for a socket to accept */
    while (nanohttpd_is_running())
    {

      /* set struct timeval to the proper timeout */
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      /* zero and set file descriptior */
      FD_ZERO(&fds);
      FD_SET(sock->sock, &fds);

      /* select socket descriptor */
      switch (select(sock->sock + 1, &fds, NULL, NULL, &timeout))
      {
      case 0:
        if (!nanohttpd_is_running())
          break;
        /* descriptor is not ready */
        continue;
      case -1:
        if (!nanohttpd_is_running())
          break;
        /* got a signal? */
        continue;
      default:
        if (!nanohttpd_is_running())
          break;
        /* no nothing */
        break;
      }
      if (FD_ISSET(sock->sock, &fds))
      {
        break;
      }
    }

    /* check signal status */
    if (!nanohttpd_is_running())
      break;

    err = hsocket_accept(sock, &(conn->sock));
    if (err != H_OK)
    {
      log_error("hsocket_accept failed (%s)", herror_message(err));

      hsocket_close(&(conn->sock));

      continue;
    }

    _httpd_start_thread(conn);
  }

  return 0;
}
#endif

#ifdef WIN32
HANDLE main_cond=NULL;
HANDLE main_mutex=NULL;
typedef HANDLE COND_T;
#else
pthread_cond_t  main_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef pthread_cond_t COND_T;
#endif
static NG_ATOMIC_T(NG_U32) condition_met;

int httpd_create_cond(COND_T *cond)
{
  int err = 0;

  ng_atomic_set(&condition_met, 0);
  
#ifdef WIN32
  *cond = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (*cond == NULL)
  {
    err = 1;
    log_error("httpd_create_mutex failed: (%d)", errno);
    goto clean0;
  }
#else
  err = httpd_create_mutex(&main_mutex);
  if (httpd_create_mutex(&main_mutex))
  {
    err = 1;
    log_error("httpd_create_mutex failed: (%s)", strerror(errno));
    goto clean0;
  }
  if (pthread_cond_init(cond, NULL))
  {
    err = 1;
    log_error("pthread_cond_init failed: %d", strerror(errno));
    goto clean1;
  }
#endif

  return err;
  
#ifndef WIN32
clean1:  
  httpd_destroy_mutex(&main_mutex);
#endif
clean0:  
  return err;
}
void httpd_destroy_cond(COND_T *cond)
{
#ifdef WIN32
  if (*cond != NULL)
  {
    closeHandle(*cond);
    *cond = NULL;
  }
#else
  httpd_destroy_mutex(&main_mutex);
  pthread_cond_destroy(cond);
#endif
}

void httpd_wait_cond(COND_T *cond)
{
#ifdef WIN32
  WaitForSingleObject(*cond, INFINITE);
#else
  pthread_mutex_lock(&main_mutex);
  while (!condition_met && nanohttpd_is_running()) 
  {
    pthread_cond_wait(cond, &main_mutex); // Wait for the condition
  }
  pthread_mutex_unlock(&main_mutex);
#endif
}
void httpd_signal_cond(COND_T *cond)
{
  int already_set = ng_atomic_read(&condition_met);
  if (already_set)
    return;

  // Set the condition
  ng_atomic_set(&condition_met, 1);
  
#ifdef WIN32
  SetEvent(*cond);
#else
  pthread_mutex_lock(&main_mutex);
  pthread_cond_signal(cond); // Signal the main thread
  pthread_mutex_unlock(&main_mutex);
#endif
}

#ifdef WIN32
DWORD WINAPI thread_function_ipv4(LPVOID arg) 
#else
static void *thread_function_ipv4(void* arg) 
#endif
{
  __httpd_run(&_httpd_socket4, "ipv4");
  ng_smp_mb();
  httpd_signal_cond(&main_cond);
#ifdef WIN32
  return NULL;
#else
  return 0;
#endif
}

#ifdef WIN32
DWORD WINAPI thread_function_ipv6(LPVOID arg) 
#else
static void *thread_function_ipv6(void* arg) 
#endif
{
  __httpd_run(&_httpd_socket6, "ipv6");
  ng_smp_mb();
  httpd_signal_cond(&main_cond);
#ifdef WIN32
  return NULL;
#else
  return 0;
#endif
}

static herror_t
__start_thread(conndata_t *conn, HTTP_THREAD_EXEFUNC_T exefunc)
{
  int err;
  herror_t status = H_OK;
#ifdef WIN32
  conn->tid = CreateThread(NULL, 65535, exefunc, conn, 0, NULL);
  if (conn->id == NULL)
  {
    err = 1;
    log_error("pthread_create failed (%d)", GetLastError());
  }
#else
  pthread_attr_init(&conn->attr);
  err = pthread_create(&conn->tid, &conn->attr, exefunc, conn);
  if (err)
    log_error("pthread_create failed (%s)", strerror(err));
#endif

  if (err)
    status = herror_new("__start_thread",  THREAD_BEGIN_ERROR,
                             "Failed to create pthread!");
  return status;
}

static conndata_t httpd_thread_ipv4;
static conndata_t httpd_thread_ipv6;
herror_t httpd_run(void)
{
  herror_t status = H_OK;
  conndata_t *c;
  
  _httpd_register_signal_handler(_httpd_terminate_signal);
  _httpd_register_signal_handler(SIGTERM);
  _httpd_register_signal_handler(SIGABRT);

  if (httpd_create_cond(&main_cond))
  {
    log_error("httpd_create_cond failed");
    status = herror_new("httpd_run",  THREAD_BEGIN_ERROR,
                             "Failed to create condition!");
    goto clean0;
  }

  c = &httpd_thread_ipv4;
  hsocket_init(&c->sock);
  httpd_thread_init(&c->tid);
  c = &httpd_thread_ipv6;
  hsocket_init(&c->sock);
  httpd_thread_init(&c->tid);
  
  status = __start_thread(c, thread_function_ipv4);
  if (status != H_OK)
    goto clean1;
  
  status = __start_thread(c, thread_function_ipv6);
  if (status != H_OK)
  {
  	_httpd_run = 0;
    ng_smp_mb();
    httpd_thread_kill(&httpd_thread_ipv4.tid);
    httpd_thread_join(&httpd_thread_ipv4.tid);
    goto clean1;
  }

  httpd_wait_cond(&main_cond);
  ng_smp_mb();
  httpd_thread_join(&httpd_thread_ipv4.tid);
  httpd_thread_join(&httpd_thread_ipv6.tid);
  
clean1:
  httpd_destroy_cond(&main_cond);
clean0:
  return status;
}

void
httpd_destroy(void)
{
  hservice_t *tmp, *cur = _httpd_services_head;

  hsocket_close(&_httpd_socket4);
  hsocket_close(&_httpd_socket6);

  while (cur != NULL)
  {
    tmp = cur->next;
    hservice_free(cur);
    cur = tmp;
  }
  log_info("[OK]");

  hsocket_module_destroy();
  _httpd_connection_slots_free();

  nanohttp_users_free();
  nanohttp_dir_free();
  http_memcache_free();
  return;
}

unsigned char *
httpd_get_postdata(httpd_conn_t * conn, struct hrequest_t * req, 
  long *received, long max)
{
  size_t content_length = 0;
  unsigned char *postdata = NULL;

  if (req->method == HTTP_REQUEST_POST)
  {
    hpair_t *content_length_pair;
    content_length_pair =
      hpairnode_get_ignore_case_len(req->header, 
      HEADER_CONTENT_LENGTH,
      sizeof(HEADER_CONTENT_LENGTH) - 1);

    if (content_length_pair != NULL)
      content_length = atol(content_length_pair->value);
  }
  else
  {
    log_warn("Not a POST method");
    return NULL;
  }

  if (content_length > max && max != -1)
    return NULL;

  if (content_length == 0)
  {
    *received = 0;
    if (!(postdata = (unsigned char *)http_malloc(1)))
    {

      log_error("http_malloc failed (%s)", strerror(errno));
      return NULL;
    }
    postdata[0] = '\0';
    return postdata;
  }
  if (!(postdata = (unsigned char *) http_malloc(content_length + 1)))
  {
    log_error("http_malloc failed (%)", strerror(errno));
    return NULL;
  }
  if (http_input_stream_read(req->in, postdata, content_length) > 0)
  {
    *received = content_length;
    postdata[content_length] = '\0';
    return postdata;
  }
  http_free(postdata);
  return NULL;
}


/*
  MIME support httpd_mime_* function set
*/

static void
_httpd_mime_get_boundary(httpd_conn_t * conn, char *dest)
{
  sprintf(dest, "---=.Part_NH_%p", conn);
  log_verbose("boundary= \"%s\"", dest);

  return;
}


/**
  Begin MIME multipart/related POST 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_send_header(httpd_conn_t * conn, const char *related_start,
                       const char *related_start_info,
                       const char *related_type, int code, const char *text)
{
  char buffer[1024];
  char temp[512];
  char boundary[250];

  /* Set Content-type Set multipart/related parameter type=..; start=.. ;
     start-info= ..; boundary=... using sprintf instead of snprintf because 
     visual c does not support snprintf */

  sprintf(buffer, "multipart/related;");

  if (related_type)
  {
    snprintf(temp, 75, " type=\"%s\";", related_type);
    strcat(buffer, temp);
  }

  if (related_start)
  {
    snprintf(temp, 250, " start=\"%s\";", related_start);
    strcat(buffer, temp);
  }

  if (related_start_info)
  {
    snprintf(temp, 250, " start-info=\"%s\";", related_start_info);
    strcat(buffer, temp);
  }

  _httpd_mime_get_boundary(conn, boundary);
  snprintf(temp, sizeof temp, " boundary=\"%s\"", boundary);
  strcat(buffer, temp);

  httpd_set_header(conn, HEADER_CONTENT_TYPE, buffer);

  return httpd_send_header(conn, code, text);
}


/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_next(httpd_conn_t * conn, const char *content_id,
                const char *content_type, const char *transfer_encoding)
{
  herror_t status;
  char buffer[512];
  char boundary[75];
  size_t len;

  /* Get the boundary string */
  _httpd_mime_get_boundary(conn, boundary);
  len = sprintf(buffer, "\r\n--%s\r\n", boundary);

  /* Send boundary */
  if ((status = http_output_stream_write(conn->out, (unsigned char *)buffer, len)) != H_OK)
    return status;

  /* Send Content header */
  len = sprintf(buffer, "%s: %s\r\n%s: %s\r\n%s: %s\r\n\r\n",
            HEADER_CONTENT_TYPE, content_type ? content_type : "text/plain",
            HEADER_CONTENT_TRANSFER_ENCODING,
            transfer_encoding ? transfer_encoding : "binary",
            HEADER_CONTENT_ID,
            content_id ? content_id : "<content-id-not-set>");

  return http_output_stream_write(conn->out, (unsigned char *)buffer, len);
}

/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_send_file(httpd_conn_t * conn, const char *content_id,
                     const char *content_type, const char *transfer_encoding,
                     const char *filename)
{
  unsigned char buffer[MAX_FILE_BUFFER_SIZE];
  herror_t status;
  FILE *fd;
  size_t size;

  if ((fd = fopen(filename, "rb")) == NULL)
    return herror_new("httpd_mime_send_file", FILE_ERROR_OPEN,
                      "Can not open file '%d'", filename);

  status = httpd_mime_next(conn, content_id, content_type, transfer_encoding);
  if (status != H_OK)
  {
    fclose(fd);
    return status;
  }

  while (!feof(fd))
  {
    size = fread(buffer, 1, MAX_FILE_BUFFER_SIZE, fd);
    if (size == -1)
    {
      fclose(fd);
      return herror_new("httpd_mime_send_file", FILE_ERROR_READ,
                        "Can not read from file '%d'", filename);
    }

    if ((status = http_output_stream_write(conn->out, buffer, size)) != H_OK)
    {
      fclose(fd);
      return status;
    }
  }

  fclose(fd);
  return H_OK;
}

/**
  Finish MIME request 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_end(httpd_conn_t * conn)
{
  herror_t status;
  char buffer[512];
  char boundary[75];
  size_t len;

  /* Get the boundary string */
  _httpd_mime_get_boundary(conn, boundary);
  len = sprintf(buffer, "\r\n--%s--\r\n\r\n", boundary);

  /* Send boundary */
  if ((status = http_output_stream_write(conn->out, (unsigned char *)buffer, len)) != H_OK)
    return status;

  return http_output_stream_flush(conn->out);
}
