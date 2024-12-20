/** @file nanohttp-response.c HTTP response handling */
/******************************************************************
*  $Id: nanohttp-response.c,v 1.19 2007/11/03 22:40:11 m0gg Exp $
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

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-mime.h"
#include "nanohttp-response.h"

static hresponse_t *
_hresponse_new(void)
{
  hresponse_t *res;

  if (!(res = (hresponse_t *) malloc(sizeof(hresponse_t))))
  {
    log_error("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  res->version = HTTP_1_1;
  res->errcode = -1;
  res->desc[0] = '\0';
  res->header = NULL;
  res->in = NULL;
  res->content_type = NULL;
  res->attachments = NULL;

  return res;
}

static hresponse_t *
_hresponse_parse_header(const char *buffer)
{
  hresponse_t *res;
  char *s1, *s2, *str;

  /* create response object */
  res = _hresponse_new();

  /* *** parse spec *** */
  /* [HTTP/1.1 | 1.2] [CODE] [DESC] */

  /* stage 1: HTTP spec */
  str = (char *) strtok_r((char *) buffer, " ", &s2);
  s1 = s2;
  if (str == NULL)
  {
    log_error("Parse error reading HTTP spec");
    return NULL;
  }

  if (!strcmp(str, "HTTP/1.0"))
    res->version = HTTP_1_0;
  else
    res->version = HTTP_1_1;

  /* stage 2: http code */
  str = (char *) strtok_r(s1, " ", &s2);
  s1 = s2;
  if (str == NULL)
  {
    log_error("Parse error reading HTTP code");
    return NULL;
  }
  res->errcode = atoi(str);

  /* stage 3: description text */
  str = (char *) strtok_r(s1, "\r\n", &s2);
  s1 = s2;
  if (str == NULL)
  {
    log_error("Parse error reading HTTP description");
    return NULL;
  }
/*	res->desc = (char *) malloc(strlen(str) + 1);*/
  strncpy(res->desc, str, RESPONSE_MAX_DESC_SIZE-1);
  res->desc[strlen(str)] = '\0';

  /* *** parse header *** */
  /* [key]: [value] */
  for (;;)
  {
    str = strtok_r(s1, "\n", &s2);
    s1 = s2;

    /* check if header ends without body */
    if (str == NULL)
    {
      return res;
    }
    /* check also for end of header */
    if (!strcmp(str, "\r"))
    {
      break;
    }
    str[strlen(str) - 1] = '\0';
    res->header = hpairnode_parse(str, ":", res->header);
  }

  /* Check Content-type */
  str = hpairnode_get(res->header, HEADER_CONTENT_TYPE);
  if (str != NULL)
    res->content_type = content_type_new(str);

  /* return response object */
  return res;
}

herror_t
hresponse_new_from_socket(struct hsocket_t *sock, hresponse_t ** out)
{
  size_t i = 0, count;
  herror_t status;
  hresponse_t *res;
  struct attachments_t *mimeMessage;
  char buffer[MAX_HEADER_SIZE + 1];

read_header:                   /* for errorcode: 100 (continue) */
  /* Read header */
  while (i < MAX_HEADER_SIZE)
  {
    if ((status = hsocket_recv(sock, (unsigned char *)&(buffer[i]), 1, 1, &count)) != H_OK)
    {
      log_error("Socket read error");
      return status;
    }

    buffer[i + 1] = '\0';       /* for strmp */

    if (i > 3)
    {
      if (!strcmp(&(buffer[i - 1]), "\n\n") ||
          !strcmp(&(buffer[i - 2]), "\n\r\n"))
        break;
    }
    i++;
  }

  /* Create response */
  res = _hresponse_parse_header(buffer);
  if (res == NULL)
  {
    log_error("Header parse error");
    return herror_new("hresponse_new_from_socket",
                      GENERAL_HEADER_PARSE_ERROR,
                      "Can not parse response header");
  }

  /* Chec for Errorcode: 100 (continue) */
  if (res->errcode == 100)
  {
    hresponse_free(res);
    i = 0;
    goto read_header;
  }

  /* Create input stream */
  res->in = http_input_stream_new(sock, res->header);

  /* Check for MIME message */
  if ((res->content_type &&
       !strcmp(res->content_type->type, "multipart/related")))
  {
    status = mime_get_attachments(res->content_type, res->in, &mimeMessage);
    if (status != H_OK)
    {
      /* TODO (#1#): Handle error */
      hresponse_free(res);
      return status;
    }
    else
    {
      res->attachments = mimeMessage;
      http_input_stream_free(res->in);
      res->in =
        http_input_stream_new_from_file(mimeMessage->root_part->filename);
      if (!res->in)
      {
        /* TODO (#1#): Handle error */

      }
      else
      {
        /* res->in->deleteOnExit = 1; */
      }
    }
  }
  *out = res;
  return H_OK;
}

void
hresponse_free(hresponse_t * res)
{
  if (res)
  {
    if (res->header)
      hpairnode_free_deep(res->header);

    if (res->in)
      http_input_stream_free(res->in);

    if (res->content_type)
      content_type_free(res->content_type);

    if (res->attachments)
      attachments_free(res->attachments);

    free(res);
  }
  return;
}
