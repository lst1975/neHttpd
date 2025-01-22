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
 *  $Id: nanohttp-client.h,v 1.40 2007/11/03 22:40:10 m0gg Exp $
 *
 * CSOAP Project:  A http client/server library in C
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
#ifndef __nanohttp_client_h
#define __nanohttp_client_h

/* XXX: Clean up nanohttp to make this unnecessary */
#ifndef __NHTTP_INTERNAL
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-response.h"
#endif

/** @page nanohttp_client_page Howto write an HTTP client
 *
 * @section client_sec Table of contents
 *
 * - @ref nanohttp_client_init_sec
 * - @ref nanohttp_client_conn_init_sec
 *   - @ref nanohttp_client_ssl_sec
 * - @ref nanohttp_client_header_sec
 *   - @ref nanohttp_client_header_uniq_sec
 *   - @ref nanohttp_client_header_mult_sec
 *   - @ref nanohttp_client_authorization_sec
 * - @ref nanohttp_client_invoke_sec
 *   - @ref nanohttp_client_get_sec
 *   - @ref nanohttp_client_post_sec
 *   - @ref nanohttp_client_mime_sec
 * - @ref nanohttp_client_result_sec
 * - @ref nanohttp_client_conn_cleanup_sec
 * - @ref nanohttp_client_cleanup_sec
 *
 * @section nanohttp_client_init_sec Client initialization
 *
 * @code
 * int main(int argc, char **argv)
 * {
 *   herror_t status;
 *   ng_size_t len;
 *   httpc_conn_s conn;
 *
 *   if (argc < 2)
 *   {
 *     ng_fprintf(stderr, "usage: %s <url> [nanoHTTP params]\n", argv[0]);
 *     exit(1);
 *   }
 *
 *   if ((status = httpc_init(argc, argv)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "Cannot init nanoHTTP client (%s)\n", herror_message(status));
 *     herror_release(status);
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_client_conn_init_sec Connection initialization
 *
 * @code
 *   if (!(conn = httpc_new()))
 *   {
 *     ng_fprintf(stderr, "Cannot create nanoHTTP client connection\n");
 *     httpc_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_client_ssl_sec SSL related functions
 *
 * T.B.D.
 *
 * @ref nanohttp_ssl_page
 *
 * @section nanohttp_client_header_sec Setting HTTP headers (optional)
 *
 * @subsection nanohttp_client_header_uniq_sec Setting an header with uniq key
 *
 * @code
 *   httpc_set_header(conn, "my-key", "my-value");
 * @endcode
 *
 * @subsection nanohttp_client_header_mult_sec Setting headers with the same key
 *
 * @code
 *   httpc_add_header(conn, "Cookie", "name1:value1");
 *   httpc_add_header(conn, "Cookie", "name2:value2");
 * @endcode
 *
 * Please see @ref http_general_header_fields and @ref http_request_header_fields
 * for more information.
 *
 * @subsection nanohttp_client_authorization_sec HTTP authorization
 *
 * @code
 *   httpc_set_basic_authorization(conn, "username", "password");
 * @endcode
 *
 * @code
 *   httpc_set_basic_proxy_authorization(conn, "username", "password");
 * @endcode
 *
 * @section nanohttp_client_invoke_sec Request the network resource
 *
 * @subsection nanohttp_client_get_sec HTTP GET method
 *
 * @code
 *   if ((status = httpc_get(conn, &result, argv[1])) != H_OK)
 *   {
 *     ng_fprintf(stderr, "nanoHTTP client connection failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @subsection nanohttp_client_post_sec HTTP POST method
 *
 * @code
 *   if ((status = httpc_post_begin(conn, argv[1])) != H_OK)
 *   {
 *     ng_fprintf(stderr, "nanoHTTP client connection failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 *   if ((status = http_output_stream_write(conn->out, buffer, len)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "nanoHTTP client sending POST data failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 *   if ((status = httpc_post_end(conn, &result)) != H_OK)
 *   {
 *     ng_fprintf(stderr, "nanoHTTP client POST failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_client_mime_sec MIME attachments
 *
 * T.B.D.
 *
 * @ref nanohttp_mime_page
 *
 * @section nanohttp_client_result_sec Fetch and print out the result
 *
 * @code
 *   while (http_input_stream_is_ready(res->in))
 *   {
 *     len = http_input_stream_read(res->in, buffer, MAX_BUFFER_SIZE);
 *     fwrite(buffer, len, 1, stdout);
 *   }
 * @endcode
 *
 * @section nanohttp_client_conn_cleanup_sec Connection cleanup
 *
 * @code
 *   hresponse_free(res);
 * @endcode
 *
 * @section nanohttp_client_cleanup_sec Client cleanup
 *
 * @code
 *   httpc_free(conn);
 *
 *   exit(0);
 * }
 * @endcode
 *
 */

