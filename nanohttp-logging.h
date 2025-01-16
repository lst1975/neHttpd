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
 *  $Id: nanohttp-logging.h,v 1.6 2007/11/03 22:40:11 m0gg Exp $
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
#ifndef __nanohttp_logging_h
#define __nanohttp_logging_h

/** @file nanohttp-logging.h Logging definitions and prototypes
 *
 * @defgroup NANOHTTP_LOGGING Logging interface
 * @ingroup NANOHTTP
 */
/**@{*/

/** @defgroup NANOHTTP_CMDLINE_LOGGING Commandline flags
 * @ingroup NANOHTTP_CMDLINE
 */
/**@{*/
#define NHTTP_ARG_LOGFILE	"-NHTTPlog"
#define NHTTP_ARG_LOGLEVEL	"-NHTTPloglevel"
/**@}*/

/** Loglevel definition */
typedef enum nanohttp_loglevel
{
  NANOHTTP_LOG_OFF,     /**< Logging completely turned off (use at your own risk). */
  NANOHTTP_LOG_VERBOSE, /**< Debugging messages that may overflow the log */
  NANOHTTP_LOG_DEBUG,   /**< Message that contain information normally
                  			     of use only when debugging the library */
  NANOHTTP_LOG_INFO,    /**< Informaional messages */
  NANOHTTP_LOG_WARN,    /**< Warning messages */
  NANOHTTP_LOG_ERROR,   /**< A condition that should be corrected
                  			     immediately, such as a broken network
                  			     connection */
  NANOHTTP_LOG_FATAL,   /**< A panic condition */
  NANOHTTP_LOG_STDERR   /**< Use syslog or stderr */
} nanohttp_loglevel_t;

#define NANOHTTP_LOG_LEVEL_OFF_STRING     "OFF"
#define NANOHTTP_LOG_LEVEL_VERBOSE_STRING "VERBOSE"
#define NANOHTTP_LOG_LEVEL_DEBUG_STRING   "DEBUG"
#define NANOHTTP_LOG_LEVEL_INFO_STRING    "INFO"
#define NANOHTTP_LOG_LEVEL_WARN_STRING    "WARN"
#define NANOHTTP_LOG_LEVEL_ERROR_STRING   "ERROR"
#define NANOHTTP_LOG_LEVEL_FATAL_STRING   "FATAL"
#define NANOHTTP_LOG_LEVEL_STDERR_STRING  "ERROR"
#define NANOHTTP_LOG_LEVEL_UNKNOWN_STRING "UNKNOWN"

#ifdef __cplusplus
extern "C" {
#endif

/** This function sets a new the loglevel and returns the previous
 * one.
 *
 * @param level The new loglevel.
 *
 * @return The old loglevel.
 */
extern nanohttp_loglevel_t nanohttp_log_set_loglevel(nanohttp_loglevel_t level);

/** This function returns the current loglevel.
 *
 * @return The current loglevel.
 */
extern nanohttp_loglevel_t nanohttp_log_get_loglevel(void);

/** This function set the name of a logfile.
 *
 * @param filename The filename of the logfile.
 */
extern void nanohttp_log_set_logfile(const char *filename);

/** This function returns the filename of the current logfile.
 *
 * @return Pointer to the filename or NULL otherwise.
 */
extern const char *nanohttp_log_get_logfile(void);

#define NANOHTTP_LOG_DISABLED   0x00 /**< Logging disabled */
#define NANOHTTP_LOG_FOREGROUND	0x01 /**< Logging to stdout enabled */
#define NANOHTTP_LOG_SYSLOG     0x02 /**< Syslog logging enabled */

/** This function sets the type of logging
 *
 * @return The old logtype.
 *
 * @see - NANOHTTP_LOG_DISABLED
 *      - NANOHTTP_LOG_FOREGROUND
 *      - NANOHTTP_LOG_SYSLOG
 */
extern int nanohttp_log_set_logtype(int type);

#ifdef WIN32
#if RTE_TOOLCHAIN_MSVC && _MSC_VER <= 1200
extern char *VisualC_funcname(const char *file, int line); /* not thread safe! */
#define __FUNCTION__  VisualC_funcname(__FILE__, __LINE__)
#endif
#endif

#ifdef __cplusplus
}
#endif

void _vnanohttp_log_printf(nanohttp_loglevel_t level, const char *format, va_list ap);
void _nanohttp_log_printf(nanohttp_loglevel_t level, const char *format, ...);

#define PRINT_FUNCTIAON

#if __NHTTP_NO_LOGGING
#define log_verbose(fmt, ...) NG_UNUSED(fmt)
#define log_debug  (fmt, ...) NG_UNUSED(fmt)
#define log_info   (fmt, ...) NG_UNUSED(fmt)
#define log_warn   (fmt, ...) NG_UNUSED(fmt)
#define log_error  (fmt, ...) NG_UNUSED(fmt)
#define log_fatal  (fmt, ...) NG_UNUSED(fmt)
#define log_stderr (fmt, ...) NG_UNUSED(fmt)
#define log_print  (fmt, ...) NG_UNUSED(fmt)
#elif defined(PRINT_FUNCTIAON)
#define __FUNCTION_FMT "[%-7s]: %16.16s... "
#define log_verbose(fmt, ...) _nanohttp_log_printf(NANOHTTP_LOG_VERBOSE, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_VERBOSE_STRING, __FUNCTION__, ## __VA_ARGS__)

#define log_debug(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_DEBUG, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_DEBUG_STRING, __FUNCTION__, ## __VA_ARGS__)

#define log_info(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_INFO, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_INFO_STRING, __FUNCTION__, ## __VA_ARGS__)

#define log_warn(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_WARN, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_WARN_STRING, __FUNCTION__, ## __VA_ARGS__)

#define log_error(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_ERROR, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_ERROR_STRING, __FUNCTION__, ## __VA_ARGS__)

#define log_fatal(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_FATAL, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_FATAL_STRING, __FUNCTION__, ## __VA_ARGS__)
#define log_stderr(fmt, ...)  _nanohttp_log_printf(NANOHTTP_LOG_STDERR, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_STDERR_STRING, __FUNCTION__, ## __VA_ARGS__)
#define log_print(fmt, ...)  _nanohttp_log_printf(NANOHTTP_LOG_INFO, fmt, ## __VA_ARGS__)
#else
#define __FUNCTION_FMT "[%-7s]: "
#define log_verbose(fmt, ...) _nanohttp_log_printf(NANOHTTP_LOG_VERBOSE, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_VERBOSE_STRING, ## __VA_ARGS__)

#define log_debug(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_DEBUG, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_DEBUG_STRING, ## __VA_ARGS__)

#define log_info(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_INFO, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_INFO_STRING, ## __VA_ARGS__)

#define log_warn(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_WARN, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_WARN_STRING, ## __VA_ARGS__)

#define log_error(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_ERROR, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_ERROR_STRING, ## __VA_ARGS__)

#define log_fatal(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_FATAL, \
                             __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_FATAL_STRING, ## __VA_ARGS__)
#define log_stderr(fmt, ...)  _nanohttp_log_printf(NANOHTTP_LOG_STDERR, \
                              __FUNCTION_FMT fmt "\n", \
                             NANOHTTP_LOG_LEVEL_STDERR_STRING, ## __VA_ARGS__)
#define log_print(fmt, ...)  _nanohttp_log_printf(NANOHTTP_LOG_INFO, fmt, ## __VA_ARGS__)
#endif
/**@}*/

int http_log_init(void);
void http_log_free(void);

#endif
