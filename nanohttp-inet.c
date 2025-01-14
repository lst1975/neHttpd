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
/*  $OpenBSD: inet_pton.c,v 1.10 2015/09/13 21:36:08 guenther Exp $  */

/* Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include "nanohttp-config.h"
#include "nanohttp-defs.h"
#include "nanohttp-error.h"
#include "nanohttp-ctype.h"
#include "nanohttp-socket.h"
#include "nanohttp-inet.h"
#include "nanohttp-atoi.h"

#define NG_INT64SZ    8      /* for systems without 64-bit ints */
#define NG_INT32SZ    4      /* for systems without 32-bit ints */
#define NG_INT16SZ    2      /* for systems without 16-bit ints */

/* int
 * inet_pton(af, src, dst)
 *  convert from presentation format (which usually means ASCII printable)
 *  to network format (which is usually some kind of binary format).
 * return:
 *  1 if the address was valid for the specified address family
 *  0 if the address wasn't valid (`dst' is untouched in this case)
 *  -1 if some other error occurred (`dst' is untouched in this case, too)
 * author:
 *  Paul Vixie, 1996.
 */
int
ng_inet_pton(int af, const char *src, int len, void *dst)
{
  switch (af) {
  case AF_INET:
    return (ng_inet_pton4(src, len, dst) ? ng_ERR_NONE : ng_ERR_EINVAL);
  case AF_INET6:
    return (ng_inet_pton6(src, len, dst) ? ng_ERR_NONE : ng_ERR_EINVAL);
  default:
    return (ng_ERR_EOPNOTSUPP);
  }
  /* NOTREACHED */
}

/* int
 * inet_pton4(src, dst)
 *  like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *  1 if `src' is a valid dotted quad, else 0.
 * notice:
 *  does not touch `dst' unless it's returning 1.
 * author:
 *  Paul Vixie, 1996.
 */
int
ng_inet_pton4(const char *src, int len, ng_uint8_t *dst)
{
  int saw_digit, octets, ch;
  const char *end = len ? src + len : NULL;
  ng_uint8_t tmp[NG_INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((end == NULL || src < end) && (ch = *src++) != '\0') {
    if (__ng_isdigit(ch)){
      ng_uint32_t _new = *tp * 10 + ch - '0';

      if (_new > 255)
        return ng_FALSE;
      if (! saw_digit) {
        if (++octets > 4)
          return ng_FALSE;
        saw_digit = 1;
      }
      *tp = _new;
    } else if (ch == '.' && saw_digit) {
      if (octets == 4)
        return ng_FALSE;
      *++tp = 0;
      saw_digit = 0;
    } else
      return ng_FALSE;
  }
  if (octets < 4)
    return ng_FALSE;

  ng_memcpy(dst, tmp, NG_INADDRSZ);
  return ng_TRUE;
}

/* int
 * inet_pton6(src, dst)
 *  convert presentation level address to network order binary form.
 * return:
 *  1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *  does not touch `dst' unless it's returning 1.
 * credit:
 *  inspired by Mark Andrews.
 * author:
 *  Paul Vixie, 1996.
 */
int
ng_inet_pton6(const char *src, int len, ng_uint8_t *dst)
{
  const char *end = len ? src + len : NULL;
  const unsigned char *xdigits = __ng_hex_digit;
  
  ng_uint8_t tmp[NG_IN6ADDRSZ], *tp, *endp, *colonp;
  const char *curtok;
  ng_uint8_t ch;
  int saw_xdigit, count_xdigit;
  ng_uint32_t val;

  ng_memset((tp = tmp), '\0', NG_IN6ADDRSZ);
  endp = tp + NG_IN6ADDRSZ;
  colonp = NULL;
  /* Leading :: requires some special handling. */
  if (*src == ':')
    if (*++src != ':')
      return ng_FALSE;
  curtok = src;
  saw_xdigit = count_xdigit = 0;
  val = 0;
  while ((end == NULL || src < end) && (ch = *src++) != '\0') {
    int x = xdigits[ch];
    if (x != 0xFF) {
      if (count_xdigit >= 4)
        return ng_FALSE;
      val <<= 4;
      val |= x;
      if (val > 0xffff)
        return ng_FALSE;
      saw_xdigit = 1;
      count_xdigit++;
      continue;
    }
    if (ch == ':') {
      curtok = src;
      if (!saw_xdigit) {
        if (colonp)
          return ng_FALSE;
        colonp = tp;
        continue;
      } else if (*src == '\0') {
        return ng_FALSE;
      }
      if (tp + NG_INT16SZ > endp)
        return ng_FALSE;
      *tp++ = (ng_uint8_t) (val >> 8) & 0xff;
      *tp++ = (ng_uint8_t) val & 0xff;
      saw_xdigit = 0;
      count_xdigit = 0;
      val = 0;
      continue;
    }
    if (ch == '.' && ((tp + NG_INADDRSZ) <= endp) &&
        ng_inet_pton4(curtok, len ? end - curtok : 0, tp) > 0) {
      tp += NG_INADDRSZ;
      saw_xdigit = 0;
      count_xdigit = 0;
      break;  /* '\0' was seen by inet_pton4(). */
    }
    return ng_FALSE;
  }
  if (saw_xdigit) {
    if (tp + NG_INT16SZ > endp)
      return (0);
    *tp++ = (ng_uint8_t) (val >> 8) & 0xff;
    *tp++ = (ng_uint8_t) val & 0xff;
  }
  if (colonp != NULL) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we'll do the shift by hand.
     */
    const int n = tp - colonp;
    int i;

    if (tp == endp)
      return ng_FALSE;
    for (i = 1; i <= n; i++) {
      endp[- i] = colonp[n - i];
      colonp[n - i] = 0;
    }
    tp = endp;
  }
  if (tp != endp)
    return (0);
  ng_memcpy(dst, tmp, NG_IN6ADDRSZ);
  return ng_TRUE;
}

