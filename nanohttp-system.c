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
#include "nanohttp-system.h"
#include "nanohttp-error.h"
#include "nanohttp-logging.h"
#include "nanohttp-time.h"
#include "nanohttp-system.h"
#include "nanohttp-mem.h"

#define NG_SOCKET_MAXCONN 0xfffffff

ng_os_info_s ng_os_info = {
  .ngx_max_sockets = NG_SOCKET_MAXCONN,
  .ngx_ncpu = 0,
};

#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#include <windows.h>
#include <time.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <stdio.h>

#pragma comment(lib, "pdh.lib")

static ng_result_t ng_get_freq_pdh(void *log, double *freq) 
{
  PDH_HQUERY query;
  PDH_HCOUNTER counter;
  PDH_FMT_COUNTERVALUE counterValue;
  DWORD status;

  // Initialize PDH
  status = PdhOpenQuery(NULL, 0, &query);
  if (status != ERROR_SUCCESS) 
  {
    ng_log_errno(NG_LOG_ERROR, log, GetLastError(),
      "PdhOpenQuery failed with status 0x%lx", status);
    return ng_ERR_ESYSTEM;
  }

  // Add counter for processor frequency
  status = PdhAddCounter(query, "\\Processor Information(_Total)\\Processor Frequency", 
    0, &counter);
  if (status != ERROR_SUCCESS) {
    ng_log_errno(NG_LOG_ERROR, log, GetLastError(),
      "PdhAddCounter failed with status 0x%lx", status);
    return ng_ERR_ESYSTEM;
  }

  // Collect data
  status = PdhCollectQueryData(query);
  if (status != ERROR_SUCCESS) {
    ng_log_errno(NG_LOG_ERROR, log, GetLastError(),
      "PdhCollectQueryData failed with status 0x%lx", status);
    return ng_ERR_ESYSTEM;
  }

  // Get counter value
  status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue);
  if (status != ERROR_SUCCESS) {
    ng_log_errno(NG_LOG_ERROR, log, GetLastError(),
      "PdhGetFormattedCounterValue failed with status 0x%lx", status);
    return ng_ERR_ESYSTEM;
  }

  ng_log_errno(NG_LOG_INFO, log, 0, "CPU Frequency: %f MHz", 
    counterValue.doubleValue / 1000.0); 

  // Cleanup
  PdhCloseQuery(query);
  *freq = counterValue.doubleValue / 1000.0;
  return ng_ERR_NONE;
}

static ng_result_t ng_get_freq_query(void *log, double *freq) 
{
  LARGE_INTEGER frequency;
  if (QueryPerformanceFrequency(&frequency)) {
    ng_log_errno(NG_LOG_INFO, log, 0, 
      "CPU Frequency: %lld Hz", frequency.QuadPart / 1000.0);
    *freq = frequency.QuadPart / 1000.0;
    return ng_ERR_NONE;
  } else {
    ng_log_errno(NG_LOG_INFO, log, GetLastError(), 
      "Failed to query performance frequency.");
    return ng_ERR_ESYSTEM;
  }
}

