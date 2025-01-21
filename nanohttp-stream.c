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
/** @file nanohttp-stream.c Stream handling */
/******************************************************************
*  $Id: nanohttp-stream.c,v 1.21 2007/11/04 08:41:20 m0gg Exp $
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
#include "nanohttp-config.h"
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-header.h"
#include "nanohttp-system.h"
#include "nanohttp-file.h"

static int
_http_stream_has_content_length(ng_list_head_s *header)
{
  return hpairnode_get_ignore_case(header, 
    __HDR_BUF(HEADER_CONTENT_LENGTH)) != NULL;
}

static int
_http_stream_is_chunked(ng_list_head_s *header)
{
  hpair_s *chunked;

  chunked = hpairnode_get_ignore_case(header, 
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

void
http_input_stream_free(http_input_stream_s *stream)
{
  if (stream->type == HTTP_TRANSFER_FILE && stream->fd != NULL)
  {
    nanohttp_file_close(stream->fd);
    if (stream->deleteOnExit)
    {
      log_info("Removing '%pS'", &stream->filename);
      nanohttp_file_delete(stream->filename.cptr, stream->filename.len); 
    }
  }
  ng_free_data_block(&stream->filename);
  herror_release(stream->err);
  ng_free(stream);
}

static int
_http_input_stream_is_content_length_ready(
  http_input_stream_s *stream)
{
  return (stream->content_length > stream->received);
}

static int
_http_input_stream_is_chunked_ready(http_input_stream_s *stream)
{
  return stream->chunk_size != 0;
}

static int
_http_input_stream_is_connection_closed_ready(
  http_input_stream_s *stream)
{
  return !stream->connection_closed;
}

static int
_http_input_stream_is_file_ready(http_input_stream_s *stream)
{
  return !nanohttp_file_iseof(stream->fd);
}

static ng_size_t
_http_input_stream_content_length_read(
  http_input_stream_s *stream, unsigned char *dest, ng_size_t size)
{
  herror_t status;
  ng_size_t read;

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
_http_input_stream_chunked_read_chunk_size(http_input_stream_s *stream)
{
  char chunk[25];
  ng_size_t status, i = 0;
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
      herror_log(err);
      log_error("hsocket_recv failed, %d:%s.", herror_code(err), herror_func(err));
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
                                 "Wrong chunk-size: %.*s.", i, chunk);
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
                   STREAM_ERROR_NO_CHUNK_SIZE, "reached max line == %d.", i);
      return -1;
    }
    else
      i++;
  }

  /* this should never happen */
  stream->err =
    herror_new("_http_input_stream_chunked_read_chunk_size",
               STREAM_ERROR_NO_CHUNK_SIZE, "reached max line == %d.", i);
  return -1;
}

static ng_size_t
_http_input_stream_chunked_read(http_input_stream_s *stream, 
  unsigned char *dest, ng_size_t size)
{
  ng_size_t status, counter;
  ng_size_t remain, read = 0;
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
        err = hsocket_recv(stream->sock, (unsigned char *)&ch, 1, 1, &status);
        if (err != H_OK)
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
                                   "Wrong chunk-size.");
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


static ng_size_t
_http_input_stream_connection_closed_read(
  http_input_stream_s *stream, unsigned char *dest, ng_size_t size)
{
  ng_size_t status;
  herror_t err;

  /* read from socket */
  err = hsocket_recv(stream->sock, dest, size, 0, &status);
  if (err != H_OK)
  {
    stream->err = err;
    return -1;
  }

  if (status == 0)
    stream->connection_closed = 1;

  stream->received += status;
  return status;
}

static ng_size_t
_http_input_stream_file_read(http_input_stream_s *stream, 
  unsigned char *dest, ng_size_t size)
{
  ng_size_t len = nanohttp_file_read_tobuffer(stream->fd, dest, size);

  if (len< 0)
  {
    stream->err = herror_new("_http_input_stream_file_read",
                    FILE_ERROR_READ, "fread() returned -1.");
    return -1;
  }

  return len;
}

/**
  Creates a new input stream. 
*/
http_input_stream_s *
http_input_stream_new(hsocket_s *sock, ng_list_head_s *header, 
  ng_buffer_s *data)
{
  http_input_stream_s *result;

  if (!(result = (http_input_stream_s *) 
    ng_malloc(sizeof(http_input_stream_s))))
  {
    log_error("ng_malloc failed %m.", ng_errno);
    return NULL;
  }

  sock->data   = *data;
  result->sock = sock;
  result->err  = H_OK;
  ng_block_init(&result->filename);

  /* Find connection type */
  hpairnode_dump_deep(header);
  
  /* Check if Content-type */
  if (_http_stream_has_content_length(header))
  {
    hpair_s *cl;
    log_verbose("Stream transfer with 'Content-length'.");
    cl = hpairnode_get_ignore_case(header, __HDR_BUF(HEADER_CONTENT_LENGTH));
    assert(cl != NULL);
    result->content_length = ng_atoi(cl->val.cptr, cl->val.len);
    result->received       = 0;
    result->type           = HTTP_TRANSFER_CONTENT_LENGTH;
    result->stream_ready   = _http_input_stream_is_content_length_ready;
    result->stream_read    = _http_input_stream_content_length_read;
  }
  /* Check if Chunked */
  else if (_http_stream_is_chunked(header))
  {
    log_verbose("Stream transfer with 'chunked'.");
    result->type           = HTTP_TRANSFER_CHUNKED;
    result->chunk_size     = -1;
    result->received       = -1;
    result->stream_ready   = _http_input_stream_is_chunked_ready;
    result->stream_read    = _http_input_stream_chunked_read;
  }
  /* Assume connection close */
  else
  {
    log_verbose("Stream transfer with 'Connection: close'.");
    result->type              = HTTP_TRANSFER_CONNECTION_CLOSE;
    result->connection_closed = 0;
    result->received          = 0;
    result->stream_ready      = _http_input_stream_is_connection_closed_ready;
    result->stream_read       = _http_input_stream_connection_closed_read;
  }
  
  return result;
}

