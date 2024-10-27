/******************************************************************
 *  $Id: nanohttp-stream.h,v 1.17 2007/11/04 06:57:32 m0gg Exp $
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
#ifndef __nanohttp_stream_h
#define __nanohttp_stream_h

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
#define STREAM_ERROR			1200
#define STREAM_ERROR_INVALID_TYPE	(STREAM_ERROR + 1)
#define STREAM_ERROR_SOCKET_ERROR	(STREAM_ERROR + 2)
#define STREAM_ERROR_NO_CHUNK_SIZE	(STREAM_ERROR + 3)
#define STREAM_ERROR_WRONG_CHUNK_SIZE	(STREAM_ERROR + 4)
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
} http_transfer_type_t;

/** HTTP INPUT STREAM. Receives data from a socket/file and cares
 * about the transfer style. */
struct http_input_stream_t
{
  struct hsocket_t *sock;
  herror_t err;
  http_transfer_type_t type;
  int received;
  int content_length;
  int chunk_size;
  char connection_closed;

  /* file handling */
  FILE *fd;
  char filename[255];
  int deleteOnExit;             /* default is 0 */
};

/** HTTP OUTPUT STREAM. Sends data to a socket and cares about the
 * transfer style. */
struct http_output_stream_t
{
  struct hsocket_t *sock;
  http_transfer_type_t type;
  int content_length;
  int sent;
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
extern struct http_input_stream_t *http_input_stream_new(struct hsocket_t *sock, hpair_t *header);

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
extern struct http_input_stream_t *http_input_stream_new_from_file(const char *filename);

/** Free input stream. Note that the socket will not be closed by this
 * functions.
 *
 * @param stream the input stream to free.
 */
extern void http_input_stream_free(struct http_input_stream_t * stream);

/** This function returns the actual status of the stream.
 *
 * @param stream the stream to check its status
 *
 * @return - 1, if there is still data to read. 
 *         - 0, if no more data exists.
 */
extern int http_input_stream_is_ready(struct http_input_stream_t * stream);

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
extern int http_input_stream_read(struct http_input_stream_t * stream, unsigned char *dest, int size);

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
extern struct http_output_stream_t *http_output_stream_new(struct hsocket_t *sock, hpair_t * header);


/** This function frees the given output stream. Note that this
 * function will not close any socket connections.
 *
 * @param stream The stream to free.
 */
extern void http_output_stream_free(struct http_output_stream_t *stream);

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
extern herror_t http_output_stream_write(struct http_output_stream_t *stream, const unsigned char *bytes, int size);

/** This function writes a null terminated string to the stream.
 *
 * @param stream the stream to use to send data
 * @param str a null terminated string to send
 *
 * @return H_OK on success. One of the followings otherwise
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t http_output_stream_write_string(struct http_output_stream_t *stream, const char *str);

/** This function sends finish flags if nesseccary (like in chunked
 * transport). Call always this function before closing the connections.
 *
 * @param stream the stream to send post data.
 *
 * @return H_OK on success. One of the followings otherwise
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t http_output_stream_flush(struct http_output_stream_t *stream);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif