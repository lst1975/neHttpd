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
#ifndef __NanoHttpConfig_H_
#define __NanoHttpConfig_H_

#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__) \
  || defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#ifndef WIN32
#define WIN32
#endif
#endif

#define HAVE_STDIO_H
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_ERRNO_H
#define HAVE_STDARG_H
#define HAVE_SYSLOG_H

#ifndef WIN32
#define HAVE_SYS_TIME_H
#define HAVE_NETINET_IN_H
#define HAVE_TIME_H
#define HAVE_SYS_SOCKET_H
#define HAVE_SOCKET_H
#define HAVE_SYS_TYPES_H
#define HAVE_SYS_SELECT_H
#define HAVE_SIGNAL_H
#define HAVE_UNISTD_H
#define HAVE_PTHREAD_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_WAIT_H
#define HAVE_FCNTL_H
#define HAVE_NETDB_H
#else
#undef HAVE_SYS_TIME_H
#undef HAVE_NETINET_IN_H
#undef HAVE_TIME_H
#undef HAVE_SYS_SOCKET_H
#undef HAVE_SOCKET_H
#undef HAVE_SYS_TYPES_H
#undef HAVE_SYS_SELECT_H
#undef HAVE_SIGNAL_H
#undef HAVE_UNISTD_H
#undef HAVE_PTHREAD_H
#undef HAVE_ARPA_INET_H
#undef HAVE_SYS_WAIT_H
#undef HAVE_FCNTL_H
#undef HAVE_NETDB_H
#endif

#define __NHTTP_INTERNAL

#define __NHTTP_TEST 1
#define __NHTTP_NO_LOGGING 0

#undef HAVE_SSL

#define _nanoConfig_PATH_MAX                      1024
#define _nanoConfig_HTTPD_PORT                    8080
#define _nanoConfig_HTTPD_MAX_CONNECTIONS         128
#define _nanoConfig_HTTPD_MAX_PENDING_CONNECTIONS 256
#define _nanoConfig_HTTPD_FILE_BLOCK              2048
#define _nanoConfig_HTTPD_FILE_SERVICE            "/config/"
#define _nanoConfig_HTTPD_DATA_SERVICE            "/data/"
#define _nanoConfig_HTTPD_LOG_LEVEL               NANOHTTP_LOG_VERBOSE
#define _nanoConfig_HTTP_URL_LEN_MAX              2048

#define _nanoConfig_NG_HTTP_PARSER_STRICT 1

#define __NHTTP_TEST_URLENCODE  0
#define __NHTTP_TEST_JSON       0
#define __NHTTP_TEST_RING       0
#define __NHTTP_TEST_VSNPRINTF  0
#define __NHTTP_TEST_URL        0

#define DEBUG_MULTIPART
#define __NHTTP_MEM_DEBUG 1
#define __HTTP_SMALL_SIZE 0

#ifdef WIN32
#define __NHTTP_USE_WSAPOLL 1
#define __NHTTP_USE_EPOLL 0
#else
#define __NHTTP_USE_WSAPOLL 0
#define __NHTTP_USE_EPOLL 1
#endif

#define __NHTTP_USE_IPV4 1
#define __NHTTP_USE_IPV6 1
#if !__NHTTP_USE_IPV4 && !__NHTTP_USE_IPV6
#error "neither __NHTTP_USE_IPV4 nor __NHTTP_USE_IPV6"
#endif

#define __NHTTP_USE_NATIVE_MEM 0

#include <assert.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdalign.h>

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

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#endif
