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
/** @file nanohttp-mime.c MIME handling */
/******************************************************************
*  _  _   _   _  _   __
* | \/ | | | | \/ | | _/
* |_''_| |_| |_''_| |_'/  PARSER
*
*  $Id: nanohttp-mime.c,v 1.20 2007/11/03 22:40:11 m0gg Exp $
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

/*----------------------------------------------------------------
Buffered Reader. A helper object to read bytes from a source
----------------------------------------------------------------*/

#include "nanohttp-defs.h"
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-mime.h"
#include "nanohttp-header.h"

/* ------------------------------------------------------------------
  MIME Parser
 ------------------------------------------------------------------*/
typedef void (*MIME_part_begin) (void *);
typedef void (*MIME_part_end) (void *);
typedef void (*MIME_parse_begin) (void *);
typedef void (*MIME_parse_end) (void *);
typedef void (*MIME_ERROR_bytes) (void *, const unsigned char *, int);

typedef enum _MIME_parser_status
{
  MIME_PARSER_INCOMPLETE_MESSAGE,
  MIME_PARSER_READ_ERROR,
  MIME_PARSER_OK
} MIME_parser_status;

typedef enum _MIME_read_status
{
  MIME_READ_OK,
  MIME_READ_EOF,
  MIME_READ_ERROR
} MIME_read_status;

#define MIME_READER_MAX_BUFFER_SIZE  1054
#define MIME_PARSER_BUFFER_SIZE 1054


typedef MIME_read_status(*MIME_read_function) (void *, unsigned char *, int *);


/**
  Reader structure. This will be use 
  by the parser
*/
typedef struct _MIME_reader
{
  int size;
  int marker;
  int current;
  MIME_read_function read_function;
  char buffer[MIME_READER_MAX_BUFFER_SIZE];
  void *userdata;
} MIME_reader;


MIME_read_status MIME_filereader_function(void *userdata,
                                          unsigned char *dest, int *size);

typedef struct _MIME_callbacks
{
  MIME_part_begin part_begin_cb;
  MIME_part_end part_end_cb;
  MIME_parse_begin parse_begin_cb;
  MIME_parse_end parse_end_cb;
  MIME_ERROR_bytes received_bytes_cb;
} MIME_callbacks;


MIME_parser_status MIME_parse(MIME_read_function reader_function,
                              void *reader_userdata,
                              const char *user_boundary,
                              const MIME_callbacks * callbacks,
                              void *callbacks_userdata);


/**
  Initialize a reader
*/
void
MIME_reader_init(MIME_reader * reader,
                 MIME_read_function reader_function, void *userdata)
{
  reader->size = 0;
  reader->marker = -1;
  reader->current = 0;
  reader->userdata = userdata;
  reader->read_function = reader_function;

}

/**
  Read data from a reader source. 
*/
MIME_read_status
MIME_reader_read(MIME_reader * reader, unsigned char *buffer, int size)
{
  MIME_read_status status;
  int len;
  int rest_size;

  /* Check if buffer is full */
  if (reader->size == reader->current)
  {
    /* Yes, so read some data */

    /* First handle marker */
    if (reader->marker > -1)
    {
      if (reader->marker != 0)
      {
        reader->current = reader->size - reader->marker;
        memcpy(reader->buffer, reader->buffer + reader->marker, reader->current);
      }
      else if (reader->current == MIME_READER_MAX_BUFFER_SIZE - 1)
      {
        log_error("%s", "Marker error");
        return MIME_READ_ERROR;
      }
      reader->marker = 0;
    }
    else
      reader->current = 0;

    len = MIME_READER_MAX_BUFFER_SIZE - reader->current - 1;
    status = reader->read_function(reader->userdata,
                                   (unsigned char *)reader->buffer + reader->current,
                                   &len);

    if (status == MIME_READ_OK)
    {
      reader->size = len + reader->current;
    }
    else
      return status;
  }

  if (size <= reader->size - reader->current)
  {
    memcpy(buffer, reader->buffer + reader->current, size);
    reader->current += size;
    return MIME_READ_OK;
  }
  else
  {
    /* Fill rest data */
    rest_size = reader->size - reader->current;
    memcpy(buffer, reader->buffer + reader->current, (unsigned int)rest_size);

    reader->current = reader->size;
    return MIME_reader_read(reader, buffer + rest_size, size - rest_size);
  }
}


void
MIME_reader_set_marker(MIME_reader * reader)
{
  reader->marker = reader->current;
}

