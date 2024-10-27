#ifndef __NanoHttpConfig_H_
#define __NanoHttpConfig_H_

#define HAVE_STDIO_H
#define HAVE_STDLIB_H
#define HAVE_SYS_TIME_H
#define HAVE_STRING_H
#define HAVE_ERRNO_H
#define HAVE_NETINET_IN_H
#define HAVE_PTHREAD_H
#define HAVE_STDARG_H
#define HAVE_CTYPE_H

#define HAVE_SYS_SELECT_H
#define HAVE_SOCKET_H
#define HAVE_SYS_TYPES_H
#define HAVE_SIGNAL_H
#define HAVE_UNISTD_H

#define HAVE_SYS_SOCKET_H
#define HAVE_ARPA_INET_H
#define HAVE_FCNTL_H
#define HAVE_SYS_WAIT_H
#define HAVE_TIME_H

#define __NHTTP_INTERNAL

#undef WIN32
#undef HAVE_SSL

#define _nanoConfig_HTTPD_PORT 8080
#define _nanoConfig_HTTPD_MAX_CONNECTIONS 8
#define _nanoConfig_HTTPD_FILE_BLOCK 1024
#define _nanoConfig_HTTPD_FILE_SERVICE "/config/"

#define DEBUG_MULTIPART

#endif
