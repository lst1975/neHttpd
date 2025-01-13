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
#include "nanohttp-config.h"
#include "nanohttp-defs.h"
#include "nanohttp-system.h"
#include "nanohttp-error.h"
#include "nanohttp-logging.h"
#include "nanohttp-time.h"
#include "nanohttp-system.h"
#include "nanohttp-mem.h"
#include "nanohttp-json.h"
#include "nanohttp-utils.h"
#include "nanohttp-time.h"

static long __os_get_tzoffset(void);

#if defined(WIN32) && defined(__GNUC__) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
typedef char STRERR_Buffer[1024];
RTE_DEFINE_PER_LCORE(STRERR_Buffer, ng_strerror_buffer);
#if defined(WIN32) && defined(__GNUC__) && (GCC_VERSION >= 100000)
#pragma GCC diagnostic pop
#endif

#define NG_SOCKET_MAXCONN 0xfffffff

ng_os_info_s ng_os_info = {
  .ngx_max_sockets = NG_SOCKET_MAXCONN,
  .ngx_ncpu = 0,
};

static void ng_print_version(void *log) 
{
#ifdef NG_COMPILER
  log_print(NG_VER_BUILD" built by " NG_COMPILER"\n");
#else
  log_print(NG_VER_BUILD"\n");
#endif
}

#ifdef WIN32 
#include <windows.h>
#include <Winsock2.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <stdio.h>
#include <locale.h>
#if defined(_MSC_VER)
#pragma comment(lib, "pdh.lib")
#endif

char *ng_get_pwd(char *path, size_t len) 
{
  // Get the current executable path
  if (!GetModuleFileName(NULL, path, len))
  {
    int err = GetLastError();
    log_error("GetModuleFileName failed. (%d:%s)", 
      err, os_strerror(err));
    return NULL;
  }

  return path;
}

static ng_result_t __ng_get_freq(void *log, double *freq) 
{
  *freq = get_tsc_freq();
  ng_print_cpufreq("CPU Frequency", *freq/1000000.0);
  return ng_ERR_NONE;
}

const char *__os_strerror(int err)
{
  size_t         len;
  static size_t  lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
  char *errstr = RTE_PER_LCORE(ng_strerror_buffer);
  size_t size = sizeof(RTE_PER_LCORE(ng_strerror_buffer));

  len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, err, lang, (char *)errstr, size, NULL);
  if (len == 0 && lang && GetLastError() == ERROR_RESOURCE_LANG_NOT_FOUND) {

    /*
     * Try to use English messages first and fallback to a language,
     * based on locale: non-English Windows have no English messages
     * at all.  This way allows to use English messages at least on
     * Windows with MUI.
     */

    lang = 0;
    len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL, err, lang, (char *)errstr, size, NULL);
  }

  if (len == 0) {
    len = ng_snprintf(errstr, size, "FormatMessage() error:(%d)", GetLastError());
  }

  /* remove ".\r\n\0" */
  while (errstr[len] == '\0' || errstr[len] == '\r'
         || errstr[len] == '\n' || errstr[len] == '.')
  {
    --len;
  }
  errstr[len+1] = '\0';
  return errstr;
}

/*
void Sleep(
  [in] DWORD dwMilliseconds
);

Parameters
[in] dwMilliseconds

The time interval for which execution is to be suspended, in milliseconds.

A value of zero causes the thread to relinquish the remainder of its time 
slice to any other thread that is ready to run. If there are no other 
threads ready to run, the function returns immediately, and the thread 
continues execution. Windows XP: A value of zero causes the thread to 
relinquish the remainder of its time slice to any other thread of equal 
priority that is ready to run. If there are no other threads of equal 
priority ready to run, the function returns immediately, and the thread 
continues execution. This behavior changed starting with Windows Server 2003.

A value of INFINITE indicates that the suspension should not time out.

Return value
  None

*/
int ng_os_usleep(int usec)
{
  Sleep(usec);
  return 0;
}

void
ng_gettimeofday(ng_tmv_s *tp)
{
  uint64_t  intervals;
  FILETIME  ft;

  GetSystemTimeAsFileTime(&ft);

  /*
   * A file time is a 64-bit value that represents the number
   * of 100-nanosecond intervals that have elapsed since
   * January 1, 1601 12:00 A.M. UTC.
   *
   * Between January 1, 1970 (Epoch) and January 1, 1601 there were
   * 134774 days,
   * 11644473600 seconds or
   * 11644473600,000,000,0 100-nanosecond intervals.
   *
   * See also MSKB Q167296.
   */

  intervals = ((uint64_t) ft.dwHighDateTime << 32) | ft.dwLowDateTime;
  intervals -= 116444736000000000;

  tp->tv_sec = (long) (intervals / 10000000);
  tp->tv_usec = (long) ((intervals % 10000000) / 10);
}

static ng_result_t 
__os_socket_init_once(void *log)
{
  WSADATA wsaData;

  /* init Winsock */
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) 
  {
    int err = WSAGetLastError();
    log_fatal("WSAStartup() failed. (%d:%s)", err, os_strerror(err));
    goto clean0;
  }
  
  /* Confirm that the WinSock DLL supports 2.2.        */
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
  {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    int err = WSAGetLastError();
    log_fatal("Could not find a usable version of Winsock.dll. (%d:%s)", 
      err, os_strerror(err));
    goto clean1;
  }
    
  log_info("The Winsock 2.2 dll was found okay");  
  
  return ng_ERR_NONE;
  
