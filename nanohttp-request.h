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
 *  $Id: nanohttp-request.h,v 1.16 2007/11/03 22:40:11 m0gg Exp $
 *
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2004  Ferhat Ayaz
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
#ifndef __nhttp_request_h
#define __nhttp_request_h

/** The set of common methods for HTTP/1.1 is defined below. Although
 * this set can be expanded, additional methods cannot be assumed to
 * share the same semantics for separately extended clients and servers.
 *
 * The Host request-header field MUST accompany all HTTP/1.1 requests.
 *
 * @see HTTP_HEADER_HOST
 *
 */
enum _hreq_method
{
  /**
   * The POST method is used to request that the origin server accept the entity
   * enclosed in the request as a new subordinate of the resource identified by
   * the Request-URI in the Request-Line. POST is designed to allow a uniform
   * method to cover the following functions:
   * - Annotation of existing resources;
   * - Posting a message to a bulletin board, newsgroup, mailing list, or
   *   similar group of articles;
   * - Providing a block of data, such as the result of submitting a form, to a
   *   data-handling process;
   * - Extending a database through an append operation.
   */
 HTTP_REQUEST_METHOD_POST,
  /**
   * The GET method means retrieve whatever information (in the form of an entity)
   * is identified by the Request-URI. If the Request-URI refers to a
   * data-producing process, it is the produced data which shall be returned as
   * the entity in the response and not the source text of the process, unless
   * that text happens to be the output of the process.
   */
  HTTP_REQUEST_METHOD_GET,
  /**
   * The OPTIONS method represents a request for information about the
   * communication options available on the request/response chain identified by
   * the Request-URI. This method allows the client to determine the options
   * and/or requirements associated with a resource, or the capabilities of a
   * server, without implying a resource action or initiating a resource
   * retrieval.
   */
  HTTP_REQUEST_METHOD_OPTIONS,
  /**
   * The HEAD method is identical to GET except that the server MUST NOT return
   * a message-body in the response. The metainformation contained in the HTTP
   * headers in response to a HEAD request SHOULD be identical to the information
   * sent in response to a GET request. This method can be used for obtaining
   * metainformation about the entity implied by the request without transferring
   * the entity-body itself. This method is often used for testing hypertext
   * links for validity, accessibility, and recent modification.
   */
  HTTP_REQUEST_METHOD_HEAD,
  /**
   * The PUT method requests that the enclosed entity be stored under the
   * supplied Request-URI. If the Request-URI refers to an already existing
   * resource, the enclosed entity SHOULD be considered as a modified version of
   * the one residing on the origin server. If the Request-URI does not point to
   * an existing resource, and that URI is capable of being defined as a new
   * resource by the requesting user agent, the origin server can create the
   * resource with that URI. If a new resource is created, the origin server MUST
   * inform the user agent via the 201 (Created) response. If an existing
   * resource is modified, either the 200 (OK) or 204 (No Content) response codes
   * SHOULD be sent to indicate successful completion of the request. If the
   * resource could not be created or modified with the Request-URI, an
   * appropriate error response SHOULD be given that reflects the nature of the
   * problem. The recipient of the entity MUST NOT ignore any Content-* (e.g.
   * Content-Range) headers that it does not understand or implement and MUST
   * return a 501 (Not Implemented) response in such cases.
   */
  HTTP_REQUEST_METHOD_PUT,
  /**
   * The DELETE method requests that the origin server delete the resource
   * identified by the Request-URI. This method MAY be overridden by human
   * intervention (or other means) on the origin server. The client cannot be
   * guaranteed that the operation has been carried out, even if the status code
   * returned from the origin server indicates that the action has been completed
   * successfully. However, the server SHOULD NOT indicate success unless, at the
   * time the response is given, it intends to delete the resource or move it to
   * an inaccessible location.
   */
  HTTP_REQUEST_METHOD_DELETE,
  /**
   * The TRACE method is used to invoke a remote, application-layer loop-back of
   * the request message. The final recipient of the request SHOULD reflect the
   * message received back to the client as the entity-body of a 200 (OK)
   * response. The final recipient is either the origin server or the first proxy
   * or gateway to receive a Max-Forwards value of zero (0) in the request (see
   * section 14.31). A TRACE request MUST NOT include an entity.
   */
  HTTP_REQUEST_METHOD_TRACE,
  /**
   * This specification reserves the method name CONNECT for use with a proxy
   * that can dynamically switch to being a tunnel (e.g. SSL tunneling [44]).
   */
  HTTP_REQUEST_METHOD_CONNECT,
  HTTP_REQUEST_METHOD_UNKOWN
};

typedef enum _hreq_method hreq_method_e;

/** @file nanohttp-request.h HTTP request handling
 *
 * @defgroup NANOHTTP_REQUEST HTTP request handling
 * @ingroup NANOHTTP_SERVER
 *
 */
/**@{*/

/** @defgroup HTTP_REQUEST_HEADER_FIELDS Request Header Fields
 * @ingroup HTTP_HEADER_FIELDS
 *
 * The request-header fields allow the client to pass additional
 * information about the request, and about the client itself, to the
 * server. These fields act as request modifiers, with semantics
 * equivalent to the parameters on a programming language method
 * invocation.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 */
/*@{*/


/*@}*/

#ifdef __NHTTP_INTERNAL
struct request_statistics
{
  unsigned long bytes_transmitted;
  unsigned long bytes_received;
  ng_uint64_t time;
};
typedef struct request_statistics request_statistics_s;
#endif

/**
 *
 * HTTP request object
 *
 */
struct hrequest_t
{
  ng_block_s path;
  ng_list_head_s query;
  ng_list_head_s header;
  void    *conn;
  ng_size_t   content_length;
#ifdef __NHTTP_INTERNAL
  request_statistics_s statistics;
#endif

  http_input_stream_s *in;
  content_type_s *content_type;
  void *attachments;
  hreq_method_e  method;
  http_version_e version;
  int   userLevel;
};
typedef struct hrequest_t hrequest_s;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NHTTP_INTERNAL
extern herror_t hrequest_new_from_socket(hsocket_s *sock, hrequest_s **out);
extern void hrequest_free(hrequest_s *req);
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
