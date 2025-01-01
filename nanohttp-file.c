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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>  // For PATH_MAX

#include "nanohttp-config.h"
#include "nanohttp-common.h"
#include "nanohttp-defs.h"
#include "nanohttp-file.h"
#include "nanohttp-error.h"
#include "nanohttp-mem.h"
#include "nanohttp-logging.h"

static httpd_buf_t httpd_base_path = __HTTPD_BUF_INIT_DECL();

herror_t
nanohttp_dir_init(const char *pfile)
{
  if (pfile == NULL)
  {
    log_error("pfile is NULL");
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_INVAL,
                      "Parameter pfile is NULL");
  }
  
  char *d = http_malloc(PATH_MAX);
  if (d == NULL)
  {
    log_error("hsocket_dir_init failed");
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_MALLOC,
                      "Unable to malloc tempary buffer");
  }
  const char *pwd = getcwd(d, PATH_MAX);
  if (pwd == NULL)  
  {
    http_free(d);
    log_error("hsocket_dir_init failed");
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_SYSTEM,
                      "Unable to execute getcwd");
  }

  const char *p = strrchr(pfile, __PATH_DLIM);
  if (p == NULL)
  {
    http_free(d);
    log_error("bad parameter pfile: %s", pfile);
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_INVAL,
                      "bad parameter pfile: %s", pfile);
  }

  size_t plen = strlen(pwd);
  size_t flen = p + 1 - pfile;
  if (pfile[0] == __PATH_DLIM 
    ||(flen >= plen 
        && (!memcmp(pwd, pfile, plen))))
  {
    memcpy(d, pwd, plen+1);
    flen = 0;
    d[plen++]=__PATH_DLIM;
  }
  else
  {
    if (flen + plen + 1 >= PATH_MAX)
    {
      http_free(d);
      log_error("dir length is too large: %u", flen + plen + 1);
      return herror_new("nanohttp_dir_init", HSERVER_ERROR_INVAL,
                        "dir length is too large: %u", flen + plen + 1);
    }

    d[plen++]=__PATH_DLIM;
    if (flen==2 && pfile[0]=='.' && pfile[1]==__PATH_DLIM)
      flen = 0;
    if (flen) memcpy(d+plen,pfile,flen);
  }
  
  httpd_base_path.buf = d;
  httpd_base_path.len = plen + flen;
  httpd_base_path.buf[httpd_base_path.len]='\0';
  
  log_info("[OK]");
  return H_OK;
}

void
nanohttp_dir_free(void)
{
  if (httpd_base_path.buf != NULL)
  {
    http_free(httpd_base_path.buf);
    BUF_SIZE_INIT(&httpd_base_path, NULL, 0);
  }
  log_info("[OK]");
}

static void normalizePath(char *inputPath) {
  char *token;
  char *pathCopy = http_strdup(inputPath);
  char *stack[100]; // Stack to hold path components
  int top = -1;
  char *outputPath = inputPath;

  token = strtok(pathCopy, __PATH_DLIM_S);
  while (token != NULL) {
    if (strcmp(token, "..") == 0) {
      if (top > -1) {
        top--; // Pop the last valid directory
      }
    } else if (strcmp(token, ".") != 0 && strlen(token) > 0) {
      stack[++top] = token; // Push valid directory onto stack
    }
    token = strtok(NULL, __PATH_DLIM_S);
  }

  // Construct the normalized path
  outputPath[0] = '\0'; // Initialize outputPath
  for (int i = 0; i <= top; i++) {
    strcat(outputPath, __PATH_DLIM_S);
    strcat(outputPath, stack[i]);
  }

  http_free(pathCopy);
}

static char *nanohttp_file_get_path(const char *file)
{
  char *path = http_malloc(PATH_MAX);
  if (path == NULL)
    return NULL;

  size_t flen = strlen(file);
  if (file[0] == __PATH_DLIM 
    ||(flen >= httpd_base_path.len 
        && (!memcmp(httpd_base_path.buf, file, httpd_base_path.len))))
  {
    if (flen >= PATH_MAX)
    {
      http_free(path);
      return NULL;
    }
    memcpy(path, file, flen+1);
    return path;
  }
  if (flen + httpd_base_path.len >= PATH_MAX)
  {
    http_free(path);
    return NULL;
  }

  memcpy(path,httpd_base_path.buf,httpd_base_path.len);
  memcpy(path+httpd_base_path.len,file,flen);
  path[httpd_base_path.len+flen]='\0';
  normalizePath(path);
  log_debug("Get path: %s", path);
  return path;
}

