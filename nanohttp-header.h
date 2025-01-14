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
#ifndef __nanohttp_header_h
#define __nanohttp_header_h

#include "nanohttp-buffer.h"
#include "nanohttp-common.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define HTTP_FILED_NAME_LEN_MAX  128
#define HTTP_FILED_VALUE_LEN_MAX 2048

/* @ingroup NANOHTTP
 *
 * There are a few header fields which have general applicability for both
 * request and response messages, but which do not apply to the entity being
 * transferred. These header fields apply only to the General-header field names
 * can be extended reliably only in combination with a change in the protocol
 * version. However, new or experimental header fields may be given the semantics
 * of general header fields if all parties in the communication recognize them to
 * be general-header fields. Unrecognized header fields are treated as
 * entity-header fields.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 * Entity Header Fields
 *
 * Entity-header fields define metainformation about the entity-body or, if no
 * body is present, about the resource identified by the request. Some of this
 * metainformation is OPTIONAL; some might be REQUIRED by portions of this
 * specification. (see RFC 2616 section 7.1)
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */

/* HTTP header enum for select HTTP field-names
 * reference:
 *   https://www.iana.org/assignments/message-headers/message-headers.xml
 *   https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
 *
 * Note: must be kept in sync with http_header.c http_headers[] 
 * Note: must be kept in sync h2.c:http_header_lc[] 
 * Note: must be kept in sync h2.c:http_header_lshpack_idx[] 
 * Note: must be kept in sync h2.c:lshpack_idx_http_header[] 
 * Note: when adding new items, must replace OTHER in existing code for item
 * Note: current implementation has limit of 64 htags
 *       Use of htags is an optimization for quick existence checks in lighttpd.
 *       (In the future, these values may also be used to map to HPACK indices.)
 *       However, listing all possible headers here is highly discouraged,
 *       as extending the bitmap greater than 64-bits may make quick bitmasks
 *       check more expensive, and the cost for looking up unmarked headers
 *       (HTTP_HEADER_OTHER) is not substantially more.  In the future, this
 *       list may be revisited and reviewed, and less frequent headers removed
 *       or replaced.
 */
