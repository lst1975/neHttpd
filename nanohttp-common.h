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
/******************************************************************
 *  $Id: nanohttp-common.h,v 1.49 2007/11/03 22:40:10 m0gg Exp $
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
#ifndef __nanohttp_common_h
#define __nanohttp_common_h

#include "nanohttp-buffer.h"
#include "nanohttp-mem.h"
#include "nanohttp-ctype.h"
#include "nanohttp-atoi.h"
#include "nanohttp-list.h"

/*@}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Common commandline arguments for client and server.
 *
 */
#define NHTTP_ARG_CERT      "-NHTTPcert"
#define NHTTP_ARG_CERTPASS  "-NHTTPcertpass"
#define NHTTP_ARG_CA        "-NHTTPCA"

#ifndef SAVE_STR
#define SAVE_STR(str) ((str==0)?("(null)"):(str))
#define SAVE_STR_LEN(str) ((str==0)?("(null)"):(str))
#endif

#define BOUNDARY_LENGTH 18

#define MAX_HEADER_SIZE 4256
#define MAX_FILE_BUFFER_SIZE 4256

#define REQUEST_MAX_PATH_SIZE  1024
#define RESPONSE_MAX_DESC_SIZE 1024

/**
 *
 * hpairnode_t represents a pair (key, value) pair. This is also a linked list.
 *
 */
struct hpair
{
  ng_block_s key;
  ng_block_s val;
  ng_list_head_s link;
};

typedef struct hpair hpair_t;

/**
 *
 * Indicates the version of the used HTTP protocol.
 *
 */
enum _http_version
{
  HTTP_VERSION_UNKOWN,
  HTTP_VERSION_1_0,
  HTTP_VERSION_1_1                      /* default */
};
typedef enum _http_version http_version_e;


/** Object representation of the content-type field in a HTTP header: 
 *
 * Example:
 *
 * @verbatim
    Content-Type: text/xml; key="value" key2="value2' ...
 * @endverbatim
 *
 * @see HEADER_CONTENT_TYPE
 */
struct _content_type
{
  ng_block_s type;
  ng_list_head_s params;
} ;
typedef struct _content_type content_type_s;

/*@}*/

/**
 *
 * Creates a new pair with the given parameters. Both strings key and value will
 * be cloned while creating the pair. 
 *
 * @param key the key of the (key,value) pair
 * @param value the value of the (key,value) pair
 * @param next next pair node in the linked list
 *
 * @return A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *         to http_free the pair.
 *
 */
extern hpair_t *
hpairnode_new(const ng_block_s *key, const ng_block_s *val, ng_list_head_s *head);

/**
 *
 * Creates a new pair from a given string. This function will split 'str' with
 * the found first delimiter 'delim'. The 'value' field  of the newly created
 * pair will have the value "", if no delimiter was found/ Whitespaces (' ') will
 * be removed from the beginnig of the parsed value.
 *
 * @param str A string to parse
 * @param delim a delimiter to use while splitting into key,value
 * @param next next pair node in the linked list
 *
 * @return A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *         to http_free the pair.
 *
 */
extern hpair_t *
hpairnode_parse(const char *str, int _size, char delim, ng_list_head_s *head);

/**
 *
 * Frees a given pair.
 *
 * @param pair the pair to http_free
 *
 */
extern void hpairnode_free(hpair_t *pair);

/**
 *
 * Makes a deep http_free operation. All pairnodes, beginning with the given pari, in
 * the linked list will be destroyed. 
 *
 * @param pair the pair to start to http_free the linked list
 *
 */
extern void hpairnode_free_deep(ng_list_head_s *head);

/**
 *
 * Returns the (key,value) pair, which key is the given 'key'.
 *
 * @param pair the first pair to start to search from.
 * @param key key to find the in the pair.
 * @returns if a value will be found, this function will 
 *
 * @return the value (do not http_free this string) or NULL if no pair was found with
 *         the key 'key'.
 *
 */
extern hpair_t *hpairnode_get(ng_list_head_s *head, const ng_block_s *key);

/**
 *
 * Returns the (key,value) pair, which key is the given 'key'. The case will be
 * ignored while comparing the key strings.
 *
 * @param pair the first pair to start to search from.
 * @param key key to find the in the pair.
 * @returns if a value will be found, this function will 
 *
 * @return the value (do not http_free this string) or NULL if no pair was found with
 *         the key 'key'.
 *
 */
extern hpair_t *
hpairnode_get_ignore_case(ng_list_head_s *head, const char *key, int len);

/**
 *
 * This function will create a new pair and fills the (key,value) fields of a
 * given pair. Note that the 'next' field will not be copied.
 *
 * @param src the source pair object to copy.
 *
 * @return a newly created pair with the same (key,value) pairs as in 'src'.
 *         This fields will be cloned. The'next' field will be set to NULL.
 *
 * @see hpairnode_copy_deep
 *
 */
extern hpair_t *hpairnode_copy(const hpair_t *src, ng_list_head_s *head);

/**
 *
 * Clones the hole linked list. 
 *
 * @param src the source pair object to copy from
 *
 * @return the first object in the linked list. 
 *
 * @see hpairnode_copy
 *
 */
extern int hpairnode_copy_deep(const ng_list_head_s *src, ng_list_head_s *dst);

/*--------------------------------------------------
FUNCTION: httpc_set_header
DESC: Adds a new (key, value) pair to the header
or modifies the old pair if this function will
finds another pair with the same 'key' value.
----------------------------------------------------*/
extern int hpairnode_set_header(ng_list_head_s *head, 
  const ng_block_s *key, const ng_block_s *val);
extern int hpairnode_set_header_list(ng_list_head_s *dst, 
  const ng_list_head_s *src);
extern int hpairnode_add_header(ng_list_head_s *header, 
  const ng_block_s *key, const ng_block_s *val);
extern int hpairnode_add_header_list(ng_list_head_s *header, 
  const ng_list_head_s *values);

/**
 *
 * Dumps a set of pairs.
 *
 * @see hpairnode_dump
 *
 */
extern void __hpairnode_dump_deep(const ng_list_head_s *head);
#if __NHTTP_NO_LOGGING
#define hpairnode_dump_deep(p) NG_UNUSED(p)
#else
#define hpairnode_dump_deep(p) __hpairnode_dump_deep(p)
#endif

/**
 *
 * Dumps the pair specified.
 *
 */
extern void hpairnode_dump(const hpair_t *pair);

/**
 *
 * Parses the given string and creates a new ccontent_type_t object. 
 *
 * @param content_type_str the string representation of the content-type field in
 *        a HTTP header.
 *
 * @return A newly created content_type_s object. Free this object with
 *         content_type_free();
 *
 * @see content_type_free
 *
 */
extern content_type_s *content_type_new(const char *content_type_str, int size);

/**
 *
 * Frees the given content_type_s object
 *
 */
extern void content_type_free(content_type_s * ct);
extern void content_type_print(content_type_s *ct);
extern void test_content_type(void);

static inline int SAFE_STRLEN(const char *x)
{
  if (likely((x)!=NULL))
    return strlen(x);
  else
    return 0;
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
