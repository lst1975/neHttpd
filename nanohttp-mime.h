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
/******************************************************************
*  _  _   _   _  _   __
* | \/ | | | | \/ | | _/
* |_''_| |_| |_''_| |_'/  PARSER
*
*  $Id: nanohttp-mime.h,v 1.18 2007/11/03 22:40:11 m0gg Exp $
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
#ifndef __nanohttp_mime_h
#define __nanohttp_mime_h

#include "nanohttp-defs.h"
#include "nanohttp-buffer.h"
#include "nanohttp-list.h"
#include "nanohttp-common.h"
#include "nanohttp-header.h"
#include "nanohttp-error.h"
#include "nanohttp-stream.h"

/** @page nanohttp_mime_page nanoHTTP MIME attachments
 *
 * @section nanohttp_mime_toc_sec Table of contents
 *
 * - @ref nanohttp_mime_client_sec
 *   - @ref nanohttp_mime_client_init_sec
 *   - @ref nanohttp_mime_client_request_sec
 *   - @ref nanohttp_mime_client_response_sec
 *   - @ref nanohttp_mime_client_cleanup_sec
 * - @ref nanohttp_mime_server_sec
 *   - @ref nanohttp_mime_server_init_sec
 *   - @ref nanohttp_mime_server_cleanup_sec
 *
 * @section nanohttp_mime_client_sec nanoHTTP client with MIME attachments
 *
 * @subsection nanohttp_mime_client_init_sec Client initialization
 *
 * See @ref nanohttp_client_init_sec for more information about client
 * initialization.
 *
 * @subsection nanohttp_mime_client_request_sec Request the server
 *
 * The following functions can be used to transmit MIME attachments:
 *
 * @code
 * httpc_mime_begin(conn, url, start_id, "", "text/tml")
 * httpc_mime_next(conn, start_id, "text/xml", "binary")
 * http_output_stream_write(conn->out, buffer, size)
 * httpc_mime_send_file()
 * httpc_mime_end(conn, &res)
 * @endcode
 *
 * @subsection nanohttp_mime_client_response_sec Read the server response
 *
 * @subsection nanohttp_mime_client_cleanup_sec Client cleanup
 *
 * See @ref nanohttp_client_cleanup_sec for more information about client
 * cleanup.
 *
 * @section nanohttp_mime_server_sec nanoHTTP server with MIME attachments
 *
 * @subsection nanohttp_mime_server_init_sec Server initialization
 *
 * @subsection nanohttp_mime_server_cleanup_sec Server cleanup
 *
 * @author	Ferhat Ayaz
 * @version	$Revision: 1.18 $
 *
 * @see		http://www.ietf.org/rfc/rfc2045.txt
 * @see		http://www.ietf.org/rfc/rfc2046.txt
 * @see		http://www.ietf.org/rfc/rfc4288.txt
 * @see		http://www.ietf.org/rfc/rfc4289.txt
 */

/** @file nanohttp-mime.h MIME handling 
 *
 * @defgroup NANOHTTP_MIME MIME handling
 * @ingroup NANOHTTP
 *
 */
/**@{*/

/** @defgroup NANOHTTP_MIME_ERRORS MIME errors
 */
/**@{*/
#define MIME_ERROR                      (1300          )
#define MIME_ERROR_NO_BOUNDARY_PARAM    (MIME_ERROR + 1)
#define MIME_ERROR_NO_START_PARAM       (MIME_ERROR + 2)
#define MIME_ERROR_PARSE_ERROR          (MIME_ERROR + 3)
#define MIME_ERROR_NO_ROOT_PART	        (MIME_ERROR + 4)
#define MIME_ERROR_NOT_MIME_MESSAGE     (MIME_ERROR + 5)
/**@}*/

#define HTTP_BOUNDARY_FMT "------=.Part_NH_%016p_%07u"
#define HTTP_BOUNDARY_LEN 40

#define MIME_TYPE_none      0
#define MIME_TYPE_related   1
#define MIME_TYPE_form_data 2

#define MIME_CONTENT_TYPE_PARAM_ct_type                0
#define MIME_CONTENT_TYPE_PARAM_ct_related_start       1
#define MIME_CONTENT_TYPE_PARAM_ct_related_start_info  2
#define MIME_CONTENT_TYPE_PARAM_ct_related_type        3
#define MIME_CONTENT_TYPE_PARAM_ct_MAX                 4