static void *nanohttp_file_open(const char *file, const char *mode)
{
  char *fpath = nanohttp_file_get_path(file);
  // Open a file in write mode
  if (fpath != NULL)
  {
    void *fp = fopen(fpath, mode);
    http_free(fpath);
    return fp;
  }

  return NULL;
}

void *nanohttp_file_open_for_read(const char *file)
{
  return nanohttp_file_open(file, "rb");
}

herror_t nanohttp_file_read_all(const char *file, 
  rwfile_f cb, void *arg)
{
  FILE *fptr=nanohttp_file_open_for_read(file);
  if (fptr == NULL)
  {
    return herror_new("nanohttp_file_read", FILE_ERROR_OPEN, 
      "Failed to open file %s.", file);
  }

  // If the file exist
  // Store the content of the file
  char buffer[_nanoConfig_HTTPD_FILE_BLOCK];
  int n;
  
  while (!feof(fptr))
  {
    herror_t r;
    n = fread(buffer, 1, sizeof(buffer), fptr);
    if (n != sizeof(buffer))
    {
      if (!feof(fptr))
      {
        nanohttp_file_close(fptr);
        return herror_new("nanohttp_file_read", FILE_ERROR_READ, 
          "Failed to read file %s.", file);
      }
    }

    r = cb(arg, buffer, n);
    if (r != NULL)
    {
      nanohttp_file_close(fptr);
      return r;
    }
  }

  nanohttp_file_close(fptr);
  return NULL;
}

herror_t nanohttp_file_read(void *file, 
  rwfile_f cb, void *arg)
{
  FILE *fptr=(FILE *)file;

  // If the file exist
  // Store the content of the file
  char buffer[_nanoConfig_HTTPD_FILE_BLOCK];
  int n;
  
  while (!feof(fptr))
  {
    herror_t r;
    n = fread(buffer, 1, sizeof(buffer), fptr);
    if (n != sizeof(buffer))
    {
      if (!feof(fptr))
      {
        return herror_new("nanohttp_file_read", FILE_ERROR_READ, 
          "Failed to read file.");
      }
    }

    r = cb(arg, buffer, n);
    if (r != NULL)
    {
      return r;
    }
  }

  return NULL;
}

void *nanohttp_file_open_for_write(const char *file)
{
  return nanohttp_file_open(file, "wb+");
}

void nanohttp_file_close(void *file)
{
  FILE *fptr = (FILE *)file;

  // Open a file in read mode
  if(fptr != NULL) 
  { 
    fclose(fptr);
  }
}

size_t nanohttp_file_write(void *file, 
  const char *buffer, size_t length)
{
  FILE *fptr = (FILE *)file;

  if (fptr == NULL) 
  { 
    return -1;
  }
  else
  { // If the file exist
    // Store the content of the file
    // if (-1 == fseek(FILE *stream, SEEK_SET, SEEK_END))
  	//   return -1;
    int n = fwrite(buffer, 1, length, fptr);
    if (n != length)
    {
      return -1;
    }
    return length;
  } 
}

size_t nanohttp_file_size(const char *file)
{
  int fd;
  size_t size = 0;
  struct stat fileStat;

  // Open a file in write mode
  char *fpath = nanohttp_file_get_path(file);
  if (fpath == NULL)
    return 0;
  
  fd = open(fpath, O_RDONLY);
  if (fd < 0) {
    log_debug("Failed to open file: %s", fpath);
    goto error0;
  }

  if (fstat(fd, &fileStat) < 0) {
    log_debug("Failed to fstat file: %s", fpath);
    goto error1;
  }

  size = fileStat.st_size;
  
error1:
  close(fd);
error0:
  http_free(fpath);
  return size;
}
