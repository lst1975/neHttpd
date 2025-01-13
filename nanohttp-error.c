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
/** @file nanohttp-error.c Error handling */
/******************************************************************
*  $Id: nanohttp-error.c,v 1.2 2007/11/03 22:40:11 m0gg Exp $
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
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-mem.h"
#include "nanohttp-system.h"

#ifdef WIN32
#include <Windows.h>
#endif

typedef struct _herror_impl_t
{
  int errcode;
  char message[250];
  char *func;
} herror_impl_t;

herror_t
herror_new(const char *func, int errcode, const char *format, ...)
{
  va_list ap;
  herror_impl_t *impl;
 
  if (!(impl = (herror_impl_t *) http_malloc(sizeof(herror_impl_t))))
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    return NULL;
  }

  impl->errcode = errcode;
  impl->func = func ? http_strdup(func) : NULL;

  va_start(ap, format);
  ng_vsnprintf(impl->message, sizeof(impl->message), format, ap);
  va_end(ap);

  return (herror_t) impl;
}

int
herror_code(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return H_OK;

  return impl->errcode;
}

const char *
herror_func(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return "";

  return impl->func;
}

const char *
herror_message(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return "";

  return impl->message;
}

void 
herror_log(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;
  log_error("%s", impl->message);
}

void
herror_release(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return;

  if (impl->func)
    http_free(impl->func);

  http_free(impl);

  return;
}

#define ENTRY(n, s) [-ng_ERR_ ## n] = DECL_CONST_STR_N(-ng_ERR_ ## n, "("#n")" s)

static const ng_str_s error_table[] =
{
  ENTRY(EPERM,             "Not owner"),
  ENTRY(ENOENT,            "No such file or directory"),
  ENTRY(ESRCH,             "No such process"),
  ENTRY(EINTR,             "Interrupted system call"),
  ENTRY(EIO,               "I/O error"),
  ENTRY(ENXIO,             "No such device or address"),
  ENTRY(E2BIG,             "Arg list too long"),
  ENTRY(ENOEXEC,           "Exec format error"),
  ENTRY(EBADF,             "Bad file number"),
  ENTRY(ECHILD,            "No child processes"),
  ENTRY(EWOULDBLOCK,       "Operation would block"),
  ENTRY(EAGAIN,            "No more processes"),
  ENTRY(ENOMEM,            "Not enough space"),
  ENTRY(EACCES,            "Permission denied"),
  ENTRY(EFAULT,            "Bad address"),
  ENTRY(ENOTBLK,           "Block device required"),
  ENTRY(EBUSY,             "Device busy"),
  ENTRY(EEXIST,            "File exists"),
  ENTRY(EXDEV,             "Cross-device link"),
  ENTRY(ENODEV,            "No such device"),
  ENTRY(ENOTDIR,           "Not a directory"),
  ENTRY(EISDIR,            "Is a directory"),
  ENTRY(EINVAL,            "Invalid argument"),
  ENTRY(ENFILE,            "File table overflow"),
  ENTRY(EMFILE,            "Too many open files"),
  ENTRY(ENOTTY,            "Not a typewriter"),
  ENTRY(ETXTBSY,           "Text file busy"),
  ENTRY(EFBIG,             "File too large"),
  ENTRY(ENOSPC,            "No space left on device"),
  ENTRY(ESPIPE,            "Illegal seek"),
  ENTRY(EROFS,             "Read-only file system"),
  ENTRY(EMLINK,            "Too many links"),
  ENTRY(EPIPE,             "Broken pipe"),
  ENTRY(EDOM,              "Math argument out of domain of func"),
  ENTRY(ERANGE,            "Math result not representable"),
  ENTRY(ENOMSG,            "No message of desired type"),
  ENTRY(EIDRM,             "Identifier removed"),
  ENTRY(ECHRNG,            "Channel number out of range"),
  ENTRY(EL2NSYNC,          "Level 2 not synchronized"),
  ENTRY(EL3HLT,            "Level 3 halted"),
  ENTRY(EL3RST,            "Level 3 reset"),
  ENTRY(ELNRNG,            "Link number out of range"),
  ENTRY(EUNATCH,           "Protocol driver not attached"),
  ENTRY(ENOCSI,            "No CSI structure available"),
  ENTRY(EL2HLT,            "Level 2 halted"),
  ENTRY(EDEADLK,           "Deadlock condition"),
  ENTRY(ENOLCK,            "No record locks available"),
  ENTRY(EBADE,             "Invalid exchange"),
  ENTRY(EBADR,             "Invalid request descriptor"),
  ENTRY(EXFULL,            "Exchange full"),
  ENTRY(ENOANO,            "No anode"),
  ENTRY(EBADRQC,           "Invalid request code"),
  ENTRY(EBADSLT,           "Invalid slot"),
  ENTRY(EDEADLOCK,         "File locking deadlock error"),
  ENTRY(EBFONT,            "Bad font file format"),
  ENTRY(ENOSTR,            "Device not a stream"),
  ENTRY(ENODATA,           "No data available"),
  ENTRY(ETIMEOUT,          "Timer expired"),
  ENTRY(ENOSR,             "Out of streams resources"),
  ENTRY(ENONET,            "Machine is not on the network"),
  ENTRY(ENOPKG,            "Package not installed"),
  ENTRY(EREMOTE,           "Object is remote"),
  ENTRY(ENOLINK,           "Link has been severed"),
  ENTRY(EADV,              "Advertise error"),
  ENTRY(ESRMNT,            "Srmount error"),
  ENTRY(ECOMM,             "Communication error on send"),
  ENTRY(EPROTO,            "Protocol error"),
  ENTRY(EMULTIHOP,         "Multihop attempted"),
  ENTRY(EDOTDOT,           "RFS specific error"),
  ENTRY(EBADMSG,           "Not a data message"),
  ENTRY(ENAMETOOLONG,      "File name too long"),
  ENTRY(EOVERFLOW,         "Value too large for defined data type"),
  ENTRY(ENOTUNIQ,          "Name not unique on network"),
  ENTRY(EBADFD,            "File descriptor in bad state"),
  ENTRY(EREMCHG,           "Remote address changed"),
  ENTRY(ELIBACC,           "Can not access a needed shared library"),
  ENTRY(ELIBBAD,           "Accessing a corrupted shared library"),
  ENTRY(ELIBSCN,           ".lib section in a.out corrupted"),
  ENTRY(ELIBMAX,           "Attempting to link in too many shared libraries"),
  ENTRY(ELIBEXEC,          "Cannot exec a shared library directly"),
  ENTRY(EILSEQ,            "Illegal byte sequence"),
  ENTRY(ENOSYS,            "Operation not applicable"),
  ENTRY(ELOOP,             "Too many symbolic links encountered"),
  ENTRY(ERESTART,          "Interrupted system call should be restarted"),
  ENTRY(ESTRPIPE,          "Streams pipe error"),
  ENTRY(ENOTEMPTY,         "Directory not empty"),
  ENTRY(EUSERS,            "Too many users"),
  ENTRY(ENOTSOCK,          "Socket operation on non-socket"),
  ENTRY(EDESTADDRREQ,      "Destination address required"),
  ENTRY(EMSGSIZE,          "Message too long"),
  ENTRY(EPROTOTYPE,        "Protocol wrong type for socket"),
  ENTRY(ENOPROTOOPT,       "Protocol not available"),
  ENTRY(EPROTONOSUPPORT,   "Protocol not supported"),
  ENTRY(ESOCKTNOSUPPORT,   "Socket type not supported"),
  ENTRY(EOPNOTSUPP,        "Operation not supported on transport endpoint"),
  ENTRY(EPFNOSUPPORT,      "Protocol family not supported"),
  ENTRY(EAFNOSUPPORT,      "Address family not supported by protocol"),
  ENTRY(EADDRINUSE,        "Address already in use"),
  ENTRY(EADDRNOTAVAIL,     "Cannot assign requested address"),
  ENTRY(ENETDOWN,          "Network is down"),
  ENTRY(ENETUNREACH,       "Network is unreachable"),
  ENTRY(ENETRESET,         "Network dropped connection because of reset"),
  ENTRY(ECONNABORTED,      "Software caused connection abort"),
  ENTRY(ECONNRESET,        "Connection reset by peer"),
  ENTRY(ENOBUFS,           "No buffer space available"),
  ENTRY(EISCONN,           "Transport endpoint is already connected"),
  ENTRY(ENOTCONN,          "Transport endpoint is not connected"),
  ENTRY(ESHUTDOWN,         "Cannot send after transport endpoint shutdown"),
  ENTRY(ETOOMANYREFS,      "Too many references: cannot splice"),
  ENTRY(ECONNTIMEOUT,      "Connection timed out"),
  ENTRY(ECONNREFUSED,      "Connection refused"),
  ENTRY(EHOSTDOWN,         "Host is down"),
  ENTRY(EHOSTUNREACH,      "No route to host"),
  ENTRY(EALREADY,          "Operation already in progress"),
  ENTRY(EINPROGRESS,       "Operation now in progress"),
  ENTRY(ESTALE,            "Stale NFS file handle"),
  ENTRY(EUCLEAN,           "Structure needs cleaning"),
  ENTRY(ENOTNAM,           "Not a XENIX named type file"),
  ENTRY(ENAVAIL,           "No XENIX semaphores available"),
  ENTRY(EISNAM,            "Is a named type file"),
  ENTRY(EREMOTEIO,         "Remote I/O error"),
  ENTRY(E2SMALL,           "Arg list too small"),
  ENTRY(EINCOMPLETE,       "Incomplete operation or argument"),
  ENTRY(ESYSTEM,           "System error"),
  ENTRY(HTTP_MIME_ERROR_NO_BOUNDARY_PARAM,     "'boundary' not set for multipart/related"),
  ENTRY(HTTP_MIME_ERROR_NO_START_PARAM,        "'start' not set for multipart/related"),
  ENTRY(HTTP_MIME_ERROR_PARSE_ERROR,           "MIME Parse Error"),
  ENTRY(HTTP_MIME_ERROR_NO_ROOT_PART,          "No root part found"),
  ENTRY(HTTP_MIME_ERROR_NOT_MIME_MESSAGE,      "Not a MIME message"),
};

const ng_str_s *ng_strerror(ng_errno_e err)
{
  return &error_table[-err];
}