#define MIME_CONTENT_TYPE_PARAM_part_name                0
#define MIME_CONTENT_TYPE_PARAM_part_content_id          1
#define MIME_CONTENT_TYPE_PARAM_part_filename            2
#define MIME_CONTENT_TYPE_PARAM_part_transfer_encoding   3
#define MIME_CONTENT_TYPE_PARAM_part_content_type        4
#define MIME_CONTENT_TYPE_PARAM_part_type                5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _multipartpart multipartpart;
typedef struct _multipartparser multipartparser;
typedef struct _multipartparser_callbacks multipartparser_callbacks;

typedef size_t (*multipart_cb) (multipartparser*);
typedef size_t (*multipart_data_cb) (multipartparser*, const char* data, size_t size);

/**
 *
 * part. Attachment
 *
 */
struct _multipartpart {
  ng_list_head_s header;
  ng_list_head_s link;
  content_type_t *content_disposition;
  int id;
  void *file_ch;
  
  ng_block_s *ContentID;
  ng_block_s *Disposition;
  ng_block_s *TransferEncoding;
  ng_block_s *ContentType;
  ng_block_s  start;
  ng_block_s  filename;
  ng_block_s  type;
};

struct _multipartparser {
  /** PRIVATE **/
  char buffer[HTTP_FILED_NAME_LEN_MAX+HTTP_FILED_VALUE_LEN_MAX];
  ng_buffer_s field;
  ng_buffer_s value;
  ng_block_s  boundary;
  uint16_t    index;
  uint16_t    err;
  uint16_t    state:14;
  uint16_t    skipdata:1;
  uint16_t    rootpart:1;
  uint16_t    mime_type;
  
  /** PUBLIC **/
  void *data;
  void *arg;

  multipartpart part;
  multipartparser_callbacks *settings;
};

struct _multipartparser_callbacks {
  multipart_cb      on_body_begin;
  multipart_cb      on_part_begin;
  multipart_cb      on_header_begin;
  multipart_data_cb on_header_field;
  multipart_data_cb on_header_value;
  multipart_cb      on_headers_complete;
  multipart_cb      on_part_end;
  multipart_data_cb on_data;
  multipart_cb      on_body_end;
};

/**
 *
 * Attachments
 *
 */
struct _attachments_t
{
  ng_list_head_s parts;
  ng_list_head_s link;
  hpair_t *boundary;
  hpair_t *root_id;
};
typedef struct _attachments_t attachments_t;

herror_t multipartparser_init(multipartparser *parser, 
        void *arg, content_type_t *ct);
void multipartparser_free(multipartparser *parser);

multipartpart *multipartpart_new(void);
void multipartpart_free(multipartpart *part);
void multipartpart_init(multipartpart *part);
herror_t multipart_get_attachment(multipartparser *p, 
  http_input_stream_s *in);

size_t multipartparser_execute(
    multipartparser* parser, 
    multipartparser_callbacks* callbacks,
    const char* data,
    size_t size);

/** "multipart/related"  MIME Message Builder
 *
 */
extern ng_result_t ng_http_mime_type_init(void);
extern herror_t mime_add_content_type_header(ng_list_head_s *header, 
  void *conn_ptr, int conn_id, const ng_block_s *params);
extern multipartpart *mime_part_new(ng_list_head_s *part_list, 
  const ng_block_s *params);

extern void mime_part_free(multipartpart * part);

/* should be used internally */
extern attachments_t *attachments_new(ng_list_head_s *attachments_list);

/**
 *
 * Free a attachment. Create attachments with MIME
 *
 * @see mime_get_attachments
 *
 */
extern void attachments_free(attachments_t *message);
extern void attachments_add_part(attachments_t *attachments, 
  multipartpart *part);

extern const ng_block_s *ng_http_get_mime_type_from_file(const ng_block_s *file);
extern const ng_block_s *ng_http_get_mime_type(const ng_block_s *ext);
extern ng_result_t ng_http_mime_type_init(void);
extern void ng_http_mime_type_free(void);

herror_t mime_send_file(void *conn, unsigned int id, 
  http_output_stream_s *out, ng_block_s *file);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