void
MIME_reader_unset_marker(MIME_reader * reader)
{
  reader->marker = -1;
}

void
MIME_reader_jump_marker(MIME_reader * reader)
{
  reader->current = reader->marker;
}



typedef struct _MIME_buffer
{
  unsigned char data[MIME_PARSER_BUFFER_SIZE];
  int size;
} MIME_buffer;


void
MIME_buffer_init(MIME_buffer * buffer)
{
  buffer->size = 0;
}

void
MIME_buffer_add(MIME_buffer * buffer, unsigned char ch)
{
  buffer->data[buffer->size++] = ch;
}

void
MIME_buffer_add_bytes(MIME_buffer * buffer, unsigned char *bytes, int size)
{
  memcpy(buffer->data, bytes, size);
  buffer->size += size;
}

int
MIME_buffer_is_full(MIME_buffer * buffer)
{
  return buffer->size + 150 >= MIME_PARSER_BUFFER_SIZE;
}

int
MIME_buffer_is_empty(MIME_buffer * buffer)
{
  return buffer->size == 0;
}

void
MIME_buffer_clear(MIME_buffer * buffer)
{
  buffer->size = 0;
}


MIME_parser_status
MIME_parse(MIME_read_function reader_function,
           void *reader_userdata,
           const char *user_boundary,
           const MIME_callbacks * callbacks, void *callbacks_userdata)
{
  char boundary[150];
  unsigned char ch[153];
  int boundary_length, n, ignore = 0;
  MIME_reader reader;
  MIME_buffer buffer;
  MIME_read_status status;

  /* Init reader */
  MIME_reader_init(&reader, reader_function, reader_userdata);

  /* Init buffer */
  MIME_buffer_init(&buffer);

  /* Set boundary related stuff */
  ng_snprintf(boundary, sizeof(boundary), "\n--%s", user_boundary);
  boundary_length = strlen(boundary);

  /* Call parse begin callback */
  callbacks->parse_begin_cb(callbacks_userdata);

  while (1)
  {
  set_marker:

    /* Set marker */
    MIME_reader_set_marker(&reader);

  read_byte:

    /* Read 1 byte */
    status = MIME_reader_read(&reader, ch, 1);
    if (status == MIME_READ_EOF)
      return MIME_PARSER_INCOMPLETE_MESSAGE;
    else if (status == MIME_READ_ERROR)
      return MIME_PARSER_READ_ERROR;

    if (ch[0] == '\r' && !ignore)
    {
      n = 0;
      while (n < boundary_length)
      {
        /* Read 1 byte */
        status = MIME_reader_read(&reader, ch, 1);
        if (status == MIME_READ_EOF)
          return MIME_PARSER_INCOMPLETE_MESSAGE;
        else if (status == MIME_READ_ERROR)
          return MIME_PARSER_READ_ERROR;

        /* Check if byte is in boundary */
        if (ch[0] == boundary[n])
        {
          n = n + 1;
          continue;
        }
        else
        {
          MIME_reader_jump_marker(&reader);
          ignore = 1;
          goto read_byte;
        }

      }                         /* while n < boundary_length */

      /* Read 1 byte */
      status = MIME_reader_read(&reader, ch, 1);


      if (status == MIME_READ_EOF)
        return MIME_PARSER_INCOMPLETE_MESSAGE;
      else if (status == MIME_READ_ERROR)
        return MIME_PARSER_READ_ERROR;

      /* Show if byte is '\r' */
      if (ch[0] == '\r')
      {
        /* Read 1 byte */
        status = MIME_reader_read(&reader, ch, 1);


        if (status == MIME_READ_EOF)
          return MIME_PARSER_INCOMPLETE_MESSAGE;
        else if (status == MIME_READ_ERROR)
          return MIME_PARSER_READ_ERROR;

        /* Check if byte is '\n' */
        if (ch[0] == '\n')
        {
          if (!MIME_buffer_is_empty(&buffer))
          {
            /* Invoke callback */
            callbacks->received_bytes_cb(callbacks_userdata, buffer.data,
                                         buffer.size);

            /* Empty the buffer */
            MIME_buffer_clear(&buffer);

            /* Invoke End Part */
            callbacks->part_end_cb(callbacks_userdata);
          }

          /* Invoke start new Part */
          callbacks->part_begin_cb(callbacks_userdata);
          goto set_marker;

        }                       /* if (ch[0] == '\n') */
        else
        {
          /* Jump to marker and read bytes */
          MIME_reader_jump_marker(&reader);
          MIME_reader_read(&reader, ch, boundary_length + 2);

          MIME_buffer_add_bytes(&buffer, ch, boundary_length + 2);

          if (MIME_buffer_is_full(&buffer))
          {
            /* Invoke callback */
            callbacks->received_bytes_cb(callbacks_userdata, buffer.data,
                                         buffer.size);


            /* Empty the buffer */
            MIME_buffer_clear(&buffer);
          }
        }                       /* else of if (ch[0] == '\n') */

      }                         /* if (ch[0] == '\r') */
      else
      {
        if (ch[0] == '-')
        {
          /* Read 1 byte */
          status = MIME_reader_read(&reader, ch, 1);

          if (status == MIME_READ_EOF)
            return MIME_PARSER_INCOMPLETE_MESSAGE;
          else if (status == MIME_READ_ERROR)
            return MIME_PARSER_READ_ERROR;

          if (ch[0] == '-')
          {
            if (!MIME_buffer_is_empty(&buffer))
            {
              /* Invoke callback */
              callbacks->received_bytes_cb(callbacks_userdata, buffer.data,
                                           buffer.size);

              /* Empty the buffer */
              MIME_buffer_clear(&buffer);

              /* Invoke End Part */
              callbacks->part_end_cb(callbacks_userdata);
            }

            /* Invoke start new Part */
            callbacks->parse_end_cb(callbacks_userdata);

            /* Finish parsing */
            /* TODO (#1#): We assume that after -- comes \r\n This is not
               always correct */

            return MIME_PARSER_OK;

          }                     /* if (ch[0] == '-') */
          else
          {
            MIME_reader_jump_marker(&reader);
            ignore = 1;
            goto read_byte;
          }                     /* else of if (ch[0] == '-') */

        }                       /* if (ch[0] == '-') */
        else
        {
          MIME_reader_jump_marker(&reader);
          ignore = 1;
          goto read_byte;
        }                       /* else of if (ch[0] == '-') */

      }                         /* else of if (ch[0] == '\r') */

    }                           /* if ch[0] == '\r' && !ignore */
    else
    {
      ignore = 0;
      MIME_buffer_add(&buffer, ch[0]);

      /* Chec if buffer is full */
      if (MIME_buffer_is_full(&buffer))
      {
        /* Invoke callback */
        callbacks->received_bytes_cb(callbacks_userdata, buffer.data,
                                     buffer.size);

        /* Empty the buffer */
        MIME_buffer_clear(&buffer);
      }
    }                           /* else of if ch[0] == '\r' && !ignore */
  }                             /* while (1) */
}

