/******************************************************************
 *  $Id: nanohttp-url.h,v 1.4 2007/11/03 22:40:15 m0gg Exp $
 * 
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2004  Ferhat Ayaz
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 * 
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef __nanohttp_file_h
#define __nanohttp_file_h

#include "nanohttp-error.h"

typedef herror_t (*rwfile_f)(void *arg, const char *buf, size_t length); 

size_t nanohttp_file_write(void *file, const char *buffer, size_t length);
herror_t nanohttp_file_read(void *file, rwfile_f cb, void *arg);
void nanohttp_file_close(void *file);
void *nanohttp_file_open_for_write(const char *file);
void *nanohttp_file_open_for_read(const char *file);
herror_t nanohttp_file_read_all(const char *file, 
  rwfile_f cb, void *arg);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