/** @file nanohttp-client.h nanoHTTP client interface
 *
 * @defgroup NANOHTTP_CLIENT Client
 * @ingroup NANOHTTP
 *
 */
/**@{*/

struct httpc_conn
{
  hsocket_s sock;
  ng_url_s url;
  ng_list_head_s header;
  http_version_e version;

  int errcode;
  char errmsg[150];
  http_output_stream_s *out;
  int id;                       /* uniq id */
};
typedef struct httpc_conn httpc_conn_s;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the nanoHTTP client module.
 *
 * @param argc		Argument count.
 * @param argv		Argument vector.
 *
 * @return H_OK on succes or a herror_t struct on failure.
 *
 * @see httpc_destroy
 * @see herror_t
 *
 */
extern herror_t httpc_init(int argc, char **argv);

/**
 *
 * Destroys the nanoHTTP client module.
 *
 * @see httpc_init
 *
 */
extern void httpc_destroy(void);

/**
 *
 * Creates a new HTTP client connection object. You need to create at least one
 * HTTP client connection to communicate via HTTP.
 *
 * @return A pointer to a httpc_conn_s structure on success, NULL on error.
 *
 * @see httpc_conn_s
 *
 */
extern httpc_conn_s *httpc_new(void);

/**
 *
 * Release a connection
 * (use httpc_close_free() instead)
 *
 * @see httpc_close_free
 *
 */
extern void httpc_free(httpc_conn_s * conn);

/**
 *
 * Close and release a connection
 *
 * @see httpc_close
 * @see httpc_free
 *
 */
extern void httpc_close_free(httpc_conn_s * conn);

/**
 *
 * Sets a HEADER_AUTHORIZATION header.
 *
 * @param conn		The HTTP connection.
 * @param user		The username.
 * @param password	The password.
 *
 * @see httpc_set_header
 * @see HEADER_AUTHORIZATION
 *
 */
extern int httpc_set_basic_authorization(httpc_conn_s *conn, 
    const ng_block_s *user, const ng_block_s *password);

/**
 *
 * Sets a HEADER_PROXY_AUTHORIZATION header.
 *
 * @param conn		The HTTP connection.
 * @param user		The username.
 * @param password	The password.
 *
 * @see httpc_set_header
 * @see HEADER_PROXY_AUTHORIZATION
 *
 */
extern int httpc_set_basic_proxy_authorization(httpc_conn_s *conn, 
    const ng_block_s *user, const ng_block_s *password);

/**
 *
 * Invoke a "GET" method request and receive the response
 *
 * @return H_OK on success
 *
 * @see HTTP_REQUEST_GET
 *
 */
extern herror_t httpc_get(httpc_conn_s *conn, hresponse_t **out, 
                  const ng_block_s *urlstr);

/**
 *
 * Start a "POST" method request
 *
 * @return H_OK on success or error flag
 *
 * @see HTTP_REQUEST_POST
 * @see httpc_post_end
 *
 */
extern herror_t httpc_post_begin(httpc_conn_s *conn, const ng_block_s *url);

/**
 *
 * End a "POST" method and receive the response.
 * You MUST call httpc_post_end() before!
 *
 * @return H_OK on success
 *
 * @see httpc_post_begin
 * @see HTTP_REQUEST_POST
 *
 */
extern herror_t httpc_post_end(httpc_conn_s *conn, hresponse_t **out);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