clean1:
  WSACleanup();
clean0:
  return ng_ERR_ESYSTEM;
}

static BOOL init__done = FALSE;
static INIT_ONCE init__once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK init__once_callback(INIT_ONCE *once,
                                         void *parameter,
                                         void** context) {
  NG_UNUSED(once);
  NG_UNUSED(context);

  /* N.b. that initialization order matters here. */
  if (__os_socket_init_once(parameter) != ng_ERR_NONE)
    return FALSE;

  init__done = TRUE;
  return TRUE;
}

static int __os_socket_init(void *log) {
  if (!init__done &&
      !InitOnceExecuteOnce(&init__once, init__once_callback, log, NULL))
    /* `InitOnceExecuteOnce()` itself is infallible, and it doesn't set any
     * error code when the once-callback returns FALSE. We return -1 here to
     * indicate that global initialization failed; the failing init function is
     * resposible for setting `errno` and calling `SetLastError()`. */
    return ng_ERR_ESYSTEM;

  return ng_ERR_NONE;
}


static void 
__os_socket_deinit(void *log)
{
  NG_UNUSED(log);
  WSACleanup();
}

#define NGX_WIN_NT        200000

typedef DWORD               ngx_pid_t;
#define NGX_INVALID_PID     0
ng_ulong_t ngx_pid;
ng_ulong_t ngx_ppid;

static ng_uint_t  ngx_win32_version;
static ng_uint_t  ngx_max_wsabufs;

typedef struct {
  WORD  wServicePackMinor;
  WORD  wSuiteMask;
  BYTE  wProductType;
} ngx_osviex_stub_t;

static u_int               osviex;
static OSVERSIONINFOEX     osvi;

#ifndef WSAID_ACCEPTEX

typedef BOOL (PASCAL FAR * LPFN_ACCEPTEX)(
    IN SOCKET sListenSocket,
    IN SOCKET sAcceptSocket,
    IN PVOID lpOutputBuffer,
    IN DWORD dwReceiveDataLength,
    IN DWORD dwLocalAddressLength,
    IN DWORD dwRemoteAddressLength,
    OUT LPDWORD lpdwBytesReceived,
    IN LPOVERLAPPED lpOverlapped
    );