/* char *
 * inet_ntop(af, src, dst, size)
 *	convert a network format address to presentation format.
 * return:
 *	pointer to presentation format address (`dst'), or NULL (see errno).
 * author:
 *	Paul Vixie, 1996.
 */
int
ng_inet_ntop(int af, const char *src, char *dst, ng_size_t size)
{
	switch (af) {
  case AF_INET:
		return (ng_inet_ntop4(src, dst, (ng_size_t)size));
  case AF_INET6:
		return (ng_inet_ntop6(src, dst, (ng_size_t)size));
	default:
		return 0;
	}
	/* NOTREACHED */
}

int
ng_inet_ntop_su(void *__sa, char *dst, ng_size_t size)
{
  switch (((struct sockaddr *)__sa)->sa_family) {
  case AF_INET:
    return ng_inet_ntop4((char *)&((struct sockaddr_in *)(__sa))->sin_addr, dst, size);
  case AF_INET6:
    return ng_inet_ntop6((char *)&((struct sockaddr_in6 *)(__sa))->sin6_addr, dst, size);
  default:
    return 0;
	}
}

/* const char *
 * inet_ntop4(src, dst, size)
 *	format an IPv4 address, more or less like inet_ntoa()
 * return:
 *	`dst' (as a const)
 * notes:
 *	(1) uses no statics
 *	(2) takes a u_char* not an in_addr as input
 * author:
 *	Paul Vixie, 1996.
 */
int
ng_inet_ntop4(const char *src, char *dst, ng_size_t size)
{
	char tmp[sizeof "255.255.255.255"];
	ng_size_t l;
  char *p = tmp;
  
  for (int i = 0; i < 4; i++) {
    /* hold each IP quad in normal order */
    const ng_block_s *b = &__ng_uint8_string[(ng_uint8_t)src[i]];
    for (int x=0;x<b->len;x++)
      *p++ = b->cptr[x];
    if (i < 3)
      *p++ = '.';
  }

  l = RTE_MIN(p-tmp, size-1);
	ng_memcpy(dst, tmp, l);
  tmp[l] = '\0';
	return l;
}

/* const char *
 * inet_ntop6(src, dst, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */
int
ng_inet_ntop6(const char *src, char *dst, ng_size_t size)
{
	/*
	 * Note that ng_int32_t and ng_int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 * char	buf[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255:255:255:255/128")];
	 */
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
	char *tp, *ep;
	struct { int base, len; } best, cur;
	ng_uint32_t words[NG_IN6ADDRSZ / NG_INT16SZ];
	int i;
	int advance;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	ng_memset(words, '\0', sizeof words);
	for (i = 0; i < NG_IN6ADDRSZ; i++)
		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (NG_IN6ADDRSZ / NG_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	ep = tmp + sizeof(tmp);
	for (i = 0; i < (NG_IN6ADDRSZ / NG_INT16SZ) && tp < ep; i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base) {
				if (tp + 1 >= ep)
          return 0;
				*tp++ = ':';
			}
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0) {
			if (tp + 1 >= ep)
				return 0;
			*tp++ = ':';
		}
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
		  int n = ng_inet_ntop4(src+12, tp, (ng_size_t)(ep - tp));
			if (!n)
				return 0;
			tp += n;
			break;
		}
		advance = ng_u32toh(words[i], tp, ep - tp, 0, 0);
		if (advance <= 0 || advance >= ep - tp)
			return 0;
		tp += advance;
	}
  
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == (NG_IN6ADDRSZ / NG_INT16SZ)) {
		if (tp + 1 >= ep)
      return 0;
		*tp++ = ':';
	}
	if (tp + 1 >= ep)
    return 0;

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((ng_size_t)(tp - tmp) > size) {
    return 0;
	}
  
	ng_memcpy(dst, tmp, tp - tmp);
	return tp - tmp;
}