MIME_read_status
MIME_filereader_function(void *userdata, unsigned char *dest, int *size)
{
  FILE *f = (FILE *) userdata;

  if (feof(f))
    return MIME_READ_EOF;

  *size = fread(dest, 1, *size, f);
  return MIME_READ_OK;
}


/* ------------------------------------------------------------------
  "multipart/related"  MIME Message Builder
 ------------------------------------------------------------------*/

/*
  Callback data to use internally
*/
typedef struct _mime_callback_data
{
  int part_id;
  struct attachments_t *message;
  struct part_t *current_part;
  int buffer_capacity;
  char header[4064];
  char root_id[256];
  int header_index;
  int header_search;
  FILE *current_fd;
  char root_dir[512];
} mime_callback_data_t;


MIME_read_status
mime_streamreader_function(void *userdata, unsigned char *dest, int *size)
{
  size_t len = 0;
  struct http_input_stream_t *in = (struct http_input_stream_t *) userdata;

  if (!http_input_stream_is_ready(in))
    return MIME_READ_EOF;

  len = http_input_stream_read(in, dest, *size);
  /* 
     log_info("http_input_stream_read() returned 0"); */
  if (len == -1)
  {
    log_error("[%d] %s():%s ", herror_code(in->err), herror_func(in->err),
               herror_message(in->err));
  }

  *size = len;
  if (*size != -1)
  {
    return MIME_READ_OK;
  }
  return MIME_READ_ERROR;
}


/*
  Start Callback functions
*/
static void
_mime_parse_begin(void *data)
{
/* Nothing to do
  mime_callback_data_t *cbdata = (mime_callback_data_t)data;
 */
  log_verbose("Begin parse (%p)", data);

  return;
}


