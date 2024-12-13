/** @file nanohttp-socket.c Socket wrapper */
/******************************************************************
*  $Id: nanohttp-socket.c,v 1.71 2007/11/03 22:40:14 m0gg Exp $
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#include "nanohttp-config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef WIN32
#include "wsockcompat.h"
#include <winsock2.h>
#include <process.h>

#ifndef __MINGW32__
typedef int ssize_t;
#endif

#undef errno
#define errno WSAGetLastError()

#endif

#include "nanohttp-error.h"
#include "nanohttp-logging.h"

#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#endif
#include "nanohttp-ssl.h"
#endif

#define	HSOCKET_FREE	-1

static int _hsocket_timeout = 10;

#ifdef WIN32
static herror_t
_hsocket_sys_accept(struct hsocket_t * sock, struct hsocket_t * dest)
{
  struct hsocket_t sockfd;
  int asize;

  asize = sizeof(struct sockaddr_in);
  while (1)
  {
    sockfd.sock = accept(sock->sock, (struct sockaddr *) &(dest->addr), &asize);
    if (sockfd.sock == INVALID_SOCKET)
    {
      if (WSAGetLastError() != WSAEWOULDBLOCK)
        return herror_new("hsocket_accept", HSOCKET_ERROR_ACCEPT, "Socket error (%s)", strerror(errno));
    }
    else
    {
      break;
    }
  }

  dest->sock = sockfd.sock;

  return H_OK;
}

static void
_hsocket_sys_close(struct hsocket_t * sock)
{
  char junk[10];

  /* shutdown(sock,SD_RECEIVE); */

  shutdown(sock->sock, SD_SEND);
  while (recv(sock->sock, junk, sizeof(junk), 0) > 0);
    /* nothing */
  closesocket(sock->sock);

  return;
}

static void
_hsocket_module_sys_init(int argc, char **argv)
{
  struct WSAData info;
  WSAStartup(MAKEWORD(2, 2), &info);

  return;
}

static void
_hsocket_module_sys_destroy(void)
{
  WSACleanup();

  return;
}
#else
static inline void
_hsocket_module_sys_init(int argc, char **argv)
{
  return;
}

static inline void
_hsocket_module_sys_destroy(void)
{
  return;
}

static herror_t
_hsocket_sys_accept(struct hsocket_t * sock, struct hsocket_t * dest)
{
  socklen_t len;

  len = sizeof(struct sockaddr_in);

  if ((dest->sock = accept(sock->sock, (struct sockaddr *) &(dest->addr), &len)) == -1)
  {
    log_warn("accept failed (%s)", strerror(errno));
    return herror_new("hsocket_accept", HSOCKET_ERROR_ACCEPT, "Cannot accept network connection (%s)", strerror(errno));
  }

  return H_OK;
}

static inline void
_hsocket_sys_close(struct hsocket_t * sock)
{
  shutdown(sock->sock, SHUT_RDWR);

  close(sock->sock);

  return;
}
#endif

herror_t
hsocket_module_init(int argc, char **argv)
{
#ifdef HAVE_SSL
  herror_t status;
#endif

  _hsocket_module_sys_init(argc, argv);

#ifdef HAVE_SSL
  if ((status = hssl_module_init(argc, argv)) != H_OK)
  {
    log_error("hssl_module_init failed (%s)", herror_message(status));
    return status;
  }
#endif

  return H_OK;
}

void
hsocket_module_destroy(void)
{
  _hsocket_module_sys_destroy();

  return;
}

herror_t
hsocket_init(struct hsocket_t * sock)
{
  memset(sock, 0, sizeof(struct hsocket_t));
  sock->sock = HSOCKET_FREE;

  return H_OK;
}

void
hsocket_free(struct hsocket_t * sock)
{
  /* nop */

  return;
}

