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
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>  // For PATH_MAX
#include <errno.h>

#include "nanohttp-common.h"
#include "nanohttp-defs.h"
#include "nanohttp-file.h"
#include "nanohttp-error.h"
#include "nanohttp-mem.h"
#include "nanohttp-logging.h"
#include "nanohttp-system.h"
#include "nanohttp-url.h"

static ng_block_s httpd_base_path = DECL_CONST_STR_NULL();

#ifdef WIN32
herror_t
hsocket_setexec(int sock, int err)
{
  return H_OK;
}

herror_t
nanohttp_dir_init(const char *pfile)
{
  int plen;
  
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

  const char *pwd = ng_get_pwd(d, PATH_MAX);
  if (pwd == NULL)  
  {
    http_free(d);
    log_error("hsocket_dir_init failed");
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_SYSTEM,
                      "Unable to execute getcwd");
  }
  
  log_debug("Current path: %s", pwd);
  char *p = strrchr(pwd, __PATH_DLIM);
  if (p == NULL)
  {
    http_free(d);
    log_error("bad parameter pfile: %s", pfile);
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_INVAL,
                      "bad parameter pfile: %s", pfile);
  }

  plen = p - pwd + 1;
  httpd_base_path.buf = d;
  httpd_base_path.len = plen;
  httpd_base_path.buf[httpd_base_path.len]='\0';

  ng_urlorpath_dlim_convert(d, plen, '\\', '/');
  
  log_info("[OK]");
  return H_OK;
}
#else
herror_t
hsocket_setexec(int sock, int err)
{
  // Set FD_CLOEXEC on the file descriptor
  int flags = fcntl(sock, F_GETFD);
  if (flags == -1 || fcntl(sock, F_SETFD, flags | FD_CLOEXEC) == -1) {
    return herror_new("hsocket_open", err,
                      "Socket error (%s)", os_strerror(ng_errno));
  }
  return H_OK;
}

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
  const char *pwd = ng_get_pwd(d, PATH_MAX);
  if (pwd == NULL)  
  {
    http_free(d);
    log_error("hsocket_dir_init failed");
    return herror_new("nanohttp_dir_init", HSERVER_ERROR_SYSTEM,
                      "Unable to execute getcwd");
  }

  log_debug("Current path: %s", pwd);
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
  
  log_info("[OK]: nanohttp_dir_init.");
  return H_OK;
}
#endif

void
nanohttp_dir_free(void)
{
  if (httpd_base_path.buf != NULL)
  {
    http_free(httpd_base_path.buf);
    ng_block_init(&httpd_base_path);
  }
  log_info("[OK]: nanohttp_dir_free.");
}

static char *
nanohttp_file_get_path(const char *file, int flen)
{
  ng_block_s url;
  ng_block_s *b = &httpd_base_path;
  char *path = http_malloc(PATH_MAX);
  if (path == NULL)
    return NULL;

  if (file[0] == __PATH_DLIM 
    ||(flen >= b->len 
        && (!ng_memcmp(b->buf, file, b->len))))
  {
    if (flen >= PATH_MAX)
    {
      http_free(path);
      return NULL;
    }
    ng_memcpy(path, file, flen+1);
    return path;
  }
  if (flen + b->len >= PATH_MAX)
  {
    http_free(path);
    return NULL;
  }

  ng_memcpy(path,b->buf,b->len);
  ng_memcpy(path+b->len,file,flen);
  path[b->len+flen]='\0';
  url.cptr = path;
  url.len  = b->len + flen;
  
  if (ng_urlorPath_normalize(&url) < 0)
  {
    http_free(path);
    return NULL;
  }
  
  log_debug("Get path: %pS", &url);

  return path;
}

static void *nanohttp_file_open(const char *file, 
  int len, const char *mode)
{
  char *fpath = nanohttp_file_get_path(file, len);
  // Open a file in write mode
  if (fpath != NULL)
  {
    void *fp = fopen(fpath, mode);
    http_free(fpath);
    return fp;
  }

  return NULL;
}

void *nanohttp_file_open_for_read(const char *file, int len)
{
  return nanohttp_file_open(file, len, "rb");
}

herror_t nanohttp_file_read_all(const char *file, 
  int len, rwfile_f cb, void *arg)
{
  FILE *fptr=nanohttp_file_open_for_read(file, len);
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

void *nanohttp_file_open_for_write(const char *file, int len)
{
  return nanohttp_file_open(file, len, "wb");
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

int nanohttp_file_delete(const char *file, int len)
{
  char *fpath = nanohttp_file_get_path(file, len);
  // Open a file in write mode
  if (fpath == NULL)
  {
    log_error("Failed to build file path.");
    return -1;
  }

  if (remove(fpath) != 0)
  {
    int err = errno;
    http_free(fpath);
    log_error("Failed to remove file %s (%d:%s).", 
      fpath, err, os_strerror(err));
    return -1;
  }

  http_free(fpath);
  return 0;
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

size_t nanohttp_file_size(const char *file, int len)
{
  int fd;
  size_t size = 0;
  struct stat fileStat;

  // Open a file in write mode
  char *fpath = nanohttp_file_get_path(file, len);
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

static int __nanohttp_dir_isok(const char *dir_path) 
{
  struct stat statbuf;
  if (stat(dir_path, &statbuf) == 0) 
  {
    if (S_ISDIR(statbuf.st_mode)) {
      return NDIR_OK;
    }
    else 
    {
      return NDIR_NOT_DIR;
    }
  }
  else 
  {
    return NDIR_NOT_EXIST;
  }

  return 0;
}

int nanohttp_dir_isok(const ng_block_s *path) 
{
  char *fpath = nanohttp_file_get_path(path->cptr, path->len);
  if (fpath == NULL)
  {
    log_error("Failed to build file path.");
    return -1;
  }

  switch (__nanohttp_dir_isok(fpath))
  {
    case NDIR_OK:
      http_free(fpath);
      return 0;
    default:
    case NDIR_NOT_EXIST:
    case NDIR_NOT_DIR:
      http_free(fpath);
      return -1;
  }
}

int nanohttp_dir_create(const ng_block_s *path) 
{
  char *fpath = nanohttp_file_get_path(path->cptr, path->len);
  // Open a file in write mode
  if (fpath == NULL)
  {
    log_error("Failed to build file path.");
    return -1;
  }

  switch (__nanohttp_dir_isok(fpath))
  {
    case NDIR_NOT_EXIST:
      break;
    case NDIR_OK:
      http_free(fpath);
      return 0;
    default:
    case NDIR_NOT_DIR:
      http_free(fpath);
      return -1;
  }

  switch (mkdir(fpath, 0755)) // 0755 sets the permissions
  {
    case 0:
      http_free(fpath);
      return 0;
    default:
      http_free(fpath);
      log_error("Failed to create file path: %pS.", path);
      return -1;
  }
}

