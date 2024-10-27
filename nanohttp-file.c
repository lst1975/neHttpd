#include "nanohttp-config.h"
#include <stdio.h>
#include "nanohttp-file.h"

herror_t nanohttp_file_read(const char *file, 
  rwfile_f cb, void *arg)
{
  FILE *fptr;

  // Open a file in read mode
  fptr = fopen(file, "r");
  if(fptr == NULL) 
  { 
    // If the file does not exist
    return herror_new("nanohttp_file_read", FILE_ERROR_OPEN, 
      "Not able to open the file %s.", file);
  }
  else
  { // If the file exist
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
          fclose(fptr);
          return herror_new("nanohttp_file_read", FILE_ERROR_READ, 
            "Failed to read file %s.", file);
        }
      }

      r = cb(arg, buffer, n);
      if (r != NULL)
      {
        fclose(fptr);
        return r;
      }
    }
  } 

  // Close the file
  fclose(fptr);
  return NULL;
}

void *nanohttp_file_open(const char *file)
{
  // Open a file in read mode
  return fopen(file, "w+");
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

int nanohttp_file_write(void *file, 
  const char *buffer, int length)
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