typedef enum __http_header nghttp_header_e;
enum __http_header {
  HTTP_HEADER_OTHER = 0
 ,HTTP_HEADER_ACCEPT
  /**
   Accept-Encoding
   Last Updated : 29 Aug, 2024
   The HTTP headers Accepts-Encoding is usually a comparison algorithm of 
   request header. All the HTTP client used to tell the server which 
   encoding or encodings it supports. Then the server will respond in 
   any of the supporting encoding formats. The server selects any one 
   of the proposals, uses it and informs the client of its choice with 
   the Content-Encoding response header.
   
   Syntax:
   
   Accept-Encoding: gzip | compress | deflate | br| identity| *
   Note:
   
   Multiple algorithm are also can be applicable.
   
   Directives:
   
   gzip: 
         It is a compression format using the Lempel-Ziv coding (LZ77), 
         with a 32-bit CRC.
   compress: 
         It is a compression format using the Lempel-Ziv-Welch (LZW) 
         algorithm.
   deflate: 
         It is a compression format using the zlib structure, with the 
         deflate compression algorithm.
   br: 
         It is a compression format using the Brotli algorithm.
   identity: 
         This directive Indicates the identity function which is always 
         acceptable.
   *: 
         This directive matches any content encoding which can be listed 
         in the header or not. It is the default value.
         
     You can check how good your
     
     Accept-Encoding
     
     and
     
     Content-Encoding
     
     is working on this
     
     site
     
     .
     
     Example:
     
     Accept-Encoding: gzip, deflate, br
     Accept-Encoding: br;q=1.0, gzip;q=0.6, *;q=0.1
   */
 ,HTTP_HEADER_ACCEPT_ENCODING
   /**
    Accept-Language
    Last Updated : 31 Oct, 2019
      This HTTP Accept-Language header tells the server about all the languages 
      that the client can understand. With the help of content negotiation, 
      there will be a set of supported languages in the HTTP Accept-Language 
      proposal then the server selects one of the proposals of those languages 
      and place that in the content-language header. In a few cases users can 
      change the languages manually otherwise server detects the supported 
      language by the browser¡¯s language. But remember one thing that it is 
      recommended that the server never overrides an explicit decision. If the 
      user is comfortable with a language that does not list on the server 
      language then the server can not serve the matching language to the 
      client in that case 406 Not Acceptable status code will be sent.
    
    Syntax:
    
    This is the specific language selection syntax.
    Accept-Language: <language>
      This syntax works as a wild card (selects all languages).
    Accept-Language: *
      Note: Multiple languages can be listed by using commas and the optional.
      Directives: 
        This header accepts two directives as mentioned above and described below:
    
        <language>: 
            This consists of a 2-3 letter base language tag representing the 
            language, followed by sub-tags separated by ¡®-¡® . The extra 
            information is the region and country variant 
            (like ¡®en-US¡¯ or ¡®fr-CA¡¯)
        *: 
            It is used as a wildcard for any language present.
        Note: 
            The ;q= it defines the factor weighting, value placed in an order 
            of preference expressed using a relative quality value
    Examples:
    
      In this example single value is on Accept-Language header that is English of US.
    accept-language: en-US
      In this example double value is on Accept-Language header that is English of US 
      and French of Canada.
    Accept-Language: en-US,fr-CA
      In this example single value is on Accept-Language header that is English of US 
      with the factor weighting.
    accept-language: en-US,en;q=0.9
    */
 ,HTTP_HEADER_ACCEPT_LANGUAGE
    /**
     *
     * The Accept-Ranges response-header field allows the server to indicate its
     * acceptance of range requests for a resource
     *
     */
 ,HTTP_HEADER_ACCEPT_RANGES
    /**
     Access-Control-Allow-Origin
     Last Updated : 22 Nov, 2019
     The Access-Control-Allow-Origin is a response header that is used to indicates 
     whether the response can be shared with requesting code from the given origin.
     
     Syntax:
     
     Access-Control-Allow-Origin: * | <origin> | null
     Directives: 
      Access-Control-Allow-Origin accepts there types of directives mentioned 
      above and described below:
     
     *: This directive tells the browsers to allow requesting code from any 
        origin to access the resource. Used as a wildcard.
     <origin>: 
        This directive defines any single origin.
     null: 
        This directive defines null that should not be used due to any origin 
        can create a hostile document with a ¡°null¡± Origin. The ¡°null¡± 
        value for the ACAO(Access-Control-Allow-Origin) header should therefore, 
        be avoided.¡±
        
     Example:
     
     This example tells the browser to allow code from any origin to access 
     a resource.

     access-control-allow-origin: *
       A response that tells the browser to allow requesting code from the origin 
       https://www.geeksforgeeks.org to access a resource will include the following:
       access-control-allow-origin:https://www.geeksforgeeks.org
       Here the value of the Origin request header is compared with the list of 
       allowed origins, and if the response header origin value is present in that 
       compared list. Then set the Access-Control-Allow-Origin value to the same 
       value as the Origin value.
     */
 ,HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN
    /**
     *
     * The Age response-header field conveys the sender's estimate of the amount of
     * time since the response (or its revalidation) was generated at the origin
     * server. A cached response is "fresh" if its age does not exceed its freshness
     * lifetime.
     *
     */
 ,HTTP_HEADER_AGE
    /**
     *
     * The Allow entity-header field lists the set of methods supported by the
     * resource identified by the Request-URI. The purpose of this field is strictly
     * to inform the recipient of valid methods associated with the resource. An
     * Allow header field MUST be present in a 405 (Method Not Allowed) response.
     *
      HTTP headers | Age
      Last Updated : 31 Oct, 2019
        The HTTP header Age defines the times in seconds of the object that have 
        been in the proxy cache. Usually, the age header is close to zero. It is 
        just summoned from the server, used to calculate the proxies¡¯ current 
        date and the date. It is a response header.

      Syntax:

      age: <delta-seconds>
      Directives: This header accepts single directive as mentioned above and 
        described below:

      <delta-seconds>: This directive defines a non-negative integer of the time 
        in second unit of the object that has been in a proxy cache. Time can not 
        be negative.
      Example:

      age: 128404
      age: 1859070
        To check the age in action go to Inspect Element -> Network check the request 
        header for age like below, age is highlighted.     
     *
     */
 ,HTTP_HEADER_ALLOW
    /**
     HTTP headers | Alt-Svc
     Last Updated : 10 May, 2020
       The HTTP headers Alt-Svc header is a response-type header it has been used 
       to advertise alternative service. Which services can be defined by a 
       protocol/host/port combination.
     
     Syntax:
       Alt-Svc: clear
       Alt-Svc: <protocol-id>=<alt-authority>; ma=<max-age>; persist=1
       Directives: This header accept five directives as mentioned above and 
           described below:
       
       clear: This directive define that all alternative services for that origin to 
           be invalidated.
       <protocol-id>: This directive is the ALPN protocol identifier.
       <alt-authority>: This directive defines an alternative authority which 
           consists of an optional host override, a colon, and a mandatory port number.
           ma=<max-age>: It holds the number that defines seconds for which the 
           alternative service is considered fresh if it is omitted then the default 
           value will be 86400. It is an optional directive.
           persist=1: This directive holds the alternative service which cleared on 
           network configuration changes. Use of the persist=1 ensures that the entry 
           is not deleted through such changes.
     Example:
     
         Alt-Svc: h2=":425"; ma=2585900; persist=1
         Alt-Svc: h2=":485"; ma=592000; 
    */
 ,HTTP_HEADER_ALT_SVC
    /**
     HTTP headers | Alt-Used
       The HTTP Alt-Used request header is used to identify the alternative service 
       in use, just as the Host HTTP header field identifies the host and port of the 
       origin.
     
       The is intended to allow alternative services to detect loops, differentiate 
       traffic for purposes of load balancing, and generally to ensure that it is 
       possible to identify the intended destination of traffic, since introducing 
       this information after a protocol is in use has proven to be problematic.
 
       When a client uses an alternative service for a request, it can indicate this 
       to the server using the Alt-Used HTTP header.

     Syntax
       Alt-Used: <host>:<port>
         <host>
           The domain name of the server.
         <port> Optional
           The TCP port number on which the server is listening.

     Examples
       Alt-Used: alternate.example.net
     */
 ,HTTP_HEADER_ALT_USED
    /**
     The HTTP Authorization request header can be used to provide credentials that 
     authenticate a user agent with a server, allowing access to protected resources.
     
     The Authorization header is usually, but not always, sent after the user agent 
     first attempts to request a protected resource without credentials. The server 
     responds with a 401 Unauthorized message that includes at least one 
     WWW-Authenticate header. This header indicates the authentication schemes that 
     can be used to access the resource and any additional information needed by 
     the client to use them. The user-agent should select the most secure 
     authentication scheme that it supports from those offered, prompt the user for 
     their credentials, and then re-request the resource with the encoded credentials 
     in the Authorization header.
     
     This header is stripped from cross-origin redirects.

     Syntax
       Authorization: <auth-scheme> <authorization-parameters>
       
       // Basic authentication
       Authorization: Basic <credentials>
       
       // Digest authentication
       Authorization: Digest username=<username>,
           realm="<realm>",
           uri="<url>",
           algorithm=<algorithm>,
           nonce="<nonce>",
           nc=<nc>,
           cnonce="<cnonce>",
           qop=<qop>,
           response="<response>",
           opaque="<opaque>"

       <auth-scheme>
         The Authentication scheme that defines how the credentials are encoded. Some 
         of the more common types are (case-insensitive): Basic, Digest, Negotiate 
         and AWS4-HMAC-SHA256.
         
         Basic authentication
           <credentials>
             The credentials, encoded according to the specified scheme.
             
         Digest authentication
           <response>
             A string of the hex digits that proves that the user knows a password. 
             The algorithm encodes the username and password, realm, cnonce, qop, nc, 
             and so on. It is described in detail in the specification.
           
           username
             A quoted string containing user's name for the specified realm in either 
             plain text or the hash code in hexadecimal notation. If the name contains 
             characters that aren't allowed in the field, then username* can be used 
             instead (not "as well").
           
           username*
             The user's name formatted using an extended notation defined in RFC5987. 
             This should be used only if the name can't be encoded in username and if 
             userhash is set "false".
           
           uri
             The Effective Request URI. See the specification for more information.
           
           realm
             Realm of the requested username/password (again, should match the value 
             in the corresponding WWW-Authenticate response for the resource being 
             requested).
           
           opaque
             The value in the corresponding WWW-Authenticate response for the resource 
             being requested.
           
           algorithm
             The algorithm used to calculate the digest. Must be a supported algorithm 
             from the WWW-Authenticate response for the resource being requested.
           
           qop
             A token indicating the quality of protection applied to the message. Must 
             match the one value in the set specified in the WWW-Authenticate response 
             for the resource being requested.
           
           "auth": 
             Authentication
             
           "auth-int": 
             Authentication with integrity protection
             
           cnonce
             An quoted ASCII-only string value provided by the client. This is used by 
             both the client and server to provide mutual authentication, provide some 
             message integrity protection, and avoid "chosen plaintext attacks". See 
             the specification for additional information.
           
           nc
             Nonce count. The hexadecimal count of requests in which the client has 
             sent the current cnonce value (including the current request). The server 
             can use duplicate nc values to recognize replay requests.
           
           userhash Optional
             "true" if the username has been hashed. "false" by default.

       Examples
         Basic authentication
           For Basic authentication, the credentials are constructed by first combining 
           the username and the password with a colon (e.g., aladdin:opensesame), and 
           then by encoding the resulting string in base64 (e.g., YWxhZGRpbjpvcGVuc2VzYW1l).

         Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l
     */
 ,HTTP_HEADER_AUTHORIZATION
    /** The Cache-Control general-header field is used to specify
     * directives that MUST be obeyed by all caching mechanisms along the
     * request/response chain. The directives specify behavior intended to
     * prevent caches from adversely interfering with the request or
     * response. These directives typically override the default caching
     * algorithms. Cache directives are unidirectional in that the presence
     * of a directive in a request does not imply that the same directive
     * is to be given in the response.
     */
 ,HTTP_HEADER_CACHE_CONTROL
    /**
     *
     * The Connection general-header field allows the sender to specify options that
     * are desired for that particular connection and MUST NOT be communicated by
     * proxies over further connections.
     *
     */
 ,HTTP_HEADER_CONNECTION
    /**
     *
     * The Content-Encoding entity-header field is used as a modifier to the
     * media-type. When present, its value indicates what additional content codings
     * have been applied to the entity-body, and thus what decoding mechanisms must
     * be applied in order to obtain the media-type referenced by the Content-Type
     * header field. Content-Encoding is primarily used to allow a document to be
     * compressed without losing the identity of its underlying media type.
     *
     * @see HEADER_CONTENT_TYPE
     *
     Syntax
       Content-Encoding: gzip
       Content-Encoding: compress
       Content-Encoding: deflate
       Content-Encoding: br
       Content-Encoding: zstd
 
       // Multiple, in the order in which they were applied
       Content-Encoding: deflate, gzip

     Directives
       gzip
         A format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC. This is 
         the original format of the UNIX gzip program. The HTTP/1.1 standard also 
         recommends that the servers supporting this content-encoding should 
         recognize x-gzip as an alias, for compatibility purposes.
 
       compress
         A format using the Lempel-Ziv-Welch (LZW) algorithm. The value name was 
         taken from the UNIX compress program, which implemented this algorithm. 
         Like the compress program, which has disappeared from most UNIX 
         distributions, this content-encoding is not used by many browsers today, 
         partly because of a patent issue (it expired in 2003).
 
       deflate
         Using the zlib structure (defined in RFC 1950) with the deflate 
         compression algorithm (defined in RFC 1951).
 
       br
         A format using the Brotli algorithm structure (defined in RFC 7932).
 
       zstd
         A format using the Zstandard algorithm structure (defined in RFC 8878).
 
       Examples
         Compressing with gzip
         On the client side, you can advertise a list of compression schemes that 
         will be sent along in an HTTP request. The Accept-Encoding header is used 
         for negotiating content encoding.
 
       Accept-Encoding: gzip, deflate
         The server responds with the scheme used, indicated by the Content-Encoding 
         response header.
 
       Content-Encoding: gzip
         Whether a server uses compression methods requested by the client depends 
         on server configuration and capabilities.
     */
 ,HTTP_HEADER_CONTENT_ENCODING
    /**
     *
     * The Content-Length entity-header field indicates the size of the entity-body,
     * in decimal number of OCTETs, sent to the recipient or, in the case of the HEAD
     * method, the size of the entity-body that would have been sent had the request
     * been a GET.
     *
     * @see HTTP_REQUEST_GET
     * @see HTTP_REQUEST_HEAD
     *
     */
 ,HTTP_HEADER_CONTENT_LENGTH
    /**
     *
     * The Content-Language entity-header field describes the natural language(s) of
     * the intended audience for the enclosed entity. Note that this might not be
     * equivalent to all the languages used within the entity-body.
     *
     */
 ,HTTP_HEADER_CONTENT_LANGUAGE
    /**
     *
     * The Content-Location entity-header field MAY be used to supply the resource
     * location for the entity enclosed in the message when that entity is accessible
     * from a location separate from the requested resource's URI. A server SHOULD
     * provide a Content-Location for the variant corresponding to the response
     * entity; especially in the case where a resource has multiple entities
     * associated with it, and those entities actually have separate locations by
     * which they might be individually accessed, the server SHOULD provide a
     * Content-Location for the particular variant which is returned.
     *
     */
 ,HTTP_HEADER_CONTENT_LOCATION
    /**
     *
     * The Content-Range entity-header is sent with a partial entity-body to specify
     * where in the full entity-body the partial body should be applied. Range units
     * are defined in RFC 2616 section 3.12.
     *
     * @see http://www.ietf.org/rfc/rcf2616.txt
     *
     */
 ,HTTP_HEADER_CONTENT_RANGE
    /**
     * The HTTP Content-Security-Policy response header allows website administrators 
     * to control resources the user agent is allowed to load for a given page. With 
     * a few exceptions, policies mostly involve specifying server origins and script 
     * endpoints. This helps guard against cross-site scripting attacks.
     *
     * For more information, see the introductory article on Content Security Policy 
     * (CSP).

     Syntax
      Content-Security-Policy: <policy-directive>; <policy-directive>
        where <policy-directive> consists of: <directive> <value> with no internal 
        punctuation.

     Fetch directives
       Fetch directives control the locations from which certain resource types may 
       be loaded.
 
     child-src
       Defines the valid sources for web workers and nested browsing contexts loaded 
       using elements such as <frame> and <iframe>.
 
     Fallback for frame-src and worker-src.
       connect-src
         Restricts the URLs which can be loaded using script interfaces.
   
       default-src
         Serves as a fallback for the other fetch directives.
 
     Fallback for all other fetch directives.
 
       fenced-frame-src Experimental
         Specifies valid sources for nested browsing contexts loaded into <fencedframe> 
         elements.
   
       font-src
         Specifies valid sources for fonts loaded using @font-face.
 
       frame-src
         Specifies valid sources for nested browsing contexts loaded into elements 
         such as <frame> and <iframe>.
 
       img-src
         Specifies valid sources of images and favicons.
   
       manifest-src
         Specifies valid sources of application manifest files.
   
       media-src
         Specifies valid sources for loading media using the <audio>, <video> and 
         <track> elements.
   
       object-src
         Specifies valid sources for the <object> and <embed> elements.
   
       prefetch-src Deprecated Non-standard
         Specifies valid sources to be prefetched or prerendered.
   
       script-src
         Specifies valid sources for JavaScript and WebAssembly resources.
   
       Fallback for script-src-elem and script-src-attr.
   
       script-src-elem
         Specifies valid sources for JavaScript <script> elements.
   
       script-src-attr
         Specifies valid sources for JavaScript inline event handlers.
   
       style-src
         Specifies valid sources for stylesheets.
   
       Fallback for style-src-elem and style-src-attr.
   
       style-src-elem
         Specifies valid sources for stylesheets <style> elements and <link> 
         elements with rel="stylesheet".
   
       style-src-attr
         Specifies valid sources for inline styles applied to individual DOM 
         elements.
   
       worker-src
         Specifies valid sources for Worker, SharedWorker, or ServiceWorker 
         scripts.

     Examples
       Content-Security-Policy: default-src 'self' http://example.com;
                                 connect-src 'none';
       Content-Security-Policy: connect-src http://example.com/;
                                 script-src http://example.com/
     */
 ,HTTP_HEADER_CONTENT_SECURITY_POLICY
    /**
     *
     * The Content-Type entity-header field indicates the media type of the
     * entity-body sent to the recipient or, in the case of the HEAD method, the
     * media type that would have been sent had the request been a GET.
     *
     */
 ,HTTP_HEADER_CONTENT_TYPE
    /**
     *
     * The Content-MD5 entity-header field, as defined in RFC 1864, is an MD5 digest
     * of the entity-body for the purpose of providing an end-to-end message
     * integrity check (MIC) of the entity-body.
     *
     * Note: a MIC is good for detecting accidental modification of the entity-body
     * in transit, but is not proof against malicious attacks.
     *
     * @see http://www.ietf.org/rfc/rfc1864.txt
     *
     */
 ,HTTP_HEADER_CONTENT_MD5
    /**
     Cookie
     Last Updated : 28 Oct, 2022
     HTTP headers are used to pass additional information with HTTP response or 
     HTTP requests. A cookie is an HTTP request header i.e. used in the requests 
     sent by the user to the server. It contains the cookies previously sent by 
     the server using one or more set-cookie headers. It is an optional header. 
     
     Syntax:
     
     Cookie: <cookie-list>
       In case of single cookie syntax is as follow:
     
     Cookie: name=value
       In case of multiple cookies syntax is as follow:
     
     Cookie: name=value; name=value; name=value
       Directives: 
        This header accept a single directive mentioned above and described below:
     
     <cookie-list> It is the list of name=value pair separated by ; and space i.e ¡®;¡¯.

     Example 1:
     Cookie: user=Bob

     Example 2:
     Cookie: user=Bob; age=28; csrftoken=u12t4o8tb9ee73
     */
 ,HTTP_HEADER_COOKIE
    /**
     *
     * The Date general-header field represents the date and time at which the
     * message was originated, having the same semantics as orig-date in RFC 822.
     * The field value is an HTTP-date, as described in RDF 2616 section 3.3.1; it
     * MUST be sent in RFC 1123 date format.
     *
     * @see http://www.ietf.org/rfc/rfc822.txt
     * @see http://www.ietf.org/rfc/rfc1123.txt
     * @see http://www.ietf.org/rfc/rfc2616.txt
     *
     */
 ,HTTP_HEADER_DATE
    /**
     * The HTTP DNT (Do Not Track) request header indicates the user's tracking 
     * preference. It lets users indicate whether they would prefer privacy rather 
     * than personalized content.

     * DNT is deprecated in favor of Global Privacy Control, which is communicated 
     * to servers using the Sec-GPC header, and accessible to clients from 
     * navigator.globalPrivacyControl.
     */
 ,HTTP_HEADER_DNT
    /**
     *
     * The Via general-header field MUST be used by gateways and proxies to indicate
     * the intermediate protocols and recipients between the user agent and the
     * server on requests, and between the origin server and the client on responses.
     * It is analogous to the "Received" field of RFC 822 and is intended to be used
     * for tracking message forwards, avoiding request loops, and identifying the
     * protocol capabilities of all senders along the request/response chain.
     *
     * @see http://www.ietf.org/rfc/rfc822.txt
     *
     */
 ,HTTP_HEADER_VIA
    /**
     *
     * The ETag response-header field provides the current value of the entity tag
     * for the requested variant. The headers used with entity tags are described in
     * RFC 2616 sections 14.24, 14.26 and 14.44. The entity tag MAY be used for
     * comparison with other entities from the same resource
     *
     * @see http://www.ietf.org/rfc/rfc/2616.txt
     *
     */
 ,HTTP_HEADER_ETAG
   /**
    * The HTTP Expect request header indicates that there are expectations that 
    * need to be met by the server in order to handle the complete request 
    * successfully.
    * 
    * When a request has an Expect: 100-continue header, a server sends a 100 
    * Continue response to indicate that the server is ready or capable of 
    * receiving the rest of the request content. Waiting for a 100 response can 
    * be helpful if a client anticipates that an error is likely, for example, 
    * when sending state-changing operations without previously verified 
    * authentication credentials.
    *
    * A 417 Expectation Failed response is returned if the server cannot meet the 
    * expectation, or any other status otherwise (e.g., a 4XX status for a client 
    * error, or a 2XX status if the request can be resolved successfully without 
    * further processing).

    * None of the more common browsers send the Expect header, but some clients 
    * (command-line tools) do so by default.
    *
    * Syntax
    *   Expect: 100-continue   
    *
    * Examples
    *   Large message body
    *     A client sends a request with Expect header and waits for the server 
    *     to respond before sending the message body.
    * 
    *   PUT /somewhere/fun HTTP/1.1
    *   Host: origin.example.com
    *   Content-Type: video/h264
    *   Content-Length: 1234567890987
    *   Expect: 100-continue
    */
 ,HTTP_HEADER_EXPECT
    /**
     * The Expect-CT response header lets sites opt in to reporting and/or 
     * enforcement of Certificate Transparency requirements. Certificate 
     * Transparency (CT) aims to prevent the use of misissued certificates for 
     * that site from going unnoticed.
     * 
     * Only Google Chrome and other Chromium-based browsers implemented Expect-CT, 
     * and Chromium has deprecated the header from version 107, because Chromium 
     * now enforces CT by default. See the Chrome Platform Status update.
     * 
     * CT requirements can be satisfied via any one of the following mechanisms:
     *
     *   X.509v3 certificate extension to allow embedding of signed certificate 
     *   timestamps issued by individual logs. Most TLS certificates issued by 
     *   publicly-trusted CAs and used online contain embedded CT.
     *
     *   A TLS extension of type signed_certificate_timestamp sent during the 
     *   handshake Supporting OCSP stapling (that is, the status_request TLS 
     *   extension) and providing a SignedCertificateTimestampList
     * 
     * Syntax
     *   Expect-CT: report-uri="<uri>",
     *     enforce,
     *     max-age=<age>
     * 
     * Directives
     *   max-age
     *     The number of seconds after reception of the Expect-CT header field 
     *     during which the user agent should regard the host of the received 
     *     message as a known Expect-CT host.
     *
     *     If a cache receives a value greater than it can represent, or if any 
     *     of its subsequent calculations overflows, the cache will consider this 
     *     value to be either 2,147,483,648 (2^31) or the greatest positive 
     *     integer it can represent.
     *
     *   report-uri="<uri>" Optional
     *     The URI where the user agent should report Expect-CT failures.
     *
     *      When present with the enforce directive, the configuration is referred 
     *      to as an "enforce-and-report" configuration, signalling to the user 
     *      agent both that compliance to the Certificate Transparency policy 
     *      should be enforced and that violations should be reported.
     *
     *   enforce Optional
     *      Signals to the user agent that compliance with the Certificate 
     *      Transparency policy should be enforced (rather than only reporting 
     *      compliance) and that the user agent should refuse future connections 
     *      that violate its Certificate Transparency policy.
     *
     *      When both the enforce directive and the report-uri directive are present, 
     *      the configuration is referred to as an "enforce-and-report" configuration, 
     *      signalling to the user agent both that compliance to the Certificate 
     *      Transparency policy should be enforced and that violations should be 
     *      reported.
     *
     * Example
     *   The following example specifies enforcement of Certificate Transparency for 
     *   24 hours and reports violations to foo.example.com.
     *   
     *   Expect-CT: max-age=86400, enforce, report-uri="https://foo.example.com/report"
     */
 ,HTTP_HEADER_EXPECT_CT
    /**
     *
     * The Expires entity-header field gives the date/time after which the response
     * is considered stale. A stale cache entry may not normally be returned by a
     * cache (either a proxy cache or a user agent cache) unless it is first
     * validated with the origin server (or with an intermediate cache that has a
     * fresh copy of the entity). See RFC 2616 section 13.2 for further discussion of
     * the expiration model.
     *
     * @see http://www.ietf.org/rfc/rfc2616.txt
     *
     */
 ,HTTP_HEADER_EXPIRES
    /**
     * The HTTP Forwarded request header contains information that may be added by 
     * reverse proxy servers (load balancers, CDNs, etc.) that would otherwise be 
     * altered or lost when proxy servers are involved in the path of the request.
     * 
     * For example, if a client is connecting to a web server through an HTTP proxy 
     * (or load balancer), server logs will only contain the IP address, host 
     * address, and protocol of the proxy; this header can be used to identify the 
     * IP address, host, and protocol, of the original request. The header is optional 
     * and may be added to, modified, or removed, by any of the proxy servers on 
     * the path to the server.
     * 
     * This header is used for debugging, statistics, and generating location-dependent 
     * content. By design, it exposes privacy sensitive information, such as the IP 
     * address of the client. Therefore, the user's privacy must be kept in mind when 
     * using this header.
     * 
     * The alternative and de-facto standard versions of this header are the 
     * X-Forwarded-For, X-Forwarded-Host and X-Forwarded-Proto headers.
     * 
     * Syntax
     *   The syntax for the forwarding header from a single proxy is shown below. 
     *   Directives are key=value pairs, separated by a semicolon.
     *  
     *   Forwarded: by=<identifier>;for=<identifier>;host=<host>;proto=<http|https>
     *  
     * Directives
     *   The interface where the request came in to the proxy server. The identifier can be:
     *     an obfuscated identifier (such as "hidden" or "secret"). This should be treated 
     *     as the default.
     *
     *     an IP address (v4 or v6, optionally with a port, and ipv6 quoted and enclosed 
     *     in square brackets)
     *
     *     "unknown" when the preceding entity is not known (and you still want to indicate 
     *     that forwarding of the request was made) for Optional
     *
     *   The client that initiated the request and subsequent proxies in a chain of proxies. 
     *   The identifier has the same possible values as the by directive.
     *

     *  host Optional
     *    The Host request header field as received by the proxy.
     *
     *  proto Optional
     *    Indicates which protocol was used to make the request (typically "http" or "https").
     *
     * Examples
     *   Using the Forwarded header
     *   
     *   Forwarded: for="_mdn"
     *   Forwarded: For="[2001:db8:cafe::17]:4711"
     *   Forwarded: for=192.0.2.60;proto=http;by=203.0.113.43
     *   Forwarded: for=192.0.2.43, for=198.51.100.17     *  
     *   
     */
 ,HTTP_HEADER_FORWARDED
    /**
     * The HTTP Host request header specifies the host and port number of the server to 
     * which the request is being sent.
     *
     * If no port is included, the default port for the service requested is implied 
     * (e.g., 443 for an HTTPS URL, and 80 for an HTTP URL).
     * 
     * A Host header field must be sent in all HTTP/1.1 request messages. A 400 Bad 
     * Request status code may be sent to any HTTP/1.1 request message that lacks or 
     * contains more than one Host header field.
     *
     * Syntax
     *   Host: <host>:<port>
     *    
     * Directives
     *  <host>
     *    The domain name of the server (for virtual hosting).
     * 
     *  <port> Optional
     *    TCP port number on which the server is listening.
     *
     * Examples
     *   Host: developer.mozilla.org
     */
 ,HTTP_HEADER_HOST
    /**
     * The HTTP2-Settings header is used to proactively supply the server with 
     * HTTP/2-specific parameters during an HTTP Upgrade request. If this HTTP 
     * header is not included then the server will refuse to upgrade the HTTP 
     * Connection. Similarly, the upgrade request will fail if more than a single 
     * HTTP2-Settings header is included in the HTTP request.
     *
     * The server does not explicitly acknowledge the HTTP2-Settings header. This 
     * success can be assumed upon receiving the HTTP response 101 Switching 
     * Protocols.
     *
     * The value (contents) of the HTTP HTTP2-Settings request header is the 
     * payload of a SETTINGS frame, encoded as a base64url string
     *
     * Example
     *   Request
     *     HTTP2-Settings: token68
     *   Response
     *     HTTP/1.1 101 Switching Protocols
     * 
     * Upgrade: HTTP/2
     *     Connection: upgrade
     */
 ,HTTP_HEADER_HTTP2_SETTINGS
    /**
     * The HTTP If-Match request header makes a request conditional. A server will 
     * return resources for GET and HEAD methods, or upload resource for PUT and 
     * other non-safe methods, only if the resource matches one of the ETag values 
     * in the If-Match request header. If the conditional does not match, the 412 
     * Precondition Failed response is returned instead.
     * 
     * The comparison with the stored ETag uses the strong comparison algorithm, 
     * meaning two files are considered identical byte-by-byte. If a listed ETag 
     * has the W/ prefix indicating a weak entity tag, this comparison algorithm 
     * will never match it.
     * 
     * There are two common use cases:
     *   For GET and HEAD methods, used in combination with a Range header, it 
     *   can guarantee that the new ranges requested come from the same resource 
     *   as the previous one.
     * 
     * For other methods, and in particular for PUT, If-Match can be used to 
     * prevent the lost update problem. It can check if the modification of a 
     * resource that the user wants to upload will not override another change 
     * that has been done since the original resource was fetched.
     *
     * Syntax
     *   If-Match: <etag_value>
     *   If-Match: <etag_value>, <etag_value>, ¡­
     *
     * Directives
     * <etag_value>
     *   Entity tags uniquely representing the requested resources. They are a 
     *   string of ASCII characters placed between double quotes (like 
     *   "675af34563dc-tr34"). They may be prefixed by W/ to indicate that they 
     *   are 'weak', i.e. that they represent the resource semantically but not 
     *   byte-by-byte. However, in an If-Match header, weak entity tags will 
     *   never match.
     *
     * The asterisk is a special value representing any resource. Note that this 
     * must match as false if the origin server does not have a current 
     * representation for the target resource.
     * 
     * Examples
     *   If-Match: "bfc13a64729c4290ef5b2c2730249c88ca92d82d"
     *   If-Match: "67ab43", "54ed21", "7892dd"
     *   If-Match: *
     */
 ,HTTP_HEADER_IF_MATCH
    /**
      If-Modified-Since
      Last Updated : 15 May, 2020
      The If-Modified-Since header is a request-header that is sent to a server 
      as a conditional request. If the contents have changed, the server responds 
      with a 200 status code and the entire requested document is updated. The 
      If-Modified-Since HTTP header indicates the time for which a browser first 
      downloaded a resource from the server. This helps to determine whether the 
      resource has changed or not, since the last time it was accessed. If the 
      status of a particular resource is 304 Not Modified, this means that the 
      file has not changed and there is no need to download it again.

      Let us take an example of this website i.e. GeeksforGeeks, it has different 
      pages about computer science subjects. Whenever a search engine visits this 
      website, it will check each page of the website and if none of the pages 
      have changed, it will not load any of those pages. But if there is a change 
      in any one of its pages, a flag will raise. So when the search engine visits, 
      it will see the flag for the changed page, and then it will access that page.

      Note: When used in combination with the If-None-Match, it is ignored, unless 
        the server does not support If-None-Match.

      Syntax:

      If-Modified-Since: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT

      Directives: This header accept eight directive as mentioned above and described below:

      <day name>: It contains the day¡¯s name like ¡°Mon¡±, ¡°Tue¡± etc.(case-sensitive).
      <day>: It contains the date in 2 digit numbers, like ¡°04¡± or ¡°23¡± for days.
      <month>: It contains the name of the month, in 3 letter month names like ¡°Jan¡±, 
               ¡°Feb¡± etc.(case-sensitive).
      <year>: It contains the 4 digit year like ¡°2020¡±
      <hour>: It contains the hour in 2 digit hour like ¡°07¡± or ¡°12¡±.
      <minute>: Same as hour minutes 2 digit minute like ¡°09¡± or ¡°55¡±
      <second>: Containing the seconds in 2 digits second like ¡°08¡± or ¡°50¡±.
      GMT: All the dates in HTTP will show in Greenwich Mean Time format, not in local time format.

      Example:
        If-Modified-Since: Sun, 10 May 2020 02:01:00 GMT     
     */
 ,HTTP_HEADER_IF_MODIFIED_SINCE
    /**
     If-None-Match
     Last Updated : 18 Nov, 2019
     The HTTP Header If-None-Match is a request-type header. Generally, it is used to 
     update the entity tags on the server. Firstly, the Client provides the Server 
     with a set of entity tags (E-tags). The Server compares the given tags with 
     those it already has for the resource. Then, the Server will provide the 
     requested page with a 200 status code only when none of the entity tags given 
     to it matches. Or else, a 304 Not Modified status is returned by the Server.
     
     There are two types of algorithms used to compare the entity tags. They are:
     
     Weak Comparison Algorithm
     Strong Comparison Algorithm
     Weak comparison algorithm: It neglects minor differences which are considered 
       to be unimportant between the entity tags. For example, two tags having the 
       same contents but with different dates are considered to be identical.
     
     Strong comparison algorithm: It checks the tags byte-to-byte.
     
     Syntax:
     
     If-None-Match: "entity_tag"
     If-None-Match: *
     Note : ¡°*¡± is used to represent any resource.
     
     Directives: This header accepts two directives as mentioned above and described below:
     
     entity_tag: It represents the resource which is requested. It is a string which 
        contains alphabets, numbers and other special characters enclosed within double 
        quotes (¡± ¡°).
     ¡°*¡±: It represents any resource and is used to avoid race conditions between 
        PUT operations. When this directive is used, the server should not perform 
        the requested method if the entity is given to it already exists for that 
        resource.
        
     Examples:
       If-None-Match:"2780-5524acffbda80-gzip"
       If-None-Match:*
     */
 ,HTTP_HEADER_IF_NONE_MATCH
    /**
     If-Range
     Baseline Widely available
     The HTTP If-Range request header makes a range request conditional. If the 
     condition is fulfilled, a range request is issued, and the server sends back 
     a 206 Partial Content response with part (or parts) of the resource in the 
     body. If the condition is not fulfilled, the full resource is sent back 
     with a 200 OK status.
     
     This header can be used either with the Last-Modified validator or with 
     ETag, but not with both.
     
     The most common use case is to resume a download with guarantees that the 
     resource on the server has not been modified since the last part has been 
     received by the client.
     
     If-Range: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
     If-Range: <etag>
     Directives
     <etag>
       An entity tag uniquely representing the requested resource. It is a 
       string of ASCII characters placed between double quotes 
       (Like "675af34563dc-tr34"). A weak entity tag (one prefixed by W/) 
       must not be used in this header.
     
     <day-name>
       One of Mon, Tue, Wed, Thu, Fri, Sat, or Sun (case-sensitive).
     
     <day>
       2 digit day number, e.g., "04" or "23".
     
     <month>
       One of Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec (case sensitive).
     
     <year>
       4 digit year number, e.g., "1990" or "2016".
     
     <hour>
       2 digit hour number, e.g., "09" or "23".
       
     <minute>
       2 digit minute number, e.g., "04" or "59".
     
     <second>
       2 digit second number, e.g., "04" or "59".
     
     GMT
       Greenwich Mean Time. HTTP dates are always expressed in GMT, never in local time.
     
     Examples
       If-Range: Wed, 21 Oct 2015 07:28:00 GMT
       If-Range: "67ab43"
     */
 ,HTTP_HEADER_IF_RANGE
    /**
     If-Unmodified-Since
     Last Updated : 20 Nov, 2021
     The HTTP If-Unmodified-Since header is a request-type header and makes the request 
     conditional on having a last-modified timestamp. The server will perform the 
     requested operation only if the requested source has not been modified after a 
     specified date. In case, the requested source has been modified then the server 
     will not perform the requested operation and respond with a 412 error 
     (Precondition Failed). 
     
     Let us consider a scenario, a patient goes to a medical laboratory to get his 
     BMI test. The lab attendant records his height and weight and tells him to 
     collect the reports within 2 days. Due to some reasons, the patient could 
     not collect the reports and goes to the laboratory after a month. But the 
     lab attendant doesn¡¯t give him the reports and informs him that the tests 
     will be valid only if his weight is still unchanged(or unmodified), otherwise, 
     the result has become inconsistent. This is how our If-Unmodified-Since HTTP 
     header works. 
     
     Syntax: 
     
     If-Unmodified-Since: <day-name>, <day> <month> <year>
     <hour>:<minute>:<second> GMT 
     Directives: The HTTP If-Unmodified-Since header accept below described directives:  
     
     <day-name>: It contains the day¡¯s name like ¡°Mon¡±, ¡°Tue¡± etc. (case-sensitive).
     <day>: It contains the date in 2 digit numbers, like ¡°04¡± or ¡°23¡± for days.
     <month>: It contains the name of the month, in 3 letter month names like ¡°Jan¡±, 
              ¡°Feb¡± etc.(case-sensitive).
     <year>: It contains the 4 digit year like ¡°2009¡±
     <hour>: It contains the hour in 2 digit hour like ¡°07¡± or ¡°12¡±.
     <minute>: Same as hour minutes 2 digit minute like ¡°09¡± or ¡°55¡±
     <second>: Containing the seconds in 2 digit second like ¡°08¡± or ¡°50¡±.
     GMT: All the dates in HTTP will show in Greenwich Mean Time format, not in local time format.

     Example:  
       Below the header informing the server that it is not modified since 10th 
       February 2004 which is Friday and mentioned time is 06:20:00, if that¡¯s 
       true then the request by the client will proceed. 
       
       If-Unmodified-Since: Fri, 10 Feb 2004 06:20:00 GMT
     */
 ,HTTP_HEADER_IF_UNMODIFIED_SINCE
    /**
     *
     * The Last-Modified entity-header field indicates the date and time at which the
     * origin server believes the variant was last modified.
     *
     */
 ,HTTP_HEADER_LAST_MODIFIED
    /**
     * The HTTP Link header provides a means for serializing one or more links in HTTP 
     * headers. This allows the server to point a client to another resource containing 
     * metadata about the requested resource. This header has the same semantics as the 
     * HTML <link> element. One benefit of using the Link header is that the browser 
     * can start preconnecting or preloading resources before the HTML itself is fetched 
     * and processed.
     * 
     * In practice, most rel link types don't have an effect when used with the HTTP 
     * header. For example, the icon relation only works in HTML, and stylesheet does 
     * not work reliably across browsers (only in Firefox). The only relations that work 
     * reliably are preconnect and preload, which can be combined with 103 Early Hints.
     *
     * Syntax
     *   Link: <uri-reference>; param1=value1; param2="value2"
     *  
     *   <uri-reference>
     *     The URI reference, must be enclosed between < and > and percent-encoded.
     *
     * Examples
     *   Enclose URLs in angle brackets
     *   The URI (absolute or relative) must be enclosed between < and >:
     * 
     *  Link: <https://example.com>; rel="preconnect"
     */
 ,HTTP_HEADER_LINK
    /**
     *
     * The Location response-header field is used to redirect the recipient to a
     * location other than the Request-URI for completion of the request or
     * identification of a new resource. For 201 (Created) responses, the Location
     * is that of the new resource which was created by the request. For 3xx
     * responses, the location SHOULD indicate the server's preferred URI for
     * automatic redirection to the resource. The field value consists of a single
     * absolute URI.
     *
     */
 ,HTTP_HEADER_LOCATION
 ,HTTP_HEADER_ONION_LOCATION
 ,HTTP_HEADER_P3P
    /**
     *
     * The Pragma general-header field is used to include implementation-specific
     * directives that might apply to any recipient along the request/response chain.
     * All pragma directives specify optional behavior from the viewpoint of the
     * protocol; however, some systems MAY require that behavior be consistent with
     * the directives.
     *
     */
 ,HTTP_HEADER_PRAGMA
    /**
     *
     * The Warning general-header field is used to carry additional information about
     * the status or transformation of a message which might not be reflected in the
     * message. This information is typically used to warn about a possible lack of
     * semantic transparency from caching operations or transformations applied to
     * the entity body of the message.
     *
     */
 ,HTTP_HEADER_WARNING
    /**
    Priority
      The HTTP Priority header indicates a client's preference for the priority order 
      at which the response containing the requested resource should be sent, relative 
      to other resource requests on the same connection. If the header is not specified 
      in the request, a default priority is assumed. The server may also include this 
      header in responses in order to indicate it has an interest in changing the 
      prioritization preferences the client advertized. In responses, this information 
      can be used as an input to the prioritization process for caching servers and 
      other servers that are forwarding the response.

      The server is not bound by client prioritization and might only use client 
      priorities as hints for its own prioritization process. For example, a server 
      may know that a specific image is vital for user experience and should be sent 
      at the highest priority. Server prioritization might also be affected by factors 
      such as network congestion.

      This request may be cached, and the server is expected to control the 
      cacheability or the applicability of the cached response using the header fields 
      that control the caching behavior, such as Cache-Control and Vary.

      Syntax
        Priority: u=<priority>
        Priority: i
        Priority: u=<priority>, i
        
      Directives
      
      u=<priority>
        
        The "urgency" (u) parameter specifies a priority value <priority> for the resource. 
        The value is an integer between 0 and 7 inclusive, in descending order of priority 
        (0 is the highest urgency). The default priority value for requests is 3. There is 
        no default priority value for responses: the absence of the header in a response 
        indicates that the server has chosen not to modify the client priority. A priority 
        of 7 should only be used for resources that are unlikely to affect the user 
        experience, such as background tasks or delivery of software updates.

        Browsers should request documents that are likely to use other resources at the 
        default priority level. The referenced resources should then be requested using 
        values that reflect the relative impact on the user experience of their arrival 
        time.
   
        Servers may have a different view on the priority than the client, and can respond 
        with a different value in order to provide a priority hint to intermediate servers. 
        The intermediate server may take this value into consideration along with the 
        original request priority. The absence of the Priority header in the response 
        indicates that the server chosen not to modify the client priority.
 
      i
        The incremental (i) directive, if present, indicates than an HTTP response can be 
        processed incrementally.
   
        Resources that can be processed incrementally are those where the recipient can 
        potentially do something useful as soon as a chunk arrives, rather than waiting 
        for the complete resource to be available.
   
        If a browser sets this directive then the server may choose to concurrently serve 
        all incremental requests with the same urgency. This distributes the multiple 
        requests across the connection bandwidth, with the result that all the requests 
        start being processed earlier, but take longer in total to complete.
   
        If the browser does not set this directive then it is indicating that it will not 
        process the resource incrementally. Servers should in this case send responses with 
        the same urgency one by one, in the order in which their associated requests were 
        generated.
   
        Note: Servers are expected to ignore directives on this header that they do not 
        understand. New directives that are added in future are expected to be compatible 
        with these existing directives so that they can be safely ignored.
 
      Examples
        Setting resource urgency
          The following example shows a request for an HTML file. The urgency isn't set, and 
          so defaults to 3, with i being false. This is the normal setting for a document 
          that owns other resources.
 
      :method = GET
      :scheme = https
      :authority = example.net
      :path = /index.html

      GET /style.css HTTP/1.1
      Host: example.net
      Priority: u=2
     */
 ,HTTP_HEADER_PRIORITY
    /**
      The HTTP Range request header indicates the part of a resource that the server should 
      return. Several parts can be requested at the same time in one Range header, and the 
      server may send back these ranges in a multipart document. If the server sends back 
      ranges, it uses the 206 Partial Content status code for the response. If the ranges 
      are invalid, the server returns the 416 Range Not Satisfiable error.

      A server that doesn't support range requests may ignore the Range header and return 
      the whole resource with a 200 status code. Older browsers used a response header of 
      Accept-Ranges: none to disable features like 'pause' or 'resume' in download managers, 
      but since a server ignoring the Range header has the same meaning as responding with 
      Accept-Ranges: none, the header is rarely used in this way.

      Currently only bytes units are registered which are offsets (zero-indexed & inclusive). 
      If the requested data has a content coding applied, each byte range represents the 
      encoded sequence of bytes, not the bytes that would be obtained after decoding.

      The header is a CORS-safelisted request header when the directive specifies a single 
      byte range.

      Syntax
        Range: <unit>=<range-start>-
        Range: <unit>=<range-start>-<range-end>
        Range: <unit>=<range-start>-<range-end>, ¡­, <range-startN>-<range-endN>
        Range: <unit>=-<suffix-length>
        
      Directives
        <unit>
          The unit in which ranges are defined. Currently only bytes are a registered unit.

        <range-start>
          An integer in the given unit indicating the start position of the request range.

        <range-end>
          An integer in the given unit indicating the end position of the requested range. 
          This value is optional and, if omitted, the end of the resource is used as the 
          end of the range.

        <suffix-length>
          An integer indicating the number of units at the end of the resource to return.

      Examples
        The following examples show how to make requests using the Range header for 
        CORS-safelisted requests, and for requesting multiple ranges. Other examples can be 
        found in the HTTP range requests guide.      

        Range: bytes=0-499
        Range: bytes=900-
        Range: bytes=-100
        Range: bytes=200-999, 2000-2499, 9500-
        Range: bytes=0-499, -499
     */
 ,HTTP_HEADER_RANGE
    /**
      The HTTP Referer request header contains the absolute or partial address from which 
      a resource has been requested. The Referer header allows a server to identify 
      referring pages that people are visiting from or where requested resources are 
      being used. This data can be used for analytics, logging, optimized caching, and more.

      When you click a link, the Referer contains the address of the page that includes the 
      link. When you make resource requests to another domain, the Referer contains the 
      address of the page that uses the requested resource.

      The Referer header can contain an origin, path, and querystring, and may not contain 
      URL fragments (i.e., #section) or username:password information. The request's 
      referrer policy defines the data that can be included. See Referrer-Policy for more 
      information and examples.

      Syntax
        Referer: <url>
      Directives
        <url>
        An absolute or partial address of the web page that makes the request. URL fragments 
        (i.e., #section) and user info (i.e., username:password in 
            https://username:password@example.com/foo/bar/) 
        are not included. Origin, path, and query string may be included, depending on the 
        referrer policy.

      Examples
        Referer: https://developer.mozilla.org/en-US/docs/Web/JavaScript
        Referer: https://example.com/page?q=123
        Referer: https://example.com/
     */
 ,HTTP_HEADER_REFERER
    /**
     *
     * The Trailer general field value indicates that the given set of header fields
     * is present in the trailer of a message encoded with chunked transfer-coding.
     *
     */
 ,HTTP_HEADER_TRAILER
    /**
      The HTTP Referrer-Policy response header controls how much referrer information 
      (sent with the Referer header) should be included with requests. Aside from the 
      HTTP header, you can set this policy in HTML.

      Syntax
        Referrer-Policy: no-referrer
        Referrer-Policy: no-referrer-when-downgrade
        Referrer-Policy: origin
        Referrer-Policy: origin-when-cross-origin
        Referrer-Policy: same-origin
        Referrer-Policy: strict-origin
        Referrer-Policy: strict-origin-when-cross-origin
        Referrer-Policy: unsafe-url

      Directives
        no-referrer
          The Referer header will be omitted: sent requests do not include any referrer 
          information.

        no-referrer-when-downgrade
          Send the origin, path, and query string in Referer when the protocol security 
          level stays the same or improves (HTTP¡úHTTP, HTTP¡úHTTPS, HTTPS¡úHTTPS). 
          Don't send the Referer header for requests to less secure destinations 
          (HTTPS¡úHTTP, HTTPS¡úfile).

        origin
          Send only the origin in the Referer header. For example, a document at 
          https://example.com/page.html will send the referrer https://example.com/.

        origin-when-cross-origin
          When performing a same-origin request to the same protocol level 
          (HTTP¡úHTTP, HTTPS¡úHTTPS), send the origin, path, and query string. Send 
          only the origin for cross origin requests and requests to less secure 
          destinations (HTTPS¡úHTTP).

        same-origin
          Send the origin, path, and query string for same-origin requests. Don't 
          send the Referer header for cross-origin requests.

        strict-origin
          Send only the origin when the protocol security level stays the same 
          (HTTPS¡úHTTPS). Don't send the Referer header to less secure destinations 
          (HTTPS¡úHTTP).

        strict-origin-when-cross-origin (default)
          Send the origin, path, and query string when performing a same-origin request. 
          For cross-origin requests send the origin (only) when the protocol security 
          level stays same (HTTPS¡úHTTPS). Don't send the Referer header to less secure 
          destinations (HTTPS¡úHTTP).

          Note: This is the default policy if no policy is specified, or if the provided 
          value is invalid (see spec revision November 2020). Previously the default 
          was no-referrer-when-downgrade.

        unsafe-url
          Send the origin, path, and query string when performing any request, 
          regardless of security.

          Warning: This policy will leak potentially-priv

        <a href="http://example.com" referrerpolicy="origin">¡­</a>
     */
 ,HTTP_HEADER_REFERRER_POLICY
    /**
     *
     * The Server response-header field contains information about the software used
     * by the origin server to handle the request. The field can contain multiple
     * product tokens (section 3.8) and comments identifying the server and any
     * significant subproducts. The product tokens are listed in order of their
     * significance for identifying the application.
     *
     */
 ,HTTP_HEADER_SERVER
    /**
      The HTTP Set-Cookie response header is used to send a cookie from the server 
      to the user agent, so that the user agent can send it back to the server 
      later. To send multiple cookies, multiple Set-Cookie headers should be sent 
      in the same response.

      Syntax
        Set-Cookie: <cookie-name>=<cookie-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Expires=<date>
        Set-Cookie: <cookie-name>=<cookie-value>; HttpOnly
        Set-Cookie: <cookie-name>=<cookie-value>; Max-Age=<number>
        Set-Cookie: <cookie-name>=<cookie-value>; Partitioned
        Set-Cookie: <cookie-name>=<cookie-value>; Path=<path-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Secure

        Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Strict
        Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Lax
        Set-Cookie: <cookie-name>=<cookie-value>; SameSite=None; Secure

        // Multiple attributes are also possible, for example:
        Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>; Secure; HttpOnly

      Attributes
        <cookie-name>=<cookie-value>
          Defines the cookie name and its value. A cookie definition begins with a name-value pair.

          A <cookie-name> can contain any US-ASCII characters except for: control characters 
          (ASCII characters 0 up to 31 and ASCII character 127) or separator characters 
          (space, tab and the characters: ( ) < > @ , ; : \ " / [ ] ? = { })

          A <cookie-value> can optionally be wrapped in double quotes and include any US-ASCII 
          character excluding control characters (ASCII characters 0 up to 31 and ASCII 
          character 127), Whitespace, double quotes, commas, semicolons, and backslashes.

        Encoding: 
          Many implementations perform percent-encoding on cookie values. However, this is 
          not required by the RFC specification. The percent-encoding does help to satisfy 
          the requirements of the characters allowed for <cookie-value>.

          Note: Some <cookie-name> have a specific semantic:

        __Secure- prefix: 
          Cookies with names starting with __Secure- (dash is part of the prefix) 
          must be set with the secure flag from a secure page (HTTPS).

        __Host- prefix: 
          Cookies with names starting with __Host- are sent only to the host subdomain or 
          domain that set them, and not to any other host. They must be set with the secure 
          flag, must be from a secure page (HTTPS), must not have a domain specified, and 
          the path must be /.

        Domain=<domain-value> Optional
          Defines the host to which the cookie will be sent.

          Only the current domain can be set as the value, or a domain of a higher order, 
          unless it is a public suffix. Setting the domain will make the cookie available 
          to it, as well as to all its subdomains.

          If omitted, this attribute defaults to the host of the current document URL, not 
          including subdomains.

          Contrary to earlier specifications, leading dots in domain names (.example.com) 
          are ignored.

          Multiple host/domain values are not allowed, but if a domain is specified, then 
          subdomains are always included.

        Expires=<date> Optional
          Indicates the maximum lifetime of the cookie as an HTTP-date timestamp. See Date 
          for the required formatting.

          If unspecified, the cookie becomes a session cookie. A session finishes when the 
          client shuts down, after which the session cookie is removed.

          Warning: Many web browsers have a session restore feature that will save all tabs 
          and restore them the next time the browser is used. Session cookies will also be 
          restored, as if the browser was never closed.

          When an Expires date is set, the deadline is relative to the client the cookie is 
          being set on, not the server.

        HttpOnly Optional
          Forbids JavaScript from accessing the cookie, for example, through the 
          Document.cookie property. Note that a cookie that has been created with HttpOnly 
          will still be sent with JavaScript-initiated requests, for example, when calling 
          XMLHttpRequest.send() or fetch(). This mitigates attacks against 
          cross-site scripting (XSS).

        Max-Age=<number> Optional
          Indicates the number of seconds until the cookie expires. A zero or negative 
          number will expire the cookie immediately. If both Expires and Max-Age are set, 
          Max-Age has precedence.

        Partitioned Optional
          Indicates that the cookie should be stored using partitioned storage. Note that 
          if this is set, the Secure directive must also be set. See Cookies Having 
          Independent Partitioned State (CHIPS) for more details.

        Path=<path-value> Optional
          Indicates the path that must exist in the requested URL for the browser to send 
          the Cookie header.

          The forward slash (/) character is interpreted as a directory separator, and 
          subdirectories are matched as well. For example, for Path=/docs,

          the request paths /docs, /docs/, /docs/Web/, and /docs/Web/HTTP will all match.
          the request paths /, /docsets, /fr/docs will not match.
          SameSite=<samesite-value> Optional
          Controls whether or not a cookie is sent with cross-site requests, providing some 
          protection against cross-site request forgery attacks (CSRF).

        The possible attribute values are:

          Strict
            Means that the browser sends the cookie only for same-site requests, that is, 
            requests originating from the same site that set the cookie. If a request 
            originates from a different domain or scheme (even with the same domain), no 
            cookies with the SameSite=Strict attribute are sent.

          Lax
            Means that the cookie is not sent on cross-site requests, such as on requests 
            to load images or frames, but is sent when a user is navigating to the origin 
            site from an external site (for example, when following a link). This is the 
            default behavior if the SameSite attribute is not specified.

            Warning: 
              Not all browsers set SameSite=Lax by default. See Browser compatibility 
              for details.

          None
            Means that the browser sends the cookie with both cross-site and same-site 
            requests. The Secure attribute must also be set when setting this value, like 
            so SameSite=None; Secure. If Secure is missing an error will be logged:

            Cookie "myCookie" rejected because it has the "SameSite=None" attribute but 
            is missing the "secure" attribute.

            This Set-Cookie was blocked because it had the "SameSite=None" attribute 
            but did not have the "Secure" attribute, which is required in order to use 
            "SameSite=None".

            Note: A Secure cookie is only sent to the server with an encrypted request 
              over the HTTPS protocol. Note that insecure sites (http:) can't set cookies 
              with the Secure directive, and therefore can't use SameSite=None.

            Warning: Cookies with the SameSite=None; Secure that do not also have the 
              Partitioned attribute may be blocked in cross-site contexts on future 
              browser versions. This behavior protects user data from cross-site tracking. 
              See Cookies Having Independent Partitioned State (CHIPS) and Third-party 
              cookies.

          Secure Optional
            Indicates that the cookie is sent to the server only when a request is made 
            with the https: scheme (except on localhost), and therefore, is more resistant 
            to man-in-the-middle attacks.

            Note: 
              Do not assume that Secure prevents all access to sensitive information 
              in cookies (session keys, login details, etc.). Cookies with this attribute 
              can still be read/modified either with access to the client's hard disk or 
              from JavaScript if the HttpOnly cookie attribute is not set.

          Insecure sites (http:) 
              cannot set cookies with the Secure attribute. The https: requirements are 
              ignored when the Secure attribute is set by localhost.

       Examples
         Set-Cookie: sessionId=38afes7a8
         Set-Cookie: id=a3fWa; Expires=Wed, 21 Oct 2015 07:28:00 GMT
         Set-Cookie: id=a3fWa; Max-Age=2592000
     */
 ,HTTP_HEADER_SET_COOKIE
 ,HTTP_HEADER_STATUS
     /**
       The HTTP Strict-Transport-Security response header (often abbreviated as HSTS) 
       informs browsers that the site should only be accessed using HTTPS, and that any 
       future attempts to access it using HTTP should automatically be upgraded to HTTPS.

       Syntax
         Strict-Transport-Security: max-age=<expire-time>
         Strict-Transport-Security: max-age=<expire-time>; includeSubDomains
         Strict-Transport-Security: max-age=<expire-time>; includeSubDomains; preload

       Directives
        max-age=<expire-time>
          The time, in seconds, that the browser should remember that a site is only to 
          be accessed using HTTPS.

        includeSubDomains Optional
          If this optional parameter is specified, this rule applies to all of the site's 
          subdomains as well.

        preload Optional Non-standard
          See Preloading Strict Transport Security for details. When using preload, the 
          max-age directive must be at least 31536000 (1 year), and the includeSubDomains 
          directive must be present. Not part of the specification.

        Examples
          Strict-Transport-Security: max-age=31536000; includeSubDomains
          Strict-Transport-Security: max-age=63072000; includeSubDomains; preload
      */
 ,HTTP_HEADER_STRICT_TRANSPORT_SECURITY
    /**
      The HTTP TE request header specifies the transfer encodings the user agent is 
      willing to accept. The transfer encodings are for message compression and 
      chunking of data during transmission.

      Transfer encodings are applied at the protocol layer, so an application consuming 
      responses receives the body as if no encoding was applied.

      Syntax
        TE: compress
        TE: deflate
        TE: gzip
        TE: trailers

        TE: trailers, deflate;q=0.5

      Directives
        compress
          A format using the Lempel-Ziv-Welch (LZW) algorithm is accepted as a transfer 
          coding name.

        deflate
          Using the zlib structure is accepted as a transfer coding name.

        gzip
          A format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC is accepted 
          as a transfer coding name.

        trailers
          Indicates that the client will not discard trailer fields in a chunked transfer 
          coding.

        q
          When multiple transfer codings are acceptable, the q parameter (quality values) 
          syntax orders codings by preference.

        Note that chunked is always supported by HTTP/1.1 recipients, so you don't need 
        to specify it using the TE header. See the Transfer-Encoding header for more details.
        
      Examples
        GET /resource HTTP/1.1
        Host: example.com
        TE: gzip; q=1.0, deflate; q=0.8
     */
 ,HTTP_HEADER_TE
    /**
     *
     * The Transfer-Encoding general-header field indicates what (if any) type of
     * transformation has been applied to the message body in order to safely
     * transfer it between the sender and the recipient. This differs from the
     * content-coding in that the transfer-coding is a property of the message, not
     * of the entity.
     *
     */
 ,HTTP_HEADER_TRANSFER_ENCODING
    /**
     *
     * The Upgrade general-header allows the client to specify what additional
     * communication protocols it supports and would like to use if the server finds
     * it appropriate to switch protocols. The server MUST use the Upgrade header
     * field within a 101 (Switching Protocols) response to indicate which
     * protocol(s) are being switched.
     *
     Syntax
       Transfer-Encoding: chunked
       Transfer-Encoding: compress
       Transfer-Encoding: deflate
       Transfer-Encoding: gzip

       // Several values can be listed, separated by a comma
       Transfer-Encoding: gzip, chunked
     Directives
       chunked
         Data is sent in a series of chunks. Content can be sent in streams of 
         unknown size to be transferred as a sequence of length-delimited buffers, 
         so the sender can keep a connection open, and let the recipient know when 
         it has received the entire message. The Content-Length header must be 
         omitted, and at the beginning of each chunk, a string of hex digits 
         indicate the size of the chunk-data in octets, followed by \r\n and 
         then the chunk itself, followed by another \r\n. The terminating chunk 
         is a zero-length chunk.
 
       compress
         A format using the Lempel-Ziv-Welch (LZW) algorithm. The value name was 
         taken from the UNIX compress program, which implemented this algorithm. 
         Like the compress program, which has disappeared from most UNIX 
         distributions, this content-encoding is used by almost no browsers 
         today, partly because of a patent issue (which expired in 2003).
 
       deflate
         Using the zlib structure (defined in RFC 1950), with the deflate 
         compression algorithm (defined in RFC 1951).
 
       gzip
         A format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC. This 
         is originally the format of the UNIX gzip program. The HTTP/1.1 
         standard also recommends that the servers supporting this content-encoding 
         should recognize x-gzip as an alias, for compatibility purposes.

       Examples
         HTTP/1.1 200 OK
         Content-Type: text/plain
         Transfer-Encoding: chunked
 
         7\r\n
         Mozilla\r\n
         11\r\n
         Developer Network\r\n
         0\r\n
         \r\n
     */
 ,HTTP_HEADER_UPGRADE
    /**
      The HTTP Upgrade-Insecure-Requests request header sends a signal to the server 
      indicating the client's preference for an encrypted and authenticated response, 
      and that the client can successfully handle the upgrade-insecure-requests CSP 
      directive.

      Syntax
        Upgrade-Insecure-Requests: <boolean>
        
      Directives
        <boolean>
         1 indicates 'true' and is the only valid value for this field.

      Examples
        Using Upgrade-Insecure-Requests
        A client's request signals to the server that it supports the upgrade 
        mechanisms of upgrade-insecure-requests:

        GET / HTTP/1.1
        Host: example.com
        Upgrade-Insecure-Requests: 1

        Location: https://example.com/
        Vary: Upgrade-Insecure-Requests
     */
 ,HTTP_HEADER_UPGRADE_INSECURE_REQUESTS
    /**
      The HTTP User-Agent request header is a characteristic string that lets servers 
      and network peers identify the application, operating system, vendor, and/or 
      version of the requesting user agent.

      Syntax
        User-Agent: <product> / <product-version> <comment>

      Directives
        <product>
          A product identifier ¡ª its name or development codename.

        <product-version>
          Version number of the product.

        <comment>
          Zero or more comments containing more details. For example, sub-product 
          information.

     Examples
       Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0
       Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:42.0) Gecko/20100101 Firefox/42.0

       Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) 
       Chrome/51.0.2704.103 Safari/537.36

       Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) 
       Chrome/51.0.2704.106 Safari/537.36 OPR/38.0.2220.41

       Opera/9.80 (Macintosh; Intel Mac OS X; U; en) Presto/2.2.15 Version/10.00
       Opera/9.60 (Windows NT 6.0; U; en) Presto/2.1.1

       Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) 
       Chrome/91.0.4472.124 Safari/537.36 Edg/91.0.864.59

       Mozilla/5.0 (iPhone; CPU iPhone OS 13_5_1 like Mac OS X) 
       AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.1.1 Mobile/15E148 Safari/604.1

       Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)
       Mozilla/5.0 (compatible; YandexAccessibilityBot/3.0; +http://yandex.com/bots)
     */
 ,HTTP_HEADER_USER_AGENT
    /**
     *
     * The Retry-After response-header field can be used with a 503 (Service
     * Unavailable) response to indicate how long the service is expected to be
     * unavailable to the requesting client. This field MAY also be used with any
     * 3xx (Redirection) response to indicate the minimum time the user-agent is
     * asked wait before issuing the redirected request. The value of this field
     * can be either an HTTP-date or an integer number of seconds (in decimal) after
     * the time of the response.
     *
     */
 ,HTTP_HEADER_RETRY_AFTER
    /**
     *
     * The Vary field value indicates the set of request-header fields that fully
     * determines, while the response is fresh, whether a cache is permitted to use
     * the response to reply to a subsequent request without revalidation. For
     * uncacheable or stale responses, the Vary field value advises the user agent
     * about the criteria that were used to select the representation. A Vary field
     * value of "*" implies that a cache cannot determine from the request headers of
     * a subsequent request whether this response is the appropriate representation.
     * See RFC 2616 section 13.6 for use of the Vary header field by caches.
     *
     * @see http://www.ietf.org/rfc/rfc2616.txt
     *
     */
 ,HTTP_HEADER_VARY
    /** The WWW-Authenticate response-header field MUST be included in 401
     * Unauthorized response messages (@ref HTTP_STATUS_401_REASON_PHRASE).
     * The field value consists of at least one challenge that indicates
     * the authentication scheme(s) and parameters applicable to the
     * Request-URI.
     *
     @verbatim
       WWW-Authenticate  = "WWW-Authenticate" ":" 1#challenge
     @endverbatim
     *
     * The HTTP access authentication process is described in "HTTP
     * Authentication: Basic and Digest Access Authentication". User agents
     * are advised to take special care in parsing the WWW-Authenticate
     * field value as it might contain more than one challenge, or if more
     * than one WWW-Authenticate header field is provided, the contents of
     * a challenge itself can contain a comma-separated list of
     * authentication parameters.
     *
     * @see http://www.ietf.org/rfc/rfc2617.txt
     */
 ,HTTP_HEADER_WWW_AUTHENTICATE
    /**
      HTTP headers | X-Content-Type-Options
      Last Updated : 14 Sep, 2021
      The HTTP headers X-Content-Type-Options acts as a marker that indicates 
      the MIME-types headers in the content types headers should not be 
      changed to the server. This header was introduced in the Internet 
      Explorer 8 of Microsoft. This header block the content sniffing 
      (non-executable MIME type into executable MIME type). After that, 
      all the other browsers also introduce the X-Content-Type-Options, 
      and their MIME sniffing algorithms were less aggressive. 
      
      Syntax:  
      
      x-content-type-options: nosniff
      Directives: 
        There is a single directive accepted by X-Content-Type-Options header.  
      
      nosniff: It blocks all request if there ¡°style¡± MIME-type is not 
        text/css and JavaScript MIME-type. Plus it enables the cross origin 
        if there MIME-Type text/html, text/plain, text/jason, 
        application/jason and any type of xml extension.
      Example:  
      
      x-content-type-options: nosniff
     */
 ,HTTP_HEADER_X_CONTENT_TYPE_OPTIONS
    /**
     X-Forwarded-For
     Last Updated : 22 Nov, 2019
       The HTTP headers are used to communicate between client and server. 
       HTTP headers let the client and server pass additional information 
       with an HTTP request or response. The X-Forwarded-For Header is a 
       request type header and is an alternative and de-facto standard 
       version of the Forwarded header which is used when a client connects 
       to a web server through an HTTP proxy or load balancer for identifying 
       the original IP address. The privacy of the user is put at risk as 
       the sensitive information is revealed by using this header. The HTTP 
       X-Forwarded-For header is used to identify the client¡¯s original IP 
       address. The modified version of the HTTP X-Forwarded-For is HTTP 
       Forwarded header.
     
     Syntax:
     
     X-Forwarded-For: <client>, <proxy>
     Note: Multiple proxies are also possible.
     
     Directives: 
        The HTTP X-Forwarded-For accepts two directives as mentioned above 
        and described below:
     
     <client>: It is the IP address of the client.
     <proxy>: 
        It is the proxies that request has to go through. If there are multiple 
        proxies then the IP addresses of each successive proxy is listed.

     Examples:
       This is an example when the request has to go through multiple proxies.
       X-Forwarded-For: 103.0.113.165, 60.91.3.17, 120.192.338.678
       This is an example when the request has to go through one proxy.
       X-Forwarded-For: fe80::780b:cd:b91:5b6d%6
    */
 ,HTTP_HEADER_X_FORWARDED_FOR
    /**
     X-Forwarded-Proto
      Last Updated : 07 Nov, 2020
      The HTTP headers are used to communicate between client and server. 
      HTTP headers let the client and server pass additional information 
      with an HTTP request or response. X-Forwarded-Proto (XPF) header is 
      used to identifying the protocol that the client used to connect 
      with a proxy or load balancer. It can be HTTP or HTTPS. Your server 
      access log usually contains information about the protocol used 
      between server and load balancer, but it does not contain any 
      information about the protocol used between the client and load 
      balancer. To get information about which protocol used between 
      client and load balancer, we can use the X-Forwarded-Proto request 
      header. Using this header, the client can make an HTTP request to 
      an HTTPS-only resource.

    Syntax:  

    X-Forwarded-Proto: <protocol>

    Directives:  

    <protocol>: This contains an X-Forwarded-Proto request header for a 
        request that originated from the client(http or https).
    Example 1: The following example contains an X-Forwarded-Proto request 
        header for a request that originated from the client as an HTTP request: 

    // X-Forwarded-Proto: originatingprotocol
    X-Forwarded-Proto: http
    
    Example 2: The following example contains an X-Forwarded-Proto request 
      header for a request that originated from the client as an HTTPS request:  

    // X-Forwarded-Proto: originatingprotocol
    X-Forwarded-Proto: https
    Example 3: This example is for other non-standard form.  

    # Microsoft
      Front-End-Https: on
       
      X-Forwarded-Protocol: http
      X-Forwarded-Ssl: on 
      X-Url-Scheme: http
   */
 ,HTTP_HEADER_X_FORWARDED_PROTO
    /**
     *
     * The Proxy-Authenticate response-header field MUST be included as part of a
     * 407 (Proxy Authentication Required) response. The field value consists of a
     * challenge that indicates the authentication scheme and parameters applicable
     * to the proxy for this Request-URI.
     *
       Proxy-Authenticate
       Last Updated : 16 Oct, 2021
       The HTTP Proxy-Authenticate is a response header gives access to a resource 
       file by defining an authorization method. It allows the proxy server to 
       transmit the request further by authenticating it. This header is sent 
       along with 407 Proxy Authentication Required which indicates the inability 
       to complete a request due to the lack of proper authorization credentials 
       for the proxy server that is intercepting the request. 
 
       Syntax: 
 
       Proxy-Authenticate:<type> realm=<realm>
       Directives: 
 
       <type>: This value is just one of the authentication types.
       realm=<realm>: This value is used give a description of protected area or 
         the extent. A formatted host name is displayed if no realm is specified.
       Examples: 
 
       Proxy-Authenticate: Basic
       The Basic authentication type accepts a username and password to validate 
       the authentication. 
 
       Proxy-Authenticate: Basic realm="Access to Geeksforgeeks website"
       This gives access to the Geeksforgeeks website after accepting a username 
       and password.      
     */
 ,HTTP_HEADER_CONTENT_ID
     /**
      The HTTP Content-Disposition header indicates whether content should be 
      displayed inline in the browser as a web page or part of a web page or 
      downloaded as an attachment locally.

      In a multipart body, the header must be used on each subpart to provide 
      information about its corresponding field. The subpart is delimited by 
      the boundary defined in the Content-Type header. When used on the body 
      itself, Content-Disposition has no effect.

      The Content-Disposition header is defined in the larger context of MIME 
      messages for email, but only a subset of the possible parameters apply 
      to HTTP forms and POST requests. Only the value form-data, as well as 
      the optional directive name and filename, can be used in the HTTP context.

      Syntax
        As a response header for the main body
        The first parameter in the HTTP context is either inline (default value, 
        indicating it can be displayed inside the Web page, or as the Web page) 
        or attachment (indicating it should be downloaded; most browsers 
        presenting a 'Save as' dialog, prefilled with the value of the filename 
        parameters if present).

        Content-Disposition: inline
        Content-Disposition: attachment
        Content-Disposition: attachment; filename="file name.jpg"
        Content-Disposition: attachment; filename*=UTF-8''file%20name.jpg

      The quotes around the file name are optional, but are necessary if you use 
      special characters in the file name, such as spaces.

      The parameters filename and filename* differ only in that filename* uses 
      the encoding defined in RFC 5987, section 3.2. When both filename and 
      filename* are present in a single header field value, filename* is 
      preferred over filename when both are understood. It's recommended to 
      include both for maximum compatibility, and you can convert filename* 
      to filename by substituting non-ASCII characters with ASCII equivalents 
      (such as converting ¨¦ to e). You may want to avoid percent escape 
      sequences in filename, because they are handled inconsistently across 
      browsers. (Firefox and Chrome decode them, while Safari does not.)

      Browsers may apply transformations to conform to the file system 
      requirements, such as converting path separators (/ and \) to underscores (_).

      As a header for a multipart body
      A multipart/form-data body requires a Content-Disposition header to provide 
      information about each subpart of the form (e.g., for every form field and 
      any files that are part of field data). The first directive is always 
      form-data, and the header must also include a name parameter to identify 
      the relevant field. Additional directives are case-insensitive and have 
      arguments that use quoted-string syntax after the = sign. Multiple 
      parameters are separated by a semicolon (;).

      Content-Disposition: form-data; name="fieldName"
      Content-Disposition: form-data; name="fieldName"; filename="filename.jpg"

      Directives
        name
          Is followed by a string containing the name of the HTML field in the 
          form that the content of this subpart refers to. When dealing with 
          multiple files in the same field (for example, the multiple attribute 
          of an <input type="file"> element), there can be several subparts 
          with the same name.

          A name with a value of '_charset_' indicates that the part is not an 
          HTML field, but the default charset to use for parts without explicit 
          charset information.

        filename
          Is followed by a string containing the original name of the file 
          transmitted. This parameter provides mostly indicative information. 
          The suggestions in RFC2183 apply:

          Prefer ASCII characters if possible (the client may percent-encode it, 
          as long as the server implementation decodes it).
          Any path information should be stripped, such as by replacing / with _.
          When writing to disk, it should not overwrite an existing file.
          Avoid creating special files with security implications, such as 
          creating a file on the command search path.
          Satisfy other file system requirements, such as restricted characters 
          and length limits.
          Note that the request header does not have the filename* parameter and 
          does not allow RFC 5987 encoding.

      Examples
        Triggering download prompt for a resource
        The following response triggers the "Save As" dialog in a browser:

        200 OK
        Content-Type: text/html; charset=utf-8
        Content-Disposition: attachment; filename="cool.html"
        Content-Length: 21

        <HTML>Save me!</HTML>

        POST /test.html HTTP/1.1
        Host: example.org
        Content-Type: multipart/form-data;boundary="delimiter123"

        --delimiter123
        Content-Disposition: form-data; name="field1"

        value1
        --delimiter123
        Content-Disposition: form-data; name="field2"; filename="example.txt"

        value2
        --delimiter123--
     */
 ,HTTP_HEADER_CONTENT_DISPOSITION
     /**
      Many Content-Types which could usefully be transported via email are represented, 
      in their "natural" format, as 8-bit character or binary data. Such data cannot be 
      transmitted over some transport protocols. For example, RFC 821 restricts mail 
      messages to 7-bit US-ASCII data with 1000 character lines.
      
      It is necessary, therefore, to define a standard mechanism for re-encoding such data 
      into a 7-bit short-line format. This document specifies that such encodings will be 
      indicated by a new "Content-Transfer-Encoding" header field. The Content-Transfer-Encoding 
      field is used to indicate the type of transformation that has been used in order to 
      represent the body in an acceptable manner for transport.

      Unlike Content-Types, a proliferation of Content-Transfer- Encoding values is 
      undesirable and unnecessary. However, establishing only a single 
      Content-Transfer-Encoding mechanism does not seem possible. There is a tradeoff 
      between the desire for a compact and efficient encoding of largely-binary data 
      and the desire for a readable encoding of data that is mostly, but not entirely, 
      7-bit data. For this reason, at least two encoding mechanisms are necessary: 
      a "readable" encoding and a "dense" encoding.

      The Content-Transfer-Encoding field is designed to specify an invertible mapping 
      between the "native" representation of a type of data and a representation that 
      can be readily exchanged using 7 bit mail transport protocols, such as those 
      defined by RFC 821 (SMTP). This field has not been defined by any previous 
      standard. The field's value is a single token specifying the type of encoding, 
      as enumerated below. Formally:

      Syntax
        Content-Transfer-Encoding := "BASE64" / "QUOTED-PRINTABLE" / 
                                     "8BIT"   / "7BIT" / 
                                     "BINARY" / x-token
               
        These values are not case sensitive. That is, Base64 and BASE64 and bAsE64 are all 
        equivalent. An encoding type of 7BIT requires that the body is already in a 7-bit 
        mail-ready representation. This is the default value -- that is, 
        "Content-Transfer-Encoding: 7BIT" is assumed if the Content-Transfer-Encoding header 
        field is not present.
        
        The values "8bit", "7bit", and "binary" all imply that NO encoding has been performed. 
        However, they are potentially useful as indications of the kind of data contained in 
        the object, and therefore of the kind of encoding that might need to be performed for 
        transmission in a given transport system. "7bit" means that the data is all represented 
        as short lines of US-ASCII data. "8bit" means that the lines are short, but there may 
        be non-ASCII characters (octets with the high-order bit set). "Binary" means that not 
        only may non-ASCII characters be present, but also that the lines are not necessarily 
        short enough for SMTP transport.

        The difference between "8bit" (or any other conceivable bit-width token) and the 
        "binary" token is that "binary" does not require adherence to any limits on line 
        length or to the SMTP CRLF semantics, while the bit-width tokens do require such 
        adherence. If the body contains data in any bit-width other than 7-bit, the 
        appropriate bit-width Content-Transfer-Encoding token must be used (e.g., "8bit" 
        for unencoded 8 bit wide data). If the body contains binary data, the "binary" 
        Content-Transfer-Encoding token must be used.

      Directives
        7Bit Encoding
          7bit simply means "My data consists only of US-ASCII characters, which only use 
          the lower 7 bits for each character." You're basically guaranteeing that all of 
          the bytes in your content already adhere to the restrictions of SMTP, and so it
          needs no special treatment. You can just read it as-is.

          Note that when you choose 7bit, you're agreeing that all of the lines in your 
          content are less than 1000 characters in length.

          As long as your content adheres to these rule, 7bit is the best transfer encoding, 
          since there's no extra work necessary; you just read/write the bytes as they 
          come off the pipe. It's also easy to eyeball 7bit content and make sense of it. 
          The idea here is that if you're just writing in "plain English text" you'll be 
          fine. But that wasn't true in 2005 and it isn't true today.

        8Bit Encoding
          8bit means "My data may include extended ASCII characters; they may use the 8th 
          (highest) bit to indicate special characters outside of the standard US-ASCII 
          7-bit characters." As with 7bit, there's still a 1000-character line limit.

          8bit, just like 7bit, does not actually do any transformation of the bytes as 
          they're written to or read from the wire. It just means that you're not 
          guaranteeing that none of the bytes will have the highest bit set to "1".

          This seems like a step up from 7bit, since it gives you more freedom in your 
          content. However, RFC 1341 contains this tidbit:

          As of the publication of this document, there are no standardized Internet 
          transports for which it is legitimate to include unencoded 8-bit or binary 
          data in mail bodies. Thus there are no circumstances in which the "8bit" 
          or "binary" Content-Transfer-Encoding is actually legal on the Internet.

          RFC 1341 came out over 20 years ago. Since then we've gotten 8bit MIME 
          Extensions in RFC 6152. But even then, line limits still may apply:

          Note that this extension does NOT eliminate the possibility of an SMTP 
          server limiting line length; servers are free to implement this extension 
          but nevertheless set a line length limit no lower than 1000 octets.

        Binary Encoding
          binary is the same as 8bit, except that there's no line length restriction. 
          You can still include any characters you want, and there's no extra encoding. 
          Similar to 8bit, RFC 1341 states that it's not really a legitimate encoding 
          transfer encoding. RFC 3030 extended this with BINARYMIME.

        Quoted Printable
          Before the 8BITMIME extension, there needed to be a way to send content 
          that couldn't be 7bit over SMTP. HTML files (which might have more than 
          1000-character lines) and files with international characters are good
          examples of this. The quoted-printable encoding (Defined in Section 5.1 
          of RFC 1341) is designed to handle this. It does two things:

          Defines how to escape non-US-ASCII characters so that they can be represented 
          in only 7-bit characters. (Short version: they get displayed as an equals 
          sign plus two 7-bit characters.)
          
          Defines that lines will be no greater than 76 characters, and that line breaks 
          will be represented using special characters (which are then escaped).
          Quoted Printable, because of the escaping and short lines, is much harder to 
          read by a human than 7bit or 8bit, but it does support a much wider range 
          of possible content.

        Base64 Encoding
          If your data is largely non-text (ex: an image file), you don't have many 
          options. 7bit is off the table. 8bit and binary were unsupported prior to 
          the MIME extension RFCs. quoted-printable would work, but is really 
          inefficient (every byte is going to be represented by 3 characters).

          base64 is a good solution for this type of data. It encodes 3 raw bytes as 
          4 US-ASCII characters, which is relatively efficient. RFC 1341 further 
          limits the line length of base64-encoded data to 76 characters to fit 
          within an SMTP message, but that's relatively easy to manage when you're 
          just splitting or concatenating arbitrary characters at fixed lengths.

          The big downside is that base64-encoded data is pretty much entirely 
          unreadable by humans, even if it's just "plain" text underneath.

        Examples
          Content-Type: text/plain; charset=ISO-8859-1 
          Content-transfer-encoding: base64
      */
 ,HTTP_HEADER_CONTENT_TRANSFER_ENCODING
     /**
      The HTTP Proxy-Authenticate response header defines the authentication 
      method (or challenge) that should be used to gain access to a resource 
      behind a proxy server. It is sent in a 407 Proxy Authentication Required 
      response so a client can identify itself to a proxy that requires 
      authentication.

      Syntax
        A comma-separated list of one or more authentication challenges:

        Proxy-Authenticate: <challenge>
          Where a <challenge> is comprised of an <auth-scheme>, followed by 
          an optional <token68> or a comma-separated list of <auth-params>:

        challenge = <auth-scheme> <auth-param>, ¡­, <auth-paramN>
        challenge = <auth-scheme> <token68>

        
        Proxy-Authenticate: <auth-scheme>
        Proxy-Authenticate: <auth-scheme> token68
        Proxy-Authenticate: <auth-scheme> auth-param1=param-token1
        Proxy-Authenticate: <auth-scheme> auth-param1=param-token1, ¡­, auth-paramN=param-tokenN

      Directives
        <auth-scheme>
          A case-insensitive token indicating the Authentication scheme used. 
          Some of the more common types are Basic, Digest, Negotiate and 
          AWS4-HMAC-SHA256. IANA maintains a list of authentication schemes, 
          but there are other schemes offered by host services.

        <auth-param> Optional
          An authentication parameter whose format depends on the <auth-scheme>. 
          <realm> is described below as it's a common authentication parameter 
          among many auth schemes.

        <realm> Optional
          The string realm followed by = and a quoted string describing a 
          protected area, for example realm="staging environment". A realm 
          allows a server to partition the areas it protects (if supported 
          by a scheme that allows such partitioning). Some clients show this 
          value to the user to inform them about which particular credentials 
          are required ¡ª though most browsers stopped doing so to counter 
          phishing. The only reliably supported character set for this value 
          is us-ascii. If no realm is specified, clients often display a 
          formatted hostname instead.

        <token68> Optional
          A token that may be useful for some schemes. The token allows the 
          66 unreserved URI characters plus a few others. It can hold a base64, 
          base64url, base32, or base16 (hex) encoding, with or without padding, 
          but excluding whitespace. The token68 alternative to auth-param lists 
          is supported for consistency with legacy authentication schemes.

        Generally, you will need to check the relevant specifications for the 
        authentication parameters needed for each <auth-scheme>.

      Examples
        Proxy-Authenticate Basic auth
        The following response indicates a Basic auth scheme is required with 
        a realm:

        Proxy-Authenticate: Basic realm="Staging server"
      */
 ,HTTP_HEADER_PROXY_AUTHENTICATE
      /**
        The HTTP Proxy-Authorization request header contains the credentials 
        to authenticate a client with a proxy server, typically after the server 
        has responded with a 407 Proxy Authentication Required status with the 
        Proxy-Authenticate header.

      Syntax
        Proxy-Authorization: <auth-scheme> <credentials>
        Directives
        
        <auth-scheme>
          A case-insensitive token indicating the Authentication scheme used. 
          Some of the more common types are Basic, Digest, Negotiate and 
          AWS4-HMAC-SHA256. IANA maintains a list of authentication schemes, 
          but there are other schemes offered by host services.

        <credentials>
          Credentials use for the authentication scheme. Generally, you will 
          need to check the relevant specifications for the format.

          Note: See Authorization for more details.

        Examples
          Basic authentication
          In Basic auth, credentials are sent in the format <username>:<password> 
          (for example, aladdin:opensesame). The resulting string is then base64 
          encoded (YWxhZGRpbjpvcGVuc2VzYW1l).

          Proxy-Authorization: Bearer kNTktNTA1My00YzLT1234
          Proxy-Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l
       */
 ,HTTP_HEADER_PROXY_AUTHORIZATION
       /**
        The HTTP X-Frame-Options response header can be used to indicate whether 
        a browser should be allowed to render a page in a <frame>, <iframe>, 
        <embed> or <object>. Sites can use this to avoid clickjacking attacks, 
        by ensuring that their content is not embedded into other sites.

        The added security is provided only if the user accessing the document 
        is using a browser that supports X-Frame-Options.

        Syntax
          There are two possible directives for X-Frame-Options:

          X-Frame-Options: DENY
          X-Frame-Options: SAMEORIGIN
          
        Directives
          If you specify DENY, not only will the browser attempt to load the page 
          in a frame fail when loaded from other sites, attempts to do so will 
          fail when loaded from the same site. On the other hand, if you specify 
          SAMEORIGIN, you can still use the page in a frame as long as the site 
          including it in a frame is the same as the one serving the page.

          DENY
            The page cannot be displayed in a frame, regardless of the site 
            attempting to do so.

          SAMEORIGIN Deprecated
            The page can only be displayed if all ancestor frames are same origin 
            to the page itself.

          ALLOW-FROM origin Deprecated
            This is an obsolete directive. Modern browsers that encounter response 
            headers with this directive will ignore the header completely. The 
            Content-Security-Policy HTTP header has a frame-ancestors directive 
            which you should use instead.

       Examples
         Warning: Setting X-Frame-Options inside the <meta> element (e.g., 
         <meta http-equiv="X-Frame-Options" content="deny">) has no effect. 
         X-Frame-Options is only enforced via HTTP headers, as shown in the 
         examples below.

        Configuring Apache
        To configure Apache to send the X-Frame-Options header for all pages, 
        add this to your site's configuration:

        apacheconf
        Copy to Clipboard
        Header always set X-Frame-Options "SAMEORIGIN"
        To configure Apache to set X-Frame-Options to DENY, add this to your 
        site's configuration:

        apacheconf
        Copy to Clipboard
        Header set X-Frame-Options "DENY"

        add_header X-Frame-Options SAMEORIGIN always;
        You can set the X-Frame-Options header to DENY using:

        nginx
        Copy to Clipboard
        add_header X-Frame-Options DENY always;
        */
 ,HTTP_HEADER_X_FRAME_OPTIONS
        /**
          The HTTP X-XSS-Protection response header was a feature of Internet 
          Explorer, Chrome and Safari that stopped pages from loading when they 
          detected reflected cross-site scripting (XSS) attacks. These 
          protections are largely unnecessary in modern browsers when sites 
          implement a strong Content-Security-Policy that disables the use 
          of inline JavaScript ('unsafe-inline').

          It is recommended that you use Content-Security-Policy instead of 
          XSS filtering.

          Syntax
            X-XSS-Protection: 0
            X-XSS-Protection: 1
            X-XSS-Protection: 1; mode=block
            X-XSS-Protection: 1; report=<reporting-uri>
            
          Directives
            0
              Disables XSS filtering.

            1
              Enables XSS filtering (usually default in browsers). If a cross-site 
              scripting attack is detected, the browser will sanitize the page 
              (remove the unsafe parts).

            1; mode=block
              Enables XSS filtering. Rather than sanitizing the page, the browser 
              will prevent rendering of the page if an attack is detected.

            1; report=<reporting-URI> (Chromium only)
              Enables XSS filtering. If a cross-site scripting attack is detected, 
              the browser will sanitize the page and report the violation. This 
              uses the functionality of the CSP report-uri directive to send a 
              report.
         */
 ,HTTP_HEADER_X_XSS_PROTECTION
 ,HTTP_HEADER_MAX
};

