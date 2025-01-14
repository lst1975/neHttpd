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
 *  $Id: nanohttp-url.h,v 1.4 2007/11/03 22:40:15 m0gg Exp $
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
#ifndef __nanohttp_url_h
#define __nanohttp_url_h

#include <stdint.h>
#include "nanohttp-defs.h"
#include "nanohttp-inet.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file nanohttp-url.h URL handling
 *
 * @defgroup NANOHTTP_URL URL handling
 * @ingroup NANOHTTP
 *
 */
/*@{*/

/** URL errors
 */
/**@{*/
#define URL_ERROR                    1100
#define URL_ERROR_UNKNOWN_PROTOCOL  (URL_ERROR + 1)
#define URL_ERROR_NO_PROTOCOL       (URL_ERROR + 2)
#define URL_ERROR_NO_HOST           (URL_ERROR + 3)
#define URL_ERROR_BAD_PORT          (URL_ERROR + 4)
#define URL_ERROR_CHARACTER         (URL_ERROR + 5)
#define URL_ERROR_BAD_IP            (URL_ERROR + 6)

/**@}*/

/** The protocol types in enumeration format. Used in some other
 * nanohttp objects like hurl_t.
 *
 * @see hurl_t
 */
#define NG_PROTOCOL_NONE  0
#define NG_PROTOCOL_HTTP  1
#define NG_PROTOCOL_HTTPS 2

/** The URL object. A representation of an URL like:
 *
 @verbatim
   [protocol]://[user]@[host]:[port]/[context]['#' fragment]['?' query]
 @endverbatim
 *
 * @see http://www.ietf.org/rfc/rfc2396.txt
 */
struct __ng_url
{
  uint16_t port;              /**< The port number. If no port number was given
                                 in the URL, one of the default port numbers
                                 will be selected: 
                                 - HTTP_DEFAULT_PORT    
                                 - HTTPS_DEFAULT_PORT */
  uint16_t protocol;          /**< The transfer protocol. Note that only
                                 - PROTOCOL_HTTP and
                                 - PROTOCOL_HTTPS
                                 are supported by nanohttp. */
  uint32_t is_ipv6;                           
  ng_block_s user;              /**< The username */
  ng_block_s pswd;              /**< The password */
  ng_block_s host;              /**< The hostname */
  ng_block_s context;           /** The string after the hostname. */
  union{
    uint8_t v4[NG_INADDRSZ];
    uint8_t v6[NG_IN6ADDRSZ];
  } u;

  char *data;
};

typedef struct __ng_url ng_url_s;

/** This function parses the given 'urlstr' and fills the given hurl_t
 * object. Parse an URI
 *
 @verbatim 
   URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
 @endverbatim
 *
 * @param obj the destination URL object to fill
 * @param url the URL in string format
 *
 * @returns H_OK on success or one of the following otherwise
 *          - URL_ERROR_UNKNOWN_PROTOCOL 
 *          - URL_ERROR_NO_PROTOCOL 
 *          - URL_ERROR_NO_HOST 
 */
extern int __ng_url_parse(ng_url_s *url, const char *urlstr, int urllen);
extern herror_t ng_url_parse(ng_url_s *url, const char *urlstr, int urllen);

extern void ng_urlorpath_dlim_convert(char *str, int len, 
 const char from, const char to);
extern int ng_urlorPath_normalize(ng_block_s *url);
extern void ng_url_dump(const ng_url_s *url);

/** This function frees the resources within a url and the url itself.
 *
 * @param url pointer to an hurl_t
 */
extern void ng_url_free(ng_url_s *url);
extern int ng_url_init(ng_url_s *url);

extern void ng_url_test(void);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
