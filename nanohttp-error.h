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
 *  $Id: nanohttp-error.h,v 1.5 2007/11/03 22:40:11 m0gg Exp $
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
#ifndef __nanohttp_error_h
#define __nanohttp_error_h

#include "nanohttp-buffer.h"

/** @file nanohttp-error.h Error handling
 *
 * nanoHTTP error handling 
 *
 * Almost all function will return a "herror_t" object. If the function returns
 * with success this object is H_OK. Another herror_t object will be returned
 * otherwise. Following functions can be used with a returned herror_t object: 
 *
 * - herror_code() - Returns the error code 
 * - herror_func() - Returns the function name, where the error occured 
 * - herror_message() - Returns the human readable error message 
 * - herror_release() - Frees the herror_t object. 
 *
 * Example:
 *
 * @code
 * herror_t err;
 *
 * if ((err = http_client_invoke("http://somewhere")) != H_OK)
 * {
 *   printf("Message: %s\n", herror_message(err));
 *   printf("Error code: %d\n", herror_code(err));
 *   printf("In function: %s\n", herror_func(err));
 *   herror_release(err);
 * }
 * @endcode
 *
 * Note that you MUST call herror_release() to http_free the resources.
 *
 */

/** @defgroup NANOHTTP_ERRORS nanoHTTP errors
 * @ingroup NANOHTTP
 */
/**@{*/

/** @defgroup NANOHTTP_GENERAL_ERROS General errors
 */ 
/**@{*/
#define H_OK				0 /**< Success flag */

#define GENERAL_ERROR                1400
#define GENERAL_INVALID_PARAM       (GENERAL_ERROR + 1)
#define GENERAL_HEADER_PARSE_ERROR  (GENERAL_ERROR + 2)
#define GENERAL_ERROR_MALLOC        (GENERAL_ERROR + 3)
#define GENERAL_ERROR_INVAL         (GENERAL_ERROR + 4)
#define GENERAL_ERROR_SYSTEM        (GENERAL_ERROR + 5)
/**@}*/

/** @defgroup NANOHTTP_THREAD_ERRORS Thread errors
 */
/**@{*/
#define THREAD_ERROR			1500
#define THREAD_BEGIN_ERROR		(THREAD_ERROR)
/**@}*/

/** @defgroup NANOHTTP_FILE_ERRORS File errors
 */
/**@{*/
#define FILE_ERROR			8000
#define FILE_ERROR_OPEN			(FILE_ERROR + 1)
#define FILE_ERROR_READ			(FILE_ERROR + 2)
#define FILE_ERROR_WRITE		(FILE_ERROR + 3)
/**@}*/

/**@}*/

/**
 *
 * @todo Remove me.
 *
 * Dummy deklaration to hide the implementation.
 *
 */
typedef void * herror_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void herror_log(herror_t err);

/**
 *
 * Creates a new error structure.
 *
 * @see printf
 *
 */
extern herror_t herror_new(const char *func, int errcode, const char *format, ...);

/**
 *
 * Returns the code of the error.
 *
 */
extern int herror_code(herror_t err);

/**
 *
 * @return The name of the function, where the error was produced.
 *
 */
extern const char *herror_func(herror_t err);

/**
 *
 * Returns the error message.
 *
 */
extern const char *herror_message(herror_t err);

/**
 *
 * Frees the error structure.
 *
 */
extern void herror_release(herror_t err);