#define WSAID_ACCEPTEX                                                       \
    {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

#endif


#ifndef WSAID_GETACCEPTEXSOCKADDRS

typedef VOID (PASCAL FAR * LPFN_GETACCEPTEXSOCKADDRS)(
    IN PVOID lpOutputBuffer,
    IN DWORD dwReceiveDataLength,
    IN DWORD dwLocalAddressLength,
    IN DWORD dwRemoteAddressLength,
    OUT struct sockaddr **LocalSockaddr,
    OUT LPINT LocalSockaddrLength,
    OUT struct sockaddr **RemoteSockaddr,
    OUT LPINT RemoteSockaddrLength
    );

#define WSAID_GETACCEPTEXSOCKADDRS                                           \
        {0xb5367df2,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

#endif


#ifndef WSAID_TRANSMITFILE

#ifndef TF_DISCONNECT

#define TF_DISCONNECT           1
#define TF_REUSE_SOCKET         2
#define TF_WRITE_BEHIND         4
#define TF_USE_DEFAULT_WORKER   0
#define TF_USE_SYSTEM_THREAD    16
#define TF_USE_KERNEL_APC       32

typedef struct _TRANSMIT_FILE_BUFFERS {
    LPVOID Head;
    DWORD HeadLength;
    LPVOID Tail;
    DWORD TailLength;
} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, FAR *LPTRANSMIT_FILE_BUFFERS;

#endif

typedef BOOL (PASCAL FAR * LPFN_TRANSMITFILE)(
    IN SOCKET hSocket,
    IN HANDLE hFile,
    IN DWORD nNumberOfBytesToWrite,
    IN DWORD nNumberOfBytesPerSend,
    IN LPOVERLAPPED lpOverlapped,
    IN LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    IN DWORD dwReserved
    );

#define WSAID_TRANSMITFILE                                                   \
    {0xb5367df0,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

#endif


#ifndef WSAID_TRANSMITPACKETS

/* OpenWatcom has a swapped TP_ELEMENT_FILE and TP_ELEMENT_MEMORY definition */

#ifndef TP_ELEMENT_FILE

#ifdef _MSC_VER
#pragma warning(disable:4201) /* Nonstandard extension, nameless struct/union */
#endif

typedef struct _TRANSMIT_PACKETS_ELEMENT {
    ULONG dwElFlags;
#define TP_ELEMENT_MEMORY   1
#define TP_ELEMENT_FILE     2
#define TP_ELEMENT_EOP      4
    ULONG cLength;
    union {
        struct {
            LARGE_INTEGER nFileOffset;
            HANDLE        hFile;
        };
        PVOID             pBuffer;
    };
} TRANSMIT_PACKETS_ELEMENT, *PTRANSMIT_PACKETS_ELEMENT,
    FAR *LPTRANSMIT_PACKETS_ELEMENT;

#ifdef _MSC_VER
#pragma warning(default:4201)
#endif

#endif

typedef BOOL (PASCAL FAR * LPFN_TRANSMITPACKETS) (
    SOCKET hSocket,
    TRANSMIT_PACKETS_ELEMENT *lpPacketArray,
    DWORD nElementCount,
    DWORD nSendSize,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags
    );

#define WSAID_TRANSMITPACKETS                                                \
    {0xd9689da0,0x1f90,0x11d3,{0x99,0x71,0x00,0xc0,0x4f,0x68,0xc8,0x76}}

#endif


#ifndef WSAID_CONNECTEX

typedef BOOL (PASCAL FAR * LPFN_CONNECTEX) (
    IN SOCKET s,
    IN const struct sockaddr FAR *name,
    IN int namelen,
    IN PVOID lpSendBuffer OPTIONAL,
    IN DWORD dwSendDataLength,
    OUT LPDWORD lpdwBytesSent,
    IN LPOVERLAPPED lpOverlapped
    );

#define WSAID_CONNECTEX \
    {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}

#endif


#ifndef WSAID_DISCONNECTEX

typedef BOOL (PASCAL FAR * LPFN_DISCONNECTEX) (
    IN SOCKET s,
    IN LPOVERLAPPED lpOverlapped,
    IN DWORD  dwFlags,
    IN DWORD  dwReserved
    );

#define WSAID_DISCONNECTEX                                                   \
    {0x7fda2e11,0x8630,0x436f,{0xa0,0x31,0xf5,0x36,0xa6,0xee,0xc1,0x57}}

#endif

extern LPFN_ACCEPTEX              ngx_acceptex;
extern LPFN_GETACCEPTEXSOCKADDRS  ngx_getacceptexsockaddrs;
extern LPFN_TRANSMITFILE          ngx_transmitfile;
extern LPFN_TRANSMITPACKETS       ngx_transmitpackets;
extern LPFN_CONNECTEX             ngx_connectex;
extern LPFN_DISCONNECTEX          ngx_disconnectex;

/* Should these pointers be per protocol ? */
LPFN_ACCEPTEX              ngx_acceptex;
LPFN_GETACCEPTEXSOCKADDRS  ngx_getacceptexsockaddrs;
LPFN_TRANSMITFILE          ngx_transmitfile;
LPFN_TRANSMITPACKETS       ngx_transmitpackets;
LPFN_CONNECTEX             ngx_connectex;
LPFN_DISCONNECTEX          ngx_disconnectex;

static GUID ax_guid = WSAID_ACCEPTEX;
static GUID as_guid = WSAID_GETACCEPTEXSOCKADDRS;
static GUID tf_guid = WSAID_TRANSMITFILE;
static GUID tp_guid = WSAID_TRANSMITPACKETS;
static GUID cx_guid = WSAID_CONNECTEX;
static GUID dx_guid = WSAID_DISCONNECTEX;

static void
__os_sys_status(void *log)
{
  ngx_osviex_stub_t  *osviex_stub;

  /* get Windows version */

  ng_bzero(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

  osviex = GetVersionEx((OSVERSIONINFO *) &osvi);
  if (osviex == 0) 
  {
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (GetVersionEx((OSVERSIONINFO *) &osvi) == 0) 
    {
      int err = WSAGetLastError();
      log_fatal("GetVersionEx() failed. (%d:%s)", 
          err, os_strerror(err));
      return;
    }
  }

  /*
   *  Windows 3.1 Win32s   0xxxxx
   *
   *  Windows 95           140000
   *  Windows 98           141000
   *  Windows ME           149000
   *  Windows NT 3.51      235100
   *  Windows NT 4.0       240000
   *  Windows NT 4.0 SP5   240050
   *  Windows 2000         250000
   *  Windows XP           250100
   *  Windows 2003         250200
   *  Windows Vista/2008   260000
   *
   *  Windows CE x.x       3xxxxx
   */

  ngx_win32_version = osvi.dwPlatformId * 100000
                      + osvi.dwMajorVersion * 10000
                      + osvi.dwMinorVersion * 100;

  if (osviex) 
  {
    ngx_win32_version += osvi.wServicePackMajor * 10
                         + osvi.wServicePackMinor;
  }

#ifdef _MSC_VER
#pragma warning(default:4996)
#endif

  ng_print_version(log);

  if (osviex) 
  {
    /*
     * the MSVC 6.0 SP2 defines wSuiteMask and wProductType
     * as WORD wReserved[2]
     */
    osviex_stub = (ngx_osviex_stub_t *) &osvi.wServicePackMinor;

    if (strlen(osvi.szCSDVersion))
      ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
        "OS: Windows %u build:%u, \"%s\", suite:%X, type:%u",
        ngx_win32_version, osvi.dwBuildNumber, osvi.szCSDVersion,
        osviex_stub->wSuiteMask, osviex_stub->wProductType);
    else
      ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
        "OS: Windows %u build:%u, suite:%X, type:%u",
        ngx_win32_version, osvi.dwBuildNumber, 
        osviex_stub->wSuiteMask, osviex_stub->wProductType);
  } 
  else 
  {
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
    {
      /* Win9x build */
      if (strlen(osvi.szCSDVersion))
        ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
            "OS: Windows %u build:%u.%u.%u, \"%s\"",
            ngx_win32_version,
            osvi.dwBuildNumber >> 24,
            (osvi.dwBuildNumber >> 16) & 0xff,
            osvi.dwBuildNumber & 0xffff,
            osvi.szCSDVersion);
      else
        ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
            "OS: Windows %u build:%u.%u.%u",
            ngx_win32_version,
            osvi.dwBuildNumber >> 24,
            (osvi.dwBuildNumber >> 16) & 0xff,
            osvi.dwBuildNumber & 0xffff);
    } 
    else 
    {
      /*
       * VER_PLATFORM_WIN32_NT
       *
       * we do not currently support VER_PLATFORM_WIN32_CE
       * and we do not support VER_PLATFORM_WIN32s at all
       */
      if (strlen(osvi.szCSDVersion))
        ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
            "OS: Windows %u build:%u, \"%s\"",
            ngx_win32_version, osvi.dwBuildNumber,
            osvi.szCSDVersion);
      else
        ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
            "OS: Windows %u build:%u",
            ngx_win32_version, osvi.dwBuildNumber);
    }
  }
  
  log_info("%s", ng_os_info.ng_os_version);
}

