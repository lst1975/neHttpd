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
		   see header of ng_snprintf.c.

format:
  int ng_snprintf(holder, sizeof_holder, format, ...)

Return values:
  (sizeof_holder - 1)


 THANKS(for the patches and ideas):
     Miles Bader
     Cyrille Rustom
     Jacek Slabocewiz
     Mike Parker(mouse)

Alain Magloire: alainm@rcsm.ee.mcgill.ca
*/

#ifndef __ngRTOS_SNPRINTF_H__
#define __ngRTOS_SNPRINTF_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
struct in_addr {
  uint32_t s_addr;
};
struct __ng_cache_align_as in6_addr {
  union {
    uint32_t u32_addr[4];
    uint16_t u16_addr[8];
    uint8_t  u8_addr[16];
  } un;
#define s6_addr    un.u8_addr
#define s6_addr16	 un.u16_addr
#define s6_addr32  un.u32_addr
};

struct sockaddr {
  uint8_t     sa_len;
  uint8_t     sa_family;
  char        sa_data[14];
};

/* members are in network byte order */
struct sockaddr_in {
  uint8_t        sin_len;
  uint8_t        sin_family;
  uint16_t       sin_port;
  struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
  char           sin_zero[SIN_ZERO_LEN];
};

struct sockaddr_in6 {
  uint8_t         sin6_len;      /* length of this structure    */
  uint8_t         sin6_family;   /* AF_INET6                    */
  uint16_t        sin6_port;     /* Transport layer port #      */
  uint32_t        sin6_flowinfo; /* IPv6 flow information       */
  struct in6_addr sin6_addr;     /* IPv6 address                */
  uint32_t        sin6_scope_id; /* Set of interfaces for scope */
};
#endif

typedef int (*ng_vsout_f)(void *arg, const char *string, size_t length);

int __ng_snprintf(char *string, size_t length, char const * format, ...);
int __ng_vsnprintf(char *string, size_t length, char const * format, va_list args);
int __ng_fprintf(void *fp, char const * format, ...);
int __ng_vfprintf(void *fp, char const * format, va_list args);

int __ng_snprintf_cb(ng_vsout_f out, void *arg, char const * format, ...);
int __ng_vsnprintf_cb(ng_vsout_f out, void *arg, char const * format, va_list args);

/* set of small tests for ng_snprintf() */
#if __NHTTP_VSNPRINTF_DEBUG
int snprintf_test(void);
#endif


#ifdef __cplusplus
}
#endif

#endif /* __ngRTOS_SNPRINTF_H__ */
