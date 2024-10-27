/******************************************************************
 *  $Id: nanohttp-url.h,v 1.4 2007/11/03 22:40:15 m0gg Exp $
 * 
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2004  Ferhat Ayaz
 *
 * This library is free software; you can redistribute it and/or
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
#define URL_ERROR			1100
#define URL_ERROR_UNKNOWN_PROTOCOL	(URL_ERROR + 1)
#define URL_ERROR_NO_PROTOCOL		(URL_ERROR + 2)
#define URL_ERROR_NO_HOST		(URL_ERROR + 3)
/**@}*/

/** The protocol types in enumeration format. Used in some other
 * nanohttp objects like hurl_t.
 *
 * @see hurl_t
 */
typedef enum _hprotocol
{
  PROTOCOL_HTTP,
  PROTOCOL_HTTPS
} hprotocol_t;

/** The URL object. A representation of an URL like:
 *
 @verbatim
   [protocol]://[user]@[host]:[port]/[context]['#' fragment]['?' query]
 @endverbatim
 *
 * @see http://www.ietf.org/rfc/rfc2396.txt
 */
struct hurl_t
{
  hprotocol_t protocol; /**< The transfer protocol. Note that only
			     - PROTOCOL_HTTP and
			     - PROTOCOL_HTTPS
			     are supported by nanohttp. */
  unsigned short port;  /**< The port number. If no port number was given
                             in the URL, one of the default port numbers
                             will be selected: 
                             - HTTP_DEFAULT_PORT    
                             - HTTPS_DEFAULT_PORT */
  char *host;           /**< The hostname */
  char *context;        /** The string after the hostname. */
};

#ifdef __cplusplus
extern "C" {
#endif

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
extern herror_t hurl_parse(struct hurl_t * obj, const char *url);

/** This function frees the resources within a url and the url itself.
 *
 * @param url pointer to an hurl_t
 */
extern void hurl_free(struct hurl_t *url);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
