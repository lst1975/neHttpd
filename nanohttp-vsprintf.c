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
 *                              https://www.ngRTOS.org
 *                              https://github.com/ngRTOS
 **************************************************************************************
 */
 
/*
   Unix ng_snprintf implementation.
   Version 1.4

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Revision History:

   1.4:
      *  integrate in FreeRADIUS's libradius:
    *  Fetched from: http://savannah.gnu.org/cgi-bin/viewcvs/mailutils/mailutils/lib/ng_snprintf.c?rev=1.4
    *  Fetched from: http://savannah.gnu.org/cgi-bin/viewcvs/mailutils/mailutils/lib/ng_snprintf.h?rev=1.4
    *  Replace config.h with autoconf.h
    *  Protect with HAVE_SNPRINTF and HAVE_VSNPRINTF
   1.3:
      *  add #include <config.h> ifdef HAVE_CONFIG_H
      *  cosmetic change, when exponent is 0 print xxxE+00
   instead of xxxE-00
   1.2:
      *  put the program under LGPL.
   1.1:
      *  added changes from Miles Bader
      *  corrected a bug with %f
      *  added support for %#g
      *  added more comments :-)
   1.0:
      *  supporting must ANSI syntaxic_sugars
   0.0:
      *  suppot %s %c %d

 THANKS(for the patches and ideas):
     Miles Bader
     Cyrille Rustom
     Jacek Slabocewiz
     Mike Parker(mouse)

*/

#include "nanohttp-defs.h"

#include <stdarg.h>
#include <stdint.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <Windows.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#endif

#include "nanohttp-ctype.h"
#include "nanohttp-itoa.h"
#include "nanohttp-atoi.h"
#include "nanohttp-time.h"
#include "nanohttp-vsprintf.h"
#include "nanohttp-dtoa.h"
#include "nanohttp-server.h"

#define PRIVATE static
#define PUBLIC

#define ngrtos_FALSE 0
#define ngrtos_TRUE  1

/*
 * For the FLOATING POINT FORMAT :
 *  the challenge was finding a way to
 *  manipulate the Real numbers without having
 *  to resort to mathematical function(it
 *  would require to link with -lm) and not
 *  going down to the bit pattern(not portable)
 *
 *  so a number, a real is:

      real = integral + fraction

      integral = ... + a(2)*10^2 + a(1)*10^1 + a(0)*10^0
      fraction = b(1)*10^-1 + b(2)*10^-2 + ...

      where:
       0 <= a(i) => 9
       0 <= b(i) => 9

    from then it was simple math
 */

/*
 * size of the buffer for the integral part
 * and the fraction part
 */
#define MAX_INT  24 + 1 /* 1 for the null */
#define MAX_FRACT 24 + 1

/*
 * If the compiler supports (long long)
 */
#ifndef LONG_LONG
# define LONG_LONG long long
/*# define LONG_LONG int64_t*/
#endif

/*
 * If the compiler supports (long double)
 */
#ifndef LONG_DOUBLE
# define LONG_DOUBLE long double
/*# define LONG_DOUBLE double*/
#endif

#define SWAP_INT(a,b) {int t; t = (a); (a) = (b); (b) = t;}

/* this struct holds everything we need */
struct DATA {
  int length;
  int nbytes;
  int err;
  char *holder;
  int counter;
  char fmtchr;
  char const *pf;
/* FLAGS */
  int width, precision;
  int justify; char pad;
  int square, space, star_w, star_p, a_long, a_longlong;
  void *arg;
  ng_vsout_f out;
  ng_vsout_f _out;
  int (*fine)(struct DATA *d);
};

/*
 * numtoa() uses PRIVATE buffers to store the results,
 * So this function is not reentrant
 */
PRIVATE inline int ng_itoa(double n, char *buf, int len) 
{
  return i64toa_jeaiii((int64_t)n, buf, len);
}

/* for the format */
PRIVATE void conv_flag(char *, struct DATA *);
PRIVATE void fdtoa(struct DATA *p, double d);
PRIVATE void decimal_i(struct DATA *, int64_t);
PRIVATE void decimal_u(struct DATA *, uint64_t);
PRIVATE void octal(struct DATA *, uint64_t);
PRIVATE void hexa(struct DATA *, uint64_t);
PRIVATE void strings(struct DATA *, const char *);

/* those are defines specific to ng_snprintf to hopefully
 * make the code clearer :-)
 */
#define RIGHT 1
#define LEFT  0
#define NOT_FOUND -1
#define FOUND 1
#define MAX_FIELD 15

/* the conversion flags */
#define isflag(c) ((c) == '#' || (c) == ' ' || \
       (c) == '*' || (c) == '+' || \
       (c) == '-' || (c) == '.' || \
       __ng_isdigit(c))

/* round off to the precision */
#define ROUND(d, p) \
      (d < 0.) ? \
       d - pow_10(-(p)->precision) * 0.5 : \
       d + pow_10(-(p)->precision) * 0.5

/* set default precision */
#define DEF_PREC(p) \
      if ((p)->precision == NOT_FOUND) \
        (p)->precision = 6

/* put a char */
#define PUT_CHAR(c, p) do {\
        if ((p)->out) \
        { \
          char b[1]={c}; \
          if ((p)->out((p), b, 1)<0) \
            return; \
          break; \
        } \
        if ((p)->counter < (p)->length) { \
          *(p)->holder++ = (uint8_t)(c); \
          (p)->counter++; \
        } \
      } while(0)

/* put a string */
#define PUT_STRING(s, l, p) do {\
        if ((p)->out) \
        { \
          if ((p)->out((p), s, l) < 0) \
            return; \
          break; \
        } \
        if ((p)->counter + l < (p)->length) { \
          ng_memcpy((p)->holder, s, l); \
          (p)->holder  += l; \
          (p)->counter += l; \
        } else { \
          int n = (p)->length - (p)->counter; \
          ng_memcpy((p)->holder, s, n); \
          (p)->holder  += n; \
          (p)->counter += n; \
        } \
      } while(0)

#define PUT_PLUS(d, p) \
      if ((d) > 0. && (p)->justify == RIGHT) \
        PUT_CHAR('+', p)

#define PUT_SPACE(d, p) \
      if ((p)->space == FOUND && (d) > 0.) \
        PUT_CHAR(' ', p)

/* pad right */
#define PAD_RIGHT(p) \
      if ((p)->width > 0 && (p)->justify != LEFT) \
        for (; (p)->width > 0; (p)->width--) \
          PUT_CHAR((p)->pad, p)