static ng_result_t __ng_get_freq(void *log, double *freq) 
{
  if (ng_get_freq_query(log,freq) != ng_ERR_NONE)
    return ng_get_freq_pdh(log,freq);
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

/* Set up a workaround for the following problem:
 *   FARPROC addr = GetProcAddress(...);
 *   MY_FUNC func = (MY_FUNC) addr;          <-- GCC 8 warning/error.
 *   MY_FUNC func = (MY_FUNC) (void*) addr;  <-- MSVC  warning/error.
 * To compile cleanly with either compiler, do casts with this "bridge" type:
 *   MY_FUNC func = (MY_FUNC) (nt__fn_ptr_cast_t) addr; */
#ifdef __GNUC__
typedef void* nt__fn_ptr_cast_t;
#else
typedef FARPROC nt__fn_ptr_cast_t;
#endif

#define X(return_type, attributes, name, parameters) \
  WEPOLL_INTERNAL return_type(attributes* name) parameters = NULL;
NT_NTDLL_IMPORT_LIST(X)
#undef X

WEPOLL_INTERNAL ng_result_t nt_global_init(void) {
  HMODULE ntdll;
  FARPROC fn_ptr;

  ntdll = GetModuleHandleW(L"ntdll.dll");
  if (ntdll == NULL)
    return ng_ERR_ESYSTEM;

#define X(return_type, attributes, name, parameters) \
  fn_ptr = GetProcAddress(ntdll, #name);             \
  if (fn_ptr == NULL)                                \
    return ng_ERR_ESYSTEM;                                       \
  name = (return_type(attributes*) parameters)(nt__fn_ptr_cast_t) fn_ptr;
  NT_NTDLL_IMPORT_LIST(X)
#undef X

  return ng_ERR_NONE;
}

static ng_result_t 
__os_socket_init_once(void *log)
{
  WSADATA wsaData;

  /* init Winsock */
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) 
  {
    if (nt_global_init() != ng_ERR_NONE)
    {
      ng_log_errno(NG_LOG_FATAL, log, WSAGetLastError(),
                    "WSAStartup() failed");
      return ng_ERR_ESYSTEM;
    }
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
    ng_log_errno(NG_LOG_FATAL, log, WSAGetLastError(), 
    "Could not find a usable version of Winsock.dll");
    goto clean1;
  }
    
  ng_log_errno(NG_LOG_INFO, log, 0,
    "The Winsock 2.2 dll was found okay");  
  
  return ng_ERR_NONE;
  
clean1:
  WSACleanup();
clean0:
  return ng_ERR_ESYSTEM;
}

static bool init__done = false;
static INIT_ONCE init__once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK init__once_callback(INIT_ONCE* once,
                                         void* parameter,
                                         void** context) {
  NG_UNUSED(once);
  NG_UNUSED(context);

  /* N.b. that initialization order matters here. */
  if (__os_socket_init_once(parameter) != ng_ERR_NONE)
    return FALSE;

  init__done = true;
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
static ng_uint_t  ngx_inherited_nonblocking = 1;
static ng_uint_t  ngx_tcp_nodelay_and_tcp_nopush;

typedef struct {
  WORD  wServicePackMinor;
  WORD  wSuiteMask;
  BYTE  wProductType;
} ngx_osviex_stub_t;

static u_int               osviex;
static OSVERSIONINFOEX     osvi;

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

#if (NGX_LOAD_WSAPOLL)
ngx_wsapoll_pt             WSAPoll;
ngx_uint_t                 ngx_have_wsapoll;
#endif

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
      ng_log_errno(NG_LOG_FATAL, log, WSAGetLastError(),
                    "GetVersionEx() failed");
      return ng_ERR_ESYSTEM;
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
  
  ng_log_errno(NG_LOG_INFO, log, 0, NG_VER_BUILD);
#ifdef NG_COMPILER
  ng_log_errno(NG_LOG_INFO, log, 0, "built by " NG_COMPILER);
#endif

  if (osviex) 
  {
    /*
     * the MSVC 6.0 SP2 defines wSuiteMask and wProductType
     * as WORD wReserved[2]
     */
    osviex_stub = (ngx_osviex_stub_t *) &osvi.wServicePackMinor;

    ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
      "OS: Windows %ui build:%ud, \"%s\", suite:%Xd, type:%ud",
      ngx_win32_version, osvi.dwBuildNumber, osvi.szCSDVersion,
      osviex_stub->wSuiteMask, osviex_stub->wProductType);
  } 
  else 
  {
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
    {
      /* Win9x build */

      ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
          "OS: Windows %ui build:%ud.%ud.%ud, \"%s\"",
          ngx_win32_version,
          osvi.dwBuildNumber >> 24,
          (osvi.dwBuildNumber >> 16) & 0xff,
          osvi.dwBuildNumber & 0xffff,
          osvi.szCSDVersion);

    } 
    else 
    {
      /*
       * VER_PLATFORM_WIN32_NT
       *
       * we do not currently support VER_PLATFORM_WIN32_CE
       * and we do not support VER_PLATFORM_WIN32s at all
       */

      ng_snprintf(ng_os_info.ng_os_version, sizeof(ng_os_info.ng_os_version),
          "OS: Windows %ui build:%ud, \"%s\"",
          ngx_win32_version, osvi.dwBuildNumber,
          osvi.szCSDVersion);
    }
  }
  
  ng_log_errno(NG_LOG_INFO, log, 0, "%s", ng_os_info.ng_os_version);
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
  WSADATA       wsd;
  ng_result_t   err;
  ng_time_t    *tp;
  ng_uint_t     n;
  SYSTEM_INFO   si;
  ngx_pid_t     pid;

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
    return ng_ERR_NONE;
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
    ng_log_errno(NG_LOG_FATAL, log, WSAGetLastError(),
                  "WSASocketW failed");
    goto clean0;
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
              &ax_guid, sizeof(GUID),
              &ngx_acceptex, sizeof(LPFN_ACCEPTEX), 
              &bytes, NULL, NULL))
  {
    ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_ACCEPTEX) failed");
  }

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &as_guid, sizeof(GUID),
              &ngx_getacceptexsockaddrs, sizeof(LPFN_GETACCEPTEXSOCKADDRS),
              &bytes, NULL, NULL))
  {
      ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_GETACCEPTEXSOCKADDRS) failed");
  }

    if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &tf_guid, sizeof(GUID),
              &ngx_transmitfile, sizeof(LPFN_TRANSMITFILE), 
              &bytes, NULL, NULL))
    {
        ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_TRANSMITFILE) failed");
    }

    if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &tp_guid, sizeof(GUID),
              &ngx_transmitpackets, sizeof(LPFN_TRANSMITPACKETS), 
              &bytes, NULL, NULL))
    {
        ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_TRANSMITPACKETS) failed");
    }

    if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &cx_guid, sizeof(GUID),
              &ngx_connectex, sizeof(LPFN_CONNECTEX), 
              &bytes, NULL, NULL))
    {
        ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_CONNECTEX) failed");
    }

    if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, 
              &dx_guid, sizeof(GUID),
              &ngx_disconnectex, sizeof(LPFN_DISCONNECTEX), 
              &bytes, NULL, NULL))
    {
        ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                  "WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, "
                           "WSAID_DISCONNECTEX) failed");
    }

    if (closesocket(s) == SOCKET_ERROR) 
    {
      ng_log_errno(NG_LOG_WARN, log, WSAGetLastError(),
                    "closesocket failed");
    }

