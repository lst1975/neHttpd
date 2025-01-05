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

/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ng_CTYPE_H__
#define __ng_CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */

#define ngctype_U	  0x01	/* upper */
#define ngctype_L	  0x02	/* lower */
#define ngctype_D	  0x04	/* digit */
#define ngctype_C	  0x08	/* cntrl */
#define ngctype_P	  0x10	/* punct */
#define ngctype_S	  0x20	/* white space (space/lf/tab) */
#define ngctype_X   0x40	/* hex digit */
#define ngctype_SP	0x80	/* hard space (0x20) */

extern const unsigned char __ng_ctype[];
extern const unsigned char __ng_lct[];
extern const unsigned char __ng_uct[];
extern const unsigned char __ng_dt[];
extern const unsigned char __ng_hex_digit[];

#define __ng_ismask(x) (__ng_ctype[(int)(unsigned char)(x)])

#define __ng_isalnum(c)	((__ng_ismask(c)&(ngctype_U|ngctype_L|ngctype_D)) != 0)
#define __ng_isalpha(c)	((__ng_ismask(c)&(ngctype_U|ngctype_L)) != 0)
#define __ng_iscntrl(c)	((__ng_ismask(c)&(ngctype_C)) != 0)
#define __ng_isgraph(c)	((__ng_ismask(c)&(ngctype_P|ngctype_U|ngctype_L|ngctype_D)) != 0)
#define __ng_islower(c)	((__ng_ismask(c)&(ngctype_L)) != 0)
#define __ng_isprint(c)	((__ng_ismask(c)&(ngctype_P|ngctype_U|ngctype_L|ngctype_D|ngctype_SP)) != 0)
#define __ng_ispunct(c)	((__ng_ismask(c)&(ngctype_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define __ng_isspace(c)	((__ng_ismask(c)&(ngctype_S)) != 0)
#define __ng_isupper(c)	((__ng_ismask(c)&(ngctype_U)) != 0)
#define __ng_isxdigit(c)	((__ng_ismask(c)&(ngctype_D|ngctype_X)) != 0)

#define __ng_isascii(c) (((unsigned char)(c))<=0x7f)
#define __ng_toascii(c) (((unsigned char)(c))&0x7f)

static inline int __ng_isdigit(int c)
{
	return __ng_dt[c];
}

static inline int __ng_digit2int(int c)
{
	return __ng_dt[c] - 1;
}

static inline int __ng_hex2int(int c)
{
	return __ng_hex_digit[c];
}

static inline unsigned char ng_tolower(unsigned char c)
{
  return __ng_lct[c];
}

static inline unsigned char ng_toupper(unsigned char c)
{
  return __ng_uct[c];
}

/* Fast check for octal digit */
static inline int ng_isodigit(const char c)
{
	return c >= '0' && c <= '7';
}

#ifdef __cplusplus
}
#endif

#endif

