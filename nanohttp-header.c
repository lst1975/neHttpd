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
#include "nanohttp-defs.h"
#include "nanohttp-buffer.h"
#include "nanohttp-header.h"
#include "nanohttp-ctype.h"

struct _nu{
  int8_t  i;
  uint8_t n;
};

/* Note: must be sorted by length */
/* Note: must be kept in sync with http_header.h enum http_header_e */
/* Note: must be kept in sync http_headers[] and http_headers_off[] */
/* Note: must be kept in sync h2.c:http_header_lc[] */
/* Note: must be kept in sync h2.c:http_header_lshpack_idx[] */
/* Note: must be kept in sync h2.c:lshpack_idx_http_header[] */
/* http_headers_off lists first offset at which string of specific len occur */
#define HTTP_HEADER_OFF_MAX 27
static const struct _nu __rte_aligned(1) http_headers_off[32] = {
  {-1, 0}, {-1, 0}, { 0, 1}, { 1, 4},  
  { 5, 5}, {10, 2}, {12, 6}, {18, 6}, 
  {24, 5}, {29, 2}, {31, 4}, {35, 2}, 
  {37, 1}, {38, 6}, {44, 3}, {47, 5}, 
  {52, 5}, {57, 3}, {60, 1}, {61, 1}, 
  {-1, 0}, {-1, 0}, {62, 1}, {63, 1}, 
  {-1, 0}, {64, 3}, {-1, 0}, {67, 1},
};

