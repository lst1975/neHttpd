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
/** @file nanohttp-logging.c Logging functions */
/******************************************************************
*  $Id: nanohttp-logging.c,v 1.3 2007/11/03 22:40:11 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2007 Heiko Ronsdorf
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
******************************************************************/
#include "nanohttp-config.h"
#include "nanohttp-logging.h"
#include "nanohttp-mem.h"
#include "nanohttp-server.h"

#ifdef WIN32
#define LOG_USE_MUTEX 1
#else
#define LOG_USE_MUTEX 0
#endif

#if LOG_USE_MUTEX
static void *log_mutex=NULL;
#define log_enter_mutex() httpd_enter_mutex((void *)&log_mutex)
#define log_leave_mutex() httpd_leave_mutex((void *)&log_mutex)
#else
#define log_enter_mutex() (void)(0)
#define log_leave_mutex() (void)(0)
#endif

static int _nanohttp_logtype = NANOHTTP_LOG_FOREGROUND;
static nanohttp_loglevel_t _nanohttp_log_loglevel = NANOHTTP_LOG_DEBUG;
static char *_nanohttp_log_logfile = NULL;

#if defined WIN32 && defined __MINGW32__
char *
VisualC_funcname(const char *file, int line)
{
  static char buffer[256];
  int i;
 
  for (i = ng_strlen(file) - 1; i > 0 && file[i] != '\\'; i--)
    /* nothing */ ;
  ng_snprintf(buffer, sizeof(buffer), "%s:%d", 
    (file[i] != '\\') ? file : (file + i + 1), line);
  return buffer;
}
#endif

nanohttp_loglevel_t
nanohttp_log_set_loglevel(nanohttp_loglevel_t loglevel)
{
  nanohttp_loglevel_t old;
 
  old  = _nanohttp_log_loglevel;
  _nanohttp_log_loglevel = loglevel;

  return old;
}

nanohttp_loglevel_t
nanohttp_log_get_loglevel(void)
{
  return _nanohttp_log_loglevel;
}

int
nanohttp_log_set_logtype(int type)
{
  int old;

  old = _nanohttp_logtype;
  _nanohttp_logtype = type;

  return old;
}

const char *
nanohttp_log_get_logfile(void)
{
  return _nanohttp_log_logfile;
}

void
nanohttp_log_set_logfile(const char *filename)
{
  if (_nanohttp_log_logfile)
    ng_free(_nanohttp_log_logfile);

  if (filename)
    _nanohttp_log_logfile = ng_strdup(filename);
}

void
_vnanohttp_log_printf(nanohttp_loglevel_t level, 
  const char *format, va_list ap)
{
  const char *filename;

  if (level < _nanohttp_log_loglevel 
    || _nanohttp_log_loglevel == NANOHTTP_LOG_OFF)
  {
    if (level < NANOHTTP_LOG_ERROR)
      return;
  }
  
  if (_nanohttp_logtype & NANOHTTP_LOG_FOREGROUND)
  {
    log_enter_mutex();
    ng_vfprintf(level==NANOHTTP_LOG_STDERR ?stderr:stdout, format, ap);
    log_leave_mutex();
  }
#ifdef HAVE_SYSLOG_H
  else if (_nanohttp_logtype & NANOHTTP_LOG_SYSLOG)
  {
    int syslog_level;

    switch (level)
    {
      case NANOHTTP_LOG_VERBOSE:
      case NANOHTTP_LOG_DEBUG:
        syslog_level = LOG_DEBUG;
      	break;
      case NANOHTTP_LOG_INFO:
        syslog_level = LOG_INFO;
      	break;
      case NANOHTTP_LOG_WARN:
      	syslog_level = LOG_WARNING;
      	break;
      case NANOHTTP_LOG_STDERR:
      case NANOHTTP_LOG_ERROR:
      	syslog_level = LOG_ERR;
        break;
      case NANOHTTP_LOG_FATAL:
      	syslog_level = LOG_CRIT;
      	break;
      default:
        syslog_level = LOG_INFO;
        break;
    }
    log_enter_mutex();
    vsyslog(syslog_level, format, ap);
    log_leave_mutex();
  }
#endif

  if ((filename = nanohttp_log_get_logfile()))
  {
    FILE *fp;

    if (!(fp = fopen(filename, "a")))
      fp = fopen(filename, "w");

    if (fp)
    {
      log_enter_mutex();
      ng_vfprintf(fp, format, ap);
      log_leave_mutex();
      fflush(fp);
      fclose(fp);
    }
  }
}

void
_nanohttp_log_printf(nanohttp_loglevel_t level, const char *format, ...)
{
  va_list ap;

  if (level < _nanohttp_log_loglevel)
    return;

  va_start(ap, format);
  _vnanohttp_log_printf(level, format, ap);
  va_end(ap);
}

int http_log_init(void)
{
  int err = 0;
#if LOG_USE_MUTEX
  err = httpd_create_mutex((void *)&log_mutex);
#endif
  if (!err)
    log_info("[OK]: http_log_init.");
  return err;
}

void http_log_free(void)
{
#if LOG_USE_MUTEX
  httpd_destroy_mutex((void *)&log_mutex);
#endif
  log_info("[OK]: http_log_free.");
}