/* pad left */
#define PAD_LEFT(p) \
      if ((p)->width > 0 && (p)->justify == LEFT) \
        for (; (p)->width > 0; (p)->width--) \
          PUT_CHAR((p)->pad, p)

/* if width and prec. in the args */
#define STAR_ARGS(p) \
      if ((p)->star_w == FOUND) \
        (p)->width = va_arg(args, int); \
      if ((p)->star_p == FOUND) \
        (p)->precision = va_arg(args, int)

/* for %d and friends, it puts in holder
 * the representation with the right padding
 */
PRIVATE void
decimal_i(struct DATA *p, int64_t d)
{
  char buf[MAX_INT];
  char *tmp=buf;
  int len;

  len = i64toa_jeaiii(d, buf, sizeof(buf));
  p->width -= len;
  PAD_RIGHT(p);
  PUT_PLUS(d, p);
  PUT_SPACE(d, p);
  PUT_STRING(tmp, len, p);
  PAD_LEFT(p);
}

/* for %d and friends, it puts in holder
 * the representation with the right padding
 */
PRIVATE void
decimal_u(struct DATA *p, uint64_t d)
{
  char buf[MAX_INT];
  char *tmp=buf;
  int len;

  len = u64toa_jeaiii(d, buf, sizeof(buf));
  p->width -= len;
  PAD_RIGHT(p);
  PUT_PLUS(d, p);
  PUT_SPACE(d, p);
  PUT_STRING(tmp, len, p);
  PAD_LEFT(p);
}

static int 
intToOctalString(uint64_t number, char *octalString) {
  int i = 0;
  int temp = number;

  // Handle the special case where number is 0
  if (number == 0) {
    octalString[i++] = '0';
    octalString[i] = '\0';
    return 2;
  }

  // Convert integer to octal string
  while (temp != 0) {
    int remainder = temp & 7; /* temp % 8*/
    octalString[i++] = '0' + remainder;
    temp >>= 3; /* temp /= 8; */
  }

  // Null-terminate the string
  octalString[i] = '\0';

  // Reverse the string to get the correct octal representation
  int start = 0;
  int end = i - 1;
  while (start < end) {
    char tempChar = octalString[start];
    octalString[start] = octalString[end];
    octalString[end] = tempChar;
    start++;
    end--;
  }

  return i;
}

/* for %o octal representation */
PRIVATE void
octal(struct DATA *p, uint64_t d)
{
  /* The octal representation of UINT64_MAX is 1777777777777777777777 
   * UINT64_MAX is 2^64 - 1, 18446744073709551615
   */
  char buf[MAX_INT];
  char *tmp = buf;
  int len;

  len = intToOctalString(d, buf);
  p->width -= len;
  PAD_RIGHT(p);
  if (p->square == FOUND) /* had prefix '0' for octal */
    PUT_CHAR('0', p);
  PUT_STRING(tmp, len, p);
  PAD_LEFT(p);
}

/*
  e, E   The  double argument is rounded and converted in the style 
         [-]d.ddde¡Àdd where there is one digit (which is nonzero 
         if the argument is nonzero) before the decimal-point 
         character and the number of digits after it is equal to 
         the precision; if the precision is missing, it is taken as 6; 
         if the precision is zero, no decimal-point character appears.  
         An E conversion uses the letter E (rather than e) to introduce
         the exponent. The exponent always contains at least two digits; 
         if the value is zero, the exponent is 00.

  f, F   The double argument is rounded and converted to decimal notation 
         in the style [-]ddd.ddd, where the number of digits after the 
         decimal-point character is equal to the precision specification. 
         If the precision is missing, it is taken as 6; if the precision 
         is explicitly zero, no decimal-point character appears. If a 
         decimal point appears, at least one digit appears before it.

         (SUSv2 does not know about F and says that character string 
         representations for infinity and NaN may be made available. 
         SUSv3 adds a specification for F. The C99 standard specifies 
         "[-]inf" or "[-]infinity" for infinity, and a string starting 
         with "nan" for NaN, in the case of f conversion, and "[-]INF"
         or "[-]INFINITY" or "NAN" in the case of F conversion.)

  g, G   The double argument is converted in style f or e (or F or E for 
         G conversions). The precision specifies the number of significant 
         digits. If the precision is missing, 6 digits are given; if the 
         precision is zero, it is treated as 1. Style e is used if the 
         exponent from its conversion is less than -4 or greater than or 
         equal to the precision. Trailing zeros are removed from the 
         fractional part of the result; a decimal point appears only if 
         it is followed by at least one digit.
*/
PRIVATE void
fdtoa(struct DATA *p, double d)
{
  /* The octal representation of UINT64_MAX is 1777777777777777777777 
   * UINT64_MAX is 2^64 - 1, 18446744073709551615
   */
  char buf[NG_DOUBLE_STR_LEN_MAX];
  char *tmp = buf;
  int ret, size = sizeof(buf);
  int flags = 0;
  char format_char;

  if (p->pad == '0')
    flags |= DCONVSTR_FLAG_PAD_WITH_ZERO;

  format_char = p->pf[0];
  if (p->pf[0] == 'G' || p->pf[0] == 'E')
  {
    flags |= DCONVSTR_FLAG_UPPERCASE;
    if (format_char == 'G')
      format_char = 'g';
    else if (format_char == 'E')
      format_char = 'e';
  }

  if (p->justify == LEFT)
    flags |= DCONVSTR_FLAG_LEFT_JUSTIFY;

  if (p->width != NOT_FOUND)
    flags |= DCONVSTR_FLAG_HAVE_WIDTH;
  
  ret = dconvstr_ieee754_print(&tmp, &size, d, format_char, 
    flags, p->width, p->precision != NOT_FOUND ? 
        p->precision : DCONVSTR_DEFAULT_PRECISION);
  if (ret)
  {
    PUT_STRING(buf, tmp - buf, p);
  }
}

/* for %o octal representation */
PRIVATE void
times(struct DATA *p, uint64_t d)
{
  char buf[32];
  int len;
  ng_rtc_time_s *tm;

  switch (p->pf[1]) {
  case 'r':
    p->pf++;
    tm = (ng_rtc_time_s *)(uintptr_t)d;
    len = raw_ng_tm(tm, buf, sizeof(buf));
		break;
	case 'R':
    p->pf++;
    tm = (ng_rtc_time_s *)(uintptr_t)d;
    len = raw_ng_http_tm(tm, buf, sizeof(buf), p->square);
		break;
  case 'T':
    p->pf++;
    fallthrough();
  default:
    if (p->square)
      len = raw_ng_http_date(d, buf, sizeof(buf), 1, " GMT");
    else
      len = raw_ng_http_date(d, buf, sizeof(buf), 0, NULL);
		break;
	}
  
  if (len > 0)
  {
    p->width -= len;
    PAD_RIGHT(p);
    PUT_STRING(buf, len, p);
    PAD_LEFT(p);
  }
}