#if (NG_LOAD_WSAPOLL)
    do {
      HMODULE  hmod;

      hmod = GetModuleHandle("ws2_32.dll");
      if (hmod == NULL) 
      {
          ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
            "GetModuleHandle(\"ws2_32.dll\") failed");
          goto nopoll;
      }

      WSAPoll = (ngx_wsapoll_pt) (void *)GetProcAddress(hmod, "WSAPoll");
      if (WSAPoll == NULL) 
      {
        ng_log_errno(NG_LOG_INFO, log, WSAGetLastError(),
                      "GetProcAddress(\"WSAPoll\") failed");
        break;
      }

      ngx_have_wsapoll = 1;
    }
    while(0);
#endif

    return ng_ERR_NONE;
    
clean1:
  if (closesocket(s) == SOCKET_ERROR) 
  {
    ng_log_errno(NG_LOG_WARN, log, WSAGetLastError(),
                  "closesocket failed");
  }
clean0:
  return ng_ERR_ESYSTEM;
}

static ng_result_t
__os_sys_deinit(void *log)
{
  NG_UNUSED(log);
}

static ng_result_t __ng_os_init(void *log)
{
  ng_result_t r;
  ng_tmv_s tp;
  long seconds;

  r = __os_sys_init(log);
  if (r != ng_ERR_NONE)
    goto clean0;

  r = __os_socket_init(log);
  if (r != ng_ERR_NONE)
    goto clean1;

  if (_get_timezone(&seconds))
  {
    ng_log_errno(NG_LOG_WARN, log, errno,
                  "_get_timezone() failed");
    goto clean2;
  }

  ng_os_info.ngx_allocation_granularity = 0;
  ng_os_info.tz_offset = seconds;
  ng_gettimeofday(&tp);
  srand((ng_os_info.ngx_pid << 16) ^ (unsigned) tp.tv_sec ^ tp.tv_usec);
  
  return ng_ERR_NONE;
  
clean2:
  __os_socket_deinit(log);
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

static ng_result_t __ng_get_freq(void *log, double *freq) 
{
  FILE *file;
  char buffer[BUFFER_SIZE];
  char *line;
  char *search = "cpu MHz";
  double cpu_freq = 0.0;
  
  file = fopen("/proc/cpuinfo", "r");
  if (file == NULL) {
    ng_log_errno(NG_LOG_ERROR, log, errno, 
      "fopen failed.");
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
    ng_log_errno(NG_LOG_INFO, log, 0, 
      "CPU Frequency: %.2f MHz", cpu_freq);
    *freq = cpu_freq * 1000000.0;
    return ng_ERR_NONE;
  }
  else 
  {
    ng_log_errno(NG_LOG_ERROR, log, errno, 
      "CPU Frequency not found.");
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
    ng_log_errno(NG_LOG_WARN, log, errno, "uname() failed");
    return ng_ERR_ESYSTEM;
  }

  ng_snprintf(ng_os_info.ng_os_version, 
    sizeof(ng_os_info.ng_os_version),
    "%s %s", u.sysname, u.release);
#endif

  return ng_ERR_NONE;
}

static void
__os_specific_status(void *log)
{
#if defined(RTE_ENV_LINUX) || defined(__linux__)
  ng_log_errno(NG_LOG_INFO, log, 0, "OS: %s",
                ng_os_info.ng_os_version);
#endif
}

static void
__os_sys_status(void *log)
{
  ng_log_errno(NG_LOG_INFO, log, 0, NG_VER_BUILD);

#ifdef NG_COMPILER
  ng_log_errno(NG_LOG_INFO, log, 0, "built by " NG_COMPILER);
#endif

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
    ng_log_errno(NG_LOG_WARN, log, errno,
      "getrlimit(RLIMIT_NOFILE) failed");
    goto clean0;
  }

  ng_os_info.ngx_max_sockets = (ng_int_t) rlmt.rlim_cur;

  ng_log_errno(NG_LOG_INFO, log, 0,
                "getrlimit(RLIMIT_NOFILE): %u:%u",
                rlmt.rlim_cur, rlmt.rlim_max);

  return ng_ERR_NONE;
  
clean0:
  return r;
}

static long
__os_get_tzoffset(void)
{
#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#include <time.h>
#include <windows.h>
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
#else
  time_t current_time;
  struct tm *local_time;

  // Get the current time
  time(&current_time);
  
  // Convert to local time
  local_time = localtime(&current_time);
  
  // Get the timezone offset in seconds
  long timezone_offset = local_time->tm_gmtoff;

  // Convert to hours and minutes
  int hours = timezone_offset / 3600;
  int minutes = (timezone_offset % 3600) / 60;

  log_debug("Local timezone offset from GMT: %02d:%02d\n", hours, minutes);
  return timezone_offset;
#endif
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
  
  return ng_ERR_NONE;
  
clean1:
  __ng_os_deinit(NULL);
clean0:
  return r;
}

void ng_os_deinit(void)
{
  __ng_os_deinit(NULL);
}