static void
_mime_parse_end(void *data)
{
/* Nothing to do
  mime_callback_data_t *cbdata = (mime_callback_data_t)data;
 */
  log_verbose("End parse (%p)", data);

  return;
}


static void
_mime_part_begin(void *data)
{
  char *buffer;
  struct part_t *part;
  mime_callback_data_t *cbdata;
  
#define _TMP_SIZE 1054
  buffer = (char *)http_malloc(_TMP_SIZE);
  if (buffer == NULL)
  {
    log_fatal("Failed to malloc temp buffer.");
    return;
  }

  cbdata = (mime_callback_data_t *) data;
  log_verbose("Begin Part (%p)", data);
  if (!(part = (struct part_t *) http_malloc(sizeof(struct part_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return;
  }
  part->next = NULL;


  if (cbdata->current_part)
    cbdata->current_part->next = part;

  cbdata->current_part = part;

  if (!cbdata->message->parts)
    cbdata->message->parts = part;

  cbdata->header[0] = '\0';
  cbdata->header_index = 0;
  cbdata->header_search = 0;

#ifdef WIN32
  ng_snprintf(buffer, _TMP_SIZE, "%s\\mime_%p_%d.part", cbdata->root_dir,
          cbdata, cbdata->part_id++);
#else
  ng_snprintf(buffer, _TMP_SIZE, "%s/mime_%p_%d.part", cbdata->root_dir,
          cbdata, cbdata->part_id++);
#endif
  http_free(buffer);

#undef _TMP_SIZE
  /*  log_info("Creating FILE ('%s') deleteOnExit=1", buffer);*/
  part->deleteOnExit = 1;
  cbdata->current_fd = fopen(buffer, "wb");
  if (cbdata->current_fd)
    strcpy(cbdata->current_part->filename, buffer);
  else
    log_error("Can not open file for write '%s'", buffer);
}


static void
_mime_part_end(void *data)
{
  mime_callback_data_t *cbdata = (mime_callback_data_t *) data;
  log_verbose("End Part (%p)", data);
  if (cbdata->current_fd)
  {
    fclose(cbdata->current_fd);
    cbdata->current_fd = NULL;
  }

  return;
}


static hpair_t *
_mime_process_header(char *buffer)
{
  int i = 0, c = 0, proc_key, begin = 0, key_len = 0;
  hpair_t *first = NULL, *last = NULL, *pair;
  char *key, *value;

#define __KV_SIZE 912
  key = (char *)http_malloc(__KV_SIZE*2+2);
  if (key == NULL)
  {
    log_fatal("Failed to malloc temp buffer.");
    goto clean0;
  }
  value = key + __KV_SIZE+1;
  key[0] = '\0';
  value[0] = '\0';
  proc_key = 1;

  while (buffer[i] != '\0')
  {
    if (buffer[i] == '\r' && buffer[i + 1] == '\n')
    {
      value[c%__KV_SIZE] = '\0';
      pair = hpairnode_new_len(key, key_len, value, c, NULL);
      if (pair == NULL)
      {
        log_fatal("Failed to hpairnode_new_len.");
        goto clean1;
      }
      if (last)
      {
        last->next = pair;
        last = last->next;
      }
      else
      {
        first = last = pair;
      }
      proc_key = 1;
      c = 0;
      i++;
    }
    else if (buffer[i] == ':')
    {
      key[c%__KV_SIZE] = '\0';
      key_len = c;
      c = 0;
      begin = 0;
      proc_key = 0;
    }
    else
    {
      if (proc_key)
      {
        key[c%__KV_SIZE] = buffer[i];
        c++;
      }
      else
      {
        if (buffer[i] != ' ')
          begin = 1;
        if (begin)
        {
          value[c%__KV_SIZE] = buffer[i];
          c++;
        }
      }
    }
    i++;
  }
#undef __KV_SIZE

  http_free(key);
  return first;
  
clean1:
  hpairnode_free_deep(first);
clean0:
  return NULL;
}


static void
_mime_received_bytes(void *data, const unsigned char *bytes, int size)
{
  int i = 0;
  mime_callback_data_t *cbdata = (mime_callback_data_t *) data;

  if (!cbdata)
  {
    log_error
      ("MIME transport error Called <received bytes> without initializing\n");
    return;
  }
  if (!cbdata->current_part)
  {
    log_error
      ("MIME transport error Called <received bytes> without initializing\n");
    return;
  }
/*  log_verbose("Received %d bytes (%p), header_search = %d", 
    size, data, cbdata->header_search);
*/
  if (cbdata->header_search < 4)
  {
    /* Find \r\n\r\n in bytes */
    for (i = 0; i < size; i++)
    {
      if (cbdata->header_search == 0)
      {
        if (bytes[i] == '\r')
          cbdata->header_search++;
        else
        {
          cbdata->header[cbdata->header_index++] = bytes[i];
          cbdata->header_search = 0;
        }
      }

      else if (cbdata->header_search == 1)
      {
        if (bytes[i] == '\n')
          cbdata->header_search++;
        else
        {
          cbdata->header[cbdata->header_index++] = '\r';
          cbdata->header[cbdata->header_index++] = bytes[i];
          cbdata->header_search = 0;
        }
      }

      else if (cbdata->header_search == 2)
      {
        if (bytes[i] == '\r')
          cbdata->header_search++;
        else
        {
          cbdata->header[cbdata->header_index++] = '\r';
          cbdata->header[cbdata->header_index++] = '\n';
          cbdata->header[cbdata->header_index++] = bytes[i];
          cbdata->header_search = 0;
        }
      }

      else if (cbdata->header_search == 3)
      {
        if (bytes[i] == '\n')
        {
          hpair_t *pair;
          cbdata->header[cbdata->header_index++] = '\r';
          cbdata->header[cbdata->header_index++] = '\n';
          cbdata->header[cbdata->header_index++] = '\0';
          cbdata->header_search = 4;
          cbdata->current_part->header = _mime_process_header(cbdata->header);
          hpairnode_dump_deep(cbdata->current_part->header);
          /* set id */
          pair = hpairnode_get_len(cbdata->current_part->header, 
                          __HDR_BUF(HEADER_CONTENT_ID));
          if (pair != NULL)
          {
            strcpy(cbdata->current_part->id, pair->value);
            if (!strcmp(pair->value, cbdata->root_id))
              cbdata->message->root_part = cbdata->current_part;
          }
          pair =
            hpairnode_get_len(cbdata->current_part->header,
                          __HDR_BUF(HEADER_CONTENT_LOCATION));
          if (pair != NULL)
          {
            strcpy(cbdata->current_part->location, pair->value);
          }
          pair =
            hpairnode_get_len(cbdata->current_part->header, 
                          __HDR_BUF(HEADER_CONTENT_TYPE));
          if (pair != NULL)
          {
            strcpy(cbdata->current_part->content_type, pair->value);
          }
          i++;
          break;
        }
        else
        {
          cbdata->header[cbdata->header_index++] = '\r';
          cbdata->header[cbdata->header_index++] = '\n';
          cbdata->header[cbdata->header_index++] = '\r';
          cbdata->header[cbdata->header_index++] = bytes[i];
          cbdata->header_search = 0;
        }
      }
      /* TODO (#1#): Check for cbdata->header overflow */

    }                           /* for (i=0;i<size;i++) */
  }                             /* if (cbdata->header_search < 4) */

  if (i >= size - 1)
    return;

  /* Write remaining bytes into the file or buffer (if root) (buffer is
     disabled in this version) */
  if (cbdata->current_fd)
    fwrite(&(bytes[i]), 1, size - i, cbdata->current_fd);

  return;
}


/*
  The mime message parser
*/

struct attachments_t *
mime_message_parse(struct http_input_stream_t * in, const char *root_id,
                   const char *boundary, const char *dest_dir)
{
  MIME_parser_status status;
  MIME_callbacks callbacks;
  struct attachments_t *message;

  mime_callback_data_t *cbdata;
 
  if (!(cbdata = (mime_callback_data_t *) http_malloc(sizeof(mime_callback_data_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  cbdata->part_id = 100;
  cbdata->buffer_capacity = 0;
  cbdata->current_fd = NULL;
  cbdata->current_part = NULL;
  cbdata->header_index = 0;
  cbdata->header_search = 0;
  strcpy(cbdata->root_id, root_id);
  strcpy(cbdata->root_dir, dest_dir);

  if (!(message = (struct attachments_t *) http_malloc(sizeof(struct attachments_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    http_free(cbdata);
    return NULL;
  }
  cbdata->message = message;
  cbdata->message->parts = NULL;
  cbdata->message->root_part = NULL;

  callbacks.parse_begin_cb = _mime_parse_begin;
  callbacks.parse_end_cb = _mime_parse_end;
  callbacks.part_begin_cb = _mime_part_begin;
  callbacks.part_end_cb = _mime_part_end;
  callbacks.received_bytes_cb = _mime_received_bytes;

  status = MIME_parse(mime_streamreader_function,
                      in, boundary, &callbacks, cbdata);

  if (status == MIME_PARSER_OK)
  {
    http_free(cbdata);
    return message;
  }
  else
  {
    log_error("MIME parser error '%s'!",
               status ==
               MIME_PARSER_READ_ERROR ? "read error" : "Incomplete message");
    return NULL;
  }
}

struct attachments_t *
mime_message_parse_from_file(FILE * in, const char *root_id,
                             const char *boundary, const char *dest_dir)
{
  MIME_parser_status status;
  MIME_callbacks callbacks;
  struct attachments_t *message;

  mime_callback_data_t *cbdata;
 
  if (!(cbdata = (mime_callback_data_t *) http_malloc(sizeof(mime_callback_data_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    return NULL;
  }

  cbdata->part_id = 100;
  cbdata->buffer_capacity = 0;
  cbdata->current_fd = NULL;
  cbdata->current_part = NULL;
  cbdata->header_index = 0;
  cbdata->header_search = 0;
  strcpy(cbdata->root_id, root_id);
  strcpy(cbdata->root_dir, dest_dir);

  if (!(message = (struct attachments_t *) http_malloc(sizeof(struct attachments_t))))
  {
    log_error("http_malloc failed (%s)", strerror(errno));
    http_free(cbdata);
    return NULL;
  }

  cbdata->message = message;
  cbdata->message->parts = NULL;
  cbdata->message->root_part = NULL;

  callbacks.parse_begin_cb = _mime_parse_begin;
  callbacks.parse_end_cb = _mime_parse_end;
  callbacks.part_begin_cb = _mime_part_begin;
  callbacks.part_end_cb = _mime_part_end;
  callbacks.received_bytes_cb = _mime_received_bytes;

  status = MIME_parse(MIME_filereader_function,
                      in, boundary, &callbacks, cbdata);

  if (status == MIME_PARSER_OK)
  {
    http_free(cbdata);
    return message;
  }
  else
  {
    /* TODO (#1#): Free objects */

    log_error("MIME parser error '%s'!",
               status ==
               MIME_PARSER_READ_ERROR ? "general error" :
               "Incomplete message");
    return NULL;
  }
}

herror_t
mime_get_attachments(content_type_t * ctype, struct http_input_stream_t * in,
                     struct attachments_t ** dest)
{
  /* MIME variables */
  struct attachments_t *mimeMessage;
  struct part_t *part, *tmp_part = NULL;
  char *boundary, *root_id;

  /* Check for MIME message */
  if (!(ctype && !strcmp(ctype->type, "multipart/related")))
    return herror_new("mime_get_attachments", MIME_ERROR_NOT_MIME_MESSAGE,
                      "Not a MIME message '%s'", ctype->type);

  boundary = hpairnode_get(ctype->params, "boundary");
  root_id = hpairnode_get(ctype->params, "start");
  if (boundary == NULL)
  {
    /* TODO (#1#): Handle Error in http form */
    log_error("'boundary' not set for multipart/related");
    return herror_new("mime_get_attachments", MIME_ERROR_NO_BOUNDARY_PARAM,
                      "'boundary' not set for multipart/related");
  }

  if (root_id == NULL)
  {
    /* TODO (#1#): Handle Error in http form */
    log_error("'start' not set for multipart/related");
    return herror_new("mime_get_attachments", MIME_ERROR_NO_START_PARAM,
                      "'start' not set for multipart/related");
  }

  mimeMessage = mime_message_parse(in, root_id, boundary, ".");
  if (mimeMessage == NULL)
  {
    /* TODO (#1#): Handle Error in http form */
    log_error("MIME Parse Error");
    return herror_new("mime_get_attachments", MIME_ERROR_PARSE_ERROR,
                      "MIME Parse Error");
  }

  /* Find root */
  if (!mimeMessage->root_part)
  {
    attachments_free(mimeMessage);
    return herror_new("mime_get_attachments", MIME_ERROR_NO_ROOT_PART,
                      "No root part found!");
  }

  /* delete root_part from list */
  part = mimeMessage->parts;
  while (part)
  {
    if (part == mimeMessage->root_part)
    {
      if (tmp_part)
        tmp_part->next = part->next;
      else
        mimeMessage->parts = part->next;

      break;
    }
    tmp_part = part;
    part = part->next;
  }
  *dest = mimeMessage;
  return H_OK;
}

