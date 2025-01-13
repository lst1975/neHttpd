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

#ifdef HAVE_SYS_SOCKET_H
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
#include "nanohttp-defs.h"
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
#include "nanohttp-system.h"
#include "nanohttp-time.h"
#include "nanohttp-ctype.h"
#include "nanohttp-signal.h"
#include "nanohttp-mime.h"
#include "nanohttp-header.h"
#include "nanohttp-const.h"
#include "nanohttp-code.h"

#ifndef WIN32
#define __NHTTP_LISTEN_DUAL_STACK 0
#else
#define __NHTTP_LISTEN_DUAL_STACK 0
#endif

#ifndef ng_timeradd
#define ng_timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;	\
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#endif

#ifndef ng_timersub
#define ng_timersub(tvp, uvp, vvp)						\
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
  char name[32];
  volatile int flag;
  int index;
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

static void _httpd_release_finished_conn(conndata_t *conn);

#if __HTTP_USE_CONN_RING
static struct rte_ring *_httpd_connection_ring = NULL;
#endif

int nanohttpd_is_running(void)
{
  return _httpd_run;
}
void nanohttpd_stop_running(void)
{
  log_debug("nanohttpd_stop_running");
  _httpd_run = 0;
}

#ifdef WIN32
#undef errno
#define errno GetLastError()
typedef HANDLE MUTEXT_T;
typedef HANDLE THREAD_T;
typedef SOCKET SOCKET_T;
typedef DWORD WINAPI (*HTTP_THREAD_EXEFUNC_T)(LPVOID arg);
#if !__HTTP_USE_CONN_RING
HANDLE _httpd_connection_lock = NULL;
#endif

#else

typedef pthread_mutex_t MUTEXT_T;
typedef pthread_t THREAD_T;
typedef int SOCKET_T;
typedef void * (*HTTP_THREAD_EXEFUNC_T)(void *data);
#if !__HTTP_USE_CONN_RING
static pthread_mutex_t _httpd_connection_lock = PTHREAD_MUTEX_INITIALIZER;;
#endif
#endif

int httpd_create_mutex(void *mutex)
{
  int err = 0;
#ifdef WIN32
  *(MUTEXT_T *)mutex = CreateMutex(NULL, FALSE, NULL);
  if (*(MUTEXT_T *)mutex == NULL)
    err = 1;
#else
  err = pthread_mutex_init((MUTEXT_T *)mutex, NULL);
#endif
  if (err)
    log_error("httpd_create_mutex failed");
  return err;
}
void httpd_destroy_mutex(void *mutex)
{
#ifdef WIN32
  if (*(MUTEXT_T *)mutex != NULL)
  {
    CloseHandle(*(MUTEXT_T *)mutex);
    *(MUTEXT_T *)mutex = NULL;
  }
#else
  if (mutex != NULL)
    pthread_mutex_destroy((MUTEXT_T *)mutex);
#endif
}
int httpd_enter_mutex(void *mutex)
{
#ifdef WIN32
  DWORD dwWaitResult; 
  dwWaitResult = WaitForSingleObject(*(MUTEXT_T *)mutex, INFINITE);
  switch (dwWaitResult) 
  {
    // The thread got ownership of the mutex
    case WAIT_OBJECT_0: 
      return 0; 
    // The thread got ownership of an abandoned mutex
    // The database is in an indeterminate state
    default:
    case WAIT_ABANDONED: 
      return -1; 
  }
#else
  pthread_mutex_lock((MUTEXT_T *)mutex);
  return 0;
#endif
}

void httpd_leave_mutex(void *mutex)
{
#ifdef WIN32
  ReleaseMutex(*(MUTEXT_T *)mutex);
#else
  pthread_mutex_unlock((MUTEXT_T *)mutex);
#endif
}

int httpd_thread_join(THREAD_T *tid)
{
#ifdef WIN32
  DWORD dwWaitResult; 
  dwWaitResult = WaitForSingleObject(*tid, INFINITE);
  switch (dwWaitResult) 
  {
    // The thread got ownership of the mutex
    case WAIT_OBJECT_0: 
      return 0; 
    // The thread got ownership of an abandoned mutex
    // The database is in an indeterminate state
    default:
    case WAIT_ABANDONED: 
      return -1; 
  }
#else
  pthread_join(*tid,NULL);
  return 0;
#endif
}

void httpd_thread_kill(THREAD_T *tid)
{
#ifdef WIN32
#else
  pthread_kill(*tid, httpd_get_terminate_signal());
#endif
}

void httpd_thread_init(conndata_t *conn)
{
#ifdef WIN32
  conn->tid = NULL;
#else
  memset(&conn->tid, 0, sizeof(conn->tid));
  pthread_attr_init(&(conn->attr));
#endif
}

void httpd_thread_cancel(conndata_t *conn)
{
#ifdef WIN32
  if (conn->tid != NULL)
  {
    log_debug("###########Close Thread Handle=%p.", conn->tid);
    CloseHandle(conn->tid);
    conn->tid = NULL;
  }
#else
  pthread_attr_destroy(&(conn->attr));
#endif

  httpd_thread_init(conn);

  log_debug("CloseHandle: %s", conn->name);
}

