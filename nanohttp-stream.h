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
 *  $Id: nanohttp-stream.h,v 1.17 2007/11/04 06:57:32 m0gg Exp $
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
#ifndef __nanohttp_stream_h
#define __nanohttp_stream_h

#include "nanohttp-socket.h"

/** @file nanohttp-stream.h Stream handling
 *
 * @section HTTP Stream modul
 *
 * nanoHTTP supports 2 different streams:
 * -# http_input_stream_t
 * -# http_output_stream_t
 *
 * These are not regular streams. They will care about transfer styles while
 * sending/receiving data.
 *
 * Supported transfer styles are
 *
 * - Content-length
 * - Chunked encoding
 * - Connection: "close"
 *
 * A stream will set its transfer style from the header information, which must
 * be given while creating a stream.
 *
 * A stream will start sending/receiving data "after" sending/receiving header
 * information (After CR LF CR LF)."
 */
/**{@*/

/** @defgroup NANOHTTP_STREAM_ERRORS Stream errors
 * @ingroup NANOHTTP_ERRORS
 */
/*@{*/
#define STREAM_ERROR			            (1200             )
#define STREAM_ERROR_INVALID_TYPE	    (STREAM_ERROR + 10)
#define STREAM_ERROR_SOCKET_ERROR	    (STREAM_ERROR + 20)
#define STREAM_ERROR_NO_CHUNK_SIZE	  (STREAM_ERROR + 30)
#define STREAM_ERROR_WRONG_CHUNK_SIZE	(STREAM_ERROR + 40)
/*@}*/

/** Transfer types supported while sending/receiving data. */
typedef enum http_transfer_type
{
  HTTP_TRANSFER_CONTENT_LENGTH,   /**< The stream cares about
				       Content-length */
  HTTP_TRANSFER_CHUNKED,          /**< The stream sends/receives
				       chunked data */
  HTTP_TRANSFER_CONNECTION_CLOSE, /**< The stream sends/receives
				       data until connection is closed */
  HTTP_TRANSFER_FILE              /**< This transfer style will be used
				       by MIME support and for debug
				       purposes. */
} http_transfer_type_e;

typedef struct http_input_stream_t http_input_stream_s;
typedef struct http_output_stream_t http_output_stream_s;

/** HTTP INPUT STREAM. Receives data from a socket/file and cares
 * about the transfer style. */
struct http_input_stream_t
{
  hsocket_s *sock;
  herror_t err;
  ng_size_t received;
  ng_size_t content_length;
  ng_size_t chunk_size;
  char connection_closed;

  int (*stream_ready)(http_input_stream_s *stream);
  ng_size_t (*stream_read)(http_input_stream_s *stream, unsigned char *dest, ng_size_t size);
  
  /* file handling */
  void *fd;
  ng_block_s filename;
  int deleteOnExit;             /* default is 0 */
  http_transfer_type_e type;
};

/** HTTP OUTPUT STREAM. Sends data to a socket and cares about the
 * transfer style. */
struct http_output_stream_t
{
  hsocket_s *sock;
  http_transfer_type_e type;
  ng_size_t content_length;
  ng_size_t sent;
  herror_t status;
};

#ifdef __cplusplus
extern "C" {
#endif

/** This function creates a new input stream. The transfer style will
 * be choosen from the given header.
 *
 * @param sock the socket to receive data from
 * @param header the http header. This must be received before creating a
 *        http_input_stream_t.
 *
 * @return a newly created http_input_stream_t object. If no transfer style was
 *         found in the header, HTTP_TRANSFER_CONNECTION_CLOSE  will be used as
 *         default.
 *
 * @see http_input_stream_free()
 */
extern http_input_stream_s *
http_input_stream_new(hsocket_s *sock, ng_list_head_s *header, 
  ng_buffer_s *data);

/** This function creates a new input stream from file. It was added
 * for MIME messages and for debugging purposes. The transfer style
 * is always HTTP_TRANSFER_FILE.
 *
 * @param filename the name of the file to open and read. 
 *
 * @return The return value is a http_input_stream_t object if the file
 *         exists and could be opened. NULL otherwise.
 *
 * @see http_input_stream_free()
 */
extern http_input_stream_s *
http_input_stream_new_from_file(const char *filename, int len);

/** Free input stream. Note that the socket will not be closed by this
 * functions.
 *
 * @param stream the input stream to ng_free.
 */
extern void 
http_input_stream_free(http_input_stream_s *stream);

/** This function returns the actual status of the stream.
 *
 * @param stream the stream to check its status
 *
 * @return - 1, if there is still data to read. 
 *         - 0, if no more data exists.
 */
extern int 
http_input_stream_is_ready(http_input_stream_s *stream);

/** This function tries to read 'size' bytes from the stream. Check
 * always with http_input_stream_is_ready() if there are some data to
 * read. If it returns 0, no more data is available on stream.
 *
 * On error this function will return -1. In this case the "err" field
 * of stream will be set to the actual error. This can be one of the
 * followings:
 * - STREAM_ERROR_NO_CHUNK_SIZE 
 * - STREAM_ERROR_WRONG_CHUNK_SIZE
 * - STREAM_ERROR_INVALID_TYPE
 * - HSOCKET_ERROR_RECEIVE  
 *
 * @param stream the stream to read data from
 * @param dest destination memory to store read bytes
 * @param size maximum size of 'dest' (size to read)
 *
 * @return the actual read bytes or -1 on error.
 */
extern ng_size_t 
http_input_stream_read(http_input_stream_s *stream, 
                  unsigned char *dest, ng_size_t size);

/** Creates a new output stream. Transfer style will be found from the
 * given header.
 *
 * @param sock the socket to to send data to
 * @param header the header which must be sent before
 *
 * @return A http_output_stream_t object. If no proper transfer style
 *         was found in the header, HTTP_TRANSFER_CONNECTION_CLOSE will
 *         be used as default.
 *
 * @see http_output_stream_free()
 */
extern http_output_stream_s *
http_output_stream_new(hsocket_s *sock, ng_list_head_s *header);


/** This function frees the given output stream. Note that this
 * function will not close any socket connections.
 *
 * @param stream The stream to ng_free.
 */
extern void 
http_output_stream_free(http_output_stream_s *stream);

/** This function writes 'size' bytes of 'bytes' into stream.
 *
 * @param stream the stream to use to send data
 * @param bytes bytes to send
 * @param size size of bytes to send
 *
 * @return H_OK on success. One of the followings otherwise:
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t 
http_output_stream_write(http_output_stream_s *stream, 
                  const unsigned char *bytes, ng_size_t size);

extern herror_t
http_output_stream_write_printf(http_output_stream_s *stream, 
  const char *format, ...);

/**
  Writes 'size' bytes of 'bytes' into stream.
  Returns socket error flags or H_OK.
*/
static inline herror_t
http_output_stream_write_buffer(http_output_stream_s * stream,
                         ng_block_s *b)
{
  return http_output_stream_write(stream, b->ptr, b->len);
}

/** This function writes a null terminated string to the stream.
 *
 * @param stream the stream to use to send data
 * @param str a null terminated string to send
 *
 * @return H_OK on success. One of the followings otherwise
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t 
http_output_stream_write_string(http_output_stream_s *stream, 
                        const char *str);

/** This function sends finish flags if nesseccary (like in chunked
 * transport). Call always this function before closing the connections.
 *
 * @param stream the stream to send post data.
 *
 * @return H_OK on success. One of the followings otherwise
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t 
http_output_stream_flush(http_output_stream_s *stream);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
