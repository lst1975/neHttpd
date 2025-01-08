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
#include "nanohttp-code.h"

/*
   HTTP Status Codes
  
     Registration Procedure(s)
  
   IETF Review
  
     Reference [RFC9110, Section 16.2.1]
  
   Note
  
     1xx: Informational - Request received, continuing process
     2xx: Success - The action was successfully received, understood, and accepted
     3xx: Redirection - Further action must be taken in order to complete the request
     4xx: Client Error - The request contains bad syntax or cannot be fulfilled
     5xx: Server Error - The server failed to fulfill an apparently valid request
  
   Available Formats
  
     Value  Description                                   Reference
      100   Continue                                      [RFC9110, Section 15.2.1]
      101   Switching Protocols                           [RFC9110, Section 15.2.2]
      102   Processing                                    [RFC2518]
      103   Early Hints                                   [RFC8297]
      104   Upload Resumption Supported (TEMPORARY - 
            registered 2024-11-13, expires 2025-11-13)    [draft-ietf-httpbis-resumable-upload-05]
    105-199 Unassigned
      200   OK                                            [RFC9110, Section 15.3.1]
      201   Created                                       [RFC9110, Section 15.3.2]
      202   Accepted                                      [RFC9110, Section 15.3.3]
      203   Non-Authoritative Information                 [RFC9110, Section 15.3.4]
      204   No Content                                    [RFC9110, Section 15.3.5]
      205   Reset Content                                 [RFC9110, Section 15.3.6]
      206   Partial Content                               [RFC9110, Section 15.3.7]
      207   Multi-Status                                  [RFC4918]
      208   Already Reported                              [RFC5842]
    209-225 Unassigned
      226   IM Used                                       [RFC3229]
    227-299 Unassigned
      300   Multiple Choices                              [RFC9110, Section 15.4.1]
      301   Moved Permanently                             [RFC9110, Section 15.4.2]
      302   Found                                         [RFC9110, Section 15.4.3]
      303   See Other                                     [RFC9110, Section 15.4.4]
      304   Not Modified                                  [RFC9110, Section 15.4.5]
      305   Use Proxy                                     [RFC9110, Section 15.4.6]
      306   (Unused)                                      [RFC9110, Section 15.4.7]
      307   Temporary Redirect                            [RFC9110, Section 15.4.8]
      308   Permanent Redirect                            [RFC9110, Section 15.4.9]
    309-399 Unassigned
      400   Bad Request                                   [RFC9110, Section 15.5.1]
      401   Unauthorized                                  [RFC9110, Section 15.5.2]
      402   Payment Required                              [RFC9110, Section 15.5.3]
      403   Forbidden                                     [RFC9110, Section 15.5.4]
      404   Not Found                                     [RFC9110, Section 15.5.5]
      405   Method Not Allowed                            [RFC9110, Section 15.5.6]
      406   Not Acceptable                                [RFC9110, Section 15.5.7]
      407   Proxy Authentication Required                 [RFC9110, Section 15.5.8]
      408   Request Timeout                               [RFC9110, Section 15.5.9]
      409   Conflict                                      [RFC9110, Section 15.5.10]
      410   Gone                                          [RFC9110, Section 15.5.11]
      411   Length Required                               [RFC9110, Section 15.5.12]
      412   Precondition Failed                           [RFC9110, Section 15.5.13]
      413   Content Too Large                             [RFC9110, Section 15.5.14]
      414   URI Too Long                                  [RFC9110, Section 15.5.15]
      415   Unsupported Media Type                        [RFC9110, Section 15.5.16]
      416   Range Not Satisfiable                         [RFC9110, Section 15.5.17]
      417   Expectation Failed                            [RFC9110, Section 15.5.18]
      418   (Unused)                                      [RFC9110, Section 15.5.19]
    419-420 Unassigned
      421   Misdirected Request                           [RFC9110, Section 15.5.20]
      422   Unprocessable Content                         [RFC9110, Section 15.5.21]
      423   Locked                                        [RFC4918]
      424   Failed Dependency                             [RFC4918]
      425   Too Early                                     [RFC8470]
      426   Upgrade Required                              [RFC9110, Section 15.5.22]
      427   Unassigned
      428   Precondition Required                         [RFC6585]
      429   Too Many Requests                             [RFC6585]
      430   Unassigned
      431   Request Header Fields Too Large               [RFC6585]
    432-450 Unassigned
      451   Unavailable For Legal Reasons                 [RFC7725]
    452-499 Unassigned
      500   Internal Server Error                         [RFC9110, Section 15.6.1]
      501   Not Implemented                               [RFC9110, Section 15.6.2]
      502   Bad Gateway                                   [RFC9110, Section 15.6.3]
      503   Service Unavailable                           [RFC9110, Section 15.6.4]
      504   Gateway Timeout                               [RFC9110, Section 15.6.5]
      505   HTTP Version Not Supported                    [RFC9110, Section 15.6.6]
      506   Variant Also Negotiates                       [RFC2295]
      507   Insufficient Storage                          [RFC4918]
      508   Loop Detected                                 [RFC5842]
      509   Unassigned
      510   Not Extended (OBSOLETED)                      [RFC2774][Status change of HTTP experiments to Historic]
      511   Network Authentication Required               [RFC6585]
    512-599 Unassigned
  */
    
