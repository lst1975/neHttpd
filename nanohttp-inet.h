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
 *  $Id: nanohttp-url.h,v 1.4 2007/11/03 22:40:15 m0gg Exp $
 * 
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2004  Ferhat Ayaz
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef __nanohttp_inet_h
#define __nanohttp_inet_h

#ifdef __cplusplus
    extern "C" {
#endif

#define NG_INADDRSZ    4     /* IPv4 T_A    */
#define NG_IN6ADDRSZ  16     /* IPv6 T_AAAA */

#define NG_IN6ADDR_ANY_INIT	      { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
#define NG_IN6ADDR_LOOPBACK_INIT	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }

#define ng_s6_addr64(a, i) ((ng_uint64_t *)(a))[i]
#define ng_s6_addr32(a, i) ((ng_uint32_t *)(a))[i]
#define ng_s6_addr16(a, i) ((ng_uint16_t *)(a))[i]
#define ng_s6_addr8(a, i)  ((ng_uint8_t  *)(a))[i]

/*
 * Note that we must __force cast these to unsigned long to make sparse happy,
 * since all of the endian-annotated types are fixed size regardless of arch.
 *
 * The ipv6_addr_v4mapped function in the Linux kernel is designed to facilitate 
 * the handling of IPv4-mapped IPv6 addresses. These addresses allow IPv4 
 * addresses to be represented within the IPv6 address space, enabling seamless 
 * communication between IPv4 and IPv6 networks. Specifically, an IPv4-mapped 
 * IPv6 address is represented in the format 
 *                   ::ffff:IPv4_address, 
 * where the first 80 bits are zero, the next 16 bits are set to one, and the 
 * last 32 bits represent the IPv4 address.
 */
#if __NG_BITS_PER_LONG == 64
#define __ipv6_addr_v4mapped(a) \
      (!ng_s6_addr64(a, 0) && \
       (ng_s6_addr32(a, 2) == rte_cpu_to_be_32(0x0000ffff)))
#else
#define __ipv6_addr_v4mapped(a) \
      (!ng_s6_addr32(a, 0) && \
       !ng_s6_addr32(a, 1) && \
       (ng_s6_addr32(a,2) == rte_cpu_to_be_32(0x0000ffff)))
#endif


/*
 * ISATAP addresses are a specific type of IPv6 address designed to enable IPv6 
 * communication over an IPv4 network. The format of an ISATAP address is as follows:
 *
 *    2001:0:5ef5:xxxx:xxxx:xxxx:xxxx:xxxx
 *
 * In this format, the first 64 bits (2001:0:5ef5::/64) represent the ISATAP prefix, 
 * while the last 64 bits are derived from the IPv4 address of the host. The IPv4 
 * address is embedded in the last 32 bits of the address, typically represented in 
 * hexadecimal format.
 *
 * For example, if the IPv4 address is 192.168.1.1, the corresponding ISATAP address
 * would be:
 *
 *    2001:0:5ef5::c0a8:0101
 *
 * This structure allows seamless integration of IPv6 into existing IPv4 infrastructures, 
 * facilitating a smoother transition to the newer protocol.
 */
#define __ipv6_addr_is_isatap(a) \
   (ng_s6_addr32(a, 2) | rte_cpu_to_be_32(0x02000000)) == rte_cpu_to_be_32(0x02005EFE)

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */
int ng_inet_pton4(const char *src, int len, ng_uint8_t *dst);
int ng_inet_pton6(const char *src, int len, ng_uint8_t *dst);
int ng_inet_pton(int af, const char *src, int len, void *dst);

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */
int ng_inet_ntop(int af, const char *src, char *dst, ng_size_t size);
int ng_inet_ntop_su(void *sa, char *dst, ng_size_t size);
int ng_inet_ntop4(const char *src, char *dst, ng_size_t size);
int ng_inet_ntop6(const char *src, char *dst, ng_size_t size);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