static ng_size_t __CacheLineSize(void) 
{
	ng_size_t lineSize = 0;
	DWORD bufferSize = 0;
	DWORD i = 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

	GetLogicalProcessorInformation(0, &bufferSize);
	buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *) os_malloc(bufferSize);
	GetLogicalProcessorInformation(&buffer[0], &bufferSize);

	for (i = 0; i != bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
		if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
			lineSize = buffer[i].Cache.LineSize;
			break;
		}
	}

	os_free(buffer);
	return lineSize;
}

static ng_result_t
__os_sys_init(void *log)
{
  DWORD         bytes;
  SOCKET        s;
  ng_uint_t     n;
  SYSTEM_INFO   si;
  ngx_pid_t     pid;
  ng_result_t status = ng_ERR_NONE;
  
  pid = GetCurrentProcessId();
  ng_os_info.ngx_pid = pid;
  ng_os_info.ngx_ppid = 0;
  
  __os_sys_status(log);

  GetSystemInfo(&si);
  ng_os_info.ngx_pagesize = si.dwPageSize;
  ng_os_info.ngx_allocation_granularity = si.dwAllocationGranularity;
  ng_os_info.ngx_ncpu = si.dwNumberOfProcessors;
  ng_os_info.ngx_cacheline_size = __CacheLineSize();

  for (n = ng_os_info.ngx_pagesize; n >>= 1; ng_os_info.ngx_pagesize_shift++) { /* void */ }

  /* delete default "C" locale for _wcsicmp() */
  setlocale(LC_ALL, "");

  if (ngx_win32_version < NGX_WIN_NT) 
  {
    ngx_max_wsabufs = 16;
    goto clean0;
  }

  /* STUB: ngx_uint_t max */
  ngx_max_wsabufs = 1024 * 1024;

  /*
   * get AcceptEx(), GetAcceptExSockAddrs(), TransmitFile(),
   * TransmitPackets(), ConnectEx(), and DisconnectEx() addresses
   */

  s = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (s == INVALID_SOCKET) 
  {
    int err = WSAGetLastError();
    log_fatal("WSASocketW failed. (%d:%s).", err, os_strerror(err));
    status = ng_ERR_ESYSTEM;
    goto clean0;
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
              &ax_guid, sizeof(GUID),
              &ngx_acceptex, sizeof(LPFN_ACCEPTEX), 
              &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_ACCEPTEX) failed. (%d:%s).", err, os_strerror(err));
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &as_guid, sizeof(GUID),
              &ngx_getacceptexsockaddrs, sizeof(LPFN_GETACCEPTEXSOCKADDRS),
              &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_GETACCEPTEXSOCKADDRS) failed. (%d:%s).", err, os_strerror(err));
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &tf_guid, sizeof(GUID),
            &ngx_transmitfile, sizeof(LPFN_TRANSMITFILE), 
            &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_TRANSMITFILE) failed. (%d:%s).", err, os_strerror(err));
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &tp_guid, sizeof(GUID),
            &ngx_transmitpackets, sizeof(LPFN_TRANSMITPACKETS), 
            &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_TRANSMITPACKETS) failed. (%d:%s).", err, os_strerror(err));
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &cx_guid, sizeof(GUID),
            &ngx_connectex, sizeof(LPFN_CONNECTEX), 
            &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_CONNECTEX) failed. (%d:%s).", err, os_strerror(err));
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &dx_guid, sizeof(GUID),
            &ngx_disconnectex, sizeof(LPFN_DISCONNECTEX), 
            &bytes, NULL, NULL))
  {
    int err = WSAGetLastError();
    log_warn("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
      "WSAID_DISCONNECTEX) failed. (%d:%s).", err, os_strerror(err));
  }

  if (closesocket(s) == SOCKET_ERROR) 
  {
    int err = WSAGetLastError();
    log_warn("closesocket failed. (%d:%s).", err, os_strerror(err));
  }

clean0:
  return status;
}

static void
__os_sys_deinit(void *log)
{
  NG_UNUSED(log);
}

