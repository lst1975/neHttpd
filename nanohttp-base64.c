/** @file nanohttp-base64.c Base64 data encoding/decoding */
/******************************************************************
*  $Id: nanohttp-base64.c,v 1.5 2007/11/03 22:40:10 m0gg Exp $
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
#include "nanohttp-base64.h"

#if !__configUseStreamBase64

char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};


int base64_encode_string(const unsigned char *instr, unsigned char *outstr)
{

    char counts = 0;
    char buffer[3];
    char* cipher = (char*)outstr; //malloc(strlen(plain) * 4 / 3 + 4);
    int i = 0, c = 0;

    for(i = 0; instr[i] != '\0'; i++) {
        buffer[(int)counts++] = (char)instr[i];
        if(counts == 3) {
            cipher[c++] = base46_map[buffer[0] >> 2];
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
            cipher[c++] = base46_map[buffer[2] & 0x3f];
            counts = 0;
        }
    }

    if(counts > 0) {
        cipher[c++] = base46_map[buffer[0] >> 2];
        if(counts == 1) {
            cipher[c++] = base46_map[(buffer[0] & 0x03) << 4];
            cipher[c++] = '=';
        } else {                      // if counts == 2
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[(buffer[1] & 0x0f) << 2];
        }
        cipher[c++] = '=';
    }

    cipher[c] = '\0';   /* string padding character */
    return c;
}


int base64_decode_string(const unsigned char *instr, unsigned char *outstr)
{
    char counts = 0;
    char buffer[4];
    char* plain = (char*)outstr; //malloc(strlen(cipher) * 3 / 4);
    int i = 0, p = 0;

    for(i = 0; instr[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base46_map[(int)k] != instr[i]; k++);
        buffer[(int)counts++] = k;
        if(counts == 4) {
            plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if(buffer[2] != 64)
                plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if(buffer[3] != 64)
                plain[p++] = (buffer[2] << 6) + buffer[3];
            counts = 0;
        }
    }

    plain[p] = '\0';    /* string padding character */
    return p;
}

#else

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
 *                              
 *                              https://github.com/ngRTOS
 **************************************************************************************
 */

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

/* BASE 64 encode table */
static const char base64en[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/',
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
  /* nul, soh, stx, etx, eot, enq, ack, bel, */
     255, 255, 255, 255, 255, 255, 255, 255,

  /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
     255, 255, 255, 255, 255, 255, 255, 255,

  /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
     255, 255, 255, 255, 255, 255, 255, 255,

  /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
     255, 255, 255, 255, 255, 255, 255, 255,

  /*  sp, '!', '"', '#', '$', '%', '&', ''', */
     255, 255, 255, 255, 255, 255, 255, 255,

  /* '(', ')', '*', '+', ',', '-', '.', '/', */
     255, 255, 255,  62, 255, 255, 255,  63,

  /* '0', '1', '2', '3', '4', '5', '6', '7', */
      52,  53,  54,  55,  56,  57,  58,  59,

  /* '8', '9', ':', ';', '<', '=', '>', '?', */
      60,  61, 255, 255, 255, 255, 255, 255,

  /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
     255,   0,   1,  2,   3,   4,   5,    6,

  /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
       7,   8,   9,  10,  11,  12,  13,  14,

  /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
      15,  16,  17,  18,  19,  20,  21,  22,

  /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
      23,  24,  25, 255, 255, 255, 255, 255,

  /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
     255,  26,  27,  28,  29,  30,  31,  32,

  /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
      33,  34,  35,  36,  37,  38,  39,  40,

  /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
      41,  42,  43,  44,  45,  46,  47,  48,

  /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
      49,  50,  51, 255, 255, 255, 255, 255
};

void
b64Encode_Start(b64_state_s *s)
{
  s->s = 0;
  s->l = 0;
  s->j = 0;
}

void
b64Encode_Finish(b64_state_s *state, ng_buffer_s *__out)
{
  unsigned char *out = __out->ptr;

  switch (state->s) {
    case 1:
      out[state->j++] = base64en[(state->l & 0x3) << 4];
      out[state->j++] = BASE64_PAD;
      out[state->j++] = BASE64_PAD;
      break;
    case 2:
      out[state->j++] = base64en[(state->l & 0xF) << 2];
      out[state->j++] = BASE64_PAD;
      break;
    default:
      break;
  }
  
  __out->len += state->j;
}

void
b64Encode_Process(b64_state_s *state, const ng_buffer_s *__in, 
  ng_buffer_s *__out)
{
  unsigned int i;
  const unsigned char *in = __in->ptr; 
  unsigned int inlen = __in->len;
  unsigned char *out = __out->ptr;

  for (i = state->j = 0; i < inlen; i++) 
  {
    unsigned char c = in[i];

    switch (state->s) {
    case 0:
      state->s = 1;
      out[state->j++] = base64en[(c >> 2) & 0x3F];
      break;
    case 1:
      state->s = 2;
      out[state->j++] = base64en[((state->l & 0x3) << 4) | ((c >> 4) & 0xF)];
      break;
    case 2:
      state->s = 0;
      out[state->j++] = base64en[((state->l & 0xF) << 2) | ((c >> 6) & 0x3)];
      out[state->j++] = base64en[c & 0x3F];
      break;
    default:
      break;
    }
    state->l = c;
  }

  __out->len += state->j;
}

int
b64Encode(const ng_buffer_s *in, ng_buffer_s *out)
{
  b64_state_s s;
  out->len = 0;
  b64Encode_Start(&s);
  b64Encode_Process(&s, in, out);
  b64Encode_Finish(&s, out);
  return out->len;
}

#if __configUseBase64Decode
int
b64Decode_Process(b64_state_s *state, const ng_buffer_s *__in, 
  ng_buffer_s *__out)
{
  unsigned int i;
  unsigned char c;
  const unsigned char *in = __in->ptr; 
  unsigned int inlen = __in->len;
  unsigned char *out = __out->ptr;

  for (i = 0; i < inlen; i++) 
  {
    if (in[i] == BASE64_PAD) 
    {
      i++;
      break;
    }
    if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST) 
    {
      return -1;
    }

    c = base64de[(unsigned char)in[i]];
    if (c == 255) 
    {
      return -1;
    }

    switch (i & 0x3) 
    {
    case 0:
      out[state->j] = (c << 2) & 0xFF;
      break;
    case 1:
      out[state->j++] |= (c >> 4) & 0x3;
      out[state->j] = (c & 0xF) << 4; 
      break;
    case 2:
      out[state->j++] |= (c >> 2) & 0xF;
      out[state->j] = (c & 0x3) << 6;
      break;
    case 3:
      out[state->j++] |= c;
      break;
    default:
      break;
    }
  }

  __out->len = state->j;
  if (i == inlen)
    return i;
  if (i < inlen && i + 1 == inlen && in[i]=='=')
  {
    return inlen;
  }
  return i;
}

int
b64Decode(const ng_buffer_s *in, ng_buffer_s *out)
{
  int len;
  b64_state_s s;

  if (in->len & 0x3) {
    return -1;
  }

  out->len = 0;
  b64Decode_Start(&s);
  len = b64Decode_Process(&s, in, out);
  if (len < 0 || len != in->len) 
    return -1;
  b64Decode_Finish(&s, out);
  return out->len;
}
#endif

#endif

