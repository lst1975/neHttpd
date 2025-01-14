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

#include <signal.h>

#include "nanohttp-config.h"

#ifdef WIN32
#include <windows.h>
#else
#endif

#include "nanohttp-logging.h"
#include "nanohttp-system.h"

extern void signal_handler_segfault(int sig);
extern void nanohttpd_stop_running(void);

#ifdef WIN32
typedef DWORD SIGNAL_T;
static DWORD _httpd_terminate_signal = CTRL_C_EVENT;
static BOOL WINAPI
_httpd_term(DWORD sig)
{
  log_debug("Got signal %d", sig);

  if (sig == SIGSEGV)
  {
    signal_handler_segfault(sig);
    return TRUE;
  }
  /* log_debug ("Got signal %d", sig); */
  if (sig == _httpd_terminate_signal
    || sig == SIGTERM || sig == SIGABRT)
    nanohttpd_stop_running();

  return TRUE;
}
void
httpd_pthread_sigmask(void)
{
}
#else
typedef int SIGNAL_T;
static int _httpd_terminate_signal = SIGINT;
static sigset_t thrsigset;
static void
_httpd_term(int sig)
{
  log_debug("Got signal %d", sig);

  if (sig == SIGSEGV)
  {
    signal_handler_segfault(sig);
    return;
  }

  if (sig == _httpd_terminate_signal
    || sig == SIGTERM || sig == SIGABRT)
    nanohttpd_stop_running();

  return;
}
void
httpd_pthread_sigmask(void)
{
  sigemptyset(&thrsigset);
  sigaddset(&thrsigset, SIGALRM);
  pthread_sigmask(SIG_BLOCK, &thrsigset, NULL);
}
#endif

/*
 * -----------------------------------------------------
 * FUNCTION: _httpd_register_signal_handler
 * -----------------------------------------------------
 */
static void
_httpd_register_signal_handler(SIGNAL_T sig)
{
  log_verbose("registering termination signal handler (SIGNAL:%d)",
               sig);
#ifdef WIN32
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE) _httpd_term, TRUE) == FALSE)
  {
    log_error("Unable to install console event handler!");
  }

#else
  signal(sig, _httpd_term);
#endif

  return;
}

void
httpd_register_signal_handler(void)
{
  _httpd_register_signal_handler(_httpd_terminate_signal);
  _httpd_register_signal_handler(SIGTERM);
  _httpd_register_signal_handler(SIGABRT);
  _httpd_register_signal_handler(SIGSEGV);
}

void
httpd_set_terminate_signal(int sig)
{
  _httpd_terminate_signal = sig;
}

int
httpd_get_terminate_signal(void)
{
  return _httpd_terminate_signal;
}

#ifdef WIN32
void signal_handler_segfault(int sig) {
  NG_UNUSED(sig);
}
uint64_t
os__rte_rdtsc_syscall(void)
{
  LARGE_INTEGER frequency, start, end;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&start);

  // Code to measure goes here

  QueryPerformanceCounter(&end);
  double elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
  return elapsedTime*1000000000LL;
}
#else
#include <execinfo.h>
// Maximum number of stack frames to capture
#define MAX_FRAMES 64
void signal_handler_segfault(int sig) {
  void *buffer[MAX_FRAMES];
  char **symbols;
  int num_frames;
  
  // Get the stack frames
  num_frames = backtrace(buffer, MAX_FRAMES);
  
  // Get the symbols (function names, file names, line numbers)
  symbols = backtrace_symbols(buffer, num_frames);
  
  // Print the stack trace
  ng_fprintf(stderr, "Error: signal %d:\n", sig);
  for (int i = 0; i < num_frames; i++) {
      ng_fprintf(stderr, "%s\n", symbols[i]);
  }
  
  // Free the allocated memory for symbols
  free(symbols);
  
  // Exit the program
  exit(1);
}
/**
 * This call is easily portable to any architecture, however,
 * it may require a system call and imprecise for some tasks.
 */
uint64_t
os__rte_rdtsc_syscall(void)
{
  struct timespec val;
  uint64_t v;

  while (clock_gettime(CLOCK_MONOTONIC_RAW, &val) != 0)
  /* no body */;

  v  = (uint64_t) val.tv_sec * 1000000000LL;
  v += (uint64_t) val.tv_nsec;
  return v;
}
#endif