/**
  Creates a new input stream from file. 
  This function was added for MIME messages 
  and for debugging.
*/
http_input_stream_s *
http_input_stream_new_from_file(const char *filename,
  int len)
{
  http_input_stream_s *result;
  void *fd;
  
  fd = nanohttp_file_open_for_read(filename, len);
  if (fd == NULL) {

    log_error("fopen failed %m.", ng_errno);
    goto clean0;
  }

  /* Create object */
  result = (http_input_stream_s *) 
      ng_malloc(sizeof(http_input_stream_s));
  if (result == NULL) 
  {
    log_error("ng_malloc failed %m.", ng_errno);
    goto clean1;
  }

  result->type = HTTP_TRANSFER_FILE;
  result->fd = fd;
  result->err = H_OK;
  result->deleteOnExit = 0;
  result->stream_ready = _http_input_stream_is_file_ready;
  result->stream_read  = _http_input_stream_file_read;
  result->filename.data = ng_strdup_size(filename, len);
  if (result->filename.data == NULL)
  {
    log_error("ng_malloc failed %m.", ng_errno);
    goto clean2;
  }
  result->filename.len = len;
  
  return result;

clean2:
  ng_free(result);
clean1:
  nanohttp_file_close(fd);
clean0:
  return NULL;
}

/**
  Returns the actual status of the stream.
*/
int
http_input_stream_is_ready(http_input_stream_s *stream)
{
  /* paranoia check */
  if (stream == NULL)
    return 0;

  /* reset error flag */
  herror_release(stream->err);
  stream->err = H_OK;

  return stream->stream_ready(stream);
}

/**
  Returns the actual read bytes
  <0 on error
*/
ng_size_t
http_input_stream_read(http_input_stream_s *stream, 
  unsigned char *dest, ng_size_t size)
{
  /* paranoia check */
  if (stream == NULL)
    return -1;

  /* reset error flag */
  herror_release(stream->err);
  stream->err = H_OK;

  return stream->stream_read(stream, dest, size);
}

/**
  Creates a new output stream. Transfer code will be found from header.
*/
http_output_stream_s *
http_output_stream_new(hsocket_s *sock, ng_list_head_s *header)
{
  http_output_stream_s *result;

  /* Create object */
  result = (http_output_stream_s *)ng_malloc(sizeof(*result));
  if (result == NULL)
  {
    log_error("ng_malloc failed %m.", ng_errno);
    return NULL;
  }

  result->sock   = sock;
  result->sent   = 0;
  result->status = H_OK;

  /* Find connection type */

  /* Check if Content-type */
  if (_http_stream_has_content_length(header))
  {
    hpair_s *cl;
    log_verbose("Stream transfer with 'Content-length'.");
    cl = hpairnode_get_ignore_case(header, 
      __HDR_BUF(HEADER_CONTENT_LENGTH));
    assert(cl != NULL);
    result->content_length = ng_atoi(cl->val.cptr, cl->val.len);
    result->type = HTTP_TRANSFER_CONTENT_LENGTH;
  }
  /* Check if Chunked */
  else if (_http_stream_is_chunked(header))
  {
    log_verbose("Stream transfer with 'chunked'.");
    result->type = HTTP_TRANSFER_CHUNKED;
  }
  /* Assume connection close */
  else
  {
    log_verbose("Stream transfer with 'Connection: close'.");
    result->type = HTTP_TRANSFER_CONNECTION_CLOSE;
  }

  return result;
}

/**
  Free output stream
*/
void
http_output_stream_free(http_output_stream_s *stream)
{
  herror_release(stream->status);
  ng_free(stream);
  return;
}

/**
  Writes 'size' bytes of 'bytes' into stream.
  Returns socket error flags or H_OK.
*/
herror_t
http_output_stream_write(http_output_stream_s *stream,
                         const unsigned char *bytes, ng_size_t size)
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
http_output_stream_write_string(http_output_stream_s *stream,
                                const char *str)
{
  return http_output_stream_write(stream, (const unsigned char *)str, strlen(str));
}

static int
__http_snprintf_out(void *arg, const char *string, ng_size_t length)
{
  http_output_stream_s *stream = (http_output_stream_s *)arg;
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
http_output_stream_write_printf(http_output_stream_s *stream, 
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
http_output_stream_flush(http_output_stream_s *stream)
{
  herror_t status;

  if (stream->type == HTTP_TRANSFER_CHUNKED)
  {
	status = hsocket_send(stream->sock, (const unsigned char *)"0\r\n\r\n", 5);
    if (status != H_OK)
      return status;
  }

  return H_OK;
}