/* for %x %X hexadecimal representation */
PRIVATE void
hexa(struct DATA *d, const uint64_t n)
{
  char buf[MAX_INT];
  int len;
  char *p,*e;
	char separator;

  switch (d->pf[1]) {
	case 'C':
      separator = ':';
      d->pf++;
      break;
	case 'D':
      separator = '-';
      d->pf++;
      break;
	case 'N':
      separator = ' ';
      d->pf++;
      break;
	default:
      separator = 0;
      break;
	}

  len = ng_u64toh(n, buf, sizeof(buf), d->fmtchr == 'X', separator);
  p=buf;
  e=p+len;
  while (*p=='0' && p < e)
  {
    len--;
    p++;
  }

  d->width -= len;
  PAD_RIGHT(d);
  if (d->square == FOUND) { /* prefix '0x' for hexa */
    PUT_CHAR('0', d); 
    PUT_CHAR(d->fmtchr, d);
  }
  PUT_STRING(p, len, d);
  PAD_LEFT(d);
}

/* %s strings */
PRIVATE void
strings(struct DATA *p, const char *tmp)
{
  int i;

  i = ng_strlen(tmp);
  if (p->precision != NOT_FOUND) /* the smallest number */
    i = (i < p->precision ? i : p->precision);
  p->width -= i;
  PAD_RIGHT(p);
  PUT_STRING(tmp, i, p);
  PAD_LEFT(p);
}

/* initialize the conversion specifiers */
PRIVATE void
conv_flag(char * s, struct DATA * p)
{
  char number[MAX_FIELD/2];
  int i;

  /* reset the flags.  */
  p->precision = p->width = NOT_FOUND;
  p->star_w = p->star_p = NOT_FOUND;
  p->square = p->space = NOT_FOUND;
  p->a_long = p->justify = NOT_FOUND;
  p->a_longlong = NOT_FOUND;
  p->pad = ' ';

  for(;s && *s ;s++)
  {
    switch (*s)
    {
      case ' ': 
        p->space = FOUND; 
        break;
        
      case '#': 
        p->square = FOUND; 
        break;
        
      case '*': 
        if (p->width == NOT_FOUND)
          p->width = p->star_w = FOUND;
        else
          p->precision = p->star_p = FOUND;
        break;
        
      case '+': 
        p->justify = RIGHT; 
        break;
        
      case '-': 
        p->justify = LEFT; 
        break;
        
      case '.': 
        if (p->width == NOT_FOUND)
          p->width = 0;
        break;
        
      case '0': 
        p->pad = '0'; 
        break;
        
      case '1': case '2': case '3':
      case '4': case '5': case '6':
      case '7': case '8': case '9':     /* gob all the digits */
        for (i = 0; __ng_isdigit(*s); i++, s++)
          if (i < MAX_FIELD/2 - 1)
            number[i] = *s;
        if (p->width == NOT_FOUND)
          p->width = ng_atoi64(number, i, NULL);
        else
          p->precision = ng_atoi64(number, i, NULL);
        s--;   /* went to far go back */
        break;
    }
  }
}

static inline char *hex_byte_pack_upper(char *buf, uint8_t byte)
{
  const char hex_asc_upper[] = "0123456789ABCDEF";
#define hex_asc_upper_lo(x)  hex_asc_upper[((x) & 0x0f)]
#define hex_asc_upper_hi(x)  hex_asc_upper[((x) & 0xf0) >> 4]
  *buf++ = hex_asc_upper_hi(byte);
  *buf++ = hex_asc_upper_lo(byte);
  return buf;
}

