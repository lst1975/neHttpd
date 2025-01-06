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
/******************************************************************
 *  $Id: nanohttp-socket.h,v 1.39 2007/11/03 22:40:15 m0gg Exp $
 *
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003  Ferhat Ayaz
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef __nanohttp_socket_h
#define __nanohttp_socket_h

#include "nanohttp-config.h"

#ifndef WIN32
#if __NHTTP_USE_EPOLL
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#else
#include <Windows.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#endif

/** @file nanohttp-socket.h Socket wrapper
 *
 * @defgroup NANOHTTP_SOCKET Socket wrapper
 * @ingroup NANOHTTP
 *
 */
/**@{*/

/** @defgroup NANOHTTP_SOCKET_ERRORS Generic socket errors
 * @ingroup NANOHTTP_ERRORS
 *
 */
/**@{*/
#define HSOCKET_ERROR			            (1000)
#define HSOCKET_ERROR_CREATE		      (HSOCKET_ERROR + 1)
#define HSOCKET_ERROR_GET_HOSTNAME	  (HSOCKET_ERROR + 2)
#define HSOCKET_ERROR_CONNECT		      (HSOCKET_ERROR + 3)
#define HSOCKET_ERROR_SEND		        (HSOCKET_ERROR + 4)
#define HSOCKET_ERROR_RECEIVE		      (HSOCKET_ERROR + 5)
#define HSOCKET_ERROR_BIND		        (HSOCKET_ERROR + 6)
#define HSOCKET_ERROR_LISTEN		      (HSOCKET_ERROR + 7)
#define HSOCKET_ERROR_ACCEPT		      (HSOCKET_ERROR + 8)
#define HSOCKET_ERROR_NOT_INITIALIZED	(HSOCKET_ERROR + 9)
#define HSOCKET_ERROR_IOCTL		        (HSOCKET_ERROR + 10)
#define HSOCKET_ERROR_INIT		        (HSOCKET_ERROR + 11)
/*@}*/

#define	HSOCKET_FREE	-1

/** Socket definition
 */
struct hsocket_t
{
#ifdef WIN32
  SOCKET sock;
#if __NHTTP_USE_EPOLL 
  SOCKET ep;
#endif
#else
  int sock;
#if __NHTTP_USE_EPOLL 
  int ep;
  struct epoll_event event;
#endif
#endif
  union{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
  };
  size_t bytes_transmitted;
  size_t bytes_received;
  int salen;
  void *ssl;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern int _hsocket_should_again(int err);

/** This function iitializes the socket modul. This should be called
 * only once for an application.
 *
 * @return This function should always return H_OK. 
 *
 * @see hssl_module_destroy()
 */
extern herror_t hsocket_module_init(int argc, char **argv);

/** This function destroys the socket modul. This should be called
 * after finishing an application.
 *
 * @see hssl_module_init()
 */
extern void hsocket_module_destroy(void);

/** This function initializes a given socket object. This function
 * (or hsokcet_init_ssl) should be called for every socket before
 * using it.
 *
 * @param sock the destination socket to initialize.
 *
 * @return This function should always return H_OK. 
 *
 * @see hssl_module_init()
 */
extern herror_t hsocket_init(struct hsocket_t * sock);

/** This function destroys and releases a given socket.
 *
 * @param sock the socket to destroy
 */
extern void hsocket_free(struct hsocket_t * sock);

/** This function connects to a given host. The hostname can be an IP
 * number or a humen readable hostname.
 * 
 * @param sock the destonation socket object to use
 * @param host hostname 
 * @param port port number to connect to
 * @param ssl  whether to open a SSL connection
 *
 * @return H_OK if success. One of the followings if fails:
 *         - HSOCKET_ERROR_CREATE 
 *         - HSOCKET_ERROR_GET_HOSTNAME 
 *         - HSOCKET_ERROR_CONNECT
 *
 */
extern herror_t hsocket_open(struct hsocket_t *sock, const char *host, int port, int ssl);

/** This function closes a socket connection.
 *
 * @param sock the socket to close
 */
extern void hsocket_close(struct hsocket_t *sock);

/** This function binds a socket to a given port number. After bind you
 * can call hsocket_listen() to listen to the port.
 *
 * @param sock socket to use.
 * @param port  port number to bind to
 * 
 * @return H_OK on success. One of the followings if fails:
 *         - HSOCKET_ERROR_CREATE
 *         - HSOCKET_ERROR_BIND
 *
 * @see hsocket_listen()
 */
extern herror_t hsocket_bind(uint8_t fam, struct hsocket_t *sock, unsigned short port);

/** This function sets the socket to the listen mode. You must bind the
 * socket to a port with hsocket_bind() before you can listen to the
 * port.
 *
 * @param sock the socket to use
 *
 * @return H_OK if success. One of the followings if fails:<
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_LISTEN
 */
extern herror_t hsocket_listen(struct hsocket_t *sock, int pend_max);

/** This function accepts an incoming socket request. Note that this
 * function will not return until a socket connection is ready.
 *
 * @param sock the socket which listents to a port
 * @param dest the destination socket which will be created
 * 
 * @return H_OK if success. One of the followings if fails:<P>
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_ACCEPT
 */
extern herror_t hsocket_accept(struct hsocket_t *sock, struct hsocket_t *dest);

/** This function sends data throught the socket.
 *
 * @param sock the socket to use to send the data
 * @param bytes bytes to send
 * @param size size of memory to sent pointed by bytes.
 *
 * @return H_OK if success. One of the followings if fails:
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t hsocket_send(struct hsocket_t *sock, const unsigned char *bytes, size_t size);

/** This function sends a string throught the socket
 *
 * @param sock the socket to use to send the data
 * @param str the null terminated string to sent
 *
 * @return H_OK on success. One of the followings if fails:
 *         - HSOCKET_ERROR_NOT_INITIALIZED
 *         - HSOCKET_ERROR_SEND
 */
extern herror_t hsocket_send_string(struct hsocket_t *sock, const char *str);

extern int hsocket_select_recv(struct hsocket_t *sock, char *buf, size_t len);
#if __NHTTP_USE_EPOLL
herror_t hsocket_epoll_create(struct hsocket_t *dest);
herror_t hsocket_epoll_ctl(int ep, int sock, struct epoll_event *event,
  int op, int flags);
#endif

/** This function reads data from the socket.
 *
 * @param sock the socket to read data from
 * @param buffer the buffer to use to save the readed bytes
 * @param size the maximum size of bytes to read
 * @param force if force is 1 then hsocket_read() will wait until maximum size
 *        of bytes (size parameter) was readed. Otherwise this function will not
 *        wait and will return with the bytes quequed on the socket.
 *
 * @return This function will return -1 if an read error was occured. Otherwise
 *         the return value is the size of bytes readed from the socket.
 */
extern herror_t hsocket_recv(struct hsocket_t * sock, unsigned char *buffer, size_t size, int force, size_t *len);

/** This function gets the socket read/write timeout.
 *
 * @return The socket timeout in seconds.
 *
 * @see hsocket_set_timeout()
 */
extern int hsocket_get_timeout(void);

/** This function sets the socket read/write timeout.
 *
 * @param secs Timeout in seconds.
 *
 * @see hsocket_get_timeout()
 */
extern void hsocket_set_timeout(int secs);


extern herror_t http_header_recv(struct hsocket_t *sock, char *buffer, size_t size, size_t *hdrlen, size_t *rcvbytes);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
