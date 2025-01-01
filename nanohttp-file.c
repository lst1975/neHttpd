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
#include "nanohttp-config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "nanohttp-file.h"

void *nanohttp_file_open_for_read(const char *file)
{
  // Open a file in read mode
  return fopen(file, "rb");
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
  // Open a file in read mode
  return fopen(file, "wb+");
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
  struct stat fileStat;

  fd = open(file, O_RDONLY);
  if (fd < 0) {
    return 0;
  }

  if (fstat(fd, &fileStat) < 0) {
    close(fd);
    return 0;
  }

  close(fd);
  return fileStat.st_size;
}
