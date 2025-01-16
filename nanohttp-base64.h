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
*  $Id: nanohttp-base64.h,v 1.5 2007/11/03 22:40:10 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
*
* This library is ng_free software; you can redistribute it and/or
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
* Email: hero@persua.de
******************************************************************/
#ifndef __nanohttp_base64_h
#define __nanohttp_base64_h

/** @file nanohttp-base64.h Base64 data encoding/decoding
 *
 * @section base64_data_enc_sec Base64 data encoding/decoding
 *
 * Base encoding of data is used in many situations to store or
 * transfer data in environments that, perhaps for legacy reasons,
 * are restricted to US-ASCII data. Base64 encoding can also be used
 * in new applications that do not have legacy restrictions, simply
 * because it makes it possible to manipulate objects with text
 * editors.
 *
 * In the past, different applications have had different requirements
 * and thus sometimes implemented base encodings in slightly different
 * ways. Today, protocol specifications sometimes use base encodings
 * in general, and "base64" in particular, without a precise
 * description or reference.  Multipurpose Internet Mail Extensions
 * (MIME) is often used as a reference for base64 without considering
 * the consequences for line-wrapping or non-alphabet characters.
 *
 * @author	H. Ronsdorf
 * @version	$Revision: 1.5 $
 *
 * @see http://www.ietf.org/rfc/rfc4648.txt
 */

#ifdef __cplusplus
extern "C" {
#endif

#define __configUseStreamBase64 1

#define B64_ENCLEN(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define B64_DECLEN(s) ((unsigned int)(((s) / 4) * 3))

#if !__configUseStreamBase64

/** Base64 encodes a NUL terminated array of characters.
 *
 * @param instr		Pointer to the input string.
 * @param outstr	Pointer to the output destination.
 *
 * @see base64_decode_string
 */
extern int base64_encode_string(const unsigned char *instr, unsigned char *outstr);

/** Base64 decodes a NUL terminated array of characters.
 *
 * @param instr		Pointer to the input string.
 * @param outstr	Pointer to the output destination.
 *
 * @see base64_encode_string
 */
extern int base64_decode_string(const unsigned char *instr, unsigned char *outstr);

#else

#include "nanohttp-config.h"
#include "nanohttp-common.h"

#define __configUseBase64Decode 1

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

struct b64_state{
  ng_int16_t s;
  ng_int16_t j;
  unsigned char c;
  unsigned char l;
  ng_uint16_t pad;
};
typedef struct b64_state b64_state_s;

/*
 * out is null-terminated encode string.
 * return values is out length, exclusive terminating `\0'
 */
int b64Encode(const ng_block_s *in, ng_block_s *out);
void b64Encode_Start(b64_state_s *s);
void b64Encode_Finish(b64_state_s *state, ng_block_s *out);
void b64Encode_Process(b64_state_s *state, const ng_block_s *in, ng_block_s *out);

static inline int b64Encode_with_len(const char *in, ng_size_t inlen, char *out, ng_size_t outlen)
{
  const ng_block_s   _in = { .cptr =  in, .len = inlen};
        ng_block_s  _out = { .buf  = out, .len = outlen};
  return b64Encode(&_in, &_out);
}

#if __configUseBase64Decode
/*
 * return values is out length
 */
int b64Decode(const ng_block_s *in, ng_block_s *out);
static inline void b64Decode_Start(b64_state_s *s) { return b64Encode_Start(s); }
static inline void b64Decode_Finish(b64_state_s *state, ng_block_s *out) { HTTPD_UNUSED(state); HTTPD_UNUSED(out);}
int b64Decode_Process(b64_state_s *state, const ng_block_s *in, ng_block_s *out);

static inline int b64Decode_with_len(const char *in, ng_size_t inlen, char *out, ng_size_t outlen)
{
  const ng_block_s   _in = { .cptr =  in, .len = inlen};
        ng_block_s  _out = { .buf  = out, .len = outlen};
  return b64Decode(&_in, &_out);
}
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