static ng_result_t __ng_os_init(void *log)
{
  ng_result_t r;
  ng_tmv_s tp;

  r = __os_socket_init(log);
  if (r != ng_ERR_NONE)
    goto clean0;

  r = __os_sys_init(log);
  if (r != ng_ERR_NONE)
    goto clean1;

  ng_os_info.tz_offset = __os_get_tzoffset();
  ng_os_info.ngx_allocation_granularity = 0;
  ng_gettimeofday(&tp);
  srand((ng_os_info.ngx_pid << 16) ^ (unsigned) tp.tv_sec ^ tp.tv_usec);
  
  return ng_ERR_NONE;
  
clean1:
  __os_sys_deinit(log);
clean0:
  return r;
}

static void __ng_os_deinit(void *log)
{
  __os_socket_deinit(log);
  __os_sys_deinit(log);
}

static long
__os_get_tzoffset(void)
{
  // Get the current system time
  SYSTEMTIME systemTime;
  GetSystemTime(&systemTime);

  // Convert system time to file time
  FILETIME fileTime;
  SystemTimeToFileTime(&systemTime, &fileTime);

  // Convert file time to local file time
  FILETIME localFileTime;
  FileTimeToLocalFileTime(&fileTime, &localFileTime);

  // Convert local file time to system time
  SYSTEMTIME localSystemTime;
  FileTimeToSystemTime(&localFileTime, &localSystemTime);

  // Calculate the difference in minutes between UTC and local time
  int offsetHours = localSystemTime.wHour - systemTime.wHour;
  int offsetMinutes = localSystemTime.wMinute - systemTime.wMinute;
  int totalOffsetMinutes = (offsetHours * 60) + offsetMinutes;

  // Print the timezone offset
  log_debug("Local timezone offset from GMT: %02d:%02d\n", offsetHours, offsetMinutes);

  return totalOffsetMinutes*60;
}

#else

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#if defined(RTE_ENV_LINUX) || defined(__linux__)
#include <sys/utsname.h>
#endif
#include <sys/resource.h>

#define BUFFER_SIZE 256

const char *__os_strerror(int err)
{
  char *errstr = RTE_PER_LCORE(ng_strerror_buffer);
  size_t size = sizeof(RTE_PER_LCORE(ng_strerror_buffer));
  int len = snprintf(errstr, size, "%s", strerror(err));
  
  /* remove ".\r\n\0" */
  while (errstr[len] == '\0' || errstr[len] == '\r'
         || errstr[len] == '\n' || errstr[len] == '.')
  {
    --len;
  }
  errstr[len+1] = '\0';
  return errstr;
}

char *ng_get_pwd(char *path, size_t len) 
{
  // Get the current executable path
  return getcwd(path, len);
}
static ng_result_t __ng_get_freq(void *log, double *freq) 
{
  FILE *file;
  char buffer[BUFFER_SIZE];
  char *line;
  char *search = "cpu MHz";
  double cpu_freq = 0.0;
  
  file = fopen("/proc/cpuinfo", "r");
  if (file == NULL) {
    int err = errno;
    log_fatal("fopen failed. (%d:%s).", err, os_strerror(err));
    return ng_ERR_ESYSTEM;
  }
  
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    if (strncmp(buffer, search, strlen(search)) == 0) {
      line = strchr(buffer, ':');
      if (line != NULL) {
        cpu_freq = strtod(line + 1, NULL);
        break;
      }
    }
  }
  
  fclose(file);
  
  if (cpu_freq > 0.0) {
    ng_print_cpufreq("CPU Frequency", cpu_freq);
    *freq = cpu_freq * 1000000.0;
    return ng_ERR_NONE;
  }
  else 
  {
    int err = errno;
    log_error("CPU Frequency not found. (%d:%s).", err, os_strerror(err));
    return ng_ERR_ESYSTEM;
  }
}

/*
int nanosleep(const struct timespec *duration,
                     struct timespec *_Nullable rem);

DESCRIPTION         top
       nanosleep() suspends the execution of the calling thread until
       either at least the time specified in *duration has elapsed, or
       the delivery of a signal that triggers the invocation of a
       handler in the calling thread or that terminates the process.

       If the call is interrupted by a signal handler, nanosleep()
       returns -1, sets errno to EINTR, and writes the remaining time
       into the structure pointed to by rem unless rem is NULL.  The
       value of *rem can then be used to call nanosleep() again and
       complete the specified pause (but see NOTES).

       The timespec(3) structure is used to specify intervals of time
       with nanosecond precision.

       The value of the nanoseconds field must be in the range [0,
       999999999].

       Compared to sleep(3) and usleep(3), nanosleep() has the following
       advantages: it provides a higher resolution for specifying the
       sleep interval; POSIX.1 explicitly specifies that it does not
       interact with signals; and it makes the task of resuming a sleep
       that has been interrupted by a signal handler easier.
RETURN VALUE         top
       On successfully sleeping for the requested duration, nanosleep()
       returns 0.  If the call is interrupted by a signal handler or
       encounters an error, then it returns -1, with errno set to
       indicate the error.
ERRORS        
       EFAULT Problem with copying information from user space.

       EINTR  The pause has been interrupted by a signal that was
              delivered to the thread (see signal(7)).  The remaining
              sleep time has been written into *rem so that the thread
              can easily call nanosleep() again and continue with the
              pause.

       EINVAL The value in the tv_nsec field was not in the range [0,
              999999999] or tv_sec was negative.

*/
int ng_os_usleep(int usec)
{
  struct timespec duration;
  duration.tv_sec = usec/1000;
  duration.tv_nsec = (usec%1000) * 1000000;
  return nanosleep(&duration, NULL);
}

