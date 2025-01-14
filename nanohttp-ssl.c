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
/** @file nanohttp-ssl.c SSL wrapper */
/******************************************************************
*  $Id: nanohttp-ssl.c,v 1.38 2007/11/03 22:40:15 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2001-2005  Rochester Institute of Technology
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
* Author: Matt Campbell
******************************************************************/
#include "nanohttp-config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#endif

#ifdef HAVE_OPENSSL_RAND_H
#include <openssl/rand.h>
#endif

#ifdef HAVE_OPENSSL_ERR_H
#include <openssl/err.h>
#endif

#ifdef HAVE_OPENSSL_SSL_H
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-logging.h"

#include "nanohttp-ssl.h"

#define CERT_SUBJECT	1

static char *_hssl_certificate = NULL;
static char *_hssl_certpass = NULL;
static char *_hssl_ca_list = NULL;
static SSL_CTX *_hssl_context = NULL;
static int _hssl_enabled = 0;

static int
_hssl_dummy_verify_cert(X509 * cert)
{
  /* TODO: Make sure that the client is providing a client cert, or that the
     Module is providing the Module cert */

  /* connect to anyone */

  log_verbose("_Not_ validating certificate.");
  return 1;
}

int (*_hssl_verify_cert) (X509 * cert) = _hssl_dummy_verify_cert;

static int
_hssl_cert_verify_callback(int prev_ok, X509_STORE_CTX * ctx)
{
/*
    if ((X509_STORE_CTX_get_error(ctx) = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN))
    {
        log_verbose("Self signed cert in chain");
        return 1;
    }
*/
  log_verbose("Cert depth = %d", X509_STORE_CTX_get_error_depth(ctx));
  if (X509_STORE_CTX_get_error_depth(ctx) == 0)
  {
    return _hssl_verify_cert(X509_STORE_CTX_get_current_cert(ctx));
  }
  else
  {
    log_verbose("Cert ok (prev)");
    return prev_ok;
  }
}

static void
_hssl_superseed(void)
{
  int buf[256], i;

  srand(time(NULL));

  for (i = 0; i < 256; i++)
  {
    buf[i] = rand();
  }
  RAND_seed((unsigned char *) buf, sizeof(buf));

  return;
}

static char *
_hssl_get_error(SSL * ssl, int ret)
{
  switch (SSL_get_error(ssl, ret))
  {
  case SSL_ERROR_NONE:
    return "None";
  case SSL_ERROR_ZERO_RETURN:
    return "Zero return";
  case SSL_ERROR_WANT_READ:
    return "Want read";
  case SSL_ERROR_WANT_WRITE:
    return "Want write";
  case SSL_ERROR_WANT_X509_LOOKUP:
    return "Want x509 lookup";
  case SSL_ERROR_SYSCALL:
    if (ERR_get_error() == 0 && ret == -1)
    {
      return os_strerror(ng_errno);
    }
    return "Syscall failed";
  case SSL_ERROR_SSL:
    return "SSL error";
  default:
    return "Unkown error";
  }
}

static int
_hssl_password_callback(char *buf, int num, int rwflag, void *userdata)
{
  int ret;

  if (!_hssl_certpass)
    return 0;

  ret = strlen(_hssl_certpass);

  if (num < ret + 1)
    return 0;

  strcpy(buf, _hssl_certpass);

  return ret;
}

int
verify_sn(X509 * cert, int who, int nid, char *str)
{
  char name[256];
  char buf[256];

  memset(name, '\0', 256);
  memset(buf, '\0', 256);

  if (who == CERT_SUBJECT)
  {
    X509_NAME_oneline(X509_get_subject_name(cert), name, 256);
  }
  else
  {
    X509_NAME_oneline(X509_get_issuer_name(cert), name, 256);
  }

  buf[0] = '/';
  strcat(buf, OBJ_nid2sn(nid));
  strcat(buf, "=");
  strcat(buf, str);

  return strstr(name, buf) ? 1 : 0;
}

void
hssl_set_hssl_verify_cert(int func(X509 * cert))
{
  _hssl_verify_cert = func;

  return;
}

void
hssl_set_certificate(const char *filename)
{
  if (_hssl_certificate)
    http_free(_hssl_certificate);

  _hssl_certificate = http_strdup(filename);

  return;
}

void
hssl_set_certpass(const char *password)
{
  if (_hssl_certpass)
    http_free(_hssl_certpass);

  _hssl_certpass = http_strdup(password);

  return;
}

void
hssl_set_ca_list(const char *filename)
{
  if (_hssl_ca_list)
    http_free(_hssl_ca_list);

  _hssl_ca_list = http_strdup(filename);

  return;
}

void
hssl_enable(void)
{
  _hssl_enabled = 1;

  return;
}

