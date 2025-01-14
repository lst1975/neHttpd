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
#include "nanohttp-defs.h"
#include "nanohttp-buffer.h"
#include "nanohttp-header.h"
#include "nanohttp-ctype.h"
#include "nanohttp-error.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-code.h"

struct _nu{
  ng_int8_t  i;
  ng_uint8_t n;
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
  {52, 5}, {57, 3}, {60, 1}, {61, 3}, 
  {-1, 0}, {-1, 0}, {64, 1}, {65, 1}, 
  {-1, 0}, {66, 3}, {-1, 0}, {69, 1},
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
   _D(HTTP_HEADER_PROXY_AUTHORIZATION,         "Proxy-Authorization"), \
   _D(HTTP_HEADER_IF_UNMODIFIED_SINCE,         "If-Unmodified-Since"), \
   _D(HTTP_HEADER_CONTENT_DISPOSITION,         "Content-Disposition"), \
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

/*
 * Tokens as defined by rfc 2616. Also lowercases them.
 *        token       = 1*<any CHAR except CTLs or separators>
 *     separators     = "(" | ")" | "<" | ">" | "@"
 *                    | "," | ";" | ":" | "\" | <">
 *                    | "/" | "[" | "]" | "?" | "="
 *                    | "{" | "}" | SP | HT
 *
 *
 * Header field name as defined by rfc 2616. Also lowercases them.
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
 //  0 nul   1 soh   2 stx   3 etx   4 eot   5 enq   6 ack   7 bel   
     0,      0,      0,      0,      0,      0,      0,      0,
 //  8 bs    9 ht    10 nl   11 vt   12 np   13 cr   14 so   15 si   
     0,      0,      0,      0,      0,      0,      0,      0,
 //  16 dle  17 dc1  18 dc2  19 dc3  20 dc4  21 nak  22 syn  23 etb  
     0,      0,      0,      0,      0,      0,      0,      0,
 //  24 can  25 em   26 sub  27 esc  28 fs   29 gs   30 rs   31 us   
     0,      0,      0,      0,      0,      0,      0,      0,
 //  32 sp   33 !    34 "    35 #    36 $    37 %    38 &    39 '    
     0,      '!',    0,      '#',    '$',    '%',    '&',    '\'',
 //  40 (    41 )    42 *    43 +    44 ,    45 -    46 .    47 /    
     0,      0,      '*',    '+',    0,      '-',    '.',    0,
 //  48 0    49 1    50 2    51 3    52 4    53 5    54 6    55 7   
     '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
 //  56 8    57 9    58 :    59 ;    60 <    61 =    62 >    63 ?    
     '8',    '9',    0,      0,      0,      0,      0,      0,
 //  64 @    65 A    66 B    67 C    68 D    69 E    70 F    71 G    
     0,      'A',    'B',    'C',    'D',    'E',    'F',    'G',
 //  72 H    73 I    74 J    75 K    76 L    77 M    78 N    79 O    
     'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
 //  80 P    81 Q    82 R    83 S    84 T    85 U    86 V    87 W    
     'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
 //  88 X    89 Y    90 Z    91 [    92 \    93 ]    94 ^    95 _    
     'X',    'Y',    'Z',     0,     0,      0,      '^',    '_',
 //  96 `    97 a    98 b    99 c    100 d   101 e   102 f   103 g   
     '`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
 //  104 h   105 i   106 j   107 k   108 l   109 m   110 n   111 o   
     'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
 //  112 p   113 q   114 r   115 s   116 t   117 u   118 v   119 w   
     'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
 //  120 x   121 y   122 z   123 {   124 |   125 }   126 ~   127 del
     'x',    'y',    'z',    0,      '|',     0,     '~',    0,
 */
#if __HTTP_SMALL_SIZE
const ng_uint8_t __rte_aligned(1) __isValidToken[256] ={
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
#else
const ng_uint8_t __rte_aligned(1) __isValidFieldName[256] ={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,
  
  /* ! => 33
   # => 35
   $ => 36
   % => 37
   & => 38
   ' => 39
   * => 42
   + => 43
   - => 45
   . => 46
   (ch >= 33 && ch <= 46) &&
   (ch != 34 && ch != 40 && ch != 41 && ch != 44)
  */
  1,0,1,1,1,1,1,0,0,1,1,0,1,1,
  0,
  
  /*
    DIGIT (decimal 0-9): ch >= 48 && ch <= 57
  */
  1,1,1,1,1,1,1,1,1,1,
  
  0,0,0,0,0,0,0,
  
  /*
   ALPHA: A-Z, (ch >= 65 && ch <= 90)
  */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,
  0,0,0,
  
  /* 
     ^ => 94
     _ => 95
     ` => 96
  */
  1,1,1,
  
  /*
   ALPHA: a-z, (ch >= 97 && ch <= 122)
  */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,

  /*
    | => 124
    ~ => 126
   */
  0,1,0,1,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0
};
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
const ng_uint8_t __rte_aligned(1) __isValidFieldValue[256] ={
  0,0,0,0,
  0,0,0,0,
  0,

  /* 
   9 HTAB &#09;   Horizontal Tab
  */
  1,
  
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  0,0,
  
  /* 
   32 SP	&#32;	 	Space
  */
  1, 
  
  /************************************************************* 
   VCHAR          =  %x21-7E[visible (printing) characters]
     ; 33-126
   *************************************************************/
   
  /*
   33	! &#33;	&excl;	Exclamation mark
   34	"	&#34;	&quot;	Double quotes (or speech marks)
   35	#	&#35;	&num;	Number sign
   36	$	&#36;	&dollar;	Dollar
   37	%	&#37;	&percnt;	Per cent sign
   38	&	&#38;	&amp;	Ampersand
   39	'	&#39;	&apos;	Single quote
   40	(	&#40;	&lparen;	Open parenthesis (or open bracket)
   41	)	&#41;	&rparen;	Close parenthesis (or close bracket)
   42	*	&#42;	&ast;	Asterisk
   43	+	&#43;	&plus;	Plus
   44	,	&#44;	&comma;	Comma
   45	-	&#45;	Hyphen-minus
   46	.	&#46;	&period;	Period, dot or full stop
   47	/	&#47;	&sol;	Slash or divide
  */
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,

  /* 
   48  0 &#48;   Zero
   49  1 &#49;   One
   50  2 &#50;   Two
   51  3 &#51;   Three
   52  4 &#52;   Four
   53  5 &#53;   Five
   54  6 &#54;   Six
   55  7 &#55;   Seven
   56  8 &#56;   Eight
   57  9 &#57;   Nine
  */
  1,1,1,1,
  1,1,1,1,
  1,1,

  /* 
   58  : &#58; &colon; Colon
   59  ; &#59; &semi;  Semicolon
   60  < &#60; &lt;  Less than (or open angled bracket)
   61  = &#61; &equals;  Equals
   62  > &#62; &gt;  Greater than (or close angled bracket)
   63  ? &#63; &quest; Question mark
   64  @ &#64; &commat;  At sign
  */
  1,1,1,1,
  1,1,1,
  
  /* 65-90 A-Z */
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,

  /* 
   91  [ &#91; &lsqb;  Opening bracket
   92  \ &#92; &bsol;  Backslash
   93  ] &#93; &rsqb;  Closing bracket
   94  ^ &#94; &Hat; Caret - circumflex
   95  _ &#95; &lowbar;  Underscore
   96  ` &#96;	&grave;	Grave accent
  */
  1,1,1,1,
  1,1,
  
  /* 97-122 a-z */
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,

  /* 
  123  { &#123;  &lcub;  Opening brace
  124  | &#124;  &verbar;  Vertical bar
  125  } &#125;  &rcub;  Closing brace
  126  ~ &#126;  &tilde; Equivalency sign - tilde
  */
  1,1,1,1,
  
  /*
   127	DEL	&#127;	 	Delete
  */
  0,
  
  /*
  obs-text       = %x80-FF
  */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
#endif

void http_header_parser_init(header_parser_s *p, int type)
{
  ng_INIT_LIST_HEAD(&p->header);
  BUF_SIZE_INIT(&p->data, p->buffer, sizeof(p->buffer));
  p->state = __HTTP_header_state_0;
  switch (type)
  {
    case __HTTP_header_type_request:
      p->method  = HTTP_REQUEST_METHOD_UNKOWN;
      p->version = HTTP_VERSION_UNKOWN;
      ng_block_init(&p->path);
      break;
    case __HTTP_header_type_response:
      p->code    = HTTP_RESPONSE_CODE_000_UNKNOWN;
      p->version = HTTP_VERSION_UNKOWN;
      ng_block_init(&p->desc);
      break;
    default:
      assert(0);
      break;
  }
}