int
httpd_parse_arguments(int argc, char **argv)
{
  int parse_result = HTTP_INIT_PARSE_RESULT_OK;
  int i = 1;
  
  while (i < argc)
  {
    if (!strcmp(argv[i], NHTTPD_ARG_PORT))
    {
      if (++i >= argc)
      {
        log_print("Need port for %s\n", NHTTPD_ARG_PORT);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!ng_str_isdigit(argv[i]))
      {
        log_print("Bad port for %s\n", NHTTPD_ARG_PORT);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      _httpd_port = ng_atoi(argv[i], 0);
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_TERMSIG))
    {
      if (++i >= argc)
      {
        log_print("Need signal number for %s\n", NHTTPD_ARG_TERMSIG);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!ng_str_isdigit(argv[i]))
      {
        log_print("Bad signal number for %s\n", NHTTPD_ARG_TERMSIG);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      httpd_set_terminate_signal(ng_atoi(argv[++i], 0));
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_MAXCONN))
    {
      if (++i >= argc)
      {
        log_print("Need number for %s\n", NHTTPD_ARG_MAXCONN);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!ng_str_isdigit(argv[i]))
      {
        log_print("Bad number for %s\n", NHTTPD_ARG_MAXCONN);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      _httpd_max_connections = ng_atoi(argv[++i], 0);
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_MAXCONN_PEND))
    {
      if (++i >= argc)
      {
        log_print("Need number for %s\n", NHTTPD_ARG_MAXCONN_PEND);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!ng_str_isdigit(argv[i]))
      {
        log_print("Bad number for %s\n", NHTTPD_ARG_MAXCONN_PEND);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      _httpd_max_pending_connections = ng_atoi(argv[++i], 0);
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_LOGLEVEL))
    {
      if (++i >= argc)
      {
        log_print("Need number for %s\n", NHTTPD_ARG_TIMEOUT);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!strcmp(argv[i], "fatal"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_FATAL);
      }
      else if (!strcmp(argv[i], "error"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_ERROR);
      }
      else if (!strcmp(argv[i], "warn"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_WARN);
      }
      else if (!strcmp(argv[i], "event"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_INFO);
      }
      else if (!strcmp(argv[i], "debug"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_DEBUG);
      }
      else if (!strcmp(argv[i], "verbose"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);
      }
      else if (!strcmp(argv[i], "off"))
      {
        nanohttp_log_set_loglevel(NANOHTTP_LOG_OFF);
      }
      else
        return HTTP_INIT_PARSE_RESULT_ERR;
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_TIMEOUT))
    {
      if (++i >= argc)
      {
        log_print("Need number for %s\n", NHTTPD_ARG_TIMEOUT);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      if (!ng_str_isdigit(argv[i]))
      {
        log_print("Bad number for %s\n", NHTTPD_ARG_TIMEOUT);
        return HTTP_INIT_PARSE_RESULT_ERR;
      }
      hsocket_set_timeout(ng_atoi(argv[++i], 0));
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_DAEMONIZE)
      || !strcmp(argv[i], "-d"))
    {
      parse_result = HTTP_INIT_PARSE_RESULT_DAEMON;
    }
    else if (!strcmp(argv[i], NHTTPD_ARG_HELP)
      || !strcmp(argv[i], "-h"))
    {
      parse_result = HTTP_INIT_PARSE_RESULT_HELP;
    }
    else if (!strcmp(argv[i], "-v"))
    {
      nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);
    }
    else
    {
      log_print("Unknown option %s\n", argv[i]);
      return HTTP_INIT_PARSE_RESULT_ERR;
    }
    i++;
  }

  return parse_result;
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
    c->flag = CONNECTION_FREE;
    hsocket_init(&c->sock);
    httpd_thread_init(c);
    c->index = i;
    ng_snprintf(c->name, sizeof(c->name), "Sess-%d", i);
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

  log_info("[OK]: connection_slots_init.");
  return H_OK;

#if __HTTP_USE_CONN_RING
clean3:
  rte_ring_free(_httpd_connection_ring);
  _httpd_connection_ring = NULL;
clean2:
  http_free(_httpd_connection);
  _httpd_connection = NULL;
#endif
clean1:
#if !__HTTP_USE_CONN_RING
  httpd_destroy_mutex(&_httpd_connection_lock);
clean0:
#endif  
  return status;
}

static void
_httpd_connection_slots_free(void)
{
  int count;
  
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
  }

  log_info("Number of active connection thread is (%d)", 
    httpd_get_conncount());

  while ((count = httpd_get_conncount()) > 0)
  {
    log_warn("Number of active connection thread is (%d)", count);
    ng_os_usleep(1000);
  }

  if (_httpd_connection)
  {
    http_free(_httpd_connection);
    _httpd_connection = NULL;
  }
  
#if __HTTP_USE_CONN_RING
  rte_ring_free(_httpd_connection_ring);
  _httpd_connection_ring = NULL;
#endif

  log_info("[OK]: _httpd_connection_slots_free");
  return;
}

static herror_t
_httpd_register_builtin_services(int argc, char **argv)
{
  herror_t status;

  if ((status = httpd_admin_init_args(argc, argv)) != H_OK)
  {
    herror_log(status);
    log_error("httpd_admin_init_args failed.");
    return status;
  }

  return H_OK;
}

herror_t
httpd_init(int argc, char **argv)
{
  herror_t status;

  httpd_pthread_sigmask();
  
  if (http_log_init() < 0)
  {
    log_error("[FAIL]: Cannot init http_log.");
    return herror_new("http_log_init",GENERAL_ERROR,"failed");
  }
  
  if (http_memcache_init() < 0)
  {
    log_error("[FAIL]: Cannot init memcache.");
    return herror_new("http_memcache_init",GENERAL_ERROR,"failed");
  }

  if (ng_http_mime_type_init() != ng_ERR_NONE)
  {
    log_error("[FAIL]: ng_http_mime_type_init failed.");
    return herror_new("ng_http_mime_type_init", GENERAL_ERROR,"failed");
  }
  
  if ((status = nanohttp_dir_init(argv[0])) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: nanohttp_dir_init failed.");
    return status;
  } 

  if (nanohttp_users_init() < 0)
  {
    log_error("[FAIL]: Cannot init users.");
    return herror_new("nanohttp_users_init",GENERAL_ERROR,"failed");
  }

  if ((status = hsocket_module_init(argc, argv)) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_modeule_init failed.");
    return status;
  } 

  if ((status = _httpd_connection_slots_init()) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_modeule_init failed.");
    return status;
  } 

  if ((status = _httpd_register_builtin_services(argc, argv)) != H_OK)
  {
    herror_log(status);
    log_error("_httpd_register_builtin_services failed.");
    return status;
  }

  if ((status = hsocket_init(&_httpd_socket4)) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_init failed.");
    return status;
  }

  if ((status = hsocket_init(&_httpd_socket6)) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_init failed.");
    return status;
  }

#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  if ((status = hsocket_bind(AF_INET, &_httpd_socket4, _httpd_port)) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_bind failed.");
    return status;
  }
#endif

  if ((status = hsocket_bind(AF_INET6, &_httpd_socket6, _httpd_port)) != H_OK)
  {
    herror_log(status);
    log_error("[FAIL]: hsocket_bind failed.");
    return status;
  }

  log_verbose("socket bind to port '%d'", _httpd_port);

  return H_OK;
}

