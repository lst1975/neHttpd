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
 *  $Id: nanohttp-admin.h,v 1.8 2007/11/04 06:57:32 m0gg Exp $
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef __nanohttp_admin_h
#define __nanohttp_admin_h

/** @file nanohttp-admin.h Administrator application
 *
 * @defgroup NANOHTTP_ADMIN Administrator application
 * @ingroup NANOHTTP_SERVER
 */
/**@{*/

/** @defgroup NANOHTTP_ADMIN_CMDLINE_FLAGS Command line flags
 */
/**@{*/
/** Commandline argument to enabled the nanoHTTP admin interface.
 * This service will be reachable via the NHTTPD_ADMIN_CONTEXT.
 *
 * @see NHTTPD_ADMIN_CONTEXT
 */
#define NHTTPD_ARG_ENABLE_ADMIN			"-NHTTPDadmin"

/**@}*/

/** Context of the nanoHTTP admin interface.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp
 @endverbatim
 *
 * @see httpd_register
 */
#define NHTTPD_ADMIN_CONTEXT			"/nhttp"

/** Query parameter for services.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp?services=list
 @endverbatim
 */
#define NHTTPD_ADMIN_QUERY_SERVICES		"services"

#ifdef __NHTTP_INTERNAL
/** Parameter to query service statistics.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp?statistics=SERVICE_CONTEXT
 @endverbatim
 */
#define NHTTPD_ADMIN_QUERY_STATISTICS		"statistics"
#endif

/** Parameter to enable a server.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp?activate=SERVICE_CONTEXT
 @endverbatim
 */
#define NHTTPD_ADMIN_QUERY_ACTIVATE_SERVICE	"activate"

/** Parameter to disable a service.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp?passivate=SERVICE_CONTEXT
 @endverbatim
 */
#define NHTTPD_ADMIN_QUERY_PASSIVATE_SERVICE	"passivate"

/** Parameter to switch loglevels.
 *
 * Example query:
 @verbatim
   GET http://localhost:10000/nhttp?loglevel=OFF
 @endverbatim
 */
#define NHTTPD_ADMIN_QUERY_SET_LOGLEVEL         "loglevel"

#ifdef __cplusplus
extern "C" {
#endif

/** This function initializes the nanoHTTP admin interface with
 * commandline arguments, if NHTTPD_ARG_ENABLED_ADMIN was specified
 * on the commandline. This service will be reachable via the
 * NHTTP_ADMIN_CONTEXT of the nanohttp server.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 * @see NHTTPD_ADMIN_CONTEXT
 * @see NHTTPD_ARG_ENABLE_ADMIN
 */
extern herror_t httpd_admin_init_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