void
ng_gettimeofday(ng_tmv_s *tp)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  tp->tv_sec = tv.tv_sec;
  tp->tv_usec = tv.tv_usec;
}

#define NGX_INVALID_PID  -1
typedef pid_t ngx_pid_t;

static ng_result_t
__os_specific_init(void *log)
{
#if defined(RTE_ENV_LINUX) || defined(__linux__)
  struct utsname  u;

  if (uname(&u) == -1) {
    int err = errno;
    log_warn("uname() failed. (%d:%s).", err, os_strerror(err));
    return ng_ERR_ESYSTEM;
  }

  ng_snprintf(ng_os_info.ng_os_version, 
    sizeof(ng_os_info.ng_os_version),
    "OS: %s build:%s", u.sysname, u.release);
#endif

  return ng_ERR_NONE;
}

static void
__os_specific_status(void *log)
{
#if defined(RTE_ENV_LINUX) || defined(__linux__)
  log_info("OS: %s", ng_os_info.ng_os_version);
#endif
}

static void
__os_sys_status(void *log)
{
  ng_print_version(log);
  __os_specific_status(log);
}

ng_result_t
__os_sys_init(void *log)
{
  ngx_pid_t pid;
  ng_result_t r;
  struct rlimit  rlmt;
  ng_uint_t   n;
#if defined(_SC_LEVEL1_DCACHE_LINESIZE)
  long         size;
#endif

  r = ng_ERR_ESYSTEM;
  pid = getpid();
  ng_os_info.ngx_pid  = pid;
  pid = getppid();
  ng_os_info.ngx_ppid  = pid;

  r = __os_specific_init(log);
  if (r != ng_ERR_NONE)
    goto clean0;

  __os_sys_status(log);

  ng_os_info.ngx_pagesize = getpagesize();
  for (n = ng_os_info.ngx_pagesize; n >>= 1; ng_os_info.ngx_pagesize_shift++) { /* void */ }

#if defined(_SC_NPROCESSORS_ONLN)
  if (ng_os_info.ngx_ncpu == 0) 
  {
    ng_os_info.ngx_ncpu = sysconf(_SC_NPROCESSORS_ONLN);
  }
#endif

  if (ng_os_info.ngx_ncpu < 1) 
  {
    ng_os_info.ngx_ncpu = 1;
  }

  ng_os_info.ngx_cacheline_size = ng_ALIGN_SIZE;
#if defined(_SC_LEVEL1_DCACHE_LINESIZE)
  size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  if (size > 0) {
    ng_os_info.ngx_cacheline_size = size;
  }
#else
  ng_os_info.ngx_cacheline_size = ng_cpuinfo();
#endif

  if (getrlimit(RLIMIT_NOFILE, &rlmt) == -1) {
    int err = errno;
    log_error("getrlimit(RLIMIT_NOFILE) failed. (%d:%s).", 
      err, os_strerror(err));
    goto clean0;
  }

  ng_os_info.ngx_max_sockets = (ng_int_t) rlmt.rlim_cur;

  log_info("getrlimit(RLIMIT_NOFILE): %u:%u",
                rlmt.rlim_cur, rlmt.rlim_max);

  return ng_ERR_NONE;
  
clean0:
  return r;
}
static long
__os_get_tzoffset(void)
{
  time_t current_time;
  struct tm *local_time;

  // Get the current time
  time(&current_time);
  
  // Convert to local time
  local_time = localtime(&current_time);
  
  // Get the timezone offset in seconds
  long timezone_offset = local_time->tm_gmtoff;

  // Convert to hours and minutes
  log_debug("Local timezone offset from GMT: %02d:%02d\n", 
      timezone_offset / 3600, (timezone_offset % 3600) / 60);
  return timezone_offset;
}

static void
__os_sys_deinit(void *log)
{
  NG_UNUSED(log);
}

static ng_result_t __ng_os_init(void *log)
{
  ng_result_t r;
  ng_tmv_s tp;

  r = __os_sys_init(log);
  if (r != ng_ERR_NONE)
    goto clean0;
  
  ng_os_info.tz_offset = __os_get_tzoffset();
  ng_gettimeofday(&tp);
  srand((ng_os_info.ngx_pid << 16) ^ (unsigned) tp.tv_sec ^ tp.tv_usec);
  
  return ng_ERR_NONE;
  
clean0:
  return r;
}

static void __ng_os_deinit(void *log)
{
  __os_sys_deinit(log);
}

#endif

#if defined(__APPLE__)

#include <sys/sysctl.h>
static size_t CacheLineSize(void) {
	size_t lineSize = 0;
	size_t sizeOfLineSize = sizeof(lineSize);
	sysctlbyname("hw.cachelinesize", &lineSize, &sizeOfLineSize, 0, 0);
	return lineSize;
}

#elif defined(_WIN32)