herror_t
httpd_register_secure(const char *context, int context_len,
  httpd_service func, httpd_auth auth, const char *service_name,
  int service_name_len)
{
  hservice_t *service;

  if (!(service = (hservice_t *)http_malloc(sizeof(hservice_t)+context_len+1)))
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    return herror_new("httpd_register_secure", 0, 
      "http_malloc failed (%s)", os_strerror(ng_errno));
  }

  STAT_u64_set(service->statistics.bytes_received, 0);
  STAT_u64_set(service->statistics.bytes_transmitted, 0);
  STAT_u64_set(service->statistics.requests, 0);
  STAT_u64_set(service->statistics.time, 0);
  stat_pthread_rwlock_init(&(service->statistics.lock), NULL);

  service->name = service_name;
  service->name_len = service_name_len;
  service->next = NULL;
  service->auth = auth;
  service->func = func;
  service->status = NHTTPD_SERVICE_UP;
  
  service->context = (char *)(service+1);
  memcpy(service->context, context, context_len+1);
  service->context_len = context_len;

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
httpd_register(const char *context, int context_len, httpd_service service, 
  const char *service_name, int service_name_len)
{
  return httpd_register_secure(context, context_len, service, 
    NULL, service_name, service_name_len);
}

herror_t
httpd_register_default_secure(const char *context, int context_len, 
  httpd_service service, httpd_auth auth)
{
  herror_t ret;

  ret = httpd_register_secure(context, context_len, service, auth, "DEFAULT", 7);

  /* XXX: this is broken, but working */
  _httpd_services_default = _httpd_services_tail;

  return ret;
}

