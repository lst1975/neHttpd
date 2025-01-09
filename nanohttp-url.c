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
/** @file nanohttp-url.c URL parsing functions */
/******************************************************************
*  $Id: nanohttp-url.c,v 1.4 2007/11/03 22:40:15 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
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
* Email: ayaz@jprogrammer.net
******************************************************************/

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "nanohttp-common.h"
#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-url.h"
#include "nanohttp-mem.h"
      
#define HTTP_DEFAULT_PORT		80
#define HTTPS_DEFAULT_PORT		443

#if _nanoConfig_NG_HTTP_PARSER_STRICT
#define T(v) 0
#else
#define T(v) v
#endif
/*
Why do we encode URL?
URLs (Uniform Resource Locators) can only contain a very limited set of characters from the 
US-ASCII charset. These characters include upper and lowercase alphabets (A-Za-z), 
digits (0-9), and the special characters (-_~.).

Some ASCII characters like ?, &, =, / have special meaning within URLs. Other ASCII 
characters like backspace, newline are unprintable. All these ASCII characters and 
any non-ASCII character must be encoded so that it can be safely placed inside URLs

Which characters are not allowed in URL?
Following class of characters are not allowed within URLs:

Reserved characters: Some characters like : / ? # [ ] @ ! $ & ' ( ) * + , ; = are 
  reserved for special purpose in the URLs. For example, the character ? is used to 
  specify query parameters, the character & is used to separate two query parameters. 
  These characters cannot be placed in URLs without encoding.
Unprintable characters: ASCII characters in the range 0-31 and 127 are unprintable. 
These are also called control characters. These characters are not allowed in URLs.
Unsafe characters: Other ASCII characters like space < > { } | ` ^ \ are considered 
unsafe and are not allowed in URLs.
Non-ASCII characters: Any character outside the US-ASCII charset are not allowed in URLs.
*/
const uint8_t __isValidUrlToken[256] ={
  /*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
       0,     T(1),      0,       0,     T(1),      0,       0,       0,
  /*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
       0,       2,       2,       2,       2,       2,       2,       2,
  /*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
       2,       2,       2,       3,       2,       1,       1,       2,
  /*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
       1,       1,       1,       1,       1,       1,       1,       1,
  /*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
       1,       1,       2,       2,     T(1),      2,     T(1),      2,
  /*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
       2,       1,       1,       1,       1,       1,       1,       1,
  /*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
       1,       1,       1,       1,       1,       1,       1,       1,
  /*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
       1,       1,       1,       1,       1,       1,       1,       1,
  /*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
       1,       1,       1,       2,     T(1),      2,     T(1),      1,
  /*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
     T(1),      1,       1,       1,       1,       1,       1,       1,
  /* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
       1,       1,       1,       1,       1,       1,       1,       1,
  /* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
       1,       1,       1,       1,       1,       1,       1,       1,
  /* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
       1,       1,       1,     T(1),    T(1),    T(1),      1,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,       
       0,       0,       0,       0,       0,       0,       0,       0
};
#undef T

void
ng_url_dump(const ng_url_s *url)
{
  if (!url)
  {
    log_error("parameter url is NULL");
    return;
  }

  log_verbose("PROTOCOL: %d",        url->protocol);
  log_verbose("    HOST: \"%pS\"",  &url->host);
  log_verbose("    PORT: %d",        url->port);
  log_verbose(" CONTEXT: \"%pS\"",  &url->context);
  log_verbose("USERNAME: \"%pS\"",  &url->user);
  log_verbose("PASSWORD: \"%pS\"",  &url->pswd);

  return;
}

void 
ng_urlorpath_dlim_convert(char *str, int len, 
 const char from, const char to)
{
        char *p = str;
  const char *e = str + len;
  while ((p = ng_memchr(p, '\\', e-p))!=NULL)
    *p++ = '/';
}

#define __TOP_MAX 1024
#define __TOP_MAX_E __TOP_MAX - 1

struct _topp{
  uint16_t i;
  uint16_t l;
};

int ng_urlorPath_normalize(ng_block_s *url)
{
  char *token;
  int top = -1;
  char *end = url->buf + url->len;
  struct _topp *t;
  char *p = url->buf;
  struct _topp *stack; // Stack to hold path components

  if (!url->len)
    return ng_ERR_EINVAL;
  if (url->len > _nanoConfig_HTTP_URL_LEN_MAX)
    return ng_ERR_E2BIG;

  ng_urlorpath_dlim_convert(url->buf, url->len, '\\', '/');
  
  stack = (struct _topp *)http_malloc(sizeof(*stack)*__TOP_MAX);
  if (stack == NULL)
  {
    log_error("http_malloc temporary buffer failed.");
    return ng_ERR_ESYSTEM;
  }
  
  while (p < end)
  {
    int dots;
    
    token = ng_memchr(p, '/', url->len);
    if (token == NULL)
    {
      if (top >= __TOP_MAX_E)
      { 
        http_free(stack);
        return ng_ERR_E2BIG;
	    }
  	  t = &stack[++top];
      t->i = p - url->cptr; // Push valid directory onto stack
  	  t->l = end - p;
  	  break;
    }
    
    dots = 0;
    for (int i=0; i < token - p; i++)
    {
      if (__ng_isspace(p[i]))
        continue;
      else if (p[i] == '.')
        dots++;
      else
      {
        dots=0;
        break;
      }
    }
    if (dots >= 2) 
  	{
      if (top > -1) 
  	  {
        top--; // Pop the last valid directory
      }
  	  p = token + 1;
    } 
  	else if (dots == 1)
  	{
  	  p = token + 1;
    }
  	else if (token > p)
  	{
      if (top >= __TOP_MAX_E)
      { 
        http_free(stack);
        return ng_ERR_E2BIG;
      }
      t = &stack[++top];
      t->i = p - url->cptr; // Push valid directory onto stack
      t->l = token - p;
  	  p = token + 1;
    }
  	else
  	{
  	  p = token + 1;
    }
  }

  // Construct the normalized path
  p = url->buf;
  if (p[0] == '/')
    p++;

  for (int i = 0; i <= top; i++) 
  {
  	if (i)
      *p++ = '/';
    t = &stack[i];
  	ng_memcpy(p, url->cptr+t->i, t->l);
  	p += t->l;
  }

  http_free(stack);
  url->len = p - url->cptr;
  *p = '\0';

  return ng_ERR_NONE;
}
#undef __TOP_MAX

/*
* URL format is:
*   [http[s]://][<user>:<password>@]<location>[:<port>]/<pathname>
*   http://[::FFFF:129.144.52.38]:80/
*/
int
__ng_url_parse(ng_url_s *url, const char *urlstr, int urllen)
{
  char *b;
  const char *p, *s, *end;

  if (!urllen)
    urllen = ng_strlen(urlstr);
  
  for (int i = 0; i < urllen; i++)
  {
    if (!__isValidUrlToken[(int)(uint8_t)urlstr[i]])
    {
      log_error("bad url character");
      return URL_ERROR_CHARACTER;
    }
  }
  
  /* find protocol */
  p = ng_strstr(urlstr, "://");
  if (p == NULL)
  {
    log_error("no host");
    return URL_ERROR_NO_HOST;
  }
  /* find protocol */
  switch (p - urlstr)
  {
    case 4:
      if (!ng_strnocasecmp(urlstr, "http", 4))
        url->protocol = NG_PROTOCOL_HTTP;
      else
        return URL_ERROR_UNKNOWN_PROTOCOL;
      break;
    case 5:
      if (!ng_strnocasecmp(urlstr, "https", 5))
        url->protocol = NG_PROTOCOL_HTTPS;
      else
        return URL_ERROR_UNKNOWN_PROTOCOL;
      break;
    case 0:
      return URL_ERROR_NO_PROTOCOL;
    default:
      return URL_ERROR_UNKNOWN_PROTOCOL;
  }

  end = urlstr + urllen;
  s = p + 3;
  p = ng_memchr(s, '@', end - s);
  if (p != NULL)
  {
    const char *cp;
    cp = ng_memchr(s, ':', p - s);
    if (cp != NULL)
    {
      url->pswd.cptr = cp + 1;
      url->pswd.len  = p - url->pswd.cptr;
      url->user.cptr = s;
      url->user.len  = cp - s;
    }
    else
    {
      url->user.cptr = s;
      url->user.len  = p - s;
    }
    s = p + 1;
  }

  p = ng_memchr(s, '/', end - s);
  if (p == NULL)
  {
    url->context.cptr = "/";
    url->context.len  = 1;
    url->host.cptr = s;
    url->host.len  = end - s;
  }
  else
  {
    const char *cp1,*cp2,*cp;

    url->context.cptr = p;
    url->context.len  = end - p;

    cp1 = ng_memchr(s, '[', p - s);
    if (cp1 != NULL)
    {
      cp2 = ng_memchr(s, ']', p - s);
      if (cp2 == NULL || cp2 < cp1)
        return URL_ERROR_BAD_IP;
      url->host.cptr = cp1 + 1;
      url->host.len  = cp2-cp1-1;
      if (!ng_inet_pton6(url->host.cptr,url->host.len,url->u.v6))
        return URL_ERROR_BAD_IP;
      s = cp2 + 1;
      cp = ng_memchr(s, ':', p - s);
    }
    else
    {
      cp = ng_memchr(s, ':', p - s);
      if (cp == NULL)
      {
        url->host.cptr = s;
        url->host.len  = p - s;
      }
      else
      {
        url->host.cptr = s;
        url->host.len  = cp - s;
      }
      ng_inet_pton4(url->host.cptr,url->host.len,url->u.v4);
    }
    if (cp != NULL)
    {
      const char *endp;
      s = cp + 1;
      url->port = ng_atou32(s,p-s,&endp);
      if (endp != p || url->port > UINT16_MAX)
        return URL_ERROR_BAD_PORT;
    }
    else
    {
      /* find right port */
      switch (url->protocol)
      {
        default:
        case NG_PROTOCOL_HTTP:
          url->port = HTTP_DEFAULT_PORT;
          break;
        case NG_PROTOCOL_HTTPS:
          url->port = HTTPS_DEFAULT_PORT;
          break;
      }
    }
  }
  
  urllen =  url->host.len + url->context.len 
          + url->user.len + url->pswd.len + 4;
  url->data = b = http_malloc(urllen);
  if (url->data == NULL)
  {
    log_fatal("Failed to malloc url.");
    return GENERAL_ERROR;
  }

  if (url->host.len)
  {
    ng_memcpy(b, url->host.cptr, url->host.len);
    b[url->host.len] = '\0';
    url->host.buf = b;
    b += url->host.len + 1;
  }
  
  if (url->context.len)
  {
    ng_memcpy(b, url->context.cptr, url->context.len);
    b[url->context.len] = '\0';
    url->context.buf = b;
    b += url->context.len + 1;
    ng_urlorPath_normalize(&url->context);
  }
  
  if (url->user.len)
  {
    ng_memcpy(b, url->user.cptr, url->user.len);
    b[url->user.len] = '\0';
    url->user.buf = b;
    b += url->user.len + 1;
  }

  if (url->pswd.len)
  {
    ng_memcpy(b, url->pswd.cptr, url->pswd.len);
    b[url->pswd.len] = '\0';
    url->pswd.buf = b;
    b += url->pswd.len + 1;
  }
  
  ng_url_dump(url);

  return ng_ERR_NONE;
}

herror_t
ng_url_parse(ng_url_s *url, const char *urlstr, int urllen)
{
  int err = __ng_url_parse(url, urlstr, urllen);
  switch (err)
  {
    default:
      return herror_new("hurl_parse", GENERAL_ERROR, "Unkown error");
    case URL_ERROR_BAD_PORT:
      return herror_new("hurl_parse", URL_ERROR_NO_HOST, "Bad port");
    case URL_ERROR_BAD_IP:
      return herror_new("hurl_parse", URL_ERROR_BAD_IP, "Bad ip");
    case URL_ERROR_UNKNOWN_PROTOCOL:
      return herror_new("hurl_parse", URL_ERROR_UNKNOWN_PROTOCOL, "Unkown protocol");
    case URL_ERROR_CHARACTER:
      return herror_new("hurl_parse", URL_ERROR_CHARACTER, "Invalid char");
    case URL_ERROR_NO_HOST:
      return herror_new("hurl_parse", URL_ERROR_NO_HOST, "No host");
    case URL_ERROR_NO_PROTOCOL:
      return herror_new("hurl_parse", URL_ERROR_NO_PROTOCOL, "No protocol");
    case ng_ERR_NONE:
      return H_OK;
  }
}

void
ng_url_free(ng_url_s *url)
{
  if (url->data != NULL)
  {
    http_free(url->data);
    ng_url_init(url);
  }

  return;
}

int
ng_url_init(ng_url_s *url)
{
  url->data     = NULL;
  url->port     = HTTP_DEFAULT_PORT;
  url->protocol = NG_PROTOCOL_HTTP;
  url->is_ipv6  = 0;
  ng_block_init(&url->context);
  ng_block_init(&url->host);
  ng_block_init(&url->user);
  ng_block_init(&url->pswd);
  ng_bzero(&url->u, sizeof(url->u));
  return 0;
}

void ng_url_test(void)
{
#define TEST1 "http://user:password@[::1]:8192/test"
#define TEST2 "http://user:password@127.0.0.1:8192/test"
#define TEST3 "https://github.com/lst1975/neHttpd"
#define TEST4 "https://github.com:888/lst1975/neHttpd/../ngRTOS"
  ng_url_s url;

  ng_url_init(&url);

  __ng_url_parse(&url, TEST1, sizeof(TEST1)-1);
  ng_url_free(&url);

  __ng_url_parse(&url, TEST2, sizeof(TEST2)-1);
  ng_url_free(&url);

  __ng_url_parse(&url, TEST3, sizeof(TEST3)-1);
  ng_url_free(&url);

  __ng_url_parse(&url, TEST4, sizeof(TEST4)-1);
  ng_url_free(&url);
}