typedef enum __ng_errno ng_errno_e;
enum __ng_errno{
  ng_ERR_NONE                     =  0,
  ng_ERR_EPERM                    = -1,
  ng_ERR_ENOENT                   = -2,
  ng_ERR_ESRCH                    = -3,
  ng_ERR_EINTR                    = -4,
  ng_ERR_EIO                      = -5,
  ng_ERR_ENXIO                    = -6,
  ng_ERR_E2BIG                    = -7,
  ng_ERR_ENOEXEC                  = -8,
  ng_ERR_EBADF                    = -9,
  ng_ERR_ECHILD                   = -10,
  ng_ERR_EWOULDBLOCK              = -11,
  ng_ERR_EAGAIN                   = -12,
  ng_ERR_ENOMEM                   = -13,
  ng_ERR_EACCES                   = -14,
  ng_ERR_EFAULT                   = -15,
  ng_ERR_ENOTBLK                  = -16,
  ng_ERR_EBUSY                    = -17,
  ng_ERR_EEXIST                   = -18,
  ng_ERR_EXDEV                    = -19,
  ng_ERR_ENODEV                   = -20,
  ng_ERR_ENOTDIR                  = -21,
  ng_ERR_EISDIR                   = -22,
  ng_ERR_EINVAL                   = -23,
  ng_ERR_ENFILE                   = -24,
  ng_ERR_EMFILE                   = -25,
  ng_ERR_ENOTTY                   = -26,
  ng_ERR_ETXTBSY                  = -27,
  ng_ERR_EFBIG                    = -28,
  ng_ERR_ENOSPC                   = -29,
  ng_ERR_ESPIPE                   = -30,
  ng_ERR_EROFS                    = -31,
  ng_ERR_EMLINK                   = -32,
  ng_ERR_EPIPE                    = -33,
  ng_ERR_EDOM                     = -34,
  ng_ERR_ERANGE                   = -35,
  ng_ERR_ENOMSG                   = -36,
  ng_ERR_EIDRM                    = -37,
  ng_ERR_ECHRNG                   = -38,
  ng_ERR_EL2NSYNC                 = -39,
  ng_ERR_EL3HLT                   = -40,
  ng_ERR_EL3RST                   = -41,
  ng_ERR_ELNRNG                   = -42,
  ng_ERR_EUNATCH                  = -43,
  ng_ERR_ENOCSI                   = -44,
  ng_ERR_EL2HLT                   = -45,
  ng_ERR_EDEADLK                  = -46,
  ng_ERR_ENOLCK                   = -47,
  ng_ERR_EBADE                    = -48,
  ng_ERR_EBADR                    = -49,
  ng_ERR_EXFULL                   = -50,
  ng_ERR_ENOANO                   = -51,
  ng_ERR_EBADRQC                  = -52,
  ng_ERR_EBADSLT                  = -53,
  ng_ERR_EDEADLOCK                = -54,
  ng_ERR_EBFONT                   = -55,
  ng_ERR_ENOSTR                   = -56,
  ng_ERR_ENODATA                  = -57,
  ng_ERR_ETIME                    = -58,
  ng_ERR_ENOSR                    = -59,
  ng_ERR_ENONET                   = -60,
  ng_ERR_ENOPKG                   = -61,
  ng_ERR_EREMOTE                  = -62,
  ng_ERR_ENOLINK                  = -63,
  ng_ERR_EADV                     = -64,
  ng_ERR_ESRMNT                   = -65,
  ng_ERR_ECOMM                    = -66,
  ng_ERR_EPROTO                   = -67,
  ng_ERR_EMULTIHOP                = -68,
  ng_ERR_EDOTDOT                  = -69,
  ng_ERR_EBADMSG                  = -70,
  ng_ERR_ENAMETOOLONG             = -71,
  ng_ERR_EOVERFLOW                = -72,
  ng_ERR_ENOTUNIQ                 = -73,
  ng_ERR_EBADFD                   = -74,
  ng_ERR_EREMCHG                  = -75,
  ng_ERR_ELIBACC                  = -76,
  ng_ERR_ELIBBAD                  = -77,
  ng_ERR_ELIBSCN                  = -78,
  ng_ERR_ELIBMAX                  = -79,
  ng_ERR_ELIBEXEC                 = -80,
  ng_ERR_EILSEQ                   = -81,
  ng_ERR_ENOSYS                   = -82,
  ng_ERR_ELOOP                    = -83,
  ng_ERR_ERESTART                 = -84,
  ng_ERR_ESTRPIPE                 = -85,
  ng_ERR_ENOTEMPTY                = -86,
  ng_ERR_EUSERS                   = -87,
  ng_ERR_ENOTSOCK                 = -88,
  ng_ERR_EDESTADDRREQ             = -89,
  ng_ERR_EMSGSIZE                 = -90,
  ng_ERR_EPROTOTYPE               = -91,
  ng_ERR_ENOPROTOOPT              = -92,
  ng_ERR_EPROTONOSUPPORT          = -93,
  ng_ERR_ESOCKTNOSUPPORT          = -94,
  ng_ERR_EOPNOTSUPP               = -95,
  ng_ERR_EPFNOSUPPORT             = -96,
  ng_ERR_EAFNOSUPPORT             = -97,
  ng_ERR_EADDRINUSE               = -98,
  ng_ERR_EADDRNOTAVAIL            = -99,
  ng_ERR_ENETDOWN                 = -100,
  ng_ERR_ENETUNREACH              = -101,
  ng_ERR_ENETRESET                = -102,
  ng_ERR_ECONNABORTED             = -103,
  ng_ERR_ECONNRESET               = -104,
  ng_ERR_ENOBUFS                  = -105,
  ng_ERR_EISCONN                  = -106,
  ng_ERR_ENOTCONN                 = -107,
  ng_ERR_ESHUTDOWN                = -108,
  ng_ERR_ETOOMANYREFS             = -109,
  ng_ERR_ECONNTIMEOUT             = -110,
  ng_ERR_ECONNREFUSED             = -111,
  ng_ERR_EHOSTDOWN                = -112,
  ng_ERR_EHOSTUNREACH             = -113,
  ng_ERR_EALREADY                 = -114,
  ng_ERR_EINPROGRESS              = -115,
  ng_ERR_ESTALE                   = -116,
  ng_ERR_EUCLEAN                  = -117,
  ng_ERR_ENOTNAM                  = -118,
  ng_ERR_ENAVAIL                  = -119,
  ng_ERR_EISNAM                   = -120,
  ng_ERR_EREMOTEIO                = -121,
  ng_ERR_E2SMALL                  = -122,
  ng_ERR_EINCOMPLETE              = -123,
  ng_ERR_ESYSTEM                  = -124,
  ng_ERR_ETIMEOUT                 = -125,
  ng_ERR_HTTP_MIME_ERROR_NO_BOUNDARY_PARAM = -126,
  ng_ERR_HTTP_MIME_ERROR_NO_START_PARAM    = -127,
  ng_ERR_HTTP_MIME_ERROR_PARSE_ERROR       = -128,
  ng_ERR_HTTP_MIME_ERROR_NO_ROOT_PART      = -129,
  ng_ERR_HTTP_MIME_ERROR_NOT_MIME_MESSAGE  = -130,
};

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef NO_ERROR
#define NO_ERROR 0
#endif

#ifdef WIN32
#define ng_errno                  GetLastError()
#define ng_set_errno(err)         SetLastError(err)
#define ng_socket_errno           WSAGetLastError()
#define ng_set_socket_errno(err)  WSASetLastError(err)
#define OS_ERR_ETIMEDOUT          WSAETIMEDOUT
#define os_strerror               __os_strerror
#else
#define ng_errno                  errno
#define ng_socket_errno           errno
#define ng_set_errno(err)         errno = err
#define ng_set_socket_errno(err)  errno = err
#define OS_ERR_ETIMEDOUT          ETIMEDOUT
#define os_strerror               __os_strerror
#endif

const ng_block_s *ng_strerror(ng_errno_e err);

#ifdef __cplusplus
}
#endif

#endif