#include <stdlib.h>
#include <windows.h>
static size_t CacheLineSize(void) {
	size_t lineSize = 0;
	DWORD bufferSize = 0;
	DWORD i = 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

	GetLogicalProcessorInformation(0, &bufferSize);
	buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *) malloc(bufferSize);
	GetLogicalProcessorInformation(&buffer[0], &bufferSize);

	for (i = 0; i != bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
		if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
			lineSize = buffer[i].Cache.LineSize;
			break;
		}
	}

	free(buffer);
	return lineSize;
}

#elif defined(__linux__)

#include <stdio.h>
static size_t CacheLineSize(void) {
	FILE * p = 0;
	p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
	int lineSize = 0;
	if (p) {
		if (EOF == fscanf(p, "%d", &lineSize))
      lineSize = 0;
	}
  fclose(p);
	return lineSize;
}

#else
static size_t CacheLineSize(void) {
  return ng_ALIGN_SIZE;
}
#endif

#if (( __i386__ || __amd64__ ) && ( __GNUC__ || __INTEL_COMPILER ))
#if ( __i386__ )
static __ng_inline__ void
__os_cpuid(uint32_t i, uint32_t *buf)
{
  /*
   * we could not use %ebx as output parameter if gcc builds PIC,
   * and we could not save %ebx on stack, because %esp is used,
   * when the -fomit-frame-pointer optimization is specified.
   */

  __asm__ (

  "    mov    %%ebx, %%esi;  "

  "    cpuid;                "
  "    mov    %%eax, (%1);   "
  "    mov    %%ebx, 4(%1);  "
  "    mov    %%edx, 8(%1);  "
  "    mov    %%ecx, 12(%1); "

  "    mov    %%esi, %%ebx;  "

  : : "a" (i), "D" (buf) : "ecx", "edx", "esi", "memory" );
}


#else /* __amd64__ */

static __ng_inline__ void
__os_cpuid(uint32_t i, uint32_t *buf)
{
    uint32_t  eax, ebx, ecx, edx;

    __asm__ (

        "cpuid"

    : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (i) );

    buf[0] = eax;
    buf[1] = ebx;
    buf[2] = edx;
    buf[3] = ecx;
}

#endif

/* auto detect the L2 cache line size of modern and widespread CPUs */
static ng_size_t
__ng_cpuinfo_cacheline(void)
{
  ng_size_t ngx_cacheline_size;

  ngx_cacheline_size = CacheLineSize();
  if (ngx_cacheline_size)
  {
    return ngx_cacheline_size;
  }
  char      *vendor;
  uint32_t   vbuf[5], cpu[4], model;

  vbuf[0] = 0;
  vbuf[1] = 0;
  vbuf[2] = 0;
  vbuf[3] = 0;
  vbuf[4] = 0;

  __os_cpuid(0, vbuf);

  vendor = (char *) &vbuf[1];

  if (vbuf[0] == 0) {
    return 0;
  }

  __os_cpuid(1, cpu);

  if (ng_strcmp(vendor, "GenuineIntel") == 0) 
  {
    switch ((cpu[0] & 0xf00) >> 8) {
      /* Pentium */
      case 5:
        ngx_cacheline_size = 32;
        break;

      /* Pentium Pro, II, III */
      case 6:
        ngx_cacheline_size = 32;

        model = ((cpu[0] & 0xf0000) >> 8) | (cpu[0] & 0xf0);
        if (model >= 0xd0) 
        {
          /* Intel Core, Core 2, Atom */
          ngx_cacheline_size = 64;
        }

        break;

      /*
       * Pentium 4, although its cache line size is 64 bytes,
       * it prefetches up to two cache lines during memory read
       */
      case 15:
        ngx_cacheline_size = 128;
        break;
    }

  }
  else if (ng_strcmp(vendor, "AuthenticAMD") == 0) 
  {
    ngx_cacheline_size = 64;
  }
  else
  {
    ngx_cacheline_size = 0;
  }
  return ngx_cacheline_size;
}

#else

static ng_size_t
__ng_cpuinfo_cacheline(void)
{
  return CacheLineSize();
}
#endif

static ng_size_t
ng_cpuinfo_cacheline(void)
{
  ng_size_t ngx_cacheline_size;
  ngx_cacheline_size = __ng_cpuinfo_cacheline();
  if (ngx_cacheline_size == 0)
    ngx_cacheline_size = ng_ALIGN_SIZE;
  return ngx_cacheline_size;
}

uint64_t ng_get_freq(void)
{
  double freq;
  if (ng_ERR_NONE != __ng_get_freq(NULL, &freq))
    return 0;
  return (uint64_t)freq;
}

