/******************************************************************
 *  $Id: nanohttp-error.h,v 1.5 2007/11/03 22:40:11 m0gg Exp $
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
#ifndef __nanohttp_error_h
#define __nanohttp_error_h

/** @file nanohttp-error.h Error handling
 *
 * nanoHTTP error handling 
 *
 * Almost all function will return a "herror_t" object. If the function returns
 * with success this object is H_OK. Another herror_t object will be returned
 * otherwise. Following functions can be used with a returned herror_t object: 
 *
 * - herror_code() - Returns the error code 
 * - herror_func() - Returns the function name, where the error occured 
 * - herror_message() - Returns the human readable error message 
 * - herror_release() - Frees the herror_t object. 
 *
 * Example:
 *
 * @code
 * herror_t err;
 *
 * if ((err = http_client_invoke("http://somewhere")) != H_OK)
 * {
 *   printf("Message: %s\n", herror_message(err));
 *   printf("Error code: %d\n", herror_code(err));
 *   printf("In function: %s\n", herror_func(err));
 *   herror_release(err);
 * }
 * @endcode
 *
 * Note that you MUST call herror_release() to free the resources.
 *
 */

/** @defgroup NANOHTTP_ERRORS nanoHTTP errors
 * @ingroup NANOHTTP
 */
/**@{*/

/** @defgroup NANOHTTP_GENERAL_ERROS General errors
 */ 
/**@{*/
#define H_OK				0 /**< Success flag */

#define GENERAL_ERROR			1400
#define GENERAL_INVALID_PARAM		(GENERAL_ERROR + 1)
#define GENERAL_HEADER_PARSE_ERROR	(GENERAL_ERROR + 2)
/**@}*/

/** @defgroup NANOHTTP_THREAD_ERRORS Thread errors
 */
/**@{*/
#define THREAD_ERROR			1500
#define THREAD_BEGIN_ERROR		(THREAD_ERROR)
/**@}*/

/** @defgroup NANOHTTP_FILE_ERRORS File errors
 */
/**@{*/
#define FILE_ERROR			8000
#define FILE_ERROR_OPEN			(FILE_ERROR + 1)
#define FILE_ERROR_READ			(FILE_ERROR + 2)
#define FILE_ERROR_WRITE		(FILE_ERROR + 3)
/**@}*/

/**@}*/

/**
 *
 * @todo Remove me.
 *
 * Dummy deklaration to hide the implementation.
 *
 */
typedef void *herror_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Creates a new error structure.
 *
 * @see printf
 *
 */
extern herror_t herror_new(const char *func, int errcode, const char *format, ...);

/**
 *
 * Returns the code of the error.
 *
 */
extern int herror_code(herror_t err);

/**
 *
 * @return The name of the function, where the error was produced.
 *
 */
extern const char *herror_func(herror_t err);

/**
 *
 * Returns the error message.
 *
 */
extern const char *herror_message(herror_t err);

/**
 *
 * Frees the error structure.
 *
 */
extern void herror_release(herror_t err);

#ifdef __cplusplus
}
#endif

#endif