static void
_hssl_parse_arguments(int argc, char **argv)
{
  int i;

  for (i=1; i<argc; i++)
  {
    if (!strcmp(argv[i - 1], NHTTP_ARG_CERT))
    {
      hssl_set_certificate(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTP_ARG_CERTPASS))
    {
      hssl_set_certpass(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTP_ARG_CA))
    {
      hssl_set_ca_list(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_HTTPS))
    {
      hssl_enable();
    }
  }

  return;
}

static void
_hssl_library_init(void)
{
  static int initialized = 0;

  if (!initialized)
  {
    log_verbose("Initializing library");

    SSL_library_init();

    SSL_load_error_strings();
    ERR_load_crypto_strings();

    OpenSSL_add_ssl_algorithms();

    initialized = 1;
  }

  return;
}

static herror_t
_hssl_server_context_init(void)
{
  log_verbose("enabled=%i, certificate=%p", _hssl_enabled, _hssl_certificate);

  if (!_hssl_enabled || !_hssl_certificate)
    return H_OK;

  if (!(_hssl_context = SSL_CTX_new(SSLv23_method())))
  {
    log_error("Cannot create SSL context");
    return herror_new("_hssl_server_context_init", HSSL_ERROR_CONTEXT,
                      "Unable to create SSL context");
  }

  if (!(SSL_CTX_use_certificate_file(_hssl_context, _hssl_certificate, SSL_FILETYPE_PEM)))
  {
    log_error("Cannot read certificate file: \"%s\"", _hssl_certificate);
    SSL_CTX_free(_hssl_context);
    return herror_new("_hssl_server_context_init", HSSL_ERROR_CERTIFICATE,
                      "Unable to use SSL certificate \"%s\"", _hssl_certificate);
  }

  SSL_CTX_set_default_passwd_cb(_hssl_context, _hssl_password_callback);

  if (!(SSL_CTX_use_PrivateKey_file(_hssl_context, _hssl_certificate, SSL_FILETYPE_PEM)))
  {
    log_error("Cannot read key file: \"%s\"", _hssl_certificate);
    SSL_CTX_free(_hssl_context);
    return herror_new("_hssl_server_context_init", HSSL_ERROR_PEM,
                      "Unable to use private key");
  }

  if (_hssl_ca_list != NULL && *_hssl_ca_list != '\0')
  {
    if (!(SSL_CTX_load_verify_locations(_hssl_context, _hssl_ca_list, NULL)))
    {
      SSL_CTX_free(_hssl_context);
      log_error("Cannot read CA list: \"%s\"", _hssl_ca_list);
      return herror_new("_hssl_server_context_init", HSSL_ERROR_CA_LIST,
                        "Unable to read certification authorities \"%s\"");
    }

    SSL_CTX_set_client_CA_list(_hssl_context, SSL_load_client_CA_file(_hssl_ca_list));
    log_verbose("Certification authority contacted");
  }

  SSL_CTX_set_verify(_hssl_context, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE,
                     _hssl_cert_verify_callback);
  log_verbose("Certificate verification callback registered");

  SSL_CTX_set_mode(_hssl_context, SSL_MODE_AUTO_RETRY);

  SSL_CTX_set_session_cache_mode(_hssl_context, SSL_SESS_CACHE_OFF);

  _hssl_superseed();

  return H_OK;
}

static void
_hssl_server_context_destroy(void)
{
  if (_hssl_context)
  {
    SSL_CTX_free(_hssl_context);
    _hssl_context = NULL;
  }
  return;
}

herror_t
hssl_module_init(int argc, char **argv)
{
  _hssl_parse_arguments(argc, argv);

  if (_hssl_enabled)
  {
    _hssl_library_init();
    log_verbose("SSL enabled");
  }
  else
  {
    log_verbose("SSL _not_ enabled");
  }

  return _hssl_server_context_init();
}

void
hssl_module_destroy(void)
{
  _hssl_server_context_destroy();

  if (_hssl_certpass)
  {
    http_free(_hssl_certpass);
    _hssl_certpass = NULL;
  }

  if (_hssl_ca_list)
  {
    http_free(_hssl_ca_list);
    _hssl_ca_list = NULL;
  }

  if (_hssl_certificate)
  {
    http_free(_hssl_certificate);
    _hssl_certificate = NULL;
  }

  return;
}

int
hssl_enabled(void)
{
  return _hssl_enabled;
}