#ifdef WIN32
static void __ng_get_tzname(void) 
{
  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
     "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation", 
     0, KEY_READ, &hKey) == ERROR_SUCCESS) 
  {
    char tzAbbreviation[256];
    DWORD size = sizeof(tzAbbreviation);
    if (RegQueryValueEx(hKey, "TimeZoneKeyName", NULL, NULL, 
      (LPBYTE)tzAbbreviation, &size) == ERROR_SUCCESS) 
    {
      log_info("Time Zone Abbreviation: %s.", tzAbbreviation);
      char *p = tzAbbreviation;
      char *e = p + size;
      int i = 1;
      while (p != NULL)
      {
        if (i < sizeof(ng_os_info.tz))
          ng_os_info.tz[i++]=*p;
        p = memchr(p, ' ', e - p);
        if (p == NULL)
          break;
        while (__ng_isspace(*p)){p++;};
      }
      ng_os_info.tz[i]='\0';
      ng_os_info.tz[0]=' ';
    }
    else
    {
      int err = GetLastError();
      log_error("Error accessing the registry. (%d:%s)", err, os_strerror(err));
    }
    RegCloseKey(hKey);
  } 
  else
  {
    int err = GetLastError();
    log_error("Error accessing the registry. (%d:%s)", err, os_strerror(err));
  }
}
#else
static void __ng_get_tzname(void) 
{
  ng_get_tzname(ng_os_info.tz+1, sizeof(ng_os_info.tz)-1);
  ng_os_info.tz[0]=' ';
}
#endif

ng_result_t ng_os_init(void)
{
  ng_result_t r;

  ng_os_info.cacheline_size = ng_cpuinfo_cacheline();
  
  if ((r = __ng_os_init(NULL)) != ng_ERR_NONE)
    goto clean0;
  
  if ((r = __ng_get_freq(NULL, &ng_os_info.freq)) != ng_ERR_NONE)
    goto clean1;

  if ((r = rte_eal_timer_init()) != ng_ERR_NONE)
    goto clean1;

  ng_update_time();
  __ng_get_tzname();

  if (ng_os_info.cacheline_size &&
    ng_os_info.cacheline_size != RTE_CACHE_LINE_SIZE)
  {
    ng_fprintf(stderr, 
      "System cachelinesize is %ld, but defined "
      "RTE_CACHE_LINE_SIZE as %d", 
      ng_os_info.cacheline_size, 
      (int)RTE_CACHE_LINE_SIZE);
    ng_fprintf(stderr, 
      "Please redefine RTE_CACHE_LINE_SIZE as %ld",
      ng_os_info.cacheline_size);
    goto clean1;
  }
  
  return ng_ERR_NONE;
  
clean1:
  __ng_os_deinit(NULL);
clean0:
  return r;
}

#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#include <windows.h>
int ng_os_get_meminfo(mg_mem_info_s *m) {
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex)) 
  {
    m->ullTotalPhys    = statex.ullTotalPhys;
    m->ullAvailPhys    = statex.ullAvailPhys;
    m->ullTotalVirtual = statex.ullTotalVirtual;
    m->ullAvailVirtual = statex.ullAvailVirtual;
  } 
  else 
  {
    log_warn("Error retrieving memory status.\n");
    return -1;
  }

  return 0;
}
#else
#include <sys/sysinfo.h>
int ng_os_get_meminfo(mg_mem_info_s *m) {
  struct sysinfo info;

  // Retrieve system information
  if (sysinfo(&info) != 0) {
    log_warn("Error retrieving memory status.\n");
    return -1;
  }

  // Calculate remaining physical and virtual memory
  m->ullTotalPhys    = info.totalram;
  m->ullAvailPhys    = info.freeram;
  m->ullTotalVirtual = info.totalswap;
  m->ullAvailVirtual = info.freeswap;
  return 0;
}
#endif

void ng_os_deinit(void)
{
  __ng_os_deinit(NULL);
}

void ng_os_dump(httpd_buf_t *b, void *printer)
{
  mg_mem_info_s m;
  JSON_PRINTER_f pr = (JSON_PRINTER_f)printer;
  if (pr == NULL)
    pr = json_printer_default;
  
  pr(b, "%-16s: %"PRIu64"\n", "Process ID", ng_os_info.ngx_pid);
  pr(b, "%-16s: %"PRIu64"\n", "Parent ID", ng_os_info.ngx_ppid);
  pr(b, "%-16s: %"PRIu64"\n", "CPU Count", ng_os_info.ngx_ncpu);
  pr(b, "%-16s: %"PRIu64"\n", "Socket Max", ng_os_info.ngx_max_sockets);
  pr(b, "%-16s: %lu\n", "Page Size", ng_os_info.ngx_pagesize);
  pr(b, "%-16s: %lu\n", "Cacheline Size", ng_os_info.ngx_cacheline_size);
  if (ng_os_info.freq/1000000 > 999.0)
    pr(b, "%-16s: %.2f GHz\n", "CPU Frequency", ng_os_info.freq/1000000000);
  else
    pr(b, "%-16s: %.2f MHz\n", "CPU Frequency", ng_os_info.freq/1000000);
  pr(b, "%-16s: %s\n", "Timezone Name", ng_os_info.tz+1);
  pr(b, "%-16s: %"PRIu64"\n", "Timezone Offset", ng_os_info.tz_offset);

  if (!ng_os_get_meminfo(&m))
  {
    pr(b, "%-28s: %"PRIu64"\n", "Total Physical Memory", m.ullTotalPhys / (1024 * 1024));
    pr(b, "%-28s: %"PRIu64"\n", "Available Physical Memory", m.ullAvailPhys / (1024 * 1024));
    pr(b, "%-28s: %"PRIu64"\n", "Total Virtual Memory", m.ullTotalVirtual / (1024 * 1024));
    pr(b, "%-28s: %"PRIu64"\n", "Available Virtual Memory", m.ullAvailVirtual / (1024 * 1024));
  }
}