herror_t
hsocket_open(struct hsocket_t * dsock, const char *hostname, int port, int ssl)
{
  int s;
  struct sockaddr_in address;

  if ((dsock->sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    return herror_new("hsocket_open", HSOCKET_ERROR_CREATE,
                      "Socket error (%s)", strerror(errno));

  // Set FD_CLOEXEC on the file descriptor
  int flags = fcntl(dsock->sock, F_GETFD);
  if (flags == -1 || fcntl(dsock->sock, F_SETFD, flags | FD_CLOEXEC) == -1) {
    return herror_new("hsocket_open", HSOCKET_ERROR_CREATE,
                      "Socket error (%s)", strerror(errno));
  }

  /* Get host data */
  s = inet_pton(AF_INET, hostname, &address.sin_addr);
  if (s <= 0) {
    return herror_new("hsocket_open", HSOCKET_ERROR_GET_HOSTNAME,
                      "Socket error (%s)", strerror(errno));
  }

  /* set server addresss */
  address.sin_family = AF_INET;
  address.sin_port = htons((unsigned short) port);

  log_verbose("Opening %s://%s:%i", ssl ? "https" : "http", hostname, port);

  /* connect to the server */
  if (connect(dsock->sock, (struct sockaddr *) &address, sizeof(address)) != 0)
    return herror_new("hsocket_open", HSOCKET_ERROR_CONNECT, "Socket error (%s)", strerror(errno));

  if (ssl)
  {
#ifdef HAVE_SSL
    herror_t status;

    if ((status = hssl_client_ssl(dsock)) != H_OK)
    {
      log_error("hssl_client_ssl failed (%s)", herror_message(status));
      return status;
    }
#else
    return herror_new("hssl_client_ssl", 0, "SSL wasn't enabled at compile time");
#endif
  }
  return H_OK;
}

herror_t
hsocket_bind(struct hsocket_t *dsock, unsigned short port)
{
  struct hsocket_t sock;
  struct sockaddr_in addr;
  int opt = 1;

  /* create socket */
  if ((sock.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    log_error("Cannot create socket (%s)", strerror(errno));
    return herror_new("hsocket_bind", HSOCKET_ERROR_CREATE,
                      "Socket error (%s)", strerror(errno));
  }

  // Set FD_CLOEXEC on the file descriptor
  int flags = fcntl(sock.sock, F_GETFD);
  if (flags == -1 || fcntl(sock.sock, F_SETFD, flags | FD_CLOEXEC) == -1) {
    return herror_new("hsocket_open", HSOCKET_ERROR_CREATE,
                      "Socket error (%s)", strerror(errno));
  }
  
  setsockopt(sock.sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  /* bind socket */
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(addr.sin_zero), '\0', 8);    /* zero the rest of the struct */

  if (bind(sock.sock, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1)
  {
    log_error("Cannot bind socket (%s)", strerror(errno));
    return herror_new("hsocket_bind", HSOCKET_ERROR_BIND, "Socket error (%s)",
                      strerror(errno));
  }
  dsock->sock = sock.sock;
  return H_OK;
}

herror_t
hsocket_accept(struct hsocket_t * sock, struct hsocket_t * dest)
{
  herror_t status;

  if (sock->sock < 0)
    return herror_new("hsocket_accept", HSOCKET_ERROR_NOT_INITIALIZED,
                      "hsocket_t not initialized");

  if ((status = _hsocket_sys_accept(sock, dest)) != H_OK)
    return status;

#ifdef HAVE_SSL
  if ((status = hssl_server_ssl(dest)) != H_OK)
  {
    log_warn("SSL startup failed (%s)", herror_message(status));
    return status;
  }
#endif

  log_verbose("accepting connection from '%s' socket=%d",
               SAVE_STR(((char *) inet_ntoa(dest->addr.sin_addr))),
               dest->sock);

  return H_OK;
}

herror_t
hsocket_listen(struct hsocket_t * sock)
{
  if (sock->sock < 0)
    return herror_new("hsocket_listen", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen before initializing!");

  if (listen(sock->sock, 15) == -1)
  {
    log_error("listen failed (%s)", strerror(errno));
    return herror_new("hsocket_listen", HSOCKET_ERROR_LISTEN,
                      "Cannot listen on this socket (%s)", strerror(errno));
  }

  return H_OK;
}

void
hsocket_close(struct hsocket_t * sock)
{
  log_verbose("closing socket %p (%d)...", sock, sock->sock);

#ifdef HAVE_SSL
  hssl_cleanup(sock);
#endif

  _hsocket_sys_close(sock);

  sock->bytes_received = 0;
  sock->bytes_transmitted = 0;

  log_verbose("socket closed");

  return;
}

herror_t
hsocket_send(struct hsocket_t * sock, const unsigned char * bytes, size_t n)
{
#ifdef HAVE_SSL
  herror_t status;
#endif
  size_t total = 0;
  size_t size;

  if (sock->sock < 0)
    return herror_new("hsocket_send", HSOCKET_ERROR_NOT_INITIALIZED,
                      "hsocket not initialized");

  /* log_verbose( "SENDING %s", bytes ); */

  while (1)
  {
#ifdef HAVE_SSL
    if ((status = hssl_write(sock, bytes + total, n, &size)) != H_OK)
    {
      log_warn("hssl_write failed (%s)", herror_message(status));
      return status;
    }
#else
    if ((size = send(sock->sock, bytes + total, n, 0)) == -1)
      return herror_new("hsocket_send", HSOCKET_ERROR_SEND, "send failed (%s)", strerror(errno));
#endif
    sock->bytes_received += size;

    n -= size;
    total += size;
    if (n <= 0)
      break;
  }

  return H_OK;
}

herror_t
hsocket_send_string(struct hsocket_t * sock, const char *str)
{
  return hsocket_send(sock, (unsigned char *)str, strlen(str));
}

int
hsocket_select_recv(int sock, char *buf, size_t len)
{
  struct timeval timeout;
  fd_set fds;

  FD_ZERO(&fds);
  FD_SET(sock, &fds);

  timeout.tv_sec = _hsocket_timeout;
  timeout.tv_usec = 0;

  if (select(sock + 1, &fds, NULL, NULL, &timeout) == 0)
  {
    errno = ETIMEDOUT;
    log_verbose("Socket %d timed out", sock);
    return -1;
  }

  return recv(sock, buf, len, 0);
}

herror_t
hsocket_recv(struct hsocket_t * sock, unsigned char * buffer, size_t total, int force, size_t *received)
{
#ifdef HAVE_SSL
  herror_t status;
#endif
  size_t totalRead;
  size_t count;

/* log_verbose("Entering hsocket_recv(total=%d,force=%d)", total, force); */

  totalRead = 0;
  do
  {

#ifdef HAVE_SSL
    if ((status = hssl_read(sock, (char *)buffer + totalRead, (size_t) total - totalRead, &count)) != H_OK)
    {
      log_warn("hssl_read failed (%s)", herror_message(status));
      return status;
    }
#else
    if ((count = hsocket_select_recv(sock->sock, (char *)buffer + totalRead, (size_t) total - totalRead)) == -1)
      return herror_new("hsocket_recv", HSOCKET_ERROR_RECEIVE, "recv failed (%s)", strerror(errno));
#endif
    sock->bytes_received += count;

    if (!force)
    {
      /* log_verbose("Leaving !force (received=%d)(status=%d)", *received,
         status); */
      *received = count;
      return H_OK;
    }

    totalRead += count;

    if (totalRead == total)
    {
      *received = totalRead;
      /* 
         log_verbose("Leaving totalRead == total
         (received=%d)(status=%d)(totalRead=%d)", *received, status,
         totalRead); */
      return H_OK;
    }
  }
  while (1);
}

int
hsocket_get_timeout(void)
{
  return _hsocket_timeout;
}

void
hsocket_set_timeout(int secs)
{
  _hsocket_timeout = secs;

  return;
}