#define __HDECL_SORTED(_D) \
   _D(HTTP_HEADER_TE,                          "Te"), \
   _D(HTTP_HEADER_AGE,                         "Age"), \
   _D(HTTP_HEADER_DNT,                         "Dnt"), \
   _D(HTTP_HEADER_VIA,                         "Via"), \
   _D(HTTP_HEADER_P3P,                         "P3p"), \
   _D(HTTP_HEADER_HOST,                        "Host"), \
   _D(HTTP_HEADER_DATE,                        "Date"), \
   _D(HTTP_HEADER_ETAG,                        "Etag"), \
   _D(HTTP_HEADER_VARY,                        "Vary"), \
   _D(HTTP_HEADER_LINK,                        "Link"), \
   _D(HTTP_HEADER_ALLOW,                       "Allow"), \
   _D(HTTP_HEADER_RANGE,                       "Range"), \
   _D(HTTP_HEADER_COOKIE,                      "Cookie"), \
   _D(HTTP_HEADER_ACCEPT,                      "Accept"), \
   _D(HTTP_HEADER_STATUS,                      "Status"), \
   _D(HTTP_HEADER_SERVER,                      "Server"), \
   _D(HTTP_HEADER_EXPECT,                      "Expect"), \
   _D(HTTP_HEADER_PRAGMA,                      "Pragma"), \
   _D(HTTP_HEADER_WARNING,                     "Warning"), \
   _D(HTTP_HEADER_UPGRADE,                     "Upgrade"), \
   _D(HTTP_HEADER_REFERER,                     "Referer"), \
   _D(HTTP_HEADER_TRAILER,                     "Trailer"), \
   _D(HTTP_HEADER_EXPIRES,                     "Expires"), \
   _D(HTTP_HEADER_ALT_SVC,                     "Alt-Svc"), \
   _D(HTTP_HEADER_LOCATION,                    "Location"), \
   _D(HTTP_HEADER_PRIORITY,                    "Priority"), \
   _D(HTTP_HEADER_IF_MATCH,                    "If-Match"), \
   _D(HTTP_HEADER_IF_RANGE,                    "If-Range"), \
   _D(HTTP_HEADER_ALT_USED,                    "Alt-Used"), \
   _D(HTTP_HEADER_FORWARDED,                   "Forwarded"), \
   _D(HTTP_HEADER_EXPECT_CT,                   "Expect-Ct"), \
   _D(HTTP_HEADER_CONNECTION,                  "Connection"), \
   _D(HTTP_HEADER_SET_COOKIE,                  "Set-Cookie"), \
   _D(HTTP_HEADER_USER_AGENT,                  "User-Agent"), \
   _D(HTTP_HEADER_CONTENT_ID,                  "Content-Id"), \
   _D(HTTP_HEADER_RETRY_AFTER,                 "Retry-After"), \
   _D(HTTP_HEADER_CONTENT_MD5,                 "Content-MD5"), \
   _D(HTTP_HEADER_CONTENT_TYPE,                "Content-Type"), \
   _D(HTTP_HEADER_LAST_MODIFIED,               "Last-Modified"), \
   _D(HTTP_HEADER_AUTHORIZATION,               "Authorization"), \
   _D(HTTP_HEADER_IF_NONE_MATCH,               "If-None-Match"), \
   _D(HTTP_HEADER_CACHE_CONTROL,               "Cache-Control"), \
   _D(HTTP_HEADER_ACCEPT_RANGES,               "Accept-Ranges"), \
   _D(HTTP_HEADER_CONTENT_RANGE,               "Content-Range"), \
   _D(HTTP_HEADER_CONTENT_LENGTH,              "Content-Length"), \
   _D(HTTP_HEADER_HTTP2_SETTINGS,              "HTTP2-Settings"), \
   _D(HTTP_HEADER_ONION_LOCATION,              "Onion-Location"), \
   _D(HTTP_HEADER_ACCEPT_ENCODING,             "Accept-Encoding"), \
   _D(HTTP_HEADER_ACCEPT_LANGUAGE,             "Accept-Language"), \
   _D(HTTP_HEADER_REFERRER_POLICY,             "Referrer-Policy"), \
   _D(HTTP_HEADER_X_FORWARDED_FOR,             "X-Forwarded-For"), \
   _D(HTTP_HEADER_X_FRAME_OPTIONS,             "X-Frame-Options"), \
   _D(HTTP_HEADER_CONTENT_LANGUAGE,            "Content-Language"), \
   _D(HTTP_HEADER_WWW_AUTHENTICATE,            "WWW-Authenticate"), \
   _D(HTTP_HEADER_CONTENT_ENCODING,            "Content-Encoding"), \
   _D(HTTP_HEADER_CONTENT_LOCATION,            "Content-Location"), \
   _D(HTTP_HEADER_X_XSS_PROTECTION,            "X-XSS-Protection"), \
   _D(HTTP_HEADER_IF_MODIFIED_SINCE,           "If-Modified-Since"), \
   _D(HTTP_HEADER_TRANSFER_ENCODING,           "Transfer-Encoding"), \
   _D(HTTP_HEADER_X_FORWARDED_PROTO,           "X-Forwarded-Proto"), \
   _D(HTTP_HEADER_PROXY_AUTHENTICATE,          "Proxy-Authenticate"), \
   _D(HTTP_HEADER_IF_UNMODIFIED_SINCE,         "If-Unmodified-Since"), \
   _D(HTTP_HEADER_X_CONTENT_TYPE_OPTIONS,      "X-Content-Type-Options"), \
   _D(HTTP_HEADER_CONTENT_SECURITY_POLICY,     "Content-Security-Policy"), \
   _D(HTTP_HEADER_STRICT_TRANSPORT_SECURITY,   "Strict-Transport-Security"), \
   _D(HTTP_HEADER_UPGRADE_INSECURE_REQUESTS,   "Upgrade-Insecure-Requests"), \
   _D(HTTP_HEADER_CONTENT_TRANSFER_ENCODING,   "Content-Transfer-Encoding"), \
   _D(HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "Access-Control-Allow-Origin"),

#define X(__n,__s) DECL_CONST_STR_N(__n,__s)
static const ng_block_s __rte_cache_aligned __http_headers_sort[] = {
  __HDECL_SORTED(X)
};
#undef X

#define X(__n,__s) [__n] = DECL_CONST_STR_N(__n,__s)
const ng_block_s __rte_cache_aligned ng_http_headers_byid[] = {
  __HDECL_SORTED(X)
};
#undef X

const ng_block_s *http_header_find(const char *key, int keyLength)
{
  struct _nu off;
  if (keyLength > HTTP_HEADER_OFF_MAX)
    return NULL;
  
  off = http_headers_off[keyLength];
  for (int i=0; i <off.n; i++) {
    const ng_block_s *s = &__http_headers_sort[off.i+i];
    if (ng_tolower(*s->cptr) == ng_tolower(*key)
      && !ng_strnocasecmp(s->cptr, key, keyLength))
    {
      return s;
    }
  }

  return NULL;
}
