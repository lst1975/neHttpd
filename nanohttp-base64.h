/******************************************************************
*  $Id: nanohttp-base64.h,v 1.5 2007/11/03 22:40:10 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
*
* This library is free software; you can redistribute it and/or
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

#include <stdint.h>
#include <stddef.h>
#include "nanohttp-config.h"
#include "nanohttp-common.h"

#define __configUseBase64Decode 1

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

struct b64_state{
  int16_t s;
  int16_t j;
  unsigned char c;
  unsigned char l;
  uint16_t pad;
};
typedef struct b64_state b64_state_s;

typedef httpd_buf_t ng_buffer_s;

/*
 * out is null-terminated encode string.
 * return values is out length, exclusive terminating `\0'
 */
int b64Encode(const ng_buffer_s *in, ng_buffer_s *out);
void b64Encode_Start(b64_state_s *s);
void b64Encode_Finish(b64_state_s *state, ng_buffer_s *out);
void b64Encode_Process(b64_state_s *state, const ng_buffer_s *in, ng_buffer_s *out);

#if __configUseBase64Decode
/*
 * return values is out length
 */
int b64Decode(const ng_buffer_s *in, ng_buffer_s *out);
static inline void b64Decode_Start(b64_state_s *s) { return b64Encode_Start(s); }
static inline void b64Decode_Finish(b64_state_s *state, ng_buffer_s *out) { (void)(state); (void)(out);}
int b64Decode_Process(b64_state_s *state, const ng_buffer_s *in, ng_buffer_s *out);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
