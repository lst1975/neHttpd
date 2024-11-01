#include "nanohttp-config.h"
#include <stdio.h>
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
        return herror_new("nanohttp_file_read", FILE_ERROR_READ, 
          "Failed to read file %s.", file);
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