static inline char *hex_byte_pack(char *buf, uint8_t byte)
{
  const char hex_asc[]       = "0123456789abcdef";
#define hex_asc_lo(x)  hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)  hex_asc[((x) & 0xf0) >> 4]
  *buf++ = hex_asc_hi(byte);
  *buf++ = hex_asc_lo(byte);
  return buf;
}
static void uuid_string(struct DATA *d, const uint8_t *addr)
{
/* UUID constants */

#define UUID_BUFFER_LENGTH          16  /* Length of UUID in memory */
#define UUID_STRING_LENGTH          36  /* Total length of a UUID string */

/* Positions for required hyphens (dashes) in UUID strings */

#define UUID_HYPHEN1_OFFSET         8
#define UUID_HYPHEN2_OFFSET         13
#define UUID_HYPHEN3_OFFSET         18
#define UUID_HYPHEN4_OFFSET         23
static const int guid_index[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};
static const int uuid_index[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  char uuid[UUID_STRING_LENGTH + 1];
  char *p = uuid;
  int i;
  const uint8_t *index = (const uint8_t *)uuid_index;
  int8_t uc = ngrtos_FALSE;

  switch (d->pf[1]) {
  case 'L':
    uc = ngrtos_TRUE;
    index = (const uint8_t *)guid_index;
    d->pf++;
    break;

  case 'l':
    index = (const uint8_t *)guid_index;
    d->pf++;
    break;
    
  case 'B':
    uc = ngrtos_TRUE;
    d->pf++;
    break;
  }

  for (i = 0; i < 16; i++) {
    if (uc)
      p = hex_byte_pack_upper(p, addr[index[i]]);
    else
      p = hex_byte_pack(p, addr[index[i]]);
    switch (i) {
    case 3:
    case 5:
    case 7:
    case 9:
      *p++ = '-';
      break;
    }
  }

  *p = 0;

  strings(d, uuid);
}

static void 
mac_address_string(struct DATA *d, const uint8_t *addr)
{
  char mac_addr[sizeof("xx:xx:xx:xx:xx:xx")];
  char *p = mac_addr;
  int i;
  char separator;
  int8_t reversed = ngrtos_FALSE;
  char pf = d->fmtchr;
  
  switch (d->pf[1]) {
  case 'F':
    separator = '-';
    d->pf++;
    break;

  case 'R':
    d->pf++;
    reversed = ngrtos_TRUE;
    separator = ':';
    break;

  default:
    separator = ':';
    break;
  }

  for (i = 0; i < 6; i++) {
    if (reversed)
      p = hex_byte_pack(p, addr[5 - i]);
    else
      p = hex_byte_pack(p, addr[i]);

    if (pf == 'M' && i != 5)
      *p++ = separator;
  }
  *p = '\0';

  strings(d, mac_addr);
}

static char *
__ip4_string(char *p, const uint8_t *addr, const char *fmt)
{
  int i;
  int8_t leading_zeros = (fmt[0] == 'i');
  int index;
  int step;

  switch (fmt[2]) {
  case 'h':
#if RTE_BYTE_ORDER == RTE_BIG_ENDIAN
    index = 0;
    step = 1;
#else
    index = 3;
    step = -1;
#endif
    break;
  case 'l':
    index = 3;
    step = -1;
    break;
  case 'n':
  case 'b':
  default:
    index = 0;
    step = 1;
    break;
  }
  
  for (i = 0; i < 4; i++) {
    /* hold each IP quad in normal order */
    const ng_block_s *b = &__ng_uint8_string[addr[index]];
    int digits = b->len;
    if (leading_zeros) {
      if (digits < 3)
        *p++ = '0';
      if (digits < 2)
        *p++ = '0';
    }
    for (int x=0;x<digits;x++)
      *p++ = b->cptr[x];
    if (i < 3)
      *p++ = '.';
    index += step;
  }
  *p = '\0';

  return p;
}

#ifdef WIN32
  #define ng_s6_addr32(a, i) ((uint32_t *)(a))[i]
  #define ng_s6_addr16(a, i) ((uint16_t *)(a))[i]
  #define ng_s6_addr8(a, i)  ((uint8_t *)(a))[i]
#else
  #define ng_s6_addr32(a, i) (a)->s6_addr32[i]
  #define ng_s6_addr16(a, i) (a)->s6_addr16[i]
  #define ng_s6_addr8(a, i)  (a)->s6_addr[i]
#endif
/*
 * Note that we must __force cast these to unsigned long to make sparse happy,
 * since all of the endian-annotated types are fixed size regardless of arch.
 */
static inline int 
ipv6_addr_v4mapped(const struct in6_addr *a)
{
  return (
#if __NG_BITS_PER_LONG == 64
    *(unsigned long *)a |
#else
    (unsigned long)(ng_s6_addr32(a, 0) | ng_s6_addr32(a, 1)) |
#endif
    (unsigned long)(ng_s6_addr32(a, 2) ^
          rte_cpu_to_be_32(0x0000ffff))) == 0UL;
}

static inline int 
ipv6_addr_is_isatap(const struct in6_addr *a)
{
  return (ng_s6_addr32(a, 2) | rte_cpu_to_be_32(0x02000000)) 
    == rte_cpu_to_be_32(0x02005EFE);
}

static char *
ip6_compressed_string(char *p, const uint8_t *addr)
{
  const char hex_asc[] = "0123456789abcdef";
#define hex_asc_lo(x)	hex_asc[((x) & 0x0f)]
  int i, j, range;
  unsigned char zerolength[8];
  int longest = 1;
  int colonpos = -1;
  uint16_t word;
  uint8_t hi, lo;
  int8_t needcolon = ngrtos_FALSE;
  int8_t useIPv4;
  struct in6_addr in6;

  ng_memcpy(&in6, addr, sizeof(struct in6_addr));

  useIPv4 = ipv6_addr_v4mapped(&in6) || ipv6_addr_is_isatap(&in6);
  *(uint64_t *)zerolength = 0;

  if (useIPv4)
    range = 6;
  else
    range = 8;

  /* find position of longest 0 run */
  for (i = 0; i < range; i++) {
    for (j = i; j < range; j++) {
      if (ng_s6_addr16(&in6, j) != 0)
        break;
      zerolength[i]++;
    }
  }
  for (i = 0; i < range; i++) {
    if (zerolength[i] > longest) {
      longest = zerolength[i];
      colonpos = i;
    }
  }
  if (longest == 1)    /* don't compress a single 0 */
    colonpos = -1;

  /* emit address */
  for (i = 0; i < range; i++) {
    if (i == colonpos) {
      if (needcolon || i == 0)
        *p++ = ':';
      *p++ = ':';
      needcolon = ngrtos_FALSE;
      i += longest - 1;
      continue;
    }
    if (needcolon) {
      *p++ = ':';
      needcolon = ngrtos_FALSE;
    }
    /* hex uint16_t without leading 0s */
    word = ng_ntohs(ng_s6_addr8(&in6, i));
    hi = word >> 8;
    lo = word & 0xff;
    if (hi) {
      if (hi > 0x0f)
        p = hex_byte_pack(p, hi);
      else
        *p++ = hex_asc_lo(hi);
      p = hex_byte_pack(p, lo);
    }
    else if (lo > 0x0f)
      p = hex_byte_pack(p, lo);
    else
      *p++ = hex_asc_lo(lo);
    needcolon = ngrtos_TRUE;
  }

  if (useIPv4) {
    if (needcolon)
      *p++ = ':';
    p = __ip4_string(p, (const uint8_t *)&ng_s6_addr8(&in6, 12), "I4");
  }
  *p = '\0';

  return p;
}
#ifdef WIN32
#undef ng_s6_addr32
#undef ng_s6_addr16
#endif

static char *
ip6_string(char *p, const uint8_t *addr, const char *fmt)
{
  int i;

  for (i = 0; i < 8; i++) {
    p = hex_byte_pack(p, *addr++);
    p = hex_byte_pack(p, *addr++);
    if (fmt[0] == 'I' && i != 7)
      *p++ = ':';
  }
  *p = '\0';

  return p;
}

static void
__ng_inet_ntop6(char *ip6_addr, const uint8_t *addr, struct DATA *d)
{
  if (d->fmtchr == 'I' && d->pf[1] == 'c')
  {
    d->pf++;
    ip6_compressed_string(ip6_addr, addr);
  }
  else
  {
    ip6_string(ip6_addr, addr, &d->fmtchr);
  }
}

static void
__ip6_addr_string(struct DATA *d, const uint8_t *addr)
{
  char ip6_addr[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255")];
  __ng_inet_ntop6(ip6_addr, addr, d);
  strings(d, ip6_addr);
}

static void
__ip4_addr_string(struct DATA *d, const uint8_t *addr)
{
  char ip4_addr[sizeof("255.255.255.255")];
  char fmt[]={d->fmtchr, '4', d->pf[1]};

  __ip4_string(ip4_addr, addr, fmt);
  if (fmt[2]=='h' || fmt[2]=='l' || fmt[2]=='n' || fmt[2]=='b' || fmt[2]=='c')
    d->pf++;
  
  strings(d, ip4_addr);
}

static void
ip6_addr_string_sa(struct DATA *d, const struct sockaddr_in6 *sa)
{
  int8_t have_p = ngrtos_FALSE;
  int8_t have_s = ngrtos_FALSE;
  int8_t have_f = ngrtos_FALSE;
  int8_t have_c = ngrtos_FALSE;
  char ip6_addr[sizeof("[xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255]") +
          sizeof(":12345") + sizeof("/123456789") +
          sizeof("%1234567890")];
  char *p = ip6_addr, *pend = ip6_addr + sizeof(ip6_addr);
  const uint8_t *addr = (const uint8_t *) &sa->sin6_addr;
  char fmt6[3] = { d->fmtchr, '6', 0 };
  uint8_t off = 0;
  int meet = 1;
  
  while (meet) 
  {
    switch (d->pf[1]) {
    case 'p':
      have_p = ngrtos_TRUE;
      break;
    case 'f':
      have_f = ngrtos_TRUE;
      break;
    case 's':
      have_s = ngrtos_TRUE;
      break;
    case 'c':
      have_c = ngrtos_TRUE;
      break;
    default:
      meet = 0;
      break;
    }
    if (meet)
      d->pf++;
  }

  if (have_p || have_s || have_f) {
    *p = '[';
    off = 1;
  }

  if (d->fmtchr == 'I' && have_c)
    p = ip6_compressed_string(ip6_addr + off, addr);
  else
    p = ip6_string(ip6_addr + off, addr, fmt6);

  if (have_p || have_s || have_f)
    *p++ = ']';

  if (have_p) {
    *p++ = ':';
    p += ng_itoa(ng_ntohs(sa->sin6_port), p, pend - p);
  }
  if (have_f) {
    *p++ = '/';
#define IPV6_FLOWINFO_MASK rte_cpu_to_be_32(0x0FFFFFFF)
    p += ng_itoa(ng_ntohl(sa->sin6_flowinfo & IPV6_FLOWINFO_MASK),
            p, pend - p);
  }
  if (have_s) 
  {
    *p++ = '%';
    p += ng_itoa(sa->sin6_scope_id, p, pend - p);
  }
  *p = '\0';

  strings(d, ip6_addr);
}

static void
ip4_addr_string_sa(struct DATA *d, const struct sockaddr_in *sa)
{
  int8_t have_p = ngrtos_FALSE;
  char *p, ip4_addr[sizeof("255.255.255.255") + sizeof(":12345")];
  char *pend = ip4_addr + sizeof(ip4_addr);
  const uint8_t *addr = (const uint8_t *) &sa->sin_addr.s_addr;
  char fmt4[3] = { d->fmtchr, '4', 0 };
  int meet = 1;
  
  while (meet) 
  {
    switch (d->pf[1]) {
    case 'p':
      have_p = ngrtos_TRUE;
      break;
    case 'h':
    case 'l':
    case 'n':
    case 'b':
    case 'c':
      fmt4[2] = *d->pf;
      break;
    default:
      meet = 0;
      break;
    }
    if (meet)
      d->pf++;
  }

  p = __ip4_string(ip4_addr, addr, fmt4);
  if (have_p) {
    *p++ = ':';
    p += ng_itoa(ng_ntohs(sa->sin_port), p, pend - p);
  }
  *p = '\0';

  strings(d, ip4_addr);
}

static void
ip_addr_string(struct DATA *d, const void *ptr)
{
  char *err_fmt_msg;

  err_fmt_msg = d->pf[0] == 'i' ? "(%pi?)" : "(%pI?)";
  
  switch (d->pf[1]) {
  case '6':
    d->pf++;
    __ip6_addr_string(d, (const uint8_t *)ptr);
    return;
  case '4':
    d->pf++;
    __ip4_addr_string(d, (const uint8_t *)ptr);
    return;
  case 'S': {
    d->pf++;
    const union {
      struct sockaddr    raw;
      struct sockaddr_in  v4;
      struct sockaddr_in6  v6;
    } *sa = ptr;

    switch (sa->raw.sa_family) {
      case AF_INET:
        ip4_addr_string_sa(d, &sa->v4);
        return;
      case AF_INET6:
        ip6_addr_string_sa(d, &sa->v6);
        return;
      default:
        strings(d, "(einval)");
        return;
      }
    }
  }

  strings(d, err_fmt_msg);
}

/* %S ng_block_s */
PRIVATE void
blocks(struct DATA *p, const ng_block_s *b)
{
  int i;

  i = b->len;
  if (p->precision != NOT_FOUND) /* the smallest number */
    i = (i < p->precision ? i : p->precision);
  p->width -= i;
  PAD_RIGHT(p);
  PUT_STRING(b->buf, i, p);
  PAD_LEFT(p);
}

/*
 * Show a '%p' thing.  A kernel extension is that the '%p' is followed
 * by an extra set of alphanumeric characters that are extended format
 * specifiers.
 *
 * Please update scripts/checkpatch.pl when adding/removing conversion
 * characters.  (Search for "check for vsprintf extension").
 *
 * Right now we handle:
 *
 * - 'M' For a 6-byte MAC address, it prints the address in the
 *       usual colon-separated hex notation
 * - 'm' For a 6-byte MAC address, it prints the hex address without colons
 * - 'MF' For a 6-byte MAC FDDI address, it prints the address
 *       with a dash-separated hex notation
 * - '[mM]R' For a 6-byte MAC address, Reverse order (Bluetooth)
 * - 'I' [46] for IPv4/IPv6 addresses printed in the usual way
 *       IPv4 uses dot-separated decimal without leading 0's (1.2.3.4)
 *       IPv6 uses colon separated network-order 16 bit hex with leading 0's
 *       [S][pfs]
 *       Generic IPv4/IPv6 address (struct sockaddr *) that falls back to
 *       [4] or [6] and is able to print port [p], flowinfo [f], scope [s]
 * - 'i' [46] for 'raw' IPv4/IPv6 addresses
 *       IPv6 omits the colons (01020304...0f)
 *       IPv4 uses dot-separated decimal with leading 0's (010.123.045.006)
 *       [S][pfs]
 *       Generic IPv4/IPv6 address (struct sockaddr *) that falls back to
 *       [4] or [6] and is able to print port [p], flowinfo [f], scope [s]
 * - '[Ii][4S][hnbl]' IPv4 addresses in host, network, big or little endian order
 * - 'I[6S]c' for IPv6 addresses printed as specified by
 *       https://tools.ietf.org/html/rfc5952
 * - 'E[achnops]' For an escaped buffer, where rules are defined by combination
 *                of the following flags (see string_escape_mem() for the
 *                details):
 *                  a - ESCAPE_ANY
 *                  c - ESCAPE_SPECIAL
 *                  h - ESCAPE_HEX
 *                  n - ESCAPE_NULL
 *                  o - ESCAPE_OCTAL
 *                  p - ESCAPE_NP
 *                  s - ESCAPE_SPACE
 *                By default ESCAPE_ANY_NP is used.

 * - 'U' For a 16 byte UUID/GUID, it prints the UUID/GUID in the form
 *       "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 *       Options for %pU are:
 *         b big endian lower case hex (default)
 *         B big endian UPPER case hex
 *         l little endian lower case hex
 *         L little endian UPPER case hex
 *           big endian output byte order is:
 *             [0][1][2][3]-[4][5]-[6][7]-[8][9]-[10][11][12][13][14][15]
 *           little endian output byte order is:
 *             [3][2][1][0]-[5][4]-[7][6]-[8][9]-[10][11][12][13][14][15]

 * - 't[RT][dt][r][s]' For time and date as represented by:
 *      R    struct rtc_time
 *      T    time64_t
 * - 'C' For a clock, it prints the name (Common Clock Framework) or address
 *       (legacy clock framework) of the clock
 * - 'Cn' For a clock, it prints the name (Common Clock Framework) or address
 *        (legacy clock framework) of the clock
 * - 'G' For flags to be printed as a collection of symbolic strings that would
 *       construct the specific value. Supported flags given by option:
 *       p page flags (see struct page) given as pointer to unsigned long
 *       g gfp flags (GFP_* and __GFP_*) given as pointer to gfp_t
 *       v vma flags (VM_*) given as pointer to unsigned long
 *
 * ** When making changes please also update:
 *  Documentation/core-api/printk-formats.rst
 *
 * Note: The default behaviour (unadorned %p) is to hash the address,
 * rendering it useful as a unique identifier.
 */
PRIVATE void
pointer(struct DATA *d, const void *ptr)
{
  switch (d->pf[1]) {
  case 'M':      /* Colon separated: 00:01:02:03:04:05 */
  case 'm':      
    /* Contiguous: 000102030405 */
    /* [mM]F (FDDI) */
    /* [mM]R (Reverse order; Bluetooth) */
    d->pf++;
    d->fmtchr = d->pf[1];
    mac_address_string(d, (const uint8_t *)ptr);
    break;
  case 'I':      
    /* Formatted IP supported
     * 4:  1.2.3.4
     * 6:  0001:0203:...:0708
     * 6c:  1::708 or 1::1.2.3.4
     */
  case 'i':      
     /* Contiguous:
     * 4:  001.002.003.004
     * 6:   000102...0f
     */
    d->pf++;
    d->fmtchr = d->pf[0];
    ip_addr_string(d, ptr);
    return;
  case 'U':
    d->pf++;
    uuid_string(d, (const uint8_t *)ptr);
    return;
  case 'S':
    d->pf++;
    blocks(d, (const ng_block_s *)ptr);
    break;
  case 'u':
  case 'k':
    d->pf++;
    switch (d->pf[2]) {
    case 's':
      d->pf++;
      strings(d, (const char *)ptr);
      return;
    default:
      strings(d, "(einval)");
      return;
    }
  default:
    d->fmtchr = 'X';
    hexa(d, (uintptr_t)ptr);
    return;
  }
}

PRIVATE void
bitsstr(struct DATA *d, unsigned LONG_LONG _uquad, const char *b)
{
  char *z, *e;
  int tmp, n;
  char buf[512];
  char *buf_end = buf + sizeof(buf);

  z = buf;
  e = buf + sizeof(buf);
  
  if (*b == 8)
    z += ng_snprintf(buf, sizeof buf, "%llo", _uquad);
  else if (*b == 10)
  {
    z += u64toa_jeaiii(_uquad, z, buf_end - z);
  }
  else if (*b == 16)
  {
    z += u64toh_jeaiii(_uquad, z, buf_end - z, ngrtos_FALSE);
  }
  else
    return;
  
  b++;
  
  if (_uquad) 
  {
    tmp = 0;
    while ((n = *b++) != 0) 
    {
      if (n & 0x80)
        n &= 0x7f;
      else if (n <= ' ')
        n = n - 1;
      if (_uquad & (1LL << n)) 
      {
        if (z < e)
          *z++ = (tmp ? ',':'<');
        while (*b > ' ' &&
            (*b & 0x80) == 0) 
        {
          if (z < e)
            *z++ = *b;
          b++;
        }
        tmp = 1;
      } 
      else 
      {
        while (*b > ' ' &&
            (*b & 0x80) == 0)
          b++;
      }
    }
    if (tmp) 
    {
      if (z < e)
        *z++ = '>';
    }
  }

  if (z < e)
    *z = '\0';
  else
    *(z-1) = '\0';
  strings(d, buf);
}

PRIVATE void
char1(struct DATA *d, int c)
{
  PUT_CHAR(c, d);
}

PRIVATE int
__ng_vsnprintf_internal(struct DATA *data, va_list args)
{
  char conv_field[MAX_FIELD];
  double d; /* temporary holder */
  uint64_t nu;
  int64_t ni;
  int state;
  int i;
  int ch;

  for (; *data->pf && data->fine(data); data->pf++) 
  {
    /* we got a magic % cookie */
    if ( *data->pf == '%' ) 
    { 
      conv_flag((char *)0, data); /* initialise format flags */
      for (state = 1; *data->pf && state;) {
        switch (*(++data->pf)) {
          case '\0': /* a NULL here ? ? bail out */
            if (data->holder && !data->out)
              *data->holder = '\0';
            return data->counter;
            break;
          case 'f':  /* float, double */
          case 'g':
          case 'G':
          case 'e':
          case 'E':  /* Exponent double */
            STAR_ARGS(data);
            if (data->a_long == FOUND)
               d = va_arg(args, LONG_DOUBLE);
            else
               d = va_arg(args, double);
            fdtoa(data, d);
            state = 0;
            break;
          case 't':
            STAR_ARGS(data);
            if (data->a_longlong == FOUND)
               d = va_arg(args, unsigned LONG_LONG);
            else
               d = va_arg(args, unsigned long);
            times(data, d);
            state = 0;
            break;
          case 'u':  /* unsigned decimal */
            STAR_ARGS(data);
            if (data->a_longlong == FOUND)
              nu = va_arg(args, unsigned LONG_LONG);
            else if (data->a_long == FOUND)
              nu = va_arg(args, unsigned long);
            else
              nu = va_arg(args, unsigned int);
            decimal_u(data, nu);
            state = 0;
            break;
          case 'd':  /* decimal */
            STAR_ARGS(data);
            if (data->a_longlong == FOUND)
              ni = va_arg(args, LONG_LONG);
            else if (data->a_long == FOUND)
              ni = va_arg(args, long);
            else
              ni = va_arg(args, int);
            decimal_i(data, ni);
            state = 0;
            break;
          case 'o':  /* octal */
            STAR_ARGS(data);
            if (data->a_longlong == FOUND)
              nu = va_arg(args, LONG_LONG);
            else if (data->a_long == FOUND)
              nu = va_arg(args, long);
            else
              nu = va_arg(args, int);
            octal(data, nu);
            state = 0;
            break;
          case 'x':
          case 'X':  /* hexadecimal */
            STAR_ARGS(data);
            if (data->a_longlong == FOUND)
              nu = va_arg(args, LONG_LONG);
            else if (data->a_long == FOUND)
              nu = va_arg(args, long);
            else
              nu = va_arg(args, int);
            data->fmtchr = data->pf[0];
            hexa(data, nu);
            state = 0;
            break;
          case 'c': /* character */
            ch = va_arg(args, int);
            char1(data, ch);
            state = 0;
            break;
          case 's':  /* string */
            STAR_ARGS(data);
            strings(data, va_arg(args, char *));
            state = 0;
            break;
          case 'n':
            /* what's the count ? */
            *(va_arg(args, int *)) = data->counter; 
            state = 0;
            break;
          case 'p':
            STAR_ARGS(data);
            if (data->pf[1] == 'b') 
            {
              unsigned LONG_LONG u = va_arg(args, unsigned LONG_LONG);
              const char *n = va_arg(args, const char *);
              bitsstr(data, u, n);
            }
            else
              pointer(data, va_arg(args, const void *));
            state = 0;
            break;
          case 'q':
            data->a_longlong = FOUND;
            break;
          case 'L':
          case 'l':
            if (data->a_long == FOUND)
              data->a_longlong = FOUND;
            else
              data->a_long = FOUND;
            break;
          case 'h':
            break;
          case '%':  /* nothing just % */
            char1(data, '%');
            state = 0;
            break;
          case '#': case ' ': case '+': case '*':
          case '-': case '.': case '0': case '1':
          case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
           /* initialize width and precision */
            for (i = 0; isflag(*data->pf); i++, data->pf++)
              if (i < MAX_FIELD - 1)
            conv_field[i] = *data->pf;
            conv_field[i] = '\0';
            conv_flag(conv_field, data);
            data->pf--;   /* went to far go back */
            break;
          default:
            /* is this an error ? maybe bail out */
            state = 0;
            break;
        } /* end switch */
      } /* end of for state */
    } 
    else 
    { /* not % */
      char1(data, *data->pf); /* add the char the string */
    }
  }

  if (data->holder && !data->out)
    *data->holder = '\0'; /* the end ye ! */

  return data->counter;
}

PRIVATE int __ng_fine_buf(struct DATA *d)
{
  return d->counter < d->length;
}

PRIVATE int __ng_fine_std(struct DATA *d)
{
  return !d->err;
}

PRIVATE PUBLIC int
__ng_std_out(void *arg, const char *string, size_t length)
{
  struct DATA *data = (struct DATA *)arg;

  if (data->nbytes + length > data->length)
  {
    int n = fwrite(data->holder, 1, data->nbytes, (FILE*)data->arg);
    if (n < 0)
    {
      data->err = n;
      return -1;
    }
      
    data->counter += n;
    data->nbytes = 0;
    fflush((FILE*)data->arg);
  }
  else
  {
    ng_memcpy(data->holder+data->nbytes, string, length);
    data->nbytes += length;
  }

  return 0;
}

PUBLIC int
__ng_vfprintf(void *fp, char const *format, va_list args)
{
  int counter;
  struct DATA data;
  char buf[2048];

  data.length  = sizeof(buf); 
  data.holder  = buf;
  data.nbytes  = 0;
  data.err     = 0;
  data.pf      = format;
  data.counter = 0;
  data.out     = __ng_std_out;
  data.arg     = fp;
  data.fine    = __ng_fine_std;

  counter = __ng_vsnprintf_internal(&data, args);
  if (data.nbytes)
  {
    int n = fwrite(data.holder, 1, data.nbytes, (FILE*)fp);
    if (n < 0)
    {
      data.err = n;
      return -1;
    }
    counter += n;
    fflush((FILE*)fp);
  }
  return counter;
}

PUBLIC int
__ng_fprintf(void *fp, char const * format, ...)
{
  int rval;
  va_list args;

  va_start(args, format);
  rval = __ng_vfprintf(fp, format, args);
  va_end(args);

  return rval;
}

PUBLIC int
__ng_vsnprintf(char *string, size_t length, char const *format, 
  va_list args)
{
  struct DATA data;
  data.length  = length - 1; /* leave room for '\0' */
  data.holder  = string;
  data.pf      = format;
  data.counter = 0;
  data.err     = 0;
  data.out     = NULL;
  data.fine    = __ng_fine_buf;
  return __ng_vsnprintf_internal(&data, args);
}

PUBLIC int
__ng_snprintf(char *string, size_t length, char const * format, ...)
{
  int rval;
  va_list args;

  va_start(args, format);
  rval = __ng_vsnprintf(string, length, format, args);
  va_end(args);

  return rval;
}

PRIVATE int
__ng_cb_out(void *arg, const char *string, size_t length)
{
  struct DATA *data = (struct DATA *)arg;

  if (data->nbytes + length > data->length)
  {
    int n = data->_out(data->arg, data->holder, data->nbytes);
    if (n < 0)
    {
      data->err = n;
      return -1;
    }
      
    data->counter += n;
    data->nbytes = 0;
  }
  else
  {
    ng_memcpy(data->holder+data->nbytes, string, length);
    data->nbytes += length;
  }
  
  return 0;
}

PRIVATE int __ng_fine_cb(struct DATA *d)
{
  return !d->err;
}

PUBLIC int
__ng_vsnprintf_cb(ng_vsout_f out, void *arg, char const *format, va_list args)
{
  char buf[2048];
  struct DATA data;
  int counter;
  
  data.length  = sizeof(buf); /* leave room for '\0' */
  data.holder  = buf;
  data.nbytes  = 0;
  data.err     = 0;
  data.pf      = format;
  data.counter = 0;
  data._out    = out;
  data.out     = __ng_cb_out;
  data.arg     = arg;
  data.fine    = __ng_fine_cb;
  counter = __ng_vsnprintf_internal(&data, args);
  if (!data.err && data.nbytes)
  {
    int n = out(arg, data.holder, data.nbytes);
    if (n < 0)
    {
      data.err = n;
      return -1;
    }
    counter += n;
  }
  return data.err ? -1 : counter;
}

PUBLIC int
__ng_snprintf_cb(ng_vsout_f out, void *arg, char const * format, ...)
{
  int rval;
  va_list args;

  va_start(args, format);
  rval = __ng_vsnprintf_cb(out, arg, format, args);
  va_end(args);

  return rval;
}

#if __NHTTP_VSNPRINTF_DEBUG

#include <stdio.h>

/* set of small tests for ng_snprintf() */
int snprintf_test(void)
{
  char holder[100];
  int i;

  printf("Suite of test for __ng_snprintf:\n");
  printf("a_format\n");
  printf("printf() format\n");
  printf("__ng_snprintf() format\n\n");

/* Checking the field widths */

  printf("/%%d/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%d/\n", 336);
  printf("/%d/\n", 336);
  printf("%s\n", holder);

  printf("/%%2d/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%2d/\n", 336);
  printf("/%2d/\n", 336);
  printf("%s\n", holder);

  printf("/%%10d/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%10d/\n", 336);
  printf("/%10d/\n", 336);
  printf("%s\n", holder);

  printf("/%%-10d/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%-10d/\n", 336);
  printf("/%-10d/\n", 336);
  printf("%s\n", holder);

/* long long  */

  printf("/%%lld/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%lld/\n", (LONG_LONG)336);
  printf("/%lld/\n", (LONG_LONG)336);
  printf("%s\n", holder);

  printf("/%%2qd/, 336\n");
  __ng_snprintf(holder, sizeof holder, "/%2qd/\n", (LONG_LONG)336);
  printf("/%2qd/\n", (LONG_LONG)336);
  printf("%s\n", holder);

/* floating points */

  printf("/%%f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%f/\n", 1234.56);
  printf("/%f/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%e/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%e/\n", 1234.56);
  printf("/%e/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%4.2f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%4.2f/\n", 1234.56);
  printf("/%4.2f/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%3.1f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%3.1f/\n", 1234.56);
  printf("/%3.1f/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%10.3f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%10.3f/\n", 1234.56);
  printf("/%10.3f/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%10.3e/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%10.3e/\n", 1234.56);
  printf("/%10.3e/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%+4.2f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%+4.2f/\n", 1234.56);
  printf("/%+4.2f/\n", 1234.56);
  printf("%s\n", holder);

  printf("/%%010.2f/, 1234.56\n");
  __ng_snprintf(holder, sizeof holder, "/%010.2f/\n", 1234.56);
  printf("/%010.2f/\n", 1234.56);
  printf("%s\n", holder);

#define BLURB "Outstanding acting !"
/* strings precisions */

  printf("/%%2s/, \"%s\"\n", BLURB);
  __ng_snprintf(holder, sizeof holder, "/%2s/\n", BLURB);
  printf("/%2s/\n", BLURB);
  printf("%s\n", holder);

  printf("/%%22s/ %s\n", BLURB);
  __ng_snprintf(holder, sizeof holder, "/%22s/\n", BLURB);
  printf("/%22s/\n", BLURB);
  printf("%s\n", holder);

  printf("/%%22.5s/ %s\n", BLURB);
  __ng_snprintf(holder, sizeof holder, "/%22.5s/\n", BLURB);
  printf("/%22.5s/\n", BLURB);
  printf("%s\n", holder);

  printf("/%%-22.5s/ %s\n", BLURB);
  __ng_snprintf(holder, sizeof holder, "/%-22.5s/\n", BLURB);
  printf("/%-22.5s/\n", BLURB);
  printf("%s\n", holder);

/* see some flags */

  printf("%%x %%X %%#x, 31, 31, 31\n");
  __ng_snprintf(holder, sizeof holder, "%x %X %#x\n", 31, 31, 31);
  printf("%x %X %#x\n", 31, 31, 31);
  printf("%s\n", holder);

  printf("**%%d**%% d**%% d**, 42, 42, -42\n");
  __ng_snprintf(holder, sizeof holder, "**%d**% d**% d**\n", 42, 42, -42);
  printf("**%d**% d**% d**\n", 42, 42, -42);
  printf("%s\n", holder);

/* other flags */

  printf("/%%g/, 31.4\n");
  __ng_snprintf(holder, sizeof holder, "/%g/\n", 31.4);
  printf("/%g/\n", 31.4);
  printf("%s\n", holder);

  printf("/%%.6g/, 31.4\n");
  __ng_snprintf(holder, sizeof holder, "/%.6g/\n", 31.4);
  printf("/%.6g/\n", 31.4);
  printf("%s\n", holder);

  printf("/%%.1G/, 31.4\n");
  __ng_snprintf(holder, sizeof holder, "/%.1G/\n", 31.4);
  printf("/%.1G/\n", 31.4);
  printf("%s\n", holder);

  printf("abc%%n\n");
  printf("abc%n", &i); printf("%d\n", i);
  __ng_snprintf(holder, sizeof holder, "abc%n", &i);
  printf("%s", holder); printf("%d\n\n", i);

  printf("%%*.*s --> 10.10\n");
  __ng_snprintf(holder, sizeof holder, "%*.*s\n", 10, 10, BLURB);
  printf("%*.*s\n", 10, 10, BLURB);
  printf("%s\n", holder);

  printf("%%%%%%%%\n");
  __ng_snprintf(holder, sizeof holder, "%%%%\n");
  printf("%%%%\n");
  printf("%s\n", holder);

#define BIG "Hello this is a too big string for the buffer"
/*  printf("A buffer to small of 10, trying to put this:\n");*/
  printf("<%%>, %s\n", BIG);
  i = __ng_snprintf(holder, 10, "%s\n", BIG);
  printf("<%s>\n", BIG);
  printf("<%s>\n", holder);

  return 0;
}
#endif /* !DRIVER */