#define _DLC(c,t,n,d) [c] = { \
    .name=DECL_CONST_STR(n), \
    .desc=DECL_CONST_STR(d), \
    .code=c, \
    .code_int_str=DECL_CONST_STR(#c), \
    .type=HTTP_StatusCodeTYpe_ ## t, \
  }
#define _DLC_NULL(x,y) [x ... y] = { \
    .name=DECL_CONST_STR_NULL(), \
    .desc=DECL_CONST_STR_NULL(), \
    .code=0, \
    .code_int_str=DECL_CONST_STR_NULL(), \
    .type=HTTP_StatusCodeTYpe_NONE, \
  }
static const http_code_s http_status_code[]={
  _DLC_NULL(0,99),
  _DLC(100, InformationalResponses, "100 Continue",
            "The HTTP 100 Continue informational status response code indicates that everything so far is "
            "OK and that the client should continue with the request or ignore it if it is already finished. "),

  _DLC(101, InformationalResponses, "101 Switching Protocol",
            "The HTTP 101 Switching Protocols response code indicates the protocol the server is switching "
            "to as requested by a client which sent the message including the Upgrade request header. "),

  _DLC(102, InformationalResponses, "102 Processing",
            "This code was used in WebDAV contexts to indicate that a request has been received by the server, "
      			"but no status was available at the time of the response."),

  _DLC(103, InformationalResponses, "103 Early Hints",
            "The HTTP 103 Early Hints information response status code is primarily intended to be used with "
            "the Link header to allow the user agent to start preloading resources while the server is still "
            "preparing a response. "),

  _DLC(104, InformationalResponses, "104 Upload Resumption Supported",
            "Once the upload resource is available and while the request content is being uploaded, the target "
            "resource MAY send one or more informational responses with a 104 (Upload Resumption Supported) "
            "status code to the client. "),

  _DLC_NULL(105,199),

  _DLC(200, SuccessfulResponses, "200 OK",
            "The HTTP 200 OK success status response code indicates that the request has succeeded. A 200 "
            "response is cacheable by default. "),

  _DLC(201, SuccessfulResponses, "201 Created",
            "The HTTP 201 Created success status response code indicates that the request has succeeded and "
            "has led to the creation of a resource. The new resource is effectively created before this response "
            "is sent back and the new resource is returned in the body of the message, its location being "
            "either the URL of the request, or the content of the Location header. "),

  _DLC(202, SuccessfulResponses, "202 Accepted",
            "The HyperText Transfer Protocol (HTTP) 202 Accepted response status code indicates that the "
            "request has been received but not yet acted upon. It is non-committal, meaning that there is "
            "no way for the HTTP to later send an asynchronous response indicating the outcome of processing "
            "the request. It is intended for cases where another process or server handles the request, or "
            "for batch processing. "),

  _DLC(203, SuccessfulResponses, "203 Non-Authoritative Information",
            "The HTTP 203 Non-Authoritative Information response status indicates that the request was successful "
            "but the enclosed payload has been modified by a transforming proxy from that of the origin server's "
            "200 (OK) response . "),

  _DLC(204, SuccessfulResponses, "204 No Content",
            "The HTTP 204 No Content success status response code indicates that the request has succeeded, "
            "but that the client doesn't need to go away from its current page. A 204 response is cacheable "
            "by default. An ETag header is included in such a response. "),

  _DLC(205, SuccessfulResponses, "205 Reset Content",
            "The HTTP 205 Reset Content response status tells the client to reset the document view, so for "
            "example to clear the content of a form, reset a canvas state, or to refresh the UI. "),

  _DLC(206, SuccessfulResponses, "206 Partial Content",
            "The HTTP 206 Partial Content success status response code indicates that the request has succeeded "
            "and has the body contains the requested ranges of data, as described in the Range header of "
            "the request. "),
    
  _DLC(207, SuccessfulResponses, "207 Multi-Status",
            "Conveys WebDAV information about multiple resources, for situations where multiple status codes might "
      			"be appropriate."),
    
  _DLC(208, SuccessfulResponses, "208 Already Reported",
            "Used inside a <dav:propstat> response element to avoid repeatedly enumerating the internal members "
      			"of multiple bindings to the same collection. (WebDAV)"),

  _DLC_NULL(209,225),

  _DLC(226, SuccessfulResponses, "226 IM Used",
            "The server has fulfilled a GET request for the resource, and the response is a representation of the "
      			"result of one or more instance-manipulations applied to the current instance. (HTTP Delta encoding)"),

  _DLC_NULL(227,299),

  _DLC(300, Redirects, "300 Multiple Choices",
            "The HTTP 300 Multiple Choices redirect status response code indicates that the request has more "
            "than one possible responses. The user-agent or the user should choose one of them. As there "
            "is no standardized way of choosing one of the responses, this response code is very rarely used. "),

  _DLC(301, Redirects, "301 Moved Permanently",
            "The HyperText Transfer Protocol (HTTP) 301 Moved Permanently redirect status response code indicates "
            "that the resource requested has been definitively moved to the URL given by the Location headers. "
            "A browser redirects to this page and search engines update their links to the resource (in 'SEO-speak', "
            "it is said that the 'link-juice' is sent to the new URL). "),

  _DLC(302, Redirects, "302 Found",
            "The HyperText Transfer Protocol (HTTP) 302 Found redirect status response code indicates that "
            "the resource requested has been temporarily moved to the URL given by the Location header. A "
            "browser redirects to this page but search engines don't update their links to the resource (in "
            "'SEO-speak', it is said that the 'link-juice' is not sent to the new URL). "),

  _DLC(303, Redirects, "303 See Other",
            "The HyperText Transfer Protocol (HTTP) 303 See Other redirect status response code indicates "
            "that the redirects don't link to the newly uploaded resources but to another page, like a confirmation "
            "page or an upload progress page. This response code is usually sent back as a result of PUT "
            "or POST. The method used to display this redirected page is always GET. "),

  _DLC(304, Redirects, "304 Not Modified",
            "The HTTP 304 Not Modified client redirection response code indicates that there is no need to "
            "retransmit the requested resources. It is an implicit redirection to a cached resource. This "
            "happens when the request method is safe, like a GET or a HEAD request, or when the request is "
            "conditional and uses a If-None-Match or a If-Modified-Since header. "),

  _DLC(305, Redirects, "305 Use Proxy",
            "Defined in a previous version of the HTTP specification to indicate that a requested response "
      			"must be accessed by a proxy. It has been deprecated due to security concerns regarding in-band "
      			"configuration of a proxy."),

  _DLC_NULL(306,306),

  _DLC(307, Redirects, "307 Temporary Redirect",
            "HTTP 307 Temporary Redirect redirect status response code indicates that the resource requested "
            "has been temporarily moved to the URL given by the Location headers. "),

  _DLC(308, Redirects, "308 Permanent Redirect",
            "The HyperText Transfer Protocol (HTTP) 308 Permanent Redirect redirect status response code "
            "indicates that the resource requested has been definitively moved to the URL given by the Location "
            "headers. A browser redirects to this page and search engines update their links to the resource "
            "(in 'SEO-speak', it is said that the 'link-juice' is sent to the new URL). "),

  _DLC_NULL(309,399),

  _DLC(400, ClientErrors, "400 Bad Request",
            "The HyperText Transfer Protocol (HTTP) 400 Bad Request response status code indicates that the "
            "server cannot or will not process the request due to something that is perceived to be a client "
            "error (e.g., malformed request syntax, invalid request message framing, or deceptive request "
            "routing). "),

  _DLC(401, ClientErrors, "401 Unauthorized",
            "The HTTP 401 Unauthorized client error status response code indicates that the request has not "
            "been applied because it lacks valid authentication credentials for the target resource. "),

  _DLC(402, ClientErrors, "402 Payment Required",
            "The HTTP 402 Payment Required is a nonstandard client error status response code that is reserved "
            "for future use. "),

  _DLC(403, ClientErrors, "403 Forbidden",
            "The HTTP 403 Forbidden client error status response code indicates that the server understood "
            "the request but refuses to authorize it. "),

  _DLC(404, ClientErrors, "404 Not Found",
            "The HTTP 404 Not Found client error response code indicates that the server can't find the requested "
            "resource. Links which lead to a 404 page are often called broken or dead links, and can be subject "
            "to link rot. "),

  _DLC(405, ClientErrors, "405 Method Not Allowed",
            "The HyperText Transfer Protocol (HTTP) 405 Method Not Allowed response status code indicates "
            "that the request method is known by the server but is not supported by the target resource. "),

  _DLC(406, ClientErrors, "406 Not Acceptable",
            "The HyperText Transfer Protocol (HTTP) 406 Not Acceptable client error response code indicates "
            "that the server cannot produce a response matching the list of acceptable values defined in "
            "the request's proactive content negotiation headers, and that the server is unwilling to supply "
            "a default representation. "),

  _DLC(407, ClientErrors, "407 Proxy Authentication Required",
            "The HTTP 407 Proxy Authentication Required  client error status response code indicates that "
            "the request has not been applied because it lacks valid authentication credentials for a proxy "
            "server that is between the browser and the server that can access the requested resource. "),

  _DLC(408, ClientErrors, "408 Request Timeout",
            "The HyperText Transfer Protocol (HTTP) 408 Request Timeout response status code means that the "
            "server would like to shut down this unused connection. It is sent on an idle connection by some "
            "servers, even without any previous request by the client. "),

  _DLC(409, ClientErrors, "409 Conflict",
            "This response is sent when a request conflicts with the current state of the server. In WebDAV "
            "remote web authoring, 409 responses are errors sent to the client so that a user might be able "
            "to resolve a conflict and resubmit the request."),

  _DLC(410, ClientErrors, "410 Gone",
            "This response is sent when the requested content has been permanently deleted from server, with "
            "no forwarding address. Clients are expected to remove their caches and links to the resource. "
            "The HTTP specification intends this status code to be used for \"limited-time, promotional "
            "services\". APIs should not feel compelled to indicate resources that have been deleted with "
            "this status code."),

  _DLC(411, ClientErrors, "411 Length Required",
            "The HyperText Transfer Protocol (HTTP) 411 Length Required client error response code indicates "
            "that the server refuses to accept the request without a defined Content-Length header. "),

  _DLC(412, ClientErrors, "412 Precondition Failed",
            "The HyperText Transfer Protocol (HTTP) 412 Precondition Failed client error response code indicates "
            "that access to the target resource has been denied. This happens with conditional requests on "
            "methods other than GET or HEAD when the condition defined by the If-Unmodified-Since or If-None-Match "
            "headers is not fulfilled. In that case, the request, usually an upload or a modification of "
            "a resource, cannot be made and this error response is sent back. "),

  _DLC(413, ClientErrors, "413 Payload Too Large",
            "The HTTP 413 Payload Too Large response status code indicates that the request entity is larger "
            "than limits defined by server; the server might close the connection or return a Retry-After "
            "header field. "),

  _DLC(414, ClientErrors, "414 URI Too Long",
            "The HTTP 414 URI Too Long response status code indicates that the URL requested by the client "
            "is longer than the server is willing to interpret. "),

  _DLC(415, ClientErrors, "415 Unsupported Media Type",
            "The HTTP 415 Unsupported Media Type client error response code indicates that the server refuses "
            "to accept the request because the payload format is in an unsupported format. "),

  _DLC(416, ClientErrors, "416 Range Not Satisfiable",
            "The HyperText Transfer Protocol (HTTP) 416 Range Not Satisfiable error response code indicates "
            "that a server cannot serve the requested ranges. The most likely reason is that the document "
            "doesn't contain such ranges, or that the Range header value, though syntactically correct, doesn't "
            "make sense. "),

  _DLC(417, ClientErrors, "417 Expectation Failed",
            "The HTTP 417 Expectation Failed client error response code indicates that the expectation given "
            "in the request's Expect header could not be met. "),

  _DLC(418, ClientErrors, "418 I'm a teapot",
            "The HTTP 418 I'm a teapot client error response code indicates that the server refuses to brew "
            "coffee because it is a teapot. This error is a reference to Hyper Text Coffee Pot Control Protocol "
            "which was an April Fools' joke in 1998. "),

  _DLC_NULL(419,420),

  _DLC(421, ClientErrors, "421 Misdirected Request",
            "The request was directed at a server that is not able to produce a response. This can be sent by "
      			"a server that is not configured to produce responses for the combination of scheme and authority "
      			"that are included in the request URI."),

  _DLC(422, ClientErrors, "422 Unprocessable Content", 
      			"The request was well-formed but was unable to be followed due to semantic errors. (WebDAV)"),

  _DLC(423, ClientErrors, "423 Locked", 
      			"The resource that is being accessed is locked. (WebDAV)"),

  _DLC(424, ClientErrors, "424 Failed Dependency", 
      			"The request failed due to failure of a previous request. (WebDAV)"),

  _DLC(425, ClientErrors, "425 Too Early",
            "The HyperText Transfer Protocol (HTTP) 425 Too Early response status code indicates that the "
            "server is unwilling to risk processing a request that might be replayed, which creates the potential "
            "for a replay attack. "),

  _DLC(426, ClientErrors, "426 Upgrade Required",
            "The HTTP 426 Upgrade Required client error response code indicates that the server refuses to "
            "perform the request using the current protocol but might be willing to do so after the client "
            "upgrades to a different protocol. "),

  _DLC_NULL(427,427),

  _DLC(428, ClientErrors, "428 Precondition Required",
            "The HTTP 428 Precondition Required response status code indicates that the server requires the "
            "request to be conditional. "),

  _DLC(429, ClientErrors, "429 Too Many Requests",
            "The HTTP 429 Too Many Requests response status code indicates the user has sent too many requests "
            "in a given amount of time ('rate limiting'). "),

  _DLC_NULL(430,430),

  _DLC(431, ClientErrors, "431 Request Header Fields Too Large",
            "The HTTP 431 Request Header Fields Too Large response status code indicates that the server "
            "refuses to process the request because the request??s HTTP headers are too long. The request "
            "may be resubmitted after reducing the size of the request headers. "),

  _DLC_NULL(432,450),

  _DLC(451, ClientErrors, "451 Unavailable For Legal Reasons",
            "The HyperText Transfer Protocol (HTTP) 451 Unavailable For Legal Reasons client error response "
            "code indicates that the user requested a resource that is not available due to legal reasons, "
            "such as a web page for which a legal action has been issued. "),

  _DLC_NULL(452,499),

  _DLC(500, ServerErrors, "500 Internal Server Error",
            "The HyperText Transfer Protocol (HTTP) 500 Internal Server Error server error response code "
            "indicates that the server encountered an unexpected condition that prevented it from fulfilling "
            "the request. "),

  _DLC(501, ServerErrors, "501 Not Implemented",
            "The HyperText Transfer Protocol (HTTP) 501 Not Implemented server error response code means "
            "that the server does not support the functionality required to fulfill the request. "),

  _DLC(502, ServerErrors, "502 Bad Gateway",
            "The information shown below has been pulled from MDN's GitHub (https://github.com/mdn/browser-compat-data). "),

  _DLC(503, ServerErrors, "503 Service Unavailable",
            "The HyperText Transfer Protocol (HTTP) 503 Service Unavailable server error response code indicates "
            "that the server is not ready to handle the request. "),

  _DLC(504, ServerErrors, "504 Gateway Timeout",
            "The HyperText Transfer Protocol (HTTP) 504 Gateway Timeout server error response code indicates "
            "that the server, while acting as a gateway or proxy, did not get a response in time from the "
            "upstream server that it needed in order to complete the request. "),

  _DLC(505, ServerErrors, "505 HTTP Version Not Supported",
            "The HyperText Transfer Protocol (HTTP) 505 HTTP Version Not Supported response status code indicates "
            "that the HTTP version used in the request is not supported by the server. "),

  _DLC(506, ServerErrors, "506 Variant Also Negotiates",
            "The HyperText Transfer Protocol (HTTP) 506 Variant Also Negotiates response status code may "
            "be given in the context of Transparent Content Negotiation (see RFC 2295). This protocol enables "
            "a client to retrieve the best variant of a given resource, where the server supports multiple "
            "variants. "),

  _DLC(507, ServerErrors, "507 Insufficient Storage",
            "The HyperText Transfer Protocol (HTTP)  507 Insufficient Storage response status code may be "
            "given in the context of the Web Distributed Authoring and Versioning (WebDAV) protocol (see "
            "RFC 4918). "),

  _DLC(508, ServerErrors, "508 Loop Detected",
            "The HyperText Transfer Protocol (HTTP) 508 Loop Detected response status code may be given in "
            "the context of the Web Distributed Authoring and Versioning (WebDAV) protocol. "),

  _DLC_NULL(509,509),

  _DLC(510, ServerErrors, "510 Not Extended",
            "The client request declares an HTTP Extension (RFC 2774) that should be used to process the request, "
            "but the extension is not supported. (OBSOLETED)"),

  _DLC(511, ServerErrors, "511 Network Authentication Required",
            "The HTTP 511 Network Authentication Required response status code indicates that the client "
            "needs to authenticate to gain network access. "),
};

static const http_code_s *__http_find_bycode(unsigned int code)
{
  if (code < 100 || code >= NG_ITEMS(http_status_code))
    return NULL;
  const http_code_s *c = &http_status_code[code];
  if (c->type == HTTP_StatusCodeTYpe_NONE)
    return NULL;
  return c;
}

const ng_block_s *http_int_code2str(unsigned int code)
{
  const http_code_s *c = __http_find_bycode(code);
  return c==NULL ? NULL : &c->code_int_str;
}

const ng_block_s *http_int_code2name(unsigned int code)
{
  const http_code_s *c = __http_find_bycode(code);
  return c==NULL ? NULL : &c->name;
}

const ng_block_s *http_int_code2desc(unsigned int code)
{
  const http_code_s *c = __http_find_bycode(code);
  return c==NULL ? NULL : &c->desc;
}

