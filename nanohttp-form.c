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
#include <string.h>
#include "nanohttp-config.h"
#include "nanohttp-form.h"

void multipartparser_callbacks_init(multipartparser_callbacks* callbacks)
{
  memset(callbacks, 0, sizeof(*callbacks));
}

#define CALLBACK_NOTIFY(NAME)                           \
    if (callbacks->on_##NAME != NULL) {                 \
        if (callbacks->on_##NAME(parser) != 0)          \
            goto error;                                 \
    }

#define CALLBACK_DATA(NAME, P, S)                       \
    if (callbacks->on_##NAME != NULL) {                 \
        if (callbacks->on_##NAME(parser, P, S) != 0)    \
            goto error;                                 \
    }

/* Header field name as defined by rfc 2616. Also lowercases them.
 *     field-name   = token
 *     token        = 1*<any CHAR except CTLs or tspecials>
 *     CTL          = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
 *     tspecials    = "(" | ")" | "<" | ">" | "@"
 *                  | "," | ";" | ":" | "\" | DQUOTE
 *                  | "/" | "[" | "]" | "?" | "="
 *                  | "{" | "}" | SP | HT
 *     DQUOTE       = <US-ASCII double-quote mark (34)>
 *     SP           = <US-ASCII SP, space (32)>
 *     HT           = <US-ASCII HT, horizontal-tab (9)>
 *
 */
const uint8_t __isValidToken[256] = {
  /*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
       0,       2,       0,       0,       0,       0,       0,       0,
  /*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
       0,       0,       0,       0,       0,       0,       0,       0,
  /*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
       2,       3,       2,       3,       3,       3,       3,       3,
  /*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
       2,       2,       3,       3,       2,       3,       3,       2,
  /*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
       3,       3,       2,       2,       2,       2,       2,       2,
  /*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
       2,       3,       3,       3,       3,       3,       3,       3,
  /*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
       3,       3,       3,       2,       2,       2,       3,       3,
  /*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
       3,       3,       3,       3,       3,       3,       3,       3,
  /* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
       3,       3,       3,       2,       3,       2,       3,       0,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
};

#define CR '\r'
#define LF '\n'
#define SP ' '
#define HT '\t'
#define HYPHEN '-'

/*
 "--------------A940F1230E6F0105F03DB2CB\r\n" \
 "Content-Type: text/html; charset=\"utf-8\"\r\n" \
 "Content-Transfer-Encoding: 8bit\r\n\r\n" \
 "<html><head>\r\n" \
 "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n" \
 "  </head>\r\n" \
 "  <body bgcolor=\"#FFFFFF\" text=\"#000000\">\r\n" \
 "    <p>This is a test.&nbsp; <img src=\"cid:part1.E16AE3B4.1505C436@chilkatsoft.com\" height=\"20\" width=\"20\"></p>\r\n" \
 "  </body>\r\n" \
 "</html>\r\n" \
 "--------------A940F1230E6F0105F03DB2CB\r\n" \
 "Content-Transfer-Encoding: base64\r\n" \
 "Content-Type: image/jpeg; name=\"starfish20.jpg\"\r\n" \
 "Content-Disposition: inline; filename=\"starfish20.jpg\"\r\n" \
 "Content-ID: <part1.E16AE3B4.1505C436@chilkatsoft.com>\r\n\r\n" \
 "/9j/4AAQSkZJRgABAQEASABIAAD//gAmRmlsZSB3cml0dGVuIGJ5IEFkb2JlIFBob3Rvc2hvcD8g\r\n" \
 "NC4w/9sAQwAQCwwODAoQDg0OEhEQExgoGhgWFhgxIyUdKDozPTw5Mzg3QEhcTkBEV0U3OFBtUVdf\r\n" \
 "YmdoZz5NcXlwZHhcZWdj/9sAQwEREhIYFRgvGhovY0I4QmNjY2NjY2NjY2NjY2NjY2NjY2NjY2Nj\r\n" \
 "Y2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2Nj/8IAEQgAFAAUAwERAAIRAQMRAf/EABcAAAMBAAAA\r\n" \
 "AAAAAAAAAAAAAAIDBAX/xAAYAQADAQEAAAAAAAAAAAAAAAABAgMEAP/aAAwDAQACEAMQAAAB2kZY\r\n" \
 "NNEijWKddfTmLgALWH//xAAbEAACAgMBAAAAAAAAAAAAAAABAgMRAAQSE//aAAgBAQABBQL0XqN+\r\n" \
 "pM2aqJGMiqFFCyg7z//EABwRAAICAgMAAAAAAAAAAAAAAAERAAIQIQMSUf/aAAgBAwEBPwHqU5aq\r\n" \
 "Axx+y1tMQl4elj//xAAcEQEAAQUBAQAAAAAAAAAAAAABEQACEBIhA1H/2gAIAQIBAT8B3Bhqy7Zc\r\n" \
 "enyiwmGgDhiOzj//xAAdEAABAwUBAAAAAAAAAAAAAAABAAIREBIhIkFR/9oACAEBAAY/ArZyn+Cg\r\n" \
 "xtxWuJaoCnqDuin/xAAcEAABBAMBAAAAAAAAAAAAAAABABEhYRAxQVH/2gAIAQEAAT8hkEwPUUR9\r\n" \
 "DYfE4nxtRpIkBTsayuALIiuY/9oADAMBAAIAAwAAABDWPTsf/8QAGhEAAwADAQAAAAAAAAAAAAAA\r\n" \
 "AAEREDFBIf/aAAgBAwEBPxC0DVPcWm+Ce4OesrkE6bjH/8QAGBEBAQEBAQAAAAAAAAAAAAAAAREA\r\n" \
 "QRD/2gAIAQIBAT8QahMiOc8YgSrnTY3ELclHXn//xAAcEAEBAAIDAQEAAAAAAAAAAAABEQAhMUFx\r\n" \
 "EFH/2gAIAQEAAT8Qn3igmSZSj+c4N4zapMy9IjFV98wncN2iuLFsCEbDGxQkI6RO/n//2Q==\r\n" \
 "--------------A940F1230E6F0105F03DB2CB--"
 */

enum state {
    s_preamble,
    s_preamble_hy_hy,
    s_first_boundary,
    s_header_field_start,
    s_header_field,
    s_header_value_start,
    s_header_value,
    s_header_value_cr,
    s_headers_done,
    s_data,
    s_data_cr,
    s_data_cr_lf,
    s_data_cr_lf_hy,
    s_data_boundary_start,
    s_data_boundary,
    s_data_boundary_done,
    s_data_boundary_done_cr_lf,
    s_data_boundary_done_hy_hy,
    s_epilogue,
};

/* SP or HTAB : ((x) == 9 || (x) == 32) : 0x101 || 0x10 */
#define __is_OWS(x) ((x) == 9 || (x) == 32)

static inline const char *
__http_strnstr(char *str, const char *p, size_t len)
{
  char c = str[len];
  const char *r;
  str[len]='\0';
  r = strstr(str, p);
  str[len]=c;
  return r;
}

size_t multipartparser_execute(multipartparser *parser,
                 multipartparser_callbacks *callbacks,
                 const char *data,
                 size_t size)
{
  const char   *mark;
  const char   *end;
  const char   *p;
  unsigned char c;

  end = data + size;
  
  for (p = data; p < end; ++p) {
    c = *p;

reexecute:
    switch (parser->state) {

      case s_preamble:
        if (c == HYPHEN)
          parser->state = s_preamble_hy_hy;
        // else ignore everything before first boundary
        break;

      case s_preamble_hy_hy:
        if (c == HYPHEN)
          parser->state = s_first_boundary;
        else
          parser->state = s_preamble;
        break;

      case s_first_boundary:
        if (!parser->index && end - p >= parser->boundary_length)
        {
          if (memcmp(p, parser->boundary, parser->boundary_length))
          {
            goto error;
          }
          parser->index = parser->boundary_length;
          p += parser->boundary_length - 1;
          break;
        }
        if (parser->index == parser->boundary_length) {
          if (c != CR)
            goto error;
          parser->index++;
          break;
        }
        if (parser->index == parser->boundary_length + 1) {
          if (c != LF)
            goto error;
          CALLBACK_NOTIFY(body_begin);
          CALLBACK_NOTIFY(part_begin);
          parser->index = 0;
          parser->state = s_header_field_start;
          break;
        }
        if (c == parser->boundary[parser->index]) {
          parser->index++;
          break;
        }
        goto error;

      case s_header_field_start:
        if (c == CR) {
          parser->state = s_headers_done;
          break;
        }
        parser->state = s_header_field;
        // fallthrough;

      case s_header_field:
        /* Header field name as defined by rfc 2616. Also lowercases them.
         *     field-name   = token
         *     token        = 1*<any CHAR except CTLs or tspecials>
         *     CTL          = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
         *     tspecials    = "(" | ")" | "<" | ">" | "@"
         *                  | "," | ";" | ":" | "\" | DQUOTE
         *                  | "/" | "[" | "]" | "?" | "="
         *                  | "{" | "}" | SP | HT
         *     DQUOTE       = <US-ASCII double-quote mark (34)>
         *     SP           = <US-ASCII SP, space (32)>
         *     HT           = <US-ASCII HT, horizontal-tab (9)>
         */
        mark = p;
        while (p != end) 
        {
          c = *p;
          if ((__isValidToken[c]&0x1) == 0)
            break;
          ++p;
        }
        if (p > mark) {
          CALLBACK_DATA(header_field, mark, p - mark);
        }
        if (p == end) {
          break;
        }
        if (c == ':') {
          parser->state = s_header_value_start;
          break;
        }
        else if (c == CR) {
          /* No headers */
          parser->state = s_header_value_cr;
          break;
        }
        goto error;

      case s_header_value_start:
        if (c == SP || c == HT) {
          break;
        }
        parser->state = s_header_value;
        // fallthrough;

      case s_header_value:
        /**
         *  field-value    = *( field-content / obs-fold )
         *  field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
         *  field-vchar    = VCHAR / obs-text
            obs-fold       = CRLF 1*( SP / HTAB )
                        ; obsolete line folding
                        ; see Section 3.2.4     
        
         field-value    = *field-content
         field-content  = field-vchar
                          [ 1*( SP / HTAB / field-vchar ) field-vchar ]
         field-vchar    = VCHAR / obs-text
         
         VCHAR          =  %x21-7E[visible (printing) characters]
         obs-text       = %x80-FF
         
         >>>>>>>>>>>>>>https://httpwg.org/specs/rfc9110.html#fields.values
         **/
        mark = memchr(p, CR, end - p);
        if (mark != NULL)
        {
          if (mark > p) {
            CALLBACK_DATA(header_value, p, mark - p);
          }
          p = mark;
          parser->state = s_header_value_cr;
          break;
        }
        mark = p;
        while (p != end) {
          c = *p;
          if (c == CR) {
            parser->state = s_header_value_cr;
            break;
          }
          if ((__isValidToken[c]&0x2) == 0)
            break;
          ++p;
        }
        if (p > mark) {
          CALLBACK_DATA(header_value, mark, p - mark);
        }
        break;

      case s_header_value_cr:
        if (c == LF) {
          parser->state = s_header_field_start;
          break;
        }
        goto error;

      case s_headers_done:
        if (c == LF) {
          CALLBACK_NOTIFY(headers_complete);
          parser->state = s_data;
          break;
        }
        goto error;

      case s_data:
        mark = memchr(p, CR, end - p);
        if (mark != NULL)
        {
          if (mark > p) {
            CALLBACK_DATA(data, p, mark - p);
          }
          parser->state = s_data_cr;
          p = mark;
          break;
        }
        mark = p;
        while (p != end) {
          c = *p;
          if (c == CR) {
            parser->state = s_data_cr;
            break;
          }
          ++p;
        }
        if (p > mark) {
          CALLBACK_DATA(data, mark, p - mark);
        }
        break;

      case s_data_cr:
        if (c == LF) {
          parser->state = s_data_cr_lf;
          break;
        }
        CALLBACK_DATA(data, "\r", 1);
        parser->state = s_data;
        goto reexecute;

      case s_data_cr_lf:
        if (c == HYPHEN) {
          parser->state = s_data_cr_lf_hy;
          break;
        }
        CALLBACK_DATA(data, "\r\n", 2);
        parser->state = s_data;
        goto reexecute;

      case s_data_cr_lf_hy:
        if (c == HYPHEN) {
          parser->state = s_data_boundary_start;
          break;
        }
        CALLBACK_DATA(data, "\r\n-", 3);
        parser->state = s_data;
        goto reexecute;

      case s_data_boundary_start:
        parser->index = 0;
        parser->state = s_data_boundary;
        // fallthrough;

      case s_data_boundary:
        if (!parser->index && end - p >= parser->boundary_length)
        {
          if (!memcmp(p, parser->boundary, parser->boundary_length))
          {
            parser->index = 0;
            parser->state = s_data_boundary_done;
            p += parser->boundary_length - 1;
            goto reexecute;
          }
        }
        if (parser->index == parser->boundary_length) {
          parser->index = 0;
          parser->state = s_data_boundary_done;
          goto reexecute;
        }
        if (c == parser->boundary[parser->index]) {
          parser->index++;
          break;
        }
        CALLBACK_DATA(data, "\r\n--", 4);
        CALLBACK_DATA(data, parser->boundary, parser->index);
        parser->state = s_data;
        goto reexecute;

      case s_data_boundary_done:
        if (c == CR) {
          parser->state = s_data_boundary_done_cr_lf;
          break;
        }
        if (c == HYPHEN) {
          parser->state = s_data_boundary_done_hy_hy;
          break;
        }
        goto error;

      case s_data_boundary_done_cr_lf:
        if (c == LF) {
          CALLBACK_NOTIFY(part_end);
          CALLBACK_NOTIFY(part_begin);
          parser->state = s_header_field_start;
          break;
        }
        goto error;

      case s_data_boundary_done_hy_hy:
        if (c == HYPHEN) {
          CALLBACK_NOTIFY(part_end);
          CALLBACK_NOTIFY(body_end);
          parser->state = s_epilogue;
          break;
        }
        goto error;

      case s_epilogue:
        // Must be ignored according to rfc 1341.
        break;
    }
  }
  return size;

error:
  return p - data;
}

int multipartparser_init(multipartparser *parser, void *arg, 
  const char *boundary, int boundaryLen)
{
  parser->boundary_length = boundaryLen;

  if (boundaryLen >= sizeof(parser->boundary))
    return -1;
  memcpy(parser->boundary, boundary, boundaryLen);
  parser->boundary[boundaryLen]='\0';

  parser->arg   = arg;
  parser->data  = NULL;
  parser->index = 0;
  parser->value_length = 0;
  parser->field_length = 0;
  parser->state        = s_preamble;
  parser->content_disposition_parsed = 0;
  return 0;
}

