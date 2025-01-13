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
#include "nanohttp-config.h"
#include "nanohttp-mime.h"
#include "nanohttp-logging.h"
#include "nanohttp-const.h"
#include "nanohttp-file.h"
#include "nanohttp-socket.h"
#include "nanohttp-system.h"

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

static size_t __multipart_cb_header_begin(multipartparser *p);
static size_t __multipart_cb_header_value(multipartparser *p, const char *data, size_t size);
static size_t __multipart_cb_header_field(multipartparser *p, const char *data, size_t size);
static size_t __multipart_cb_data(multipartparser *p, const char *data, size_t size);
static size_t __multipart_cb_part_end(multipartparser *p);
static size_t __multipart_cb_part_begin(multipartparser *p);
static size_t __multipart_cb_body_end(multipartparser *p);
static size_t __multipart_cb_body_begin(multipartparser *p);
static size_t __multipart_cb_headers_complete(multipartparser *p);

#define CR '\r'
#define LF '\n'
#define SP ' '
#define HT '\t'
#define HYPHEN '-'

/*

 ***********************************************************************************
 
 POST /upload HTTP/1.1
 Host: example.com
 Content-Type: multipart/form-data; boundary=---1234567890
 
 -----1234567890
 Content-Disposition: form-data; name="username"
 
 jane_doe
 -----1234567890
 Content-Disposition: form-data; name="file1"; filename="document.pdf"
 Content-Type: application/pdf
 
 [Binary data of the first file]
 -----1234567890
 Content-Disposition: form-data; name="file2"; filename="image.png"
 Content-Type: image/png
 
 [Binary data of the second file]
 -----1234567890--

 ***********************************************************************************
 POST /post/wia.bin HTTP/1.1
 Accept: \*\/\*
 Accept-Encoding: gzip, deflate, br, zstd
 Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6
 Authorization: Basic Ym9iOmJ1aWxkZXI=
 Connection: keep-alive
 Content-Length: 322
 Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryYKVURI44VQAmaON1
 Host: localhost:8080
 Origin: http://localhost:8080
 Referer: http://localhost:8080/
 Sec-Fetch-Dest: empty
 Sec-Fetch-Mode: cors
 Sec-Fetch-Site: same-origin
 User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0
 sec-ch-ua: "Microsoft Edge";v="131", "Chromium";v="131", "Not_A Brand";v="24"
 sec-ch-ua-mobile: ?0
 sec-ch-ua-platform: "Windows"
 ------WebKitFormBoundaryYKVURI44VQAmaON1
 Content-Disposition: form-data; name="File"; filename="a.txt"
 Content-Type: text/plain
 
 aaaa
 ------WebKitFormBoundaryYKVURI44VQAmaON1
 Content-Disposition: form-data; name="File"; filename="b.txt"
 Content-Type: text/plain
 
 bbbb
 ------WebKitFormBoundaryYKVURI44VQAmaON1--

 ***********************************************************************************

 POST /upload HTTP/1.1
 Host: example.com
 Content-Type: multipart/related; boundary="boundary123"
 
 --boundary123
 Content-Type: text/html
 
 <html>
   <body>
     <h1>Hello World</h1>
     <img src="cid:image1" alt="Example Image">
   </body>
 </html>
 --boundary123
 Content-Type: image/png
 Content-ID: <image1>
 
 [Binary data of the image]
 --boundary123--

 ***********************************************************************************

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
 
 ***********************************************************************************

 "--------------A940F1230E6F0105F03DB2CB\r\n" \
 "Content-Type: text/html; charset=\"utf-8\"\r\n" \
 "Content-Transfer-Encoding: 8bit\r\n\r\n" \
 "<html><head>\r\n" \
 "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n" \
 "  </head>\r\n" \
 "  <body bgcolor=\"#FFFFFF\" text=\"#000000\">\r\n" \
 "    <p>This is a test.&nbsp; <img src=\"cid:part1.E16AE3B4.1505C436@chilkatsoft.com\" height=\"20\" width=\"20\"></p>\r\n" \
 "  </body>\r\n" \
 "</html>\r\n----------A940F1230E6F0105F03DB2CB\r\n" \
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
   
 ***********************************************************************************

 "--f93dcbA3\r\n" \
 "Content-Type: application/xml; charset=UTF-8\r\n" \
 "Content-ID: <980119.X53GGT@example.com>\r\n\r\n" \
 "<?xml version=\"1.0\"?>\r\n" \
 "<uploadDocument>\r\n" \
 "  <title>My Proposal</title>\r\n" \
 "  <author>E. X. Ample</author>\r\n" \
 "  <summary>A proposal for a new project.</summary>\r\n" \
 "  <notes image=\"cid:980119.X17AXM@example.com\">(see handwritten region)</notes>\r\n" \
 "  <keywords>project proposal funding</keywords>\r\n" \
 "  <readonly>false</readonly>\r\n" \
 "  <filename>image.png</filename>\r\n" \
 "  <content>cid:980119.X25MNC@example.com</content>\r\n" \
 "</uploadDocument>\r\n" \
 "--f93dcbA3\r\n" \
 "Content-Type: image/png\r\n" \
 "Content-Transfer-Encoding: binary\r\n" \
 "Content-ID: <980119.X25MNC@example.com>\r\n\r\n" \
 "...Binary data here...\r\n" \
 "--f93dcbA3\r\n" \
 "Content-Type: image/png\r\n" \
 "Content-Transfer-Encoding: binary\r\n" \
 "Content-ID: <980119.X17AXM@example.com>\r\n\r\n" \
 "...Binary data here...\r\n" \
 "--f93dcbA3--"
 
 ***********************************************************************************

 "--boundary_1\r\n" \
 "Content-Type: text/html; charset=UTF-8\r\n" \
 "Content-ID: <page1.html>\r\n" \
 "Content-Disposition: inline\r\n\r\n" \
 "<!DOCTYPE html>\r\n" \
 "<html>\r\n" \
 "  <head><title>Multipart Example</title></head>\r\n" \
 "  <body>\r\n" \
 "    <h1>Welcome to Multipart Example</h1>\r\n" \
 "    <img src=\"cid:image1\" alt=\"Image 1\">\r\n" \
 "  </body>\r\n" \
 "</html>\r\n" \
 "--boundary_1\r\n" \
 "Content-Type: image/jpeg\r\n" \
 "Content-ID: <image1>\r\n" \
 "Content-Disposition: inline; filename=\"image1.jpg\"\r\n\r\n" \
 "...binary image data...\r\n" \
 "--boundary_1--"

 ***********************************************************************************

 //"POST /users HTTP/1.1\r\n"
 //"Content-Type: multipart/related; boundary=\"the_boundary\"; type=\"application/json\"; start=\"json\"\r\n"
 "--the_boundary\r\n" \
 "Content-Type: application/json; charset=UTF-8\r\n" \
 "Content-Disposition: inline; name=\"json\"\r\n\r\n" \
 "{\"user\":{\"name\": \"Jhon\", \"avatar\": \"cid:avatar_image\" }}\r\n" \
 "--the_boundary\r\n" \
 "Content-Type: image/png\r\n" \
 "Content-Disposition: inline; name=\"avatar_image\"; filename=\"avatar.png\"\r\n\r\n" \
 "<the binary content of image comes here>\r\n" \
 "--the_boundary--"
 
 ***********************************************************************************
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

  //printf("%.*s\n", (int)size, data);
  
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
        if (!parser->index && end - p >= parser->boundary.len)
        {
          if (ng_memcmp(p, parser->boundary.cptr, parser->boundary.len))
          {
            goto error;
          }
          parser->index = parser->boundary.len;
          p += parser->boundary.len - 1;
          break;
        }
        if (parser->index == parser->boundary.len) {
          if (c != CR)
            goto error;
          parser->index++;
          break;
        }
        if (parser->index == parser->boundary.len + 1) {
          if (c != LF)
            goto error;
          CALLBACK_NOTIFY(body_begin);
          CALLBACK_NOTIFY(part_begin);
          parser->index = 0;
          parser->state = s_header_field_start;
          break;
        }
        if (c == parser->boundary.cptr[parser->index]) {
          parser->index++;
          break;
        }
        goto error;

      case s_header_field_start:
        if (c == CR) {
          parser->state = s_headers_done;
          break;
        }
        CALLBACK_NOTIFY(header_begin);
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
        mark = ng_memchr(p, ':', end - p);
        if (unlikely(mark != NULL))
        {
          if (mark > p) {
            CALLBACK_DATA(header_field, p, mark - p);
            p = mark;
          }
          parser->state = s_header_value_start;
          break;
        }
        mark = ng_memchr(p, CR, end - p);
        if (mark != NULL)
        {
          if (mark > p) {
            CALLBACK_DATA(header_field, p, mark - p);
            p = mark;
          }
          /* No headers */
          parser->state = s_header_value_cr;
          break;
        }
        
        if (end > p) {
          CALLBACK_DATA(header_field, p, end - p);
          p = end;
        }
        break;

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
        mark = ng_memchr(p, CR, end - p);
        if (mark != NULL)
        {
          if (mark > p) {
            CALLBACK_DATA(header_value, p, mark - p);
          }
          p = mark;
          parser->state = s_header_value_cr;
          break;
        }
        if (end > p) {
          CALLBACK_DATA(header_value, p, end - p);
          p = end;
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
        mark = ng_memchr(p, CR, end - p);
        if (mark != NULL)
        {
          if (mark > p) 
          {
            CALLBACK_DATA(data, p, mark - p);
          }
          parser->state = s_data_cr;
          p = mark;
          break;
        }
        if (end > p) 
        {
          CALLBACK_DATA(data, p, end - p);
          p = end;
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
        if (!parser->index && end - p >= parser->boundary.len)
        {
          if (!ng_memcmp(p, parser->boundary.cptr, parser->boundary.len))
          {
            parser->index = 0;
            parser->state = s_data_boundary_done;
            p += parser->boundary.len - 1;
            break;
          }
        }
        if (parser->index == parser->boundary.len) {
          parser->index = 0;
          parser->state = s_data_boundary_done;
          goto reexecute;
        }
        if (c == parser->boundary.cptr[parser->index]) {
          parser->index++;
          break;
        }
        CALLBACK_DATA(data, "\r\n--", 4);
        CALLBACK_DATA(data, parser->boundary.cptr, parser->index);
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

void
multipartpart_init(multipartpart *part)
{
  ng_INIT_LIST_HEAD(&part->header);
  part->content_disposition = NULL;
  part->id      = 0;
  part->file_ch = NULL;
  return;
}

multipartpart * 
multipartpart_new(void)
{
  multipartpart *part;

  part = http_malloc(sizeof(*part));
  if (part == NULL)
  {
    log_fatal("Failed to malloc multipartpart.");
    return NULL;
  }

  multipartpart_init(part);
  return part;
}

void
multipartpart_free(multipartpart *part)
{
  if (part == NULL)
    return;

  content_type_free(part->content_disposition);
  hpairnode_free_deep(&part->header);
  part->content_disposition = NULL;
  return;
}

static multipartparser_callbacks __multipart_settings = {
  .on_data             = __multipart_cb_data,
  .on_header_begin     = __multipart_cb_header_begin,
  .on_header_field     = __multipart_cb_header_field,
  .on_header_value     = __multipart_cb_header_value,
  .on_headers_complete = __multipart_cb_headers_complete,
  .on_body_begin       = __multipart_cb_body_begin,
  .on_body_end         = __multipart_cb_body_end,
  .on_part_begin       = __multipart_cb_part_begin,
  .on_part_end         = __multipart_cb_part_end,
};

herror_t 
multipartparser_init(multipartparser *parser, 
  void *arg, content_type_t *ct)
{
  hpair_t *pair;
  herror_t status;

  /* Check for MIME message */
  if (ct != NULL)
  {
    if (ng_block_isequal__(&ct->type, &cstr_mime_related))
      parser->mime_type = MIME_TYPE_related;
    else if (ng_block_isequal__(&ct->type, &cstr_mime_form_data))
      parser->mime_type = MIME_TYPE_form_data;
    else
      parser->mime_type = MIME_TYPE_none;
  }
  else
  {
    parser->mime_type = MIME_TYPE_none;
  }

  switch (parser->mime_type)
  {
    case MIME_TYPE_none:
      status = herror_new("multipartparser_init", 
                        MIME_ERROR_NOT_MIME_MESSAGE,
                        "Not a MIME message '%pS'", 
                        &ct->type);
      return status;
    case MIME_TYPE_related:
      pair = hpairnode_get(&ct->params, &cstr_start);
      if (pair == NULL)
      {
        /* TODO (#1#): Handle Error in http form */
        status = herror_new("multipartparser_init", 
                        MIME_ERROR_NO_START_PARAM,
                        "No 'start' in %pS.", 
                        &ct->type);
        return status;
      }
      parser->part.start = pair->val;
      break;
      
    case MIME_TYPE_form_data:
      break;
  }

  pair = hpairnode_get(&ct->params, &cstr_mime_boundary);
  if (pair == NULL || pair->val.cptr == NULL || !pair->val.len)
  {
    status = herror_new("multipartparser_init", 
                      MIME_ERROR_NO_BOUNDARY_PARAM, 
                      "No 'boundary' in '%pS'.",
                      &ct->type);
    return status;
  }
  
  parser->boundary     = pair->val;
  parser->arg          = arg;
  parser->data         = NULL;
  parser->index        = 0;
  parser->state        = s_preamble;
  parser->skipdata     = ng_FALSE;
  parser->rootpart     = ng_FALSE;
  parser->err          = 0;
  
  BUF_SIZE_INIT(&parser->field, 
        parser->buffer, HTTP_FILED_NAME_LEN_MAX);
  BUF_SIZE_INIT(&parser->value, 
        parser->buffer+HTTP_FILED_NAME_LEN_MAX, 
        HTTP_FILED_VALUE_LEN_MAX);
  
  multipartpart_init(&parser->part);
  parser->settings = &__multipart_settings;
  
  return H_OK;
}

void 
multipartparser_free(multipartparser *parser)
{
  multipartpart_free(&parser->part);
}

static size_t
__build_header_field_value(multipartparser *p)
{
  if (BUF_LEN(&p->field))
  {
    if (NULL == hpairnode_new(&p->field.b,
      &p->value.b, &p->part.header))
    {
      log_warn("Field length is too long, max is %d.", 
        HTTP_FILED_NAME_LEN_MAX);
      return -1;
    }
    BUF_CLEAR(&p->field);
    BUF_CLEAR(&p->value);
  }
  
  return 0;
}

static size_t 
__multipart_cb_header_begin(multipartparser *p)
{
  return __build_header_field_value(p);
}

/*
  Content-Disposition: form-data; name=\"File\"; 
                       filename=\"nanoHttpd-2.10.tar.xz\"\r\n
  Content-Type: application/x-xz\r\n\r\n
 */
static size_t 
__multipart_cb_headers_complete(multipartparser *p)
{
  int n;
  hpair_t *pair;
  content_type_t *ct;
  multipartpart *part = &p->part;

  if (__build_header_field_value(p) < 0)
  {
    log_warn("__build_header_field_value failed.");
    return -1;
  }
  
  switch (p->mime_type)
  {
    case MIME_TYPE_none:
      log_fatal("Unsupported mime type.");
      p->err = MIME_ERROR_NOT_MIME_MESSAGE;
      return -1;
    case MIME_TYPE_related:
      pair = hpairnode_get(&part->header, 
                &__HDR_BUF__(HEADER_CONTENT_ID));
      if (pair == NULL)
      {
        p->skipdata = ng_TRUE;
        return 0;
      }
      else
      {
        if (!p->rootpart 
          && ng_block_isequal__(&pair->val, &part->start))
        {
          p->rootpart = ng_TRUE;
          p->skipdata = ng_FALSE;
        }
      }
      break;
      
    case MIME_TYPE_form_data:
      break;
  }

  pair = hpairnode_get(&part->header, 
            &__HDR_BUF__(HEADER_CONTENT_DISPOSITION));
  if (pair == NULL)
  {
    if (p->part.id == 1)
    {
      log_fatal("No Content-Disposition in header.");
      return -1;
    }
    else
    {
      return 0;
    }
  }
  part->Disposition = &pair->val;
  part->content_disposition = content_type_new(pair->val.cptr, 
          pair->val.len);
  if (part->content_disposition == NULL)
  {
    log_fatal("content_type_new failed.");
    return -1;
  }
  ct = part->content_disposition;
  
  if (!ng_block_isequal__(&ct->type, &cstr_form_data))
  {
    log_warn("ContentType is not form-data.");
    return -1;
  }
  
  pair = hpairnode_get(&ct->params, &cstr_name);
  if (pair == NULL)
  {
    log_warn("Content-Disposition does not has param 'name'.");
    return -1;
  }

  pair = hpairnode_get(&ct->params, &cstr_mime_filename);
  if (pair == NULL)
  {
    log_warn("Content-Disposition does not has param 'filename'.");
    return -1;
  }

  part->filename = pair->val;
    
  log_info("Try to open the file %pS for writing.", &pair->val);
  
  if (nanohttp_dir_create(&cstr_dir_uploads) < 0)
  {
    log_fatal("nanohttp_dir_create failed.");
    return -1;
  }

  if (part->file_ch != NULL)
    nanohttp_file_close(part->file_ch);
  
  n = ng_snprintf(p->buffer, sizeof(p->buffer), 
          "uploads/%pS", &pair->val);
  part->file_ch = nanohttp_file_open_for_write(p->buffer, n);
  if (part->file_ch == NULL)
  {
    log_warn("Not able to open the file %pS for writing.", &pair->val);
    return -1;
  }
  
  return 0;
}

static size_t 
__multipart_cb_body_begin(multipartparser *p)
{
  return 0;
}

static size_t 
__multipart_cb_body_end(multipartparser *p)
{
  switch (p->mime_type)
  {
    case MIME_TYPE_none:
      log_fatal("Unsupported mime type.");
      p->err = MIME_ERROR_NOT_MIME_MESSAGE;
      return -1;
      
    case MIME_TYPE_related:
      if (!p->rootpart)
      {
        log_fatal("No root part.");
        p->err = MIME_ERROR_NO_ROOT_PART;
        return -1;
      }
      break;
      
    case MIME_TYPE_form_data:
      break;
  }

  return 0;
}

static size_t 
__multipart_cb_part_begin(multipartparser *p)
{
  p->part.id++;
  return 0;
}

static size_t 
__multipart_cb_part_end(multipartparser *p)
{
  multipartpart_free(&p->part);
  return 0;
}

static size_t
__multipart_cb_data(multipartparser *p, const char *data, 
  size_t size)
{
  size_t nbytes;

  if (p->skipdata)
    return 0;
  
  nbytes = nanohttp_file_write(p->part.file_ch, data, size);
  if (nbytes != size)
  {
    log_warn("nanohttp_file_write failed.");
    return -1;
  }
  
  return 0;
}

static size_t
__multipart_cb_header_field(multipartparser *p, 
  const char *data, size_t size)
{
  if (BUF_REMAIN(&p->field) < size)
  {
    log_warn("Field length is too long, max is %d.", 
      HTTP_FILED_NAME_LEN_MAX);
    return -1;
  }
  ng_memcpy(BUF_CUR_PTR(&p->field), data, size);
  BUF_GO(&p->field, size);
  return 0;
}

static size_t
__multipart_cb_header_value(multipartparser *p, 
  const char *data, size_t size)
{
  if (BUF_REMAIN(&p->value) < size)
  {
    log_warn("Value length is too long, max is %d.", 
      HTTP_FILED_VALUE_LEN_MAX);
    return -1;
  }
  ng_memcpy(BUF_CUR_PTR(&p->value), data, size);
  BUF_GO(&p->value, size);
  return 0;
}

herror_t
multipart_get_attachment(multipartparser *p, 
  http_input_stream_s *in)
{
  herror_t r = H_OK;
  
  while (http_input_stream_is_ready(in))
  {
    size_t len;
    unsigned char buffer[4096];
    
    len = http_input_stream_read(in, buffer, sizeof(buffer));
    if (len == -1)
    {
      r = herror_new("multipart_get_attachment", HSOCKET_ERROR_RECEIVE, 
        "Failed to read stream %pS", p->part.filename);
      goto finished;
    }
    
    if (len != multipartparser_execute(p, p->settings, (char *)buffer, len))
    {
      if (!p->err) p->err = MIME_ERROR;
      r = herror_new("multipart_get_attachment", p->err, 
        "multipartparser_execute failed.");
      goto finished;
    }
  }

finished:  
  nanohttp_file_close(p->part.file_ch);
  p->part.file_ch = NULL;
  return r;
}

void
mime_part_free(multipartpart *part)
{
  if (part == NULL)
    return;

  hpairnode_free_deep(&part->header);
  content_type_free(part->content_disposition);
  part->content_disposition = NULL;
  http_free(part);
}

void
attachments_init(attachments_t *attachments)               
{
  ng_INIT_LIST_HEAD(&attachments->parts);
  attachments->boundary = NULL;
  attachments->root_id  = NULL;
}

attachments_t *
attachments_new(ng_list_head_s *attachments_list)               
{
  attachments_t *attachments;

  attachments = (attachments_t *)http_malloc(sizeof(attachments_t));
  if (attachments == NULL)
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    return NULL;
  }

  attachments_init(attachments);
  if (attachments_list != NULL)
    ng_list_add_tail(&attachments->link, attachments_list);

  return attachments;
}

void
attachments_add_part(attachments_t *attachments, multipartpart *part)
{
  if (!attachments)
    return;

  ng_list_add_tail(&part->link, &attachments->parts);
  return;
}

/*
  Free a mime message 
*/
void
attachments_free(attachments_t *message)
{
  if (!message)
    return;

  while (!ng_list_empty(&message->parts))
  {
    multipartpart *part;
    part = ng_list_first_entry(&message->parts,multipartpart,link);
    assert(part != NULL);
    ng_list_del(&part->link);
    mime_part_free(part);
  }

  http_free(message);

  return;
}

#define _HTTPC_MIME_BOUNDARY_SIZE_MAX 75

static herror_t _mime_toolarge(const char *func, size_t size)
{
  return herror_new(func, 
                    MIME_ERROR,
                    "Tempary buffer is too short: \"%d\"", 
                    size);
}

static inline herror_t
_mime_copy3(char *buffer, size_t blen, const char *d1, size_t d1len, 
  const char *d2, size_t d2len, const char *d3, size_t d3len)
{
  if (blen < d1len+d2len+d3len)
  {
    return _mime_toolarge("httpc_mime_copy3", blen);
  }
  memcpy(buffer, d1, d1len);
  memcpy(buffer+d1len, d2, d2len);
  memcpy(buffer+d1len+d2len, d3, d3len);
  return H_OK;
}

herror_t
mime_add_content_type_header(ng_list_head_s *header, 
  void *conn_ptr, int conn_id, const ng_block_s *params)
{
  int n;
  size_t tsize;
  httpd_buf_t b;
  char *buffer;
  herror_t status = H_OK;
  const ng_block_s *type; 
  const ng_block_s *related_start;
  const ng_block_s *related_start_info; 
  const ng_block_s *related_type;

  type               = &params[MIME_CONTENT_TYPE_PARAM_ct_type];
  related_start      = &params[MIME_CONTENT_TYPE_PARAM_ct_related_start_info];
  related_start_info = &params[MIME_CONTENT_TYPE_PARAM_ct_related_start];
  related_type       = &params[MIME_CONTENT_TYPE_PARAM_ct_related_type];
  
#define ___BUFSZ 512
  tsize = ___BUFSZ+_HTTPC_MIME_BOUNDARY_SIZE_MAX;
  buffer = (char *)http_malloc(tsize);
  if (buffer == NULL)
  {
    log_fatal("Failed to malloc temp buffer.");
    status = herror_new("httpc_mime_begin", 
                      HSERVER_ERROR_MALLOC,
                      "Can malloc \"%d\" (%s)", 
                      tsize, 
                      os_strerror(ng_errno));
    goto clean0;
  }
  
  BUF_SIZE_INIT(&b, buffer, ___BUFSZ);

  /* 
     Set Content-type Set multipart/related parameter type=..; start=.. ;
     start-info= ..; boundary=...

   */
  ng_memcpy(BUF_CUR_PTR(&b), type->cptr, type->len);
  BUF_GO(&b, type->len);
  BUF_SET_CHR(&b, ';');
  BUF_GO(&b, 1);

  if (related_type)
  {
    status = _mime_copy3(BUF_CUR_PTR(&b), BUF_REMAIN(&b),
      " type=\"", 7, related_type->cptr, related_type->len, "\";", 2);
    if (status != H_OK) goto clean1;
  }

  if (related_start)
  {
    status = _mime_copy3(BUF_CUR_PTR(&b), BUF_REMAIN(&b),
      " start=\"", 8, related_start->cptr, related_start->len, "\";", 2);
    if (status != H_OK) goto clean1;
  }

  if (related_start_info)
  {
    status = _mime_copy3(BUF_CUR_PTR(&b), BUF_REMAIN(&b),
      " start-info=\"", 13, related_start_info->cptr, 
      related_start_info->len, "\";", 2);
    if (status != H_OK) goto clean1;
  }

  n = ng_snprintf(BUF_CUR_PTR(&b), BUF_REMAIN(&b), 
    " boundary=\""HTTP_BOUNDARY_FMT"\"", conn_ptr, conn_id);
  BUF_GO(&b, n);
  if (n != HTTP_BOUNDARY_LEN)
  {
    status = _mime_toolarge("mime_add_content_type_header", tsize);
    goto clean1;
  }
  
  if (hpairnode_set_header(header, &__HDR_BUF__(HEADER_CONTENT_TYPE), &b.b) < 0)
  {
    status = herror_new("mime_add_content_type_header", 
                      GENERAL_ERROR,
                      "hpairnode_set_header failed.");
  }
  
#undef ___BUFSZ
  
clean1:
  http_free(buffer);
clean0:
  return status;
}

multipartpart *
mime_part_new(ng_list_head_s *part_list, const ng_block_s *params)
{
  int n;
  multipartpart  *part;
  char    *buffer;
  hpair_t *pair;
  httpd_buf_t disposition;

  const ng_block_s *id;
  const ng_block_s *filename; 
  const ng_block_s *content_type;
  const ng_block_s *type;
  const ng_block_s *name;
  const ng_block_s *transfer_encoding;

  type              = &params[MIME_CONTENT_TYPE_PARAM_part_type];
  name              = &params[MIME_CONTENT_TYPE_PARAM_part_name];
  filename          = &params[MIME_CONTENT_TYPE_PARAM_part_filename];
  id                = &params[MIME_CONTENT_TYPE_PARAM_part_content_id];
  content_type      = &params[MIME_CONTENT_TYPE_PARAM_part_content_type];
  transfer_encoding = &params[MIME_CONTENT_TYPE_PARAM_part_transfer_encoding];
  
#define ___BUFSZ 1892
  buffer = (char *)http_malloc(___BUFSZ);
  if (buffer == NULL)
  {
    log_fatal("Failed to malloc temp buffer.");
    goto clean0;
  }
  BUF_SIZE_INIT(&disposition, buffer, ___BUFSZ);

  part = (multipartpart *)http_malloc(sizeof(multipartpart));
  if (part == NULL)
  {
    log_error("http_malloc failed (%s)", os_strerror(ng_errno));
    goto clean1;
  }
  part->content_disposition = NULL;

  if (id != NULL)
  {
    pair = hpairnode_new(&__HDR_BUF__(HEADER_CONTENT_ID), 
      id, &part->header);
    if (pair == NULL)
    {
      log_error("hpairnode_new Id failed.");
      goto clean2;
    }
    part->ContentID = &pair->val;
  }

  /* TODO (#1#): encoding is always binary. implement also others! */
  if (transfer_encoding == NULL) 
    transfer_encoding = &cstr_binary;
  pair = hpairnode_new(&__HDR_BUF__(HEADER_CONTENT_TRANSFER_ENCODING), 
    transfer_encoding, &part->header);
  if (pair == NULL)
  {
    log_error("hpairnode_new Id failed.");
    goto clean2;
  }
  part->TransferEncoding = &pair->val;
    
  if (content_type == NULL)
  {
    if (filename != NULL)
    {
      content_type = ng_http_get_mime_type_from_file(filename);
    }
  }

  if (content_type == NULL)
    content_type = ng_http_get_mime_type(NULL);
  pair = hpairnode_new(&__HDR_BUF__(HEADER_CONTENT_TYPE), 
    content_type, &part->header);
  if (pair == NULL)
  {
    log_error("hpairnode_new Id failed.");
    goto clean2;
  }
  part->ContentType = &pair->val;

  n = ng_snprintf(BUF_CUR_PTR(&disposition), BUF_REMAIN(&disposition),
    "%pS", type);
  ng_block_set(&part->type, BUF_CUR_PTR(&disposition), type->len);
  BUF_GO(&disposition, n);
  if (name != NULL)
  {
    n = ng_snprintf(BUF_CUR_PTR(&disposition), BUF_REMAIN(&disposition),
      "; name=\"%pS\"", name);
    BUF_GO(&disposition, n);
  }
  if (filename != NULL)
  {
    n = ng_snprintf(BUF_CUR_PTR(&disposition), BUF_REMAIN(&disposition),
      "; filename=\"%pS\"", filename);
    ng_block_set(&part->filename, BUF_CUR_PTR(&disposition)+12, filename->len);
    BUF_GO(&disposition, n);
  }

  pair = hpairnode_new(&__HDR_BUF__(HEADER_CONTENT_DISPOSITION), 
    &disposition.b, &part->header);
  if (pair == NULL)
  {
    log_error("hpairnode_new Content-Type failed.");
    goto clean2;
  }
  part->Disposition = &pair->val;

  ng_list_add_tail(&part->link, part_list);
  ng_free_data_block(&disposition.b);
  return part;
#undef ___BUFSZ
  
clean2:
  mime_part_free(part);
clean1:
  ng_free_data_block(&disposition.b);
clean0:
  return NULL;
}

static herror_t
mime_write_begin(void *conn, unsigned int id, 
  http_output_stream_s *out)
{
  /* Send Content header */
  return http_output_stream_write_printf(out, 
            "\r\n----"HTTP_BOUNDARY_FMT"\r\n",
            conn, id);
}

static herror_t
mime_write_next(void *conn, unsigned int id, 
  http_output_stream_s *out)
{
  /* Send Content header */
  return http_output_stream_write_printf(out, 
            "\r\n--"HTTP_BOUNDARY_FMT"\r\n",
            conn, id);
}

static herror_t
mime_write_end(void *conn, unsigned int id, 
  http_output_stream_s *out)
{
  herror_t status;
  
  status = http_output_stream_write_printf(out, 
    "\r\n--"HTTP_BOUNDARY_FMT"--\r\n\r\n", conn, id);
  if (status != H_OK) return status;

  status = http_output_stream_flush(out);
  if (status != H_OK) return status;

  return H_OK;
}

struct _mime_send_param{
  void *conn;
  unsigned int id; 
  http_output_stream_s *out;
};

static herror_t 
__file_send(void *arg, const char *buf, size_t length) 
{
  struct _mime_send_param *param = (struct _mime_send_param *)arg;
  
  herror_t status = mime_write_next(param->conn, param->id, param->out);
  if (status != H_OK)
    return status;

  return http_output_stream_write(param->out, (unsigned char *)buf, length);
}

/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
mime_send_file(void *conn, unsigned int id, 
  http_output_stream_s *out, ng_block_s *file)
{
  herror_t status;
  struct _mime_send_param param;

  param.conn = conn;
  param.id = id;
  param.out = out;
  
  status = mime_write_begin(conn, id, out);
  if (status != H_OK)
    return status;

  status = nanohttp_file_read_all(file->cptr, file->len, __file_send, &param);
  if (status != H_OK)
  {
    herror_log(status);
    return status;
  }

  status = mime_write_end(conn, id, out);
  if (status != H_OK)
    return status;

  log_verbose("file sent!");

  return H_OK;
}

