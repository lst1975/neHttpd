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
#ifndef __nanohttp_header_h
#define __nanohttp_header_h

#include "nanohttp-buffer.h"

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
 ,HTTP_HEADER_ALT_USED
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
 ,HTTP_HEADER_EXPECT
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
 ,HTTP_HEADER_FORWARDED
 ,HTTP_HEADER_HOST
 ,HTTP_HEADER_HTTP2_SETTINGS
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
 ,HTTP_HEADER_PRIORITY
 ,HTTP_HEADER_RANGE
 ,HTTP_HEADER_REFERER
    /**
     *
     * The Trailer general field value indicates that the given set of header fields
     * is present in the trailer of a message encoded with chunked transfer-coding.
     *
     */
 ,HTTP_HEADER_TRAILER
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
 ,HTTP_HEADER_SET_COOKIE
 ,HTTP_HEADER_STATUS
 ,HTTP_HEADER_STRICT_TRANSPORT_SECURITY
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
     */
 ,HTTP_HEADER_UPGRADE
 ,HTTP_HEADER_UPGRADE_INSECURE_REQUESTS
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
 ,HTTP_HEADER_CONTENT_DISPOSITION
 ,HTTP_HEADER_CONTENT_TRANSFER_ENCODING
 ,HTTP_HEADER_PROXY_AUTHENTICATE
 ,HTTP_HEADER_PROXY_AUTHORIZATION
 ,HTTP_HEADER_X_FRAME_OPTIONS
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
extern const uint8_t __isValidToken[256];
#define ng_isValidFieldName(c) (__isValidToken[c]&0x1)
#define ng_isValidFieldValue(c) (__isValidToken[c]&0x2)
#else
extern const uint8_t __isValidFieldValue[256];
extern const uint8_t __isValidFieldName[256];
#define ng_isValidFieldName(c) (__isValidFieldName[c])
#define ng_isValidFieldValue(c) (__isValidFieldValue[c])
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
