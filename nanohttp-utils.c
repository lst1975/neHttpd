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

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <syslog.h>
#include <fcntl.h>

#include "nanohttp-logging.h"
#include "nanohttp-time.h"

void http_daemonize(int nochdir, int noclose) {
  pid_t pid;
  
  // Fork off the parent process
  pid = fork();
  
  // If the fork failed, exit
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  
  // If we got a good PID, exit the parent process
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  
  // Change the file mode mask
  umask(0);
  
  // Open any logs here
  openlog("httpd", LOG_PID, LOG_DAEMON);
  
  // Create a new SID for the child process
  if (setsid() < 0) {
    // Log the failure
    syslog(LOG_ERR, "Could not create new SID for child process");
    exit(EXIT_FAILURE);
  }
  
  // Change the current working directory
  if (!nochdir){
    if ((chdir("/")) < 0) {
      // Log the failure
      syslog(LOG_ERR, "Could not change working directory to /");
      exit(EXIT_FAILURE);
    }
  }
  
  if (!noclose){
    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Optionally, redirect standard file descriptors to /dev/null
    open("/dev/null", O_RDONLY); // stdin
    open("/dev/null", O_RDWR);   // stdout
    open("/dev/null", O_RDWR);   // stderr
  }
  
  // Daemon-specific initialization goes here
  syslog(LOG_INFO, "Daemon started successfully");
}

int ng_get_tzname(char *tz, int size) 
{
  time_t now = time(NULL);
  struct tm *local = localtime(&now);
  return ng_snprintf(tz, size, "%s", local->tm_zone);
}

void
__ng_gettimeofday(void *__tp)
{
  ng_tmv_s *tp = (ng_tmv_s *)__tp;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  tp->tv_sec = tv.tv_sec;
  tp->tv_usec = tv.tv_usec;
}

void ng_print_cpufreq(const char *n, double hz) 
{
  if (hz > 999.0)
    log_info("%s: %.2f GHz.", n, hz / 1000.0); 
  else
    log_info("%s: %.2f MHz.", n, hz); 
}

int ng_local_strncasecmp(const char *s1, const char *s2, ng_size_t n) 
{
  return strncasecmp(s1,s2,n);
}