herror_t
hssl_client_ssl(hsocket_s *sock)
{
  SSL_CTX *ctx;
  SSL *ssl;
  int ret;

  log_verbose("Starting SSL client initialization");

  _hssl_library_init();

  if (!(ctx = SSL_CTX_new(SSLv23_method())))
  {
    log_error("SSL_CTX_new failed (ctx == %p)", ctx);
    return herror_new("hssl_client_ssl", HSSL_ERROR_CONTEXT, "Cannot create SSL client context");
  }

  if (!(ssl = SSL_new(ctx)))
  {
    log_error("Cannot create new SSL object");
    return herror_new("hssl_client_ssl", HSSL_ERROR_CLIENT, "SSL_new failed");
  }

  SSL_set_fd(ssl, sock->sock);

  if ((ret = SSL_connect(ssl)) <= 0)
  {
    herror_t err;

    log_error("SSL connect error (%s)", _hssl_get_error(ssl, -1));
    err =
      herror_new("hssl_client_ssl", HSSL_ERROR_CONNECT,
                 "SSL_connect failed (%s)", _hssl_get_error(ssl, ret));
    SSL_free(ssl);
    return err;
  }

  /* SSL_connect should take care of this for us. if
     (SSL_get_peer_certificate(ssl) == NULL) { log_error("No certificate
     provided"); SSL_free(ssl); return herror_new("hssl_client_ssl",
     HSSL_ERROR_CERTIFICATE, "No certificate provided"); }

     if (SSL_get_verify_result(ssl) != X509_V_OK) { log_error("Certificate
     did not verify"); SSL_free(ssl); return herror_new("hssl_client_ssl",
     HSSL_ERROR_CERTIFICATE, "Verfiy certificate failed"); } */

  log_verbose("SSL client initialization completed");

  sock->ssl = ssl;

  return H_OK;
}

static int
_hssl_bio_read(BIO * b, char *out, int outl)
{
  return hsocket_select_recv(b->num, out, outl);;
}

herror_t
hssl_server_ssl(hsocket_s *sock)
{
  SSL *ssl;
  int ret;
  BIO *sbio;

  if (!_hssl_enabled)
    return H_OK;

  log_verbose("Starting SSL initialization for socket %d", sock->sock);

  if (!(ssl = SSL_new(_hssl_context)))
  {
    log_warn("SSL_new failed");
    return herror_new("hssl_server_ssl", HSSL_ERROR_SERVER,
                      "Cannot create SSL object");
  }
  /* SSL_set_fd(ssl, sock->sock); */

  sbio = BIO_new_socket(sock->sock, BIO_NOCLOSE);

  if (sbio == NULL)
  {
    log_error("BIO_new_socket failed");
    return NULL;
  }
  /* BIO_set_callback(sbio, hssl_bio_cb); */
  sbio->method->bread = _hssl_bio_read;
  SSL_set_bio(ssl, sbio, sbio);

  if ((ret = SSL_accept(ssl)) <= 0)
  {
    herror_t err;

    log_error("SSL_accept failed (%s)", _hssl_get_error(ssl, ret));

    err = herror_new("hssl_server_ssl", HSSL_ERROR_SERVER, 
      "SSL_accept failed (%s)", _hssl_get_error(ssl, ret));
    SSL_free(ssl);

    return err;
  }

  sock->ssl = ssl;

  return H_OK;
}

void
hssl_cleanup(hsocket_s *sock)
{
  if (sock->ssl)
  {
    SSL_shutdown(sock->ssl);
    SSL_free(sock->ssl);
    sock->ssl = NULL;
  }

  return;
}

herror_t
hssl_read(hsocket_s *sock, char *buf, ng_size_t len, ng_size_t * received)
{
  int count;

/* log_verbose("sock->sock=%d sock->ssl=%p, len=%li", sock->sock, sock->ssl, len); */

  if (sock->ssl)
  {
    if ((count = SSL_read(sock->ssl, buf, len)) < 1)
      return herror_new("hssl_read", HSOCKET_ERROR_RECEIVE,
                        "SSL_read failed (%s)", _hssl_get_error(sock->ssl, count));
  }
  else
  {
    if ((count = hsocket_select_recv(sock->sock, buf, len)) == -1)
      return herror_new("hssl_read", HSOCKET_ERROR_RECEIVE,
                        "recv failed (%s)", os_strerror(ng_socket_errno));
  }
  *received = count;

  return H_OK;
}


herror_t
hssl_write(hsocket_s *sock, const char *buf, ng_size_t len, ng_size_t *sent)
{
  int count;

/*  log_verbose("sock->sock=%d, sock->ssl=%p, len=%li", sock->sock, sock->ssl, len); */

  if (sock->ssl)
  {
    if ((count = SSL_write(sock->ssl, buf, len)) == -1)
      return herror_new("hssl_write", HSOCKET_ERROR_SEND,
                        "SSL_write failed (%s)", _hssl_get_error(sock->ssl,
                                                                 count));
  }
  else
  {
    if ((count = send(sock->sock, buf, len, 0)) == -1)
      return herror_new("hssl_write", HSOCKET_ERROR_SEND, "send failed (%s)",
                        os_strerror(ng_socket_errno));
  }
  *sent = count;

  return H_OK;
}

#endif
