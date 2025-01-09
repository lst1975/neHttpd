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
/** @file nanohttp-stream.c Stream handling */
/******************************************************************
*  $Id: nanohttp-stream.c,v 1.21 2007/11/04 08:41:20 m0gg Exp $
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
#include "nanohttp-config.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <stdarg.h>

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-header.h"

static int
_http_stream_has_content_length(hpair_t * header)
{
  return hpairnode_get_ignore_case_len(header, 
    __HDR_BUF(HEADER_CONTENT_LENGTH)) != NULL;
}

static int
_http_stream_is_chunked(hpair_t * header)
{
  hpair_t *chunked;

  chunked = hpairnode_get_ignore_case_len(header, 
    __HDR_BUF(HEADER_TRANSFER_ENCODING));
  if (chunked != NULL)
  {
    if (ng_block_isequal(&chunked->val, TRANSFER_ENCODING_CHUNKED, 
      sizeof(TRANSFER_ENCODING_CHUNKED)-1))
    {
      return 1;
    }
  }
  return 0;
}

/**
  Creates a new input stream. 
*/
struct http_input_stream_t *
http_input_stream_new(struct hsocket_t *sock, hpair_t *header)
{
  struct http_input_stream_t *result;

  if (!(result = (struct http_input_stream_t *) 
    http_malloc(sizeof(struct http_input_stream_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  result->sock = sock;
  result->err = H_OK;

  /* Find connection type */
  hpairnode_dump_deep(header);
  
  /* Check if Content-type */
  if (_http_stream_has_content_length(header))
  {
    hpair_t *cl;
    log_verbose("Stream transfer with 'Content-length'");
    cl = hpairnode_get_ignore_case_len(header, 
      __HDR_BUF(HEADER_CONTENT_LENGTH));
    assert(cl != NULL);
    result->content_length = ng_atoi(cl->val.cptr, cl->val.len);
    result->received = 0;
    result->type = HTTP_TRANSFER_CONTENT_LENGTH;
  }
  /* Check if Chunked */
  else if (_http_stream_is_chunked(header))
  {
    log_verbose("Stream transfer with 'chunked'");
    result->type = HTTP_TRANSFER_CHUNKED;
    result->chunk_size = -1;
    result->received = -1;
  }
  /* Assume connection close */
  else
  {
    log_verbose("Stream transfer with 'Connection: close'");
    result->type = HTTP_TRANSFER_CONNECTION_CLOSE;
    result->connection_closed = 0;
    result->received = 0;
  }
  return result;
}

/**
  Creates a new input stream from file. 
  This function was added for MIME messages 
  and for debugging.
*/
struct http_input_stream_t *
http_input_stream_new_from_file(const char *filename)
{
  struct http_input_stream_t *result;
  FILE *fd;
 
  if (!(fd = fopen(filename, "rb"))) {

    log_error("fopen failed (%s)", strerror(errno));
    return NULL;
  }

  /* Create object */
  if (!(result = (struct http_input_stream_t *) 
      http_malloc(sizeof(struct http_input_stream_t)))) 
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    fclose(fd);
    return NULL;
  }

  result->type = HTTP_TRANSFER_FILE;
  result->fd = fd;
  result->err = H_OK;
  result->deleteOnExit = 0;
  strcpy(result->filename, filename);

  return result;
}

void
http_input_stream_free(struct http_input_stream_t * stream)
{
  if (stream->type == HTTP_TRANSFER_FILE && stream->fd)
  {
    fclose(stream->fd);
    if (stream->deleteOnExit)
      log_info("Removing '%s'", stream->filename);
    /* remove(stream->filename); */
  }

  herror_release(stream->err);
  http_free(stream);
}

static int
_http_input_stream_is_content_length_ready(
  struct http_input_stream_t *stream)
{
  return (stream->content_length > stream->received);
}

static int
_http_input_stream_is_chunked_ready(struct http_input_stream_t * stream)
{
  return stream->chunk_size != 0;
}

static int
_http_input_stream_is_connection_closed_ready(
  struct http_input_stream_t * stream)
{
  return !stream->connection_closed;
}

static int
_http_input_stream_is_file_ready(struct http_input_stream_t * stream)
{
  return !feof(stream->fd);
}

static int
_http_input_stream_content_length_read(
  struct http_input_stream_t *stream, unsigned char *dest, size_t size)
{
  herror_t status;
  size_t read;

  /* check limit */
  if (stream->content_length - stream->received < size)
    size = stream->content_length - stream->received;

  /* read from socket */
  if ((status = hsocket_recv(stream->sock, dest, size, 1, &read)) != H_OK)
  {
    stream->err = status;
    return -1;
  }

  stream->received += read;
  return read;
}

static int
_http_input_stream_chunked_read_chunk_size(struct http_input_stream_t * stream)
{
  char chunk[25];
  size_t status, i = 0;
  int chunk_size;
  herror_t err;

  while (1)
  {
    err = hsocket_recv(stream->sock, (unsigned char *)&(chunk[i]), 1, 1, &status);
    if (status != 1)
    {
      stream->err = herror_new("_http_input_stream_chunked_read_chunk_size",
                               GENERAL_INVALID_PARAM,
                               "This should never happen!");
      return -1;
    }

    if (err != H_OK)
    {
      log_error("[%d] %s(): %s ", herror_code(err), herror_func(err),
                 herror_message(err));

      stream->err = err;
      return -1;
    }

    if (chunk[i] == '\r' || chunk[i] == ';')
    {
      chunk[i] = '\0';
    }
    else if (chunk[i] == '\n')
    {
      const char *end;
      chunk[i] = '\0';          /* double check */
      chunk_size = ng_htou64(chunk, i, &end);   /* hex to dec */
      if (end != chunk + i)
      {
        stream->err = herror_new("_http_input_stream_chunked_read_chunk_size",
                                 STREAM_ERROR_WRONG_CHUNK_SIZE,
                                 "Wrong chunk-size: %.*s", i, chunk);
        return -1;
      }
      /* 
         log_verbose("chunk_size: '%s' as dec: '%d'", chunk, chunk_size); */
      return chunk_size;
    }

    if (i == 24)
    {
      stream->err =
        herror_new("_http_input_stream_chunked_read_chunk_size",
                   STREAM_ERROR_NO_CHUNK_SIZE, "reached max line == %d", i);
      return -1;
    }
    else
      i++;
  }

  /* this should never happen */
  stream->err =
    herror_new("_http_input_stream_chunked_read_chunk_size",
               STREAM_ERROR_NO_CHUNK_SIZE, "reached max line == %d", i);
  return -1;
}

static int
_http_input_stream_chunked_read(struct http_input_stream_t * stream, unsigned char *dest,
                                int size)
{
  size_t status, counter;
  size_t remain, read = 0;
  char ch;
  herror_t err;

  while (size > 0)
  {
    remain = stream->chunk_size - stream->received;

    if (remain == 0 && stream->chunk_size != -1)
    {
      /* This is not the first chunk. so skip new line until chunk size
         string */
      counter = 100;            /* maximum for stop infinity */
      while (1)
      {
        if ((err = hsocket_recv(stream->sock, (unsigned char *)&ch, 1, 1, &status)) != H_OK)
        {
          stream->err = err;
          return -1;
        }

        if (ch == '\n')
        {
          break;
        }
        if (counter-- == 0)
        {
          stream->err = herror_new("_http_input_stream_chunked_read",
                                   STREAM_ERROR_WRONG_CHUNK_SIZE,
                                   "Wrong chunk-size");
          return -1;
        }
      }
    }

    if (remain == 0)
    {
      /* receive new chunk size */
      stream->chunk_size = _http_input_stream_chunked_read_chunk_size(stream);
      stream->received = 0;

      if (stream->chunk_size < 0)
      {
        /* TODO (#1#): set error flag */
        return stream->chunk_size;
      }
      else if (stream->chunk_size == 0)
      {
        return read;
      }
      remain = stream->chunk_size;
    }

    /* show remaining chunk size in socket */
    if (remain < size)
    {
      /* read from socket */
      if ((err = hsocket_recv(stream->sock, &(dest[read]), remain, 1, &status)) != H_OK)
      {
        stream->err = err;
        return -1;
      }
      if (status != remain)
      {
        stream->err = herror_new("_http_input_stream_chunked_read",
                                 GENERAL_INVALID_PARAM,
                                 "This should never happen (remain=%d)(status=%d)!",
                                 remain, status);
        return -1;
      }
    }
    else
    {
      /* read from socket */
      err = hsocket_recv(stream->sock, &(dest[read]), size, 1, &status);
      if (status != size)
      {
        stream->err = herror_new("_http_input_stream_chunked_read",
                                 GENERAL_INVALID_PARAM,
                                 "This should never happen (size=%d)(status=%d)!",
                                 size, status);
        return -1;
      }
      if (err != H_OK)
      {
        stream->err = err;
        return -1;
      }
    }

    read += status;
    size -= status;
    stream->received += status;
  }

  return read;
}


static int
_http_input_stream_connection_closed_read(struct http_input_stream_t *stream, 
  unsigned char *dest, size_t size)
{
  size_t status;
  herror_t err;

  /* read from socket */
  if ((err = hsocket_recv(stream->sock, dest, size, 0, &status)) != H_OK)
  {
    stream->err = err;
    return -1;
  }

  if (status == 0)
    stream->connection_closed = 1;

  stream->received += status;
  return status;
}

static int
_http_input_stream_file_read(struct http_input_stream_t * stream, unsigned char *dest, int size)
{
  size_t len;

  if ((len = fread(dest, 1, size, stream->fd)) == -1)
  {
    stream->err = herror_new("_http_input_stream_file_read",
                             HSOCKET_ERROR_RECEIVE, "fread() returned -1");
    return -1;
  }

  return len;
}

/**
  Returns the actual status of the stream.
*/
int
http_input_stream_is_ready(struct http_input_stream_t * stream)
{
  /* paranoia check */
  if (stream == NULL)
    return 0;

  /* reset error flag */
  if (stream->err)
    herror_release(stream->err);
  stream->err = H_OK;

  switch (stream->type)
  {
  case HTTP_TRANSFER_CONTENT_LENGTH:
    return _http_input_stream_is_content_length_ready(stream);
  case HTTP_TRANSFER_CHUNKED:
    return _http_input_stream_is_chunked_ready(stream);
  case HTTP_TRANSFER_CONNECTION_CLOSE:
    return _http_input_stream_is_connection_closed_ready(stream);
  case HTTP_TRANSFER_FILE:
    return _http_input_stream_is_file_ready(stream);
  default:
    return 0;
  }
}

/**
  Returns the actual read bytes
  <0 on error
*/
size_t
http_input_stream_read(struct http_input_stream_t * stream, 
  unsigned char *dest, size_t size)
{
  size_t len = 0;

  /* paranoia check */
  if (stream == NULL)
    return -1;

  /* reset error flag */
  if (stream->err)
    herror_release(stream->err);
  stream->err = H_OK;

  switch (stream->type)
  {
  case HTTP_TRANSFER_CONTENT_LENGTH:
    len = _http_input_stream_content_length_read(stream, dest, size);
    break;
  case HTTP_TRANSFER_CHUNKED:
    len = _http_input_stream_chunked_read(stream, dest, size);
    break;
  case HTTP_TRANSFER_CONNECTION_CLOSE:
    len = _http_input_stream_connection_closed_read(stream, dest, size);
    break;
  case HTTP_TRANSFER_FILE:
    len = _http_input_stream_file_read(stream, dest, size);
    break;
  default:
    stream->err = herror_new("http_input_stream_read",
                             STREAM_ERROR_INVALID_TYPE,
                             "%d is invalid stream type", stream->type);
    return -1;
  }
  return len;
}

/**
  Creates a new output stream. Transfer code will be found from header.
*/
struct http_output_stream_t *
http_output_stream_new(struct hsocket_t *sock, hpair_t * header)
{
  struct http_output_stream_t *result;

  /* Create object */
  result = (struct http_output_stream_t *)http_malloc(sizeof(*result));
  if (result == NULL)
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  result->sock   = sock;
  result->sent   = 0;
  result->status = H_OK;

  /* Find connection type */

  /* Check if Content-type */
  if (_http_stream_has_content_length(header))
  {
    hpair_t *cl;
    log_verbose("Stream transfer with 'Content-length'");
    cl = hpairnode_get_ignore_case_len(header, 
      __HDR_BUF(HEADER_CONTENT_LENGTH));
    assert(cl != NULL);
    result->content_length = ng_atoi(cl->val.cptr, cl->val.len);
    result->type = HTTP_TRANSFER_CONTENT_LENGTH;
  }
  /* Check if Chunked */
  else if (_http_stream_is_chunked(header))
  {
    log_verbose("Stream transfer with 'chunked'");
    result->type = HTTP_TRANSFER_CHUNKED;
  }
  /* Assume connection close */
  else
  {
    log_verbose("Stream transfer with 'Connection: close'");
    result->type = HTTP_TRANSFER_CONNECTION_CLOSE;
  }

  return result;
}

/**
  Free output stream
*/
void
http_output_stream_free(struct http_output_stream_t * stream)
{
  herror_release(stream->status);
  http_free(stream);
  return;
}

/**
  Writes 'size' bytes of 'bytes' into stream.
  Returns socket error flags or H_OK.
*/
herror_t
http_output_stream_write(struct http_output_stream_t * stream,
                         const unsigned char *bytes, size_t size)
{
  herror_t status;

  if (stream->type == HTTP_TRANSFER_CHUNKED)
  {
    char chunked[24];
    int n = ng_u64toh(size, chunked, sizeof(chunked)-1, 0, 0);
    if ((status = hsocket_send(stream->sock, (const unsigned char *)chunked, n)) != H_OK)
      return status;
  }

  if (size > 0)
  {
    if ((status = hsocket_send(stream->sock, bytes, size)) != H_OK)
      return status;
  }

  if (stream->type == HTTP_TRANSFER_CHUNKED)
  {
    if ((status = hsocket_send(stream->sock, (const unsigned char *)"\r\n", 2)) != H_OK)
      return status;
  }

  return H_OK;
}

/**
  Writes 'strlen()' bytes of 'str' into stream.
  Returns socket error flags or H_OK.
*/
herror_t
http_output_stream_write_string(struct http_output_stream_t * stream,
                                const char *str)
{
  return http_output_stream_write(stream, (const unsigned char *)str, strlen(str));
}

static int
__http_snprintf_out(void *arg, const char *string, size_t length)
{
  struct http_output_stream_t *stream = (struct http_output_stream_t *)arg;
  herror_t status;
  status = http_output_stream_write(stream, (const unsigned char *)string, length);
  if (status != H_OK)
  {
    stream->status = status;
    return -1;
  }
  return length;
}

/**
  Writes 'strlen()' bytes of 'str' into stream.
  Returns socket error flags or H_OK.
*/
herror_t
http_output_stream_write_printf(struct http_output_stream_t *stream, 
  const char *format, ...)
{
  int n;
  va_list ap;

  va_start(ap, format);
  n = __ng_vsnprintf_cb(__http_snprintf_out, stream, format, ap);
  va_end(ap);

  if (n < 0)
  {
    herror_t status = stream->status;
    if (status == H_OK)
    {
      status = herror_new("http_output_stream_write_printf",
                 GENERAL_ERROR,
                 "__ng_vsnprintf_cb error.");
    }
    else
    {
      stream->status = NULL;
    }
    
    return status;
  }
  
  return H_OK;
}

herror_t
http_output_stream_flush(struct http_output_stream_t *stream)
{
  herror_t status;

  if (stream->type == HTTP_TRANSFER_CHUNKED)
  {
    if ((status = hsocket_send(stream->sock, (const unsigned char *)"0\r\n\r\n", 5)) != H_OK)
      return status;
  }

  return H_OK;
}