#define TRANSFER_ENCODING_CHUNKED	"chunked"

const ng_block_s *http_header_find(const char *key, int keyLength);

extern const ng_block_s ng_http_headers_byid[];

#define __HDR_BUF__(x) ng_http_headers_byid[HTTP ## _ ## x]
#define __HDR_BUF(x) __HDR_BUF__(x).cptr, __HDR_BUF__(x).len
#define __HDR_BUF_PTR(x) __HDR_BUF__(x).cptr
#define __HDR_BUF_DEF(x) (x), sizeof(x)-1

#if __HTTP_SMALL_SIZE
extern const ng_uint8_t __isValidToken[256];
#define ng_isValidFieldName(c) (__isValidToken[c]&0x1)
#define ng_isValidFieldValue(c) (__isValidToken[c]&0x2)
#else
extern const ng_uint8_t __isValidFieldValue[256];
extern const ng_uint8_t __isValidFieldName[256];
#define ng_isValidFieldName(c) (__isValidFieldName[c])
#define ng_isValidFieldValue(c) (__isValidFieldValue[c])
#endif

#define __HTTP_header_type_request  1
#define __HTTP_header_type_response 2

#define __HTTP_header_state_0      0
#define __HTTP_header_state_LN     1
#define __HTTP_header_state_FIELD  2
#define __HTTP_header_state_VALUE  3
#define __HTTP_header_state_1      4
#define __HTTP_header_state_2      5
#define __HTTP_header_state_3      6

struct _http_header_parser {
  char buffer[MAX_HEADER_SIZE];
  ng_buffer_s data;
  ng_list_head_s header;
  ng_uint16_t state;
  ng_uint16_t type;
  ng_uint16_t version;
  union{
    struct{
      ng_uint16_t method;
      ng_block_s path;
    };
    struct{
      ng_uint16_t code;
      ng_block_s desc;
    };
  };
};
typedef struct _http_header_parser header_parser_s;

void http_header_parser_init(header_parser_s *p, int type);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