herror_t
httpd_register_default(const char *context, int context_len, httpd_service service)
{
  return httpd_register_default_secure(context, context_len, service, NULL);
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

hservice_t *
httpd_get_services(void)
{
  return _httpd_services_head;
}

static void
hservice_free(hservice_t *service)
{
  if (!service)
    return;

  log_verbose("unregister service \"%s\"", service->context);
  http_free(service);

  return;
}

static void
_httpd_unregister_services(void)
{
  hservice_t *tmp, *cur;
  
  for (cur = _httpd_services_head; cur != NULL;)
  {
    log_info("service %p:%p.", cur, cur->next);
    tmp = cur->next;
    hservice_free(cur);
    cur = tmp;
  }
  log_info("[OK]: _httpd_unregister_services.");
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
    if (cur->name_len == 4 
      &&  (*(uint32_t*)cur->name == *(const uint32_t*)"FILE"
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

herror_t
httpd_send_header(httpd_conn_t *res, int code)
{
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
  n = ng_snprintf(__BUF, "HTTP/1.1 %pS\r\n", http_code2name(code));
  BUF_GO(&b, n);

  /* set date */
  n = ng_http_date_s(__BUF);
  if (n < 0)
  {
    log_warn("Tempary buffer size is too small: %d", __BUF_SZ);
    status = herror_new("httpd_send_header", GENERAL_ERROR,
                      "Create HTTP header Date failed: buffer size %d", 
                      BUF_REMAIN(&b));
    goto clean1;
  }
  BUF_GO(&b, n);

  /* set content-type */
  /* 
   * if (res->content_type[0] == '\0') { strcat(header, "Content-Type:
   * text/html\r\n"); } else { sprintf(buffer, "Content-Type: %s\r\n",
   * res->content_type); strcat(header, buffer); }
   */

  /* set server name */
  n = ng_snprintf(__BUF, "%s", "Server: nanoHTTP library\r\n");
  BUF_GO(&b, n);

  /* set _httpd_connection status */
  /* strcat (header, "Connection: close\r\n"); */

  /* add pairs */
  ng_list_for_each_entry(cur, hpair_t, &res->header, link)
  {
    if (BUF_REMAIN(&b) > cur->key.len + cur->val.len + 4)
    {
      ng_memcpy(BUF_CUR_PTR(&b), cur->key.cptr, cur->key.len);
      BUF_GO(&b, cur->key.len);
      *(uint16_t*)BUF_CUR_PTR(&b) = *(const uint16_t*)": ";
      BUF_GO(&b, 2);
      ng_memcpy(BUF_CUR_PTR(&b), cur->val.cptr, cur->val.len);
      BUF_GO(&b, cur->val.len);
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

  res->out = http_output_stream_new(res->sock, &res->header);
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
  const char *msg)
{
  herror_t r;
  char *buf;
  char slen[5];
  size_t len;
  ng_block_s b;
  const ng_block_s *c = http_code2name(reason);
  
  const char *tmpl =
    "<html>"
      "<head>"
        "<title>%pS</title>"
      "</head>"
      "<body>"
        "<h3>%.*s</h3>"
      	"<hr/>"
        "<div>Message: '%s'</div>"
      "</body>"
    "</html>";

#define __BUF_SZ 4096
  buf = http_malloc(__BUF_SZ);
  if (buf == NULL)
  {
    r = herror_new("_httpd_send_html_message", GENERAL_ERROR,
                      "Failed to malloc tempary buffer!");
    goto clean0;
  }
  
  len = ng_snprintf(buf, __BUF_SZ, tmpl, c, c->len-4, c->ptr+4, msg);
  b.len = ng_snprintf(slen, 5, "%lu", len);
  b.buf = slen;
  if (hpairnode_set_header(&conn->header, &__HDR_BUF__(HEADER_CONTENT_LENGTH), &b))
  {
    r = herror_new("_httpd_send_html_message", GENERAL_ERROR,
                      "Failed to httpd_set_header!");
    goto clean1;
  }

  if ((r = httpd_send_header(conn, reason)) != H_OK)
  {
    goto clean1;
  }
  
  r = http_output_stream_write(conn->out, (unsigned char *)buf, len);
#undef __BUF_SZ

clean1:
  http_free(buf);
clean0:
  return r;
}

herror_t
httpd_send_bad_request(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, HTTP_RESPONSE_CODE_400_Bad_Request, msg);
}

#define _(x) { .cptr = x, .len = sizeof(x) -1}
ng_block_s __nanohttp_html[]={
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

  log_debug("httpd_send_unauthorized: building %pS.",
    http_code2name(HTTP_RESPONSE_CODE_401_Unauthorized));

  r = httpd_send_header(conn, HTTP_RESPONSE_CODE_401_Unauthorized);
  if (r != H_OK)
  {
    log_debug("httpd_send_header failed (5s).", herror_message(r));
    return r;
  }
  
  log_info("secure_service received, redirect to login.html");

  do {
    r = http_output_stream_write_buffer(conn->out, 
          nanohttp_index_html_head_DECL1);
    if (r != H_OK) break;
    r = http_output_stream_write_buffer(conn->out, 
          nanohttp_index_html_head_JS_MENU_GUEST);
    if (r != H_OK) break;
    r = http_output_stream_write_buffer(conn->out, 
          nanohttp_index_html_head_DECL2);
    if (r != H_OK) break;
    r = http_output_stream_write_buffer(conn->out, 
          nanohttp_index_html_head_LOGIN);
  } 
  while (0);
  
  return r;
}

herror_t
httpd_send_not_found(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 
    HTTP_RESPONSE_CODE_404_Not_Found, msg);
}

herror_t
httpd_send_internal_error(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 
    HTTP_RESPONSE_CODE_500_Internal_Server_Error, msg);
}

herror_t
httpd_send_not_implemented(httpd_conn_t *conn, const char *msg)
{
  return _httpd_send_html_message(conn, 
    HTTP_RESPONSE_CODE_501_Not_Implemented, msg);
}

static void
_httpd_request_print(struct hrequest_t * req)
{
  hpair_t *pair;
  const ng_block_s *level;

  log_verbose("++++++ Request +++++++++");
  log_verbose(" Method : %s",
               (req->method == HTTP_REQUEST_METHOD_POST) ? "POST" : "GET");
  log_verbose(" Path   : %pS", &req->path);
  log_verbose(" Spec   : %s",
               (req->version == HTTP_VERSION_1_0) ? "HTTP/1.0" : "HTTP/1.1");
  log_verbose(" ++++++ Parsed Query string :");

  ng_list_for_each_entry(pair,hpair_t,&req->query,link)
    log_verbose(" %pS = %pS", &pair->key, &pair->val);

  log_verbose(" ++++++ Parsed Header :");
  ng_list_for_each_entry(pair,hpair_t,&req->header,link)
    log_verbose(" %pS = %pS", &pair->key, &pair->val);

  content_type_print(req->content_type);
  log_verbose("++++++++++++++++++++++++");

  level = __nanohttp_level2string(req->userLevel);
  if (level != NULL)
    log_verbose(" User Level : %pS", level);
  log_verbose(" Connection : %p", req->conn);

  return;
}

httpd_conn_t *
httpd_new(struct hsocket_t *sock)
{
  httpd_conn_t *conn;

  if (!(conn = (httpd_conn_t *) http_malloc(sizeof(httpd_conn_t))))
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    return NULL;
  }
  conn->sock = sock;
  conn->out = NULL;
  conn->content_type[0] = '\0';
  ng_INIT_LIST_HEAD(&conn->header);

  return conn;
}

void
httpd_free(httpd_conn_t *conn)
{
  if (!conn)
    return;

  if (conn->out)
    http_output_stream_free(conn->out);

  hpairnode_free_deep(&conn->header);
  http_free(conn);

  return;
}

static void *
_httpd_decode_authorization(int *tmplen, 
  const char *_value, size_t inlen, ng_block_s *user, 
  ng_block_s *pass)
{
  size_t len, value_len;
  unsigned char *tmp, *tmp2;
  const char *value;

  len = B64_DECLEN(inlen);
  if (!(tmp = (unsigned char *)http_malloc(len)))
  {
    log_error("http_calloc failed (%s)", os_strerror(ng_errno));
    return NULL;
  }
  value = ng_memchr(_value, ' ', inlen);
  if (value == NULL)
  {
    log_error("Authorization malformed.");
    http_free(tmp);
    return NULL;
  }
  value++;
  value_len = inlen - (value - _value);
  log_verbose("Authorization (base64) = \"%.*s\"", value_len, value);

#if __configUseStreamBase64
  len = b64Decode_with_len(value, value_len, (char *)tmp, len);
#else
  len = base64_decode_string((const unsigned char *)value, tmp);
#endif
  if (len < 0)
  {
    log_error("b64Decode failed");
    http_free(tmp);
    return NULL;
  }

  log_verbose("Authorization (ascii) = \"%.*s\"", len, tmp);

  *tmplen = len;
  tmp2 = (unsigned char *)ng_memchr((char *)tmp, ':', len);
  if (tmp2 != NULL)
  {
    user->len = tmp2 - tmp;
    user->ptr = tmp;

    tmp2++;
    pass->len = tmp + len - tmp2;
    pass->ptr = tmp2;
  }
  else
  {
    pass->len = 0;
    pass->cptr = "";
    user->len = len;
    user->ptr = tmp;
  }
  
  return tmp;
}

static int
_httpd_authenticate_request(struct hrequest_t *req, httpd_auth auth)
{
  ng_block_s user, pass;
  hpair_t *authorization_pair;
  int ret, tmplen;
  void *tmp;

  if (!auth)
    return 1;

  authorization_pair = hpairnode_get_ignore_case(&req->header, 
    __HDR_BUF(HEADER_AUTHORIZATION));
  if (authorization_pair == NULL)
  {
    log_debug("\"%pS\" header not set", &__HDR_BUF__(HEADER_AUTHORIZATION));
    return 0;
  }

  tmp = _httpd_decode_authorization(&tmplen, authorization_pair->val.cptr, 
    authorization_pair->val.len, &user, &pass);
  if (tmp == NULL)
  {
    log_error("httpd_base64_decode_failed");
    return 0;
  }

  if ((ret = auth(req, &user, &pass)))
    log_debug("Access granted for user=\"%pS\"", &user);
  else
    log_info("Authentication failed for user=\"%pS\"", &user);

  ng_bzero(tmp, tmplen);
  http_free(tmp);
  return ret;
}

#ifdef WIN32
DWORD WINAPI httpd_session_main(LPVOID data) 
#else
static void *
httpd_session_main(void *data)
#endif
{
  conndata_t *conn;
  httpd_conn_t *rconn;
  hservice_t *service;
  herror_t status;
  uint64_t start, duration;
  int done;

  rconn = NULL;
  conn = (conndata_t *)data;
  start = ng_get_time();
  
  log_debug("starting new httpd session on socket %d.", conn->sock.sock);

  rconn = httpd_new(&(conn->sock));
  if (rconn == NULL)
  {
    log_fatal("httpd_new failed.");
    done = 1;
  }
  else
    done = 0;

  while (!done)
  {
    struct hrequest_t *req;

    log_verbose("starting HTTP request on socket %d (%p).", 
      conn->sock.sock, conn);

    status = hrequest_new_from_socket(&(conn->sock), &req);
    if (status != H_OK)
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
          herror_log(status);
          log_error("hrequest_new_from_socket failed.");
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
      
      conn_pair = hpairnode_get_ignore_case(&req->header, 
        __HDR_BUF(HEADER_CONNECTION));
      if (conn_pair != NULL && 
        (ng_block_isequal__(&conn_pair->val, &cstr_close)))
        done = 1;

      if (!done)
        done = req->version == HTTP_VERSION_1_0 ? 1 : 0;

      if ((service = httpd_find_service(req->path.cptr, req->path.len)))
      {
        log_debug("service '%s' for '%pS'(%s) found", 
                service->context, &req->path, service->name);

      	if (service->status == NHTTPD_SERVICE_UP)
      	{
          stat_pthread_rwlock_wrlock(&(service->statistics.lock));
          STAT_u64_inc(service->statistics.requests);
          stat_pthread_rwlock_unlock(&(service->statistics.lock));

          if (service->auth == NULL || _httpd_authenticate_request(req, service->auth))
          {
            if (service->func != NULL)
            {
              service->func(rconn, req);

              duration = ng_get_time() - start;
              stat_pthread_rwlock_wrlock(&(service->statistics.lock));
              STAT_u64_add(service->statistics.bytes_received, rconn->sock->bytes_received);
              STAT_u64_add(service->statistics.bytes_transmitted, rconn->sock->bytes_transmitted);
              STAT_u64_add(service->statistics.time, duration);
              stat_pthread_rwlock_unlock(&(service->statistics.lock));

              if (rconn->out && rconn->out->type == HTTP_TRANSFER_CONNECTION_CLOSE)
              {
                log_verbose("Connection close requested");
                done = 1;
              }
            }
            else
            {
              ng_snprintf(buffer, sizeof(buffer), 
                "service '%pS' is not registered properly (service function is NULL)", &req->path);
              log_warn("%s", buffer);
              httpd_send_not_implemented(rconn, buffer);
            }
      	  }
          else
          {
            httpd_send_unauthorized(rconn, req->path.cptr);
            done = 1;
          }
        }
        else
        {
          ng_snprintf(buffer, sizeof(buffer), "service for '%pS' is disabled", &req->path);
          log_warn("%s", buffer);
          httpd_send_internal_error(rconn, buffer);
        }
      }
      else
      {
        ng_snprintf(buffer, sizeof(buffer), "no service for '%pS' found", &req->path);
        log_warn("%s", buffer);
      	httpd_send_not_implemented(rconn, buffer);
        done = 1;
      }
      hrequest_free(req);
    }
  }

  httpd_free(rconn);
  _httpd_release_finished_conn(conn);

  ng_date_print();
  log_debug("Connection used is: %d", httpd_get_conncount());
  
#ifdef WIN32
  ExitThread(0);
  return 0;
#else
  /* pthread_exits automagically */
  return NULL;
#endif
}

static __rte_unused void _httpd_sys_sleep(int secs) 
{
  ng_os_usleep(secs*1000);
  return;
}

int
httpd_get_conncount(void)
{
#if __HTTP_USE_CONN_RING
  if (!_httpd_connection_ring)
    return 0;
  return rte_ring_free_count(_httpd_connection_ring);
#else
  httpd_enter_mutex(&_httpd_connection_lock);
  int i, ret;
  for (ret = i = 0; i<_httpd_max_connections; i++)
  {
    if (_httpd_connection[i].flag == CONNECTION_IN_USE)
      ret++;
  }
  httpd_leave_mutex(&_httpd_connection_lock);
  return ret;
#endif
}

static conndata_t *
_httpd_wait_for_empty_conn(void)
{
  conndata_t *conn = NULL;
  
  log_debug("trying to get new connection.");

#if !__HTTP_USE_CONN_RING
  int ret = httpd_enter_mutex(&_httpd_connection_lock);
  if (ret < 0) return NULL;
  
  for (uint32_t i=0;nanohttpd_is_running();i++)
  {
    int k = i %_httpd_max_connections;
    conn = &_httpd_connection[k];
    if (conn->flag == CONNECTION_FREE)
    {
      conn->flag = CONNECTION_IN_USE;
      break;
    }
  }
  httpd_leave_mutex(&_httpd_connection_lock);

#else
  while (nanohttpd_is_running())
  {
    if (!rte_ring_mc_dequeue(_httpd_connection_ring, (void **)&conn))
    {
      conn->flag = CONNECTION_IN_USE;
      break;
    }
  };
#endif

  if (conn != NULL)
  {
    ng_smp_mb();
    hsocket_init(&conn->sock);
    httpd_thread_init(conn);
  }
  
  log_debug("get new connection %d, (%p).", conn->index, conn);
  return conn;
}

static inline void
_httpd_release_finished_conn(conndata_t *conn)
{
  if (conn == NULL)
    return;
  
  httpd_thread_cancel(conn);
  hsocket_close(&(conn->sock));

  conn->flag = CONNECTION_FREE;
  ng_smp_mb();
#if __HTTP_USE_CONN_RING
  if (rte_ring_mp_enqueue(_httpd_connection_ring, conn) < 0)
  {
    log_fatal("rte_ring_mp_enqueue failed.");
  }
#endif
}
      
static int
_httpd_start_thread(conndata_t *conn)
{
  int err = 0;

#ifdef WIN32
  conn->tid = CreateThread(NULL, 65535, httpd_session_main, conn, 0, NULL);
  if (conn->tid == NULL)
  {
    log_error("CreateThread failed (%d:%s)", os_strerror(ng_errno));
    err = -1;
  }
  log_debug("###########Create Thread Handle=%p.", conn->tid);
#else

#ifdef PTHREAD_CREATE_DETACHED
  pthread_attr_setdetachstate(&(conn->attr), PTHREAD_CREATE_DETACHED);
#endif

  if ((err =
       pthread_create(&(conn->tid), &(conn->attr), httpd_session_main, conn)))
  {
    log_error("pthread_create failed (%s)", os_strerror(ng_errno));
    err = -1;
  }
#endif

  return err;
}

#define __NHTTP_EPOLL_WAIT_TIMEOUT 1000
#define __NHTTP_EPOLL_WAIT_LOG 5

static void __httpd_log_poll_timeout(SOCKET_T sock, SOCKET_T ep, uint64_t rt)
{
  static rte_atomic64_t last_log = RTE_ATOMIC64_INIT(0);
  uint64_t last_rt = rte_atomic64_read(&last_log);
  if (ng_difftime(rt, last_rt) < __NHTTP_EPOLL_WAIT_LOG)
    return;
  rte_atomic64_set(&last_log, rt);
  if (ep != INVALID_SOCKET)
  {
    log_debug("Epoll [%lu:%lu]"
      " wait timeout, running %"PRIu64" seconds fine ...", ep, sock, rt);
  }
  else
  {
    log_debug("Epoll [%lu]"
      " wait timeout, running %"PRIu64" seconds fine ...", sock, rt);
  }
}

#if __NHTTP_USE_EPOLL
static inline int
__httpd_run(struct hsocket_t *sock, const char *name)
{
  herror_t err;
  struct epoll_event event;
  uint64_t start = ng_get_time();
  
  log_verbose("starting run routine: %s", name);

  if ((err = hsocket_listen(sock, _httpd_max_pending_connections)) != H_OK)
  {
    herror_log(err);
    log_error("hsocket_listen failed.");
    herror_release(err);
    return -1;
  }

  if ((err = hsocket_epoll_create(sock)) != H_OK)
  {
    herror_log(err);
    log_error("hsocket_epoll_create failed.");
    herror_release(err);
    return -1;
  }
  
  if ((err = hsocket_epoll_ctl(sock->ep, sock->sock, 
    &sock->event, EPOLL_CTL_ADD, EPOLLIN|EPOLLRDHUP|EPOLLERR)) != H_OK)
  {
    herror_log(err);
    log_error("hsocket_epoll_ctl failed.");
    herror_release(err);
    return -1;
  }
  
  while (nanohttpd_is_running())
  {
    conndata_t *conn;
    
    conn = _httpd_wait_for_empty_conn();
    if (!nanohttpd_is_running())
    {
      _httpd_release_finished_conn(conn);
      return 0;
    }
    
    /* Wait for a socket to accept */
    while (nanohttpd_is_running())
    {
      /* select socket descriptor with proper timeout */
      int n = epoll_wait(sock->ep, &event, 1, __NHTTP_EPOLL_WAIT_TIMEOUT);
      if (n == 0)
      {
        /* descriptor is not ready */
        if (!nanohttpd_is_running())
        {
          _httpd_release_finished_conn(conn);
          return 0;
        }
		
        /* timeout */
        __httpd_log_poll_timeout(sock->sock, sock->ep, 
          ng_difftime(ng_get_time(), start));
        continue;
      }
      else if (n == -1)
      {
        /* got a signal? */
        n = ng_socket_errno;
        if (_hsocket_should_again(n))
        {
          log_info("Socket %d epoll_wait INTR: (%d:%s)", 
            sock->ep, n, os_strerror(n));
          continue;
        }
        log_fatal("Socket %d epoll_wait error: (%d:%s)", 
          sock->ep, n, os_strerror(n));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else if (n == 1)
      {
        if (!nanohttpd_is_running()
          || (event.events & (EPOLLRDHUP|EPOLLERR)))
        {
          _httpd_release_finished_conn(conn);
          return 0;
        }
        /* no nothing */
        if ((event.events & EPOLLIN) && event.data.fd == sock->sock)
          break;
        
        log_fatal("Socket %d unknown error: (%s)", 
          sock->ep, os_strerror(ng_socket_errno));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else
      {
        log_fatal("Socket %d unknown error: (%s)", 
          sock->ep, os_strerror(ng_socket_errno));
        _httpd_release_finished_conn(conn);
        return -1;
      }
    }

    /* check signal status */
    if (!nanohttpd_is_running())
    {
      _httpd_release_finished_conn(conn);
      break;
    }
    
    err = hsocket_accept(sock, &(conn->sock));
    if (err != H_OK)
    {
      herror_log(err);
      log_error("hsocket_accept failed.");
      _httpd_release_finished_conn(conn);
      continue;
    }
    
    if (_httpd_start_thread(conn) < 0)
    {
      log_error("_httpd_start_thread failed.");
      _httpd_release_finished_conn(conn);
    }
  }

  return 0;
}
#elif __NHTTP_USE_WSAPOLL
static inline int
__httpd_run(struct hsocket_t *sock, const char *name)
{
  herror_t err;
  WSAPOLLFD event = { 0 };
  uint64_t start = ng_get_time();
  
  log_verbose("starting run routine: %s", name);

  if ((err = hsocket_listen(sock, _httpd_max_pending_connections)) != H_OK)
  {
    herror_log(err);
    log_error("hsocket_listen failed.");
    herror_release(err);
    return -1;
  }

  event.fd = sock->sock;
  event.events = POLLIN;

  while (nanohttpd_is_running())
  {
    conndata_t *conn;
    
    conn = _httpd_wait_for_empty_conn();
    
    if (!nanohttpd_is_running())
    {
      _httpd_release_finished_conn(conn);
      return 0;
    }
    
    /* Wait for a socket to accept */
    while (nanohttpd_is_running())
    {
      /* select socket descriptor with proper timeout */
      int n = WSAPoll(&event, 1, __NHTTP_EPOLL_WAIT_TIMEOUT);
      if (n == 0)
      {
        /* descriptor is not ready */
        if (!nanohttpd_is_running())
        {
          _httpd_release_finished_conn(conn);
          return 0;
        }
        
        /* timeout */
        __httpd_log_poll_timeout(sock->sock, INVALID_SOCKET, 
          ng_difftime(ng_get_time(), start));
        continue;
      }
      else if (n == -1)
      {
        /* got a signal? */
        n = ng_socket_errno;
        if (_hsocket_should_again(n))
        {
          log_info("Socket %d epoll_wait INTR: (%d:%s)", 
            sock->sock, n, os_strerror(n));
          continue;
        }
        log_fatal("Socket %d epoll_wait error: (%d:%s)", 
          sock->sock, n, os_strerror(n));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else if (n == 1)
      {
        if (!nanohttpd_is_running()
          || (event.revents & (POLLERR|POLLHUP|POLLNVAL)))
        {
          _httpd_release_finished_conn(conn);
          return 0;
        }
        /* no nothing */
        if (event.revents & POLLIN)
          break;
        
        log_fatal("Socket %d unknown error: (%s)", 
          sock->sock, os_strerror(ng_socket_errno));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else
      {
        log_fatal("Socket %d unknown error: (%s)", 
          sock->sock, os_strerror(ng_socket_errno));
        _httpd_release_finished_conn(conn);
        return -1;
      }
    }
  
    /* check signal status */
    if (!nanohttpd_is_running())
    {
      _httpd_release_finished_conn(conn);
      break;
    }
    
    err = hsocket_accept(sock, &(conn->sock));
    if (err != H_OK)
    {
      herror_log(err);
      log_error("hsocket_accept failed.");
      _httpd_release_finished_conn(conn);
      continue;
    }
    
    if (_httpd_start_thread(conn) < 0)
    {
      log_error("_httpd_start_thread failed.");
      _httpd_release_finished_conn(conn);
    }
  }
  
  return 0;
}
#else
static inline int
__httpd_run(struct hsocket_t *sock, const char *name)
{
#ifdef WIN32
 TIMEVAL timeout = {
    .tv_sec  = __NHTTP_EPOLL_WAIT_TIMEOUT/1000, 
    .tv_usec = __NHTTP_EPOLL_WAIT_TIMEOUT%1000};
#else
  struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
#endif
  herror_t err;
  fd_set fds;
  uint64_t start = ng_get_time();

  log_verbose("starting run routine: %s", name);

  if ((err = hsocket_listen(sock, _httpd_max_pending_connections)) != H_OK)
  {
    herror_log(err);
    log_error("hsocket_listen failed.");
    herror_release(err);
    return -1;
  }

  while (nanohttpd_is_running())
  {
    conndata_t *conn;

    conn = _httpd_wait_for_empty_conn();
    if (!nanohttpd_is_running())
      break;

    /* Wait for a socket to accept */
    while (nanohttpd_is_running())
    {
      int n;
      
      /* set struct timeval to the proper timeout */
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      /* zero and set file descriptior */
      FD_ZERO(&fds);
      FD_SET(sock->sock, &fds);

      /* select socket descriptor */
      n = select(sock->sock + 1, &fds, NULL, NULL, &timeout);
      if (n == 0)
      {
        /* descriptor is not ready */
        if (!nanohttpd_is_running())
        {
          _httpd_release_finished_conn(conn);
          return 0;
        }

        /* timeout */
        __httpd_log_poll_timeout(sock->sock, INVALID_SOCKET, 
          ng_difftime(ng_get_time(), start));
        continue;
      }
      else if (n == -1)
      {
        n = ng_socket_errno;

        /* got a signal? */
        if (_hsocket_should_again(ng_socket_errno))
        {
          log_warn("Socket %d select INTR. (%d:%s)", 
            sock->sock, n, os_strerror(n));
          continue;
        }

        log_error("Socket %d select error. (%d:%s)", 
          sock->sock, n, os_strerror(n));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else if (n == 1)
      {
        if (FD_ISSET(sock->sock, &fds))
        {
          break;
        }
        n = ng_socket_errno;
        log_verbose("Socket %d select error. (%d:%s)", 
          sock->sock, n, os_strerror(n));
        _httpd_release_finished_conn(conn);
        return -1;
      }
      else
      {
        log_verbose("Socket %d select error. (%d:%s)", 
          sock->sock, n, os_strerror(n));
        _httpd_release_finished_conn(conn);
        return -1;
      }
    }

    /* check signal status */
    if (!nanohttpd_is_running())
    {
      _httpd_release_finished_conn(conn);
      break;
    }

    err = hsocket_accept(sock, &(conn->sock));
    if (err != H_OK)
    {
      herror_log(err);
      log_error("hsocket_accept failed.");
      _httpd_release_finished_conn(conn);
      continue;
    }

    if (_httpd_start_thread(conn) < 0)
    {
      _httpd_release_finished_conn(conn);
    }
  }

  return 0;
}
#endif

typedef struct {
#ifdef WIN32
  HANDLE handle;
#else
  pthread_cond_t  handle;
  MUTEXT_T mutex;
#endif
  NG_ATOMIC_T(NG_U32) set;
} COND_T;

#ifdef WIN32
COND_T main_cond = {
  .handle  = NULL,
};
#else
COND_T main_cond = {
  .mutex  = PTHREAD_MUTEX_INITIALIZER,
  .handle = PTHREAD_COND_INITIALIZER,
};
#endif

int httpd_create_cond(COND_T *cond)
{
  int err = 0;

  ng_atomic_set(&cond->set, 0);
  
#ifdef WIN32
  cond->handle = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (cond->handle == NULL)
  {
    err = -1;
    log_error("httpd_create_mutex failed: (%s)", os_strerror(ng_errno));
    goto clean0;
  }
#else
  err = httpd_create_mutex(&cond->mutex);
  if (err)
  {
    err = -1;
    log_error("httpd_create_mutex failed: (%s)", os_strerror(ng_errno));
    goto clean0;
  }
  if (pthread_cond_init(&cond->handle, NULL))
  {
    err = -1;
    log_error("pthread_cond_init failed: %d", os_strerror(ng_errno));
    goto clean1;
  }
#endif

  log_debug("httpd_create_cond.");
  return err;
  
#ifndef WIN32
clean1:  
  httpd_destroy_mutex(&cond->mutex);
#endif
clean0:  
  return err;
}
void httpd_destroy_cond(COND_T *cond)
{
#ifdef WIN32
  if (cond->handle != NULL)
  {
    CloseHandle(cond->handle);
    cond->handle = NULL;
  }
#else
  pthread_cond_destroy(&cond->handle);
  httpd_destroy_mutex(&cond->mutex);
#endif

  log_debug("httpd_destroy_cond.");
}

void httpd_wait_cond(COND_T *cond)
{
#ifdef WIN32
  WaitForSingleObject(cond->handle, INFINITE);
#else
  httpd_enter_mutex(&cond->mutex);
  while (!ng_atomic_read(&cond->set)) 
  {
    log_debug("Httpd Eenter WAIT.");
    pthread_cond_wait(&cond->handle, &cond->mutex); // Wait for the condition
    log_debug("Httpd Leave WAIT.");
  }
  httpd_leave_mutex(&cond->mutex);
#endif
  log_debug("Httpd returned from WAIT.");
}
void httpd_signal_cond(COND_T *cond)
{
  int already_set = ng_atomic_read(&cond->set);
  if (already_set)
  {
    log_debug("httpd signal already set.");
    return;
  }
  // Set the condition
  ng_atomic_set(&cond->set, 1);
  log_debug("httpd condition set write: %d.", 
    ng_atomic_read(&cond->set));
  ng_smp_mb();
  
#ifdef WIN32
  SetEvent(cond->handle);
#else
  httpd_enter_mutex(&cond->mutex);
  pthread_cond_signal(&cond->handle); // Signal the main thread
  httpd_leave_mutex(&cond->mutex);
#endif
  log_debug("httpd_signal_cond.");
}

#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
#ifdef WIN32
DWORD WINAPI thread_function_ipv4(LPVOID arg) 
#else
static void *thread_function_ipv4(void* arg) 
#endif
{
  __httpd_run(&_httpd_socket4, "ipv4");
  log_debug("thread_function_ipv4 finished.");
  ng_smp_mb();
  httpd_signal_cond(&main_cond);
  return 0;
}
#endif

#ifdef WIN32
DWORD WINAPI thread_function_ipv6(LPVOID arg) 
#else
static void *thread_function_ipv6(void* arg) 
#endif
{
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  __httpd_run(&_httpd_socket6, "ipv6");
  log_debug("thread function ipv6 finished.");
#else
  __httpd_run(&_httpd_socket6, "ipv4 and ipv6");
  log_debug("thread function ipv4 and ipv6 finished.");
#endif
  ng_smp_mb();
  httpd_signal_cond(&main_cond);
  return 0;
}

static herror_t
__start_thread(conndata_t *conn, HTTP_THREAD_EXEFUNC_T exefunc)
{
  int err = 0;
  herror_t status = H_OK;
#ifdef WIN32
  conn->tid = CreateThread(NULL, 65535, exefunc, conn, 0, NULL);
  if (conn->tid == NULL)
  {
    err = GetLastError();
    log_error("pthread_create failed (%d:%s)", err, os_strerror(err));
  }
#else
  pthread_attr_init(&conn->attr);
  err = pthread_create(&conn->tid, &conn->attr, exefunc, conn);
  if (err)
  {
    err = ng_errno;
    log_error("pthread_create failed (%d:%s)", err, os_strerror(err));
  }
#endif

  if (err)
    status = herror_new("__start_thread",  THREAD_BEGIN_ERROR,
                             "Failed to create pthread!");
  return status;
}

#define USE_PTHREAD_WAIT 1

#if USE_PTHREAD_WAIT
#ifdef WIN32
DWORD WINAPI httpd_run_thread(LPVOID arg) 
#else
static void *httpd_run_thread(void* arg) 
#endif
{
  httpd_wait_cond(&main_cond);
#ifdef WIN32
  return 0;
#else
  return NULL;
#endif
}
#endif

#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
static conndata_t httpd_thread_ipv4={
  .flag=CONNECTION_IN_USE, 
  .index=0, 
  .name="EPoll-IPv4"};
#endif
static conndata_t httpd_thread_ipv6={
  .flag=CONNECTION_IN_USE, 
  .index=1, 
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  .name="EPoll-IPv6"
#else
  .name="EPoll-IPv4&IPv6"
#endif
};
herror_t httpd_run(void)
{
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  conndata_t *c4;
#endif
  conndata_t *c6;
  herror_t status = H_OK;

  httpd_register_signal_handler();
 
  if (httpd_create_cond(&main_cond) < 0)
  {
    log_error("httpd_create_cond failed");
    status = herror_new("httpd_run",  THREAD_BEGIN_ERROR,
                             "Failed to create condition!");
    goto clean0;
  }
  
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  c4 = &httpd_thread_ipv4;
  hsocket_init(&c4->sock);
  httpd_thread_init(c4);
  status = __start_thread(c4, thread_function_ipv4);
  if (status != H_OK)
    goto clean1;
#endif

  c6 = &httpd_thread_ipv6;
  hsocket_init(&c6->sock);
  httpd_thread_init(c6);
  status = __start_thread(c6, thread_function_ipv6);
  if (status != H_OK)
  {
  	nanohttpd_stop_running();
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
    ng_smp_mb();
    httpd_thread_kill(&c4->tid);
    httpd_thread_join(&c4->tid);
    httpd_thread_cancel(c4);
    c4->flag = CONNECTION_FREE;
#endif
    goto clean1;
  }

#if USE_PTHREAD_WAIT
  conndata_t main_conn = { .name="Main" };
  status = __start_thread(&main_conn, httpd_run_thread);
  if (status != H_OK)
  {
    goto clean2;
  }  
  httpd_thread_join(&main_conn.tid);
  httpd_thread_cancel(&main_conn);
#else
  httpd_wait_cond(&main_cond);
#endif

#if USE_PTHREAD_WAIT
clean2:  
#endif
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  httpd_thread_join(&c4->tid);
  ng_smp_mb();
  httpd_thread_cancel(c4);
  c4->flag = CONNECTION_FREE;
#endif
  httpd_thread_join(&c6->tid);
  ng_smp_mb();
  httpd_thread_cancel(c6);
  c6->flag = CONNECTION_FREE;
  
clean1:
  httpd_destroy_cond(&main_cond);
clean0:
  return status;
}

void
httpd_destroy(void)
{
#if !__NHTTP_LISTEN_DUAL_STACK || !defined(IPV6_V6ONLY)
  hsocket_close(&_httpd_socket4);
  log_debug("_httpd_socket4 closed.");
#endif
  hsocket_close(&_httpd_socket6);
  log_debug("_httpd_socket6 closed.");

  _httpd_unregister_services();
  hsocket_module_destroy();
  _httpd_connection_slots_free();
  nanohttp_users_free();
  nanohttp_dir_free();
  ng_http_mime_type_free();
  http_memcache_free();
  http_log_free();
  return;
}

unsigned char *
httpd_get_postdata(httpd_conn_t *conn, struct hrequest_t *req, 
  long *received, long max)
{
  size_t content_length = 0, rcved, total_len;
  unsigned char *postdata = NULL;

  if (req->method == HTTP_REQUEST_METHOD_POST)
  {
    hpair_t *content_length_pair;
    content_length_pair = hpairnode_get_ignore_case(&req->header, 
        __HDR_BUF(HEADER_CONTENT_LENGTH));
    if (content_length_pair != NULL)
    {
      content_length = ng_atoi(content_length_pair->val.cptr, 
        content_length_pair->val.len);
      req->content_length = content_length;
    }
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
      log_error("http_malloc failed (%s)", os_strerror(ng_errno));
      return NULL;
    }
    postdata[0] = '\0';
    return postdata;
  }
  if (!(postdata = (unsigned char *) http_malloc(content_length + 1)))
  {
    log_error("http_malloc failed (%)", os_strerror(ng_errno));
    return NULL;
  }

  total_len = 0;
  postdata[content_length] = '\0';
  while(1) {
    rcved = http_input_stream_read(req->in, postdata + total_len, 
      content_length - total_len);
    if (rcved < 0)
      break;
    total_len += rcved;
    if (total_len >= content_length)
    {
      *received = content_length;
      return postdata;
    }
  }
  
  http_free(postdata);
  return NULL;
}
