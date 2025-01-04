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
/*
 * coreJSON v3.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file core_json.c
 * @brief The source file that implements the user-facing functions in core_json.h.
 */

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>

#include "nanohttp-json.h"
#include "nanohttp-user.h"

/** @cond DO_NOT_DOCUMENT */

/* A compromise to satisfy both MISRA and CBMC */
typedef union
{
  char c;
  uint8_t u;
} char_;

#if ( CHAR_MIN == 0 )
  #define isascii_( x )  ( ( x ) <= '\x7F' )
#else
  #define isascii_( x )  ( ( x ) >= '\0' )
#endif
#define iscntrl_( x )    ( isascii_( x ) && ( ( x ) < ' ' ) )
#define isdigit_( x )    ( ( ( x ) >= '0' ) && ( ( x ) <= '9' ) )
/* NB. This is whitespace as defined by the JSON standard (ECMA-404). */
#define isspace_( x )              \
  ( ( ( x ) == ' ' ) || ( ( x ) == '\t' ) || \
    ( ( x ) == '\n' ) || ( ( x ) == '\r' ) )

#define isOpenBracket_( x )       ( ( ( x ) == '{' ) || ( ( x ) == '[' ) )
#define isCloseBracket_( x )      ( ( ( x ) == '}' ) || ( ( x ) == ']' ) )
#define isCurlyPair_( x, y )      ( ( ( x ) == '{' ) && ( ( y ) == '}' ) )
#define isSquarePair_( x, y )     ( ( ( x ) == '[' ) && ( ( y ) == ']' ) )
#define isMatchingBracket_( x, y )  ( isCurlyPair_( x, y ) || isSquarePair_( x, y ) )
#define isSquareOpen_( x )      ( ( x ) == '[' )
#define isSquareClose_( x )       ( ( x ) == ']' )

/**
 * @brief Advance buffer index beyond whitespace.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 */
static void skipSpace( const char * buf,
             size_t * start,
             size_t max )
{
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  for( i = *start; i < max; i++ )
  {
    if( !isspace_( buf[ i ] ) )
    {
      break;
    }
  }

  *start = i;
}

/**
 * @brief Count the leading 1s in a byte.
 *
 * The high-order 1 bits of the first byte in a UTF-8 encoding
 * indicate the number of additional bytes to follow.
 *
 * @return the count
 */
static size_t countHighBits( uint8_t c )
{
  uint8_t n = c;
  size_t i = 0;

  while( ( n & 0x80U ) != 0U )
  {
    i++;
    n = ( n & 0x7FU ) << 1U;
  }

  return i;
}

/**
 * @brief Is the value a legal Unicode code point and encoded with
 * the fewest bytes?
 *
 * The last Unicode code point is 0x10FFFF.
 *
 * Unicode 3.1 disallows UTF-8 interpretation of non-shortest form sequences.
 * 1 byte encodes 0 through 7 bits
 * 2 bytes encode 8 through 5+6 = 11 bits
 * 3 bytes encode 12 through 4+6+6 = 16 bits
 * 4 bytes encode 17 through 3+6+6+6 = 21 bits
 *
 * Unicode 3.2 disallows UTF-8 code point values in the surrogate range,
 * [U+D800 to U+DFFF].
 *
 * @note Disallow ASCII, as this is called only for multibyte sequences.
 */
static bool shortestUTF8( size_t length,
              uint32_t value )
{
  bool ret = false;
  uint32_t min = 0U, max = 0U;

  assert( ( length >= 2U ) && ( length <= 4U ) );

  switch( length )
  {
    case 2:
      min = ( uint32_t ) 1 << 7U;
      max = ( ( uint32_t ) 1 << 11U ) - 1U;
      break;

    case 3:
      min = ( uint32_t ) 1 << 11U;
      max = ( ( uint32_t ) 1 << 16U ) - 1U;
      break;

    default:
      min = ( uint32_t ) 1 << 16U;
      max = 0x10FFFFU;
      break;
  }

  if( ( value >= min ) && ( value <= max ) &&
    ( ( value < 0xD800U ) || ( value > 0xDFFFU ) ) )
  {
    ret = true;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a UTF-8 code point.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid code point was present;
 * false otherwise.
 *
 * 00-7F  Single-byte character
 * 80-BF  Trailing byte
 * C0-DF  Leading byte of two-byte character
 * E0-EF  Leading byte of three-byte character
 * F0-F7  Leading byte of four-byte character
 * F8-FB  Illegal (formerly leading byte of five-byte character)
 * FC-FD  Illegal (formerly leading byte of six-byte character)
 * FE-FF  Illegal
 *
 * The octet values C0, C1, and F5 to FF are illegal, since C0 and C1
 * would introduce a non-shortest sequence, and F5 or above would
 * introduce a value greater than the last code point, 0x10FFFF.
 */
static bool skipUTF8MultiByte( const char * buf,
                 size_t * start,
                 size_t max )
{
  bool ret = false;
  size_t i = 0U, bitCount = 0U, j = 0U;
  uint32_t value = 0U;
  char_ c = { 0 };

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;
  assert( i < max );
  assert( !isascii_( buf[ i ] ) );

  c.c = buf[ i ];

  if( ( c.u > 0xC1U ) && ( c.u < 0xF5U ) )
  {
    bitCount = countHighBits( c.u );
    value = ( ( uint32_t ) c.u ) & ( ( ( uint32_t ) 1 << ( 7U - bitCount ) ) - 1U );

    /* The bit count is 1 greater than the number of bytes,
     * e.g., when j is 2, we skip one more byte. */
    for( j = bitCount - 1U; j > 0U; j-- )
    {
      i++;

      if( i >= max )
      {
        break;
      }

      c.c = buf[ i ];

      /* Additional bytes must match 10xxxxxx. */
      if( ( c.u & 0xC0U ) != 0x80U )
      {
        break;
      }

      value = ( value << 6U ) | ( c.u & 0x3FU );
    }

    if( ( j == 0U ) && ( shortestUTF8( bitCount, value ) == true ) )
    {
      *start = i + 1U;
      ret = true;
    }
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond an ASCII or UTF-8 code point.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid code point was present;
 * false otherwise.
 */
static bool skipUTF8( const char * buf,
            size_t * start,
            size_t max )
{
  bool ret = false;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  if( *start < max )
  {
    if( isascii_( buf[ *start ] ) )
    {
      *start += 1U;
      ret = true;
    }
    else
    {
      ret = skipUTF8MultiByte( buf, start, max );
    }
  }

  return ret;
}

/**
 * @brief Convert a hexadecimal character to an integer.
 *
 * @param[in] c  The character to convert.
 *
 * @return the integer value upon success or NOT_A_HEX_CHAR on failure.
 */
#define NOT_A_HEX_CHAR  ( 0x10U )
static uint8_t hexToInt( char c )
{
  char_ n = { 0 };

  n.c = c;

  if( ( c >= 'a' ) && ( c <= 'f' ) )
  {
    n.c -= 'a';
    n.u += 10U;
  }
  else if( ( c >= 'A' ) && ( c <= 'F' ) )
  {
    n.c -= 'A';
    n.u += 10U;
  }
  else if( isdigit_( c ) )
  {
    n.c -= '0';
  }
  else
  {
    n.u = NOT_A_HEX_CHAR;
  }

  return n.u;
}

/**
 * @brief Advance buffer index beyond a single \u Unicode
 * escape sequence and output the value.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[out] outValue  The value of the hex digits.
 *
 * @return true if a valid escape sequence was present;
 * false otherwise.
 *
 * @note For the sake of security, \u0000 is disallowed.
 */
static bool skipOneHexEscape( const char * buf,
                size_t * start,
                size_t max,
                uint16_t * outValue )
{
  bool ret = false;
  size_t i = 0U, end = 0U;
  uint16_t value = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  assert( outValue != NULL );

  i = *start;
#define HEX_ESCAPE_LENGTH  ( 6U )   /* e.g., \u1234 */
  /* MISRA Ref 14.3.1 [Configuration dependent invariant] */
  /* More details at: https://github.com/FreeRTOS/coreJSON/blob/main/MISRA.md#rule-143 */
  /* coverity[misra_c_2012_rule_14_3_violation] */
  end = ( i <= ( SIZE_MAX - HEX_ESCAPE_LENGTH ) ) ? ( i + HEX_ESCAPE_LENGTH ) : SIZE_MAX;

  if( ( end < max ) && ( buf[ i ] == '\\' ) && ( buf[ i + 1U ] == 'u' ) )
  {
    for( i += 2U; i < end; i++ )
    {
      uint8_t n = hexToInt( buf[ i ] );

      if( n == NOT_A_HEX_CHAR )
      {
        break;
      }

      value = ( value << 4U ) | n;
    }
  }

  if( ( i == end ) && ( value > 0U ) )
  {
    ret = true;
    *outValue = value;
    *start = i;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond one or a pair of \u Unicode escape sequences.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * Surrogate pairs are two escape sequences that together denote
 * a code point outside the Basic Multilingual Plane.  They must
 * occur as a pair with the first "high" value in [U+D800, U+DBFF],
 * and the second "low" value in [U+DC00, U+DFFF].
 *
 * @return true if a valid escape sequence was present;
 * false otherwise.
 *
 * @note For the sake of security, \u0000 is disallowed.
 */
#define isHighSurrogate( x )  ( ( ( x ) >= 0xD800U ) && ( ( x ) <= 0xDBFFU ) )
#define isLowSurrogate( x )   ( ( ( x ) >= 0xDC00U ) && ( ( x ) <= 0xDFFFU ) )

static bool skipHexEscape( const char * buf,
               size_t * start,
               size_t max )
{
  bool ret = false;
  size_t i = 0U;
  uint16_t value = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( skipOneHexEscape( buf, &i, max, &value ) == true )
  {
    if( isHighSurrogate( value ) )
    {
      if( ( skipOneHexEscape( buf, &i, max, &value ) == true ) &&
        ( isLowSurrogate( value ) ) )
      {
        ret = true;
      }
    }
    else if( isLowSurrogate( value ) )
    {
      /* premature low surrogate */
    }
    else
    {
      ret = true;
    }
  }

  if( ret == true )
  {
    *start = i;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond an escape sequence.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid escape sequence was present;
 * false otherwise.
 *
 * @note For the sake of security, \NUL is disallowed.
 */
static bool skipEscape( const char * buf,
            size_t * start,
            size_t max )
{
  bool ret = false;
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < ( max - 1U ) ) && ( buf[ i ] == '\\' ) )
  {
    char c = buf[ i + 1U ];

    switch( c )
    {
      case '\0':
        break;

      case 'u':
        ret = skipHexEscape( buf, &i, max );
        break;

      case '"':
      case '\\':
      case '/':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
        i += 2U;
        ret = true;
        break;

      default:

        /* a control character: (NUL,SPACE) */
        if( iscntrl_( c ) )
        {
          i += 2U;
          ret = true;
        }

        break;
    }
  }

  if( ret == true )
  {
    *start = i;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a double-quoted string.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid string was present;
 * false otherwise.
 */
static bool skipString( const char * buf,
            size_t * start,
            size_t max )
{
  bool ret = false;
  size_t i = 0;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( buf[ i ] == '"' ) )
  {
    i++;

    while( i < max )
    {
      if( buf[ i ] == '"' )
      {
        ret = true;
        i++;
        break;
      }

      if( buf[ i ] == '\\' )
      {
        if( skipEscape( buf, &i, max ) != true )
        {
          break;
        }
      }
      /* An unescaped control character is not allowed. */
      else if( iscntrl_( buf[ i ] ) )
      {
        break;
      }
      else if( skipUTF8( buf, &i, max ) != true )
      {
        break;
      }
      else
      {
        /* MISRA 15.7 */
      }
    }
  }

  if( ret == true )
  {
    *start = i;
  }

  return ret;
}

/**
 * @brief Compare the leading n bytes of two character sequences.
 *
 * @param[in] a  first character sequence
 * @param[in] b  second character sequence
 * @param[in] n  number of bytes
 *
 * @return true if the sequences are the same;
 * false otherwise
 */
static bool strnEq( const char * a,
          const char * b,
          size_t n )
{
  size_t i = 0U;

  assert( ( a != NULL ) && ( b != NULL ) );

  for( i = 0; i < n; i++ )
  {
    if( a[ i ] != b[ i ] )
    {
      break;
    }
  }

  return ( i == n ) ? true : false;
}

/**
 * @brief Advance buffer index beyond a literal.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[in] literal  The type of literal.
 * @param[in] length  The length of the literal.
 *
 * @return true if the literal was present;
 * false otherwise.
 */
static bool skipLiteral( const char * buf,
             size_t * start,
             size_t max,
             const char * literal,
             size_t length )
{
  bool ret = false;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  assert( literal != NULL );

  if( ( *start < max ) && ( length <= ( max - *start ) ) )
  {
    ret = strnEq( &buf[ *start ], literal, length );
  }

  if( ret == true )
  {
    *start += length;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a JSON literal.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid literal was present;
 * false otherwise.
 */
static bool skipAnyLiteral( const char * buf,
              size_t * start,
              size_t max )
{
  bool ret = false;

#define skipLit_( x ) \
  ( skipLiteral( buf, start, max, ( x ), ( sizeof( x ) - 1UL ) ) == true )

  if( skipLit_( "true" ) )
  {
    ret = true;
  }
  else if( skipLit_( "false" ) )
  {
    ret = true;
  }
  else if( skipLit_( "null" ) )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond one or more digits.
 * Optionally, output the integer value of the digits.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[out] outValue  The integer value of the digits.
 *
 * @note outValue may be NULL.  If not NULL, and the output
 * exceeds ~2 billion, then -1 is output.
 *
 * @return true if a digit was present;
 * false otherwise.
 */
#define MAX_FACTOR  ( MAX_INDEX_VALUE / 10 )
static bool skipDigits( const char * buf,
            size_t * start,
            size_t max,
            int32_t * outValue )
{
  bool ret = false;
  size_t i = 0U, saveStart = 0U;
  int32_t value = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  saveStart = *start;

  for( i = *start; i < max; i++ )
  {
    if( !isdigit_( buf[ i ] ) )
    {
      break;
    }

    if( ( outValue != NULL ) && ( value > -1 ) )
    {
      int8_t n = ( int8_t ) hexToInt( buf[ i ] );

      if( value <= MAX_FACTOR )
      {
        value = ( value * 10 ) + n;
      }
      else
      {
        value = -1;
      }
    }
  }

  if( i > saveStart )
  {
    ret = true;
    *start = i;

    if( outValue != NULL )
    {
      *outValue = value;
    }
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond the decimal portion of a number.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 */
static void skipDecimals( const char * buf,
              size_t * start,
              size_t max )
{
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( buf[ i ] == '.' ) )
  {
    i++;

    if( skipDigits( buf, &i, max, NULL ) == true )
    {
      *start = i;
    }
  }
}

/**
 * @brief Advance buffer index beyond the exponent portion of a number.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 */
static void skipExponent( const char * buf,
              size_t * start,
              size_t max )
{
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( ( buf[ i ] == 'e' ) || ( buf[ i ] == 'E' ) ) )
  {
    i++;

    if( ( i < max ) && ( ( buf[ i ] == '-' ) || ( buf[ i ] == '+' ) ) )
    {
      i++;
    }

    if( skipDigits( buf, &i, max, NULL ) == true )
    {
      *start = i;
    }
  }
}

/**
 * @brief Advance buffer index beyond a number.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a valid number was present;
 * false otherwise.
 */
static bool skipNumber( const char * buf,
            size_t * start,
            size_t max )
{
  bool ret = false;
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( buf[ i ] == '-' ) )
  {
    i++;
  }

  if( i < max )
  {
    /* JSON disallows superfluous leading zeroes, so an
     * initial zero must either be alone, or followed by
     * a decimal or exponent.
     *
     * Should there be a digit after the zero, that digit
     * will not be skipped by this function, and later parsing
     * will judge this an illegal document. */
    if( buf[ i ] == '0' )
    {
      ret = true;
      i++;
    }
    else
    {
      ret = skipDigits( buf, &i, max, NULL );
    }
  }

  if( ret == true )
  {
    skipDecimals( buf, &i, max );
    skipExponent( buf, &i, max );
    *start = i;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a scalar value.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a scalar value was present;
 * false otherwise.
 */
static bool skipAnyScalar( const char * buf,
               size_t * start,
               size_t max )
{
  bool ret = false;

  if( skipString( buf, start, max ) == true )
  {
    ret = true;
  }
  else if( skipAnyLiteral( buf, start, max ) == true )
  {
    ret = true;
  }
  else if( skipNumber( buf, start, max ) == true )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a comma separator
 * and surrounding whitespace.
 *
 * JSON uses a comma to separate values in an array and key-value
 * pairs in an object.  JSON does not permit a trailing comma.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return true if a non-terminal comma was present;
 * false otherwise.
 */
static bool skipSpaceAndComma( const char * buf,
                 size_t * start,
                 size_t max )
{
  bool ret = false;
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  skipSpace( buf, start, max );
  i = *start;

  if( ( i < max ) && ( buf[ i ] == ',' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    if( ( i < max ) && !isCloseBracket_( buf[ i ] ) )
    {
      ret = true;
      *start = i;
    }
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond the scalar values of an array.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @note Stops advance if a value is an object or array.
 */
static void skipArrayScalars( const char * buf,
                size_t * start,
                size_t max )
{
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  while( i < max )
  {
    if( skipAnyScalar( buf, &i, max ) != true )
    {
      break;
    }

    if( skipSpaceAndComma( buf, &i, max ) != true )
    {
      break;
    }
  }

  *start = i;
}

/**
 * @brief Advance buffer index beyond the scalar key-value pairs
 * of an object.
 *
 * In JSON, objects consist of comma-separated key-value pairs.
 * A key is always a string (a scalar) while a value may be a
 * scalar, an object, or an array.  A colon must appear between
 * each key and value.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @note Stops advance if a value is an object or array.
 */
static void skipObjectScalars( const char * buf,
                 size_t * start,
                 size_t max )
{
  size_t i = 0U;
  bool comma = false;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  while( i < max )
  {
    if( skipString( buf, &i, max ) != true )
    {
      break;
    }

    skipSpace( buf, &i, max );

    if( ( i < max ) && ( buf[ i ] != ':' ) )
    {
      break;
    }

    i++;
    skipSpace( buf, &i, max );

    if( ( i < max ) && isOpenBracket_( buf[ i ] ) )
    {
      *start = i;
      break;
    }

    if( skipAnyScalar( buf, &i, max ) != true )
    {
      break;
    }

    comma = skipSpaceAndComma( buf, &i, max );
    *start = i;

    if( comma != true )
    {
      break;
    }
  }
}

/**
 * @brief Advance buffer index beyond one or more scalars.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[in] mode  The first character of an array '[' or object '{'.
 */
static void skipScalars( const char * buf,
             size_t * start,
             size_t max,
             char mode )
{
  assert( isOpenBracket_( mode ) );

  skipSpace( buf, start, max );

  if( mode == '[' )
  {
    skipArrayScalars( buf, start, max );
  }
  else
  {
    skipObjectScalars( buf, start, max );
  }
}

/**
 * @brief Advance buffer index beyond a collection and handle nesting.
 *
 * A stack is used to continue parsing the prior collection type
 * when a nested collection is finished.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 *
 * @return #JSONSuccess if the buffer contents are a valid JSON collection;
 * #JSONIllegalDocument if the buffer contents are NOT valid JSON;
 * #JSONMaxDepthExceeded if object and array nesting exceeds a threshold;
 * #JSONPartial if the buffer contents are potentially valid but incomplete.
 */
#ifndef JSON_MAX_DEPTH
  #define JSON_MAX_DEPTH  32
#endif
static JSONStatus_t skipCollection( const char * buf,
                  size_t * start,
                  size_t max )
{
  JSONStatus_t ret = JSONPartial;
  char c, stack[ JSON_MAX_DEPTH ];
  int16_t depth = -1;
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  while( i < max )
  {
    c = buf[ i ];
    i++;

    switch( c )
    {
      case '{':
      case '[':
        depth++;

        if( depth >= JSON_MAX_DEPTH )
        {
          ret = JSONMaxDepthExceeded;
          break;
        }

        stack[ depth ] = c;
        skipScalars( buf, &i, max, stack[ depth ] );
        break;

      case '}':
      case ']':

        if( ( depth > 0 ) && ( depth < JSON_MAX_DEPTH ) &&
          isMatchingBracket_( stack[ depth ], c ) )
        {
          depth--;

          if( ( skipSpaceAndComma( buf, &i, max ) == true ) &&
            isOpenBracket_( stack[ depth ] ) )
          {
            skipScalars( buf, &i, max, stack[ depth ] );
          }

          break;
        }

        ret = ( ( depth == 0 ) && isMatchingBracket_( stack[ depth ], c ) ) ?
            JSONSuccess : JSONIllegalDocument;
        break;

      default:
        ret = JSONIllegalDocument;
        break;
    }

    if( ret != JSONPartial )
    {
      break;
    }
  }

  if( ret == JSONSuccess )
  {
    *start = i;
  }

  return ret;
}

/** @endcond */

/**
 * See core_json.h for docs.
 *
 * Verify that the entire buffer contains exactly one scalar
 * or collection within optional whitespace.
 */
JSONStatus_t JSON_Validate( const char * buf,
              size_t max )
{
  JSONStatus_t ret;
  size_t i = 0U;

  if( buf == NULL )
  {
    ret = JSONNullParameter;
  }
  else if( max == 0U )
  {
    ret = JSONBadParameter;
  }
  else
  {
    skipSpace( buf, &i, max );

    /** @cond DO_NOT_DOCUMENT */
    #ifndef JSON_VALIDATE_COLLECTIONS_ONLY
      if( skipAnyScalar( buf, &i, max ) == true )
      {
        ret = JSONSuccess;
      }
      else
    #endif
    /** @endcond */
    {
      ret = skipCollection( buf, &i, max );
    }
  }

  if( ( ret == JSONSuccess ) && ( i < max ) )
  {
    skipSpace( buf, &i, max );

    if( i != max )
    {
      ret = JSONIllegalDocument;
    }
  }

  return ret;
}

/** @cond DO_NOT_DOCUMENT */

/**
 * @brief Output index and length for the next value.
 *
 * Also advances the buffer index beyond the value.
 * The value may be a scalar or a collection.
 * The start index should point to the beginning of the value.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[out] value  A pointer to receive the index of the value.
 * @param[out] valueLength  A pointer to receive the length of the value.
 *
 * @return true if a value was present;
 * false otherwise.
 */
static bool nextValue( const char * buf,
             size_t * start,
             size_t max,
             size_t * value,
             size_t * valueLength )
{
  bool ret = true;
  size_t i = 0U, valueStart = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  assert( ( value != NULL ) && ( valueLength != NULL ) );

  i = *start;
  valueStart = i;

  if( skipAnyScalar( buf, &i, max ) == true )
  {
    *value = valueStart;
    *valueLength = i - valueStart;
  }
  else if( skipCollection( buf, &i, max ) == JSONSuccess )
  {
    *value = valueStart;
    *valueLength = i - valueStart;
  }
  else
  {
    ret = false;
  }

  if( ret == true )
  {
    *start = i;
  }

  return ret;
}

/**
 * @brief Output indexes for the next key-value pair of an object.
 *
 * Also advances the buffer index beyond the key-value pair.
 * The value may be a scalar or a collection.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[out] key  A pointer to receive the index of the key.
 * @param[out] keyLength  A pointer to receive the length of the key.
 * @param[out] value  A pointer to receive the index of the value.
 * @param[out] valueLength  A pointer to receive the length of the value.
 *
 * @return true if a key-value pair was present;
 * false otherwise.
 */
static bool nextKeyValuePair( const char * buf,
                size_t * start,
                size_t max,
                size_t * key,
                size_t * keyLength,
                size_t * value,
                size_t * valueLength )
{
  bool ret = true;
  size_t i = 0U, keyStart = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  assert( ( key != NULL ) && ( keyLength != NULL ) );
  assert( ( value != NULL ) && ( valueLength != NULL ) );

  i = *start;
  keyStart = i;

  if( skipString( buf, &i, max ) == true )
  {
    *key = keyStart + 1U;
    *keyLength = i - keyStart - 2U;
  }
  else
  {
    ret = false;
  }

  if( ret == true )
  {
    skipSpace( buf, &i, max );

    if( ( i < max ) && ( buf[ i ] == ':' ) )
    {
      i++;
      skipSpace( buf, &i, max );
    }
    else
    {
      ret = false;
    }
  }

  if( ret == true )
  {
    ret = nextValue( buf, &i, max, value, valueLength );
  }

  if( ret == true )
  {
    *start = i;
  }

  return ret;
}

/**
 * @brief Find a key in a JSON object and output a pointer to its value.
 *
 * @param[in] buf  The buffer to search.
 * @param[in] max  size of the buffer.
 * @param[in] query  The object keys and array indexes to search for.
 * @param[in] queryLength  Length of the key.
 * @param[out] outValue  A pointer to receive the index of the value found.
 * @param[out] outValueLength  A pointer to receive the length of the value found.
 *
 * Iterate over the key-value pairs of an object, looking for a matching key.
 *
 * @return true if the query is matched and the value output;
 * false otherwise.
 *
 * @note Parsing stops upon finding a match.
 */
static bool objectSearch( const char * buf,
              size_t max,
              const char * query,
              size_t queryLength,
              size_t * outValue,
              size_t * outValueLength )
{
  bool ret = false;

  size_t i = 0U, key = 0U, keyLength = 0U, value = 0U, valueLength = 0U;

  assert( ( buf != NULL ) && ( query != NULL ) );
  assert( ( outValue != NULL ) && ( outValueLength != NULL ) );

  skipSpace( buf, &i, max );

  if( ( i < max ) && ( buf[ i ] == '{' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    while( i < max )
    {
      if( nextKeyValuePair( buf, &i, max, &key, &keyLength,
                  &value, &valueLength ) != true )
      {
        break;
      }

      if( ( queryLength == keyLength ) &&
        ( strnEq( query, &buf[ key ], keyLength ) == true ) )
      {
        ret = true;
        break;
      }

      if( skipSpaceAndComma( buf, &i, max ) != true )
      {
        break;
      }
    }
  }

  if( ret == true )
  {
    *outValue = value;
    *outValueLength = valueLength;
  }

  return ret;
}

/**
 * @brief Find an index in a JSON array and output a pointer to its value.
 *
 * @param[in] buf  The buffer to search.
 * @param[in] max  size of the buffer.
 * @param[in] queryIndex  The index to search for.
 * @param[out] outValue  A pointer to receive the index of the value found.
 * @param[out] outValueLength  A pointer to receive the length of the value found.
 *
 * Iterate over the values of an array, looking for a matching index.
 *
 * @return true if the queryIndex is found and the value output;
 * false otherwise.
 *
 * @note Parsing stops upon finding a match.
 */
static bool arraySearch( const char * buf,
             size_t max,
             uint32_t queryIndex,
             size_t * outValue,
             size_t * outValueLength )
{
  bool ret = false;
  size_t i = 0U, value = 0U, valueLength = 0U;
  uint32_t currentIndex = 0U;

  assert( buf != NULL );
  assert( ( outValue != NULL ) && ( outValueLength != NULL ) );

  skipSpace( buf, &i, max );

  if( ( i < max ) && ( buf[ i ] == '[' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    while( i < max )
    {
      if( nextValue( buf, &i, max, &value, &valueLength ) != true )
      {
        break;
      }

      if( currentIndex == queryIndex )
      {
        ret = true;
        break;
      }

      if( ( skipSpaceAndComma( buf, &i, max ) != true ) ||
        ( currentIndex == UINT32_MAX ) )
      {
        break;
      }

      currentIndex++;
    }
  }

  if( ret == true )
  {
    *outValue = value;
    *outValueLength = valueLength;
  }

  return ret;
}

/**
 * @brief Advance buffer index beyond a query part.
 *
 * The part is the portion of the query which is not
 * a separator or array index.
 *
 * @param[in] buf  The buffer to parse.
 * @param[in,out] start  The index at which to begin.
 * @param[in] max  The size of the buffer.
 * @param[out] outLength  The length of the query part.
 *
 * @return true if a valid string was present;
 * false otherwise.
 */
#ifndef JSON_QUERY_KEY_SEPARATOR
  #define JSON_QUERY_KEY_SEPARATOR  '.'
#endif
#define isSeparator_( x )  ( ( x ) == JSON_QUERY_KEY_SEPARATOR )
static bool skipQueryPart( const char * buf,
               size_t * start,
               size_t max,
               size_t * outLength )
{
  bool ret = false;
  size_t i = 0U;

  assert( ( buf != NULL ) && ( start != NULL ) && ( outLength != NULL ) );
  assert( max > 0U );

  i = *start;

  while( ( i < max ) &&
       !isSeparator_( buf[ i ] ) &&
       !isSquareOpen_( buf[ i ] ) )
  {
    i++;
  }

  if( i > *start )
  {
    ret = true;
    *outLength = i - *start;
    *start = i;
  }

  return ret;
}

/**
 * @brief Handle a nested search by iterating over the parts of the query.
 *
 * @param[in] buf  The buffer to search.
 * @param[in] max  size of the buffer.
 * @param[in] query  The object keys and array indexes to search for.
 * @param[in] queryLength  Length of the key.
 * @param[out] outValue  A pointer to receive the index of the value found.
 * @param[out] outValueLength  A pointer to receive the length of the value found.
 *
 * @return #JSONSuccess if the query is matched and the value output;
 * #JSONBadParameter if the query is empty, or any part is empty,
 * or an index is too large to convert;
 * #JSONNotFound if the query is NOT found.
 *
 * @note Parsing stops upon finding a match.
 */
static JSONStatus_t multiSearch( const char * buf,
                 size_t max,
                 const char * query,
                 size_t queryLength,
                 size_t * outValue,
                 size_t * outValueLength )
{
  JSONStatus_t ret = JSONSuccess;
  size_t i = 0U, start = 0U, queryStart = 0U, value = 0U, length = max;

  assert( ( buf != NULL ) && ( query != NULL ) );
  assert( ( outValue != NULL ) && ( outValueLength != NULL ) );
  assert( ( max > 0U ) && ( queryLength > 0U ) );

  while( i < queryLength )
  {
    bool found = false;

    if( isSquareOpen_( query[ i ] ) )
    {
      int32_t queryIndex = -1;
      i++;

      ( void ) skipDigits( query, &i, queryLength, &queryIndex );

      if( ( queryIndex < 0 ) ||
        ( i >= queryLength ) || !isSquareClose_( query[ i ] ) )
      {
        ret = JSONBadParameter;
        break;
      }

      i++;

      found = arraySearch( &buf[ start ], length, 
        ( uint32_t ) queryIndex, &value, &length );
    }
    else
    {
      size_t keyLength = 0;

      queryStart = i;

      if( ( skipQueryPart( query, &i, queryLength, &keyLength ) != true ) ||
        /* catch an empty key part or a trailing separator */
        ( i == ( queryLength - 1U ) ) )
      {
        ret = JSONBadParameter;
        break;
      }

      found = objectSearch( &buf[ start ], length, 
        &query[ queryStart ], keyLength, &value, &length );
    }

    if( found == false )
    {
      ret = JSONNotFound;
      break;
    }

    start += value;

    if( ( i < queryLength ) && isSeparator_( query[ i ] ) )
    {
      i++;
    }
  }

  if( ret == JSONSuccess )
  {
    *outValue = start;
    *outValueLength = length;
  }

  return ret;
}

/**
 * @brief Return a JSON type based on a separator character or
 * the first character of a value.
 *
 * @param[in] c  The character to classify.
 *
 * @return an enum of JSONTypes_t
 */
static JSONTypes_t getType( char c )
{
  JSONTypes_t t;

  switch( c )
  {
    case '"':
    t = JSONString;
    break;

    case '{':
    t = JSONObject;
    break;

    case '[':
    t = JSONArray;
    break;

    case 't':
    t = JSONTrue;
    break;

    case 'f':
    t = JSONFalse;
    break;

    case 'n':
    t = JSONNull;
    break;

    default:
    t = JSONNumber;
    break;
  }

  return t;
}

/** @endcond */

/**
 * See core_json.h for docs.
 */
JSONStatus_t JSON_SearchConst( const char * buf,
                 size_t max,
                 const char * query,
                 size_t queryLength,
                 const char ** outValue,
                 size_t * outValueLength,
                 JSONTypes_t * outType )
{
  JSONStatus_t ret;
  size_t value = 0U;

  if( ( buf == NULL ) || ( query == NULL ) ||
    ( outValue == NULL ) || ( outValueLength == NULL ) )
  {
    ret = JSONNullParameter;
  }
  else if( ( max == 0U ) || ( queryLength == 0U ) )
  {
    ret = JSONBadParameter;
  }
  else
  {
    ret = multiSearch( buf, max, query, queryLength, &value, outValueLength );
  }

  if( ret == JSONSuccess )
  {
    JSONTypes_t t = getType( buf[ value ] );

    if( t == JSONString )
    {
      /* strip the surrounding quotes */
      value++;
      *outValueLength -= 2U;
    }

    *outValue = &buf[ value ];

    if( outType != NULL )
    {
      *outType = t;
    }
  }

  return ret;
}

/**
 * See core_json.h for docs.
 */
JSONStatus_t JSON_SearchT( char * buf,
               size_t max,
               const char * query,
               size_t queryLength,
               char ** outValue,
               size_t * outValueLength,
               JSONTypes_t * outType )
{
  /* MISRA Ref 11.3.1 [Misaligned access] */
  /* More details at: https://github.com/FreeRTOS/coreJSON/blob/main/MISRA.md#rule-113 */
  /* coverity[misra_c_2012_rule_11_3_violation] */
  return JSON_SearchConst( ( const char * ) buf, max, query, queryLength,
               ( const char ** ) outValue, outValueLength, outType );
}

/** @cond DO_NOT_DOCUMENT */

/**
 * @brief Output the next key-value pair or value from a collection.
 *
 * @param[in] buf  The buffer to search.
 * @param[in] max  size of the buffer.
 * @param[in] start  The index at which the collection begins.
 * @param[in,out] next  The index at which to seek the next value.
 * @param[out] outKey  A pointer to receive the index of the value found.
 * @param[out] outKeyLength  A pointer to receive the length of the value found.
 * @param[out] outValue  A pointer to receive the index of the value found.
 * @param[out] outValueLength  A pointer to receive the length of the value found.
 *
 * @return #JSONSuccess if a value is output;
 * #JSONIllegalDocument if the buffer does not begin with '[' or '{';
 * #JSONNotFound if there are no further values in the collection.
 */
static JSONStatus_t iterate( const char * buf,
               size_t max,
               size_t * start,
               size_t * next,
               size_t * outKey,
               size_t * outKeyLength,
               size_t * outValue,
               size_t * outValueLength )
{
  JSONStatus_t ret = JSONNotFound;
  bool found = false;

  assert( ( buf != NULL ) && ( max > 0U ) );
  assert( ( start != NULL ) && ( next != NULL ) );
  assert( ( outKey != NULL ) && ( outKeyLength != NULL ) );
  assert( ( outValue != NULL ) && ( outValueLength != NULL ) );

  if( *start < max )
  {
    switch( buf[ *start ] )
    {
      case '[':
      found = nextValue( buf, next, max, outValue, outValueLength );

      if( found == true )
      {
        *outKey = 0;
        *outKeyLength = 0;
      }

      break;

      case '{':
      found = nextKeyValuePair( buf, next, max, outKey, outKeyLength,
                    outValue, outValueLength );
      break;

      default:
      ret = JSONIllegalDocument;
      break;
    }
  }

  if( found == true )
  {
    ret = JSONSuccess;
    ( void ) skipSpaceAndComma( buf, next, max );
  }

  return ret;
}

/** @endcond */

/**
 * See core_json.h for docs.
 */
JSONStatus_t JSON_Iterate( const char * buf,
               size_t max,
               size_t * start,
               size_t * next,
               JSONPair_t * outPair )
{
  JSONStatus_t ret;
  size_t key = 0U, keyLength = 0U, value = 0U, valueLength = 0U;

  if( ( buf == NULL ) || ( start == NULL ) || ( next == NULL ) ||
    ( outPair == NULL ) )
  {
    ret = JSONNullParameter;
  }
  else if( ( max == 0U ) || ( *start >= max ) || ( *next > max ) )
  {
    ret = JSONBadParameter;
  }
  else
  {
    skipSpace( buf, start, max );

    if( *next <= *start )
    {
      *next = *start + 1U;
      skipSpace( buf, next, max );
    }

  ret = iterate( buf, max, start, next, &key, &keyLength,
           &value, &valueLength );
  }

  if( ret == JSONSuccess )
  {
    JSONTypes_t t = getType( buf[ value ] );

    if( t == JSONString )
    {
      /* strip the surrounding quotes */
      value++;
      valueLength -= 2U;
    }

    outPair->key = ( key == 0U ) ? NULL : &buf[ key ];
    outPair->keyLength = keyLength;
    outPair->value = &buf[ value ];
    outPair->valueLength = valueLength;
    outPair->jsonType = t;
  }

  return ret;
}

static char * json_types[] =
{
  "invalid",
  "string",
  "number",
  "true",
  "false",
  "null",
  "object",
  "array"
};
const char *json_type2str(JSONTypes_t type)
{
  return (type >= JSONTypes_MAX) ? "<unknown>" : json_types[type];
}

void json_pairs_free(JSONPair_t *pair)
{
  if (pair == NULL)
    return;
  if (pair->children)
    json_pairs_free(pair->children);
  if (pair->siblings)
    json_pairs_free(pair->siblings);
  if (pair->jsonType == JSONString && pair->str != NULL)
  {
    http_free(pair->str);
    pair->str = NULL;
  }
  http_free(pair);
}

#define json_MIN(a,b) ((a)<(b) ? (a) : (b))

static JSONStatus_t __json_parse(JSONTypes_t parent_jsonType, 
  JSONPair_t **__pair, const char *json, size_t length)
{
  size_t start = 0, next = 0;
  JSONStatus_t result;
  JSONPair_t *prev = NULL;
  JSONPair_t *cur = NULL;

  result = JSON_Validate(json, length);
  if (result != JSONSuccess)
  {
    *__pair = NULL;
    return result;
  }

  while (result != JSONNotFound)
  {
    JSONPair_t *pair = http_malloc(sizeof(*pair));
    if (pair == NULL)
    {
      result = JSONBadParameter;
      goto clean0;
    }
    memset(pair,0,sizeof(*pair));

    result = JSON_Iterate(json, length, &start, &next, pair);
    if (result != JSONSuccess)
    {
      http_free(pair);
      if (result == JSONNotFound)
      {
        result = JSONSuccess;
        break;
      }
      goto clean0;
    }
    else
    {
      if ((pair->key == NULL || pair->keyLength == 0)
        && (pair->jsonType != JSONArray 
            && pair->jsonType != JSONObject
            && parent_jsonType != JSONArray))
      {
        http_free(pair);
        result = JSONBadParameter;
        goto clean0;
      }
    }

    if (prev == NULL)
    {
      prev = cur = pair;
    }
    else
    {
      cur = cur->siblings = pair;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        result = JSONBadParameter;
        goto clean0;
      case JSONString:
        break;
      case JSONNumber:
        {
          char str[128];
          char *p=NULL;
          int len = json_MIN(sizeof(str)-1, pair->valueLength);
          memcpy(str, pair->value, len);
          str[len] = '\0';
          if (NULL != strchr(str, '.'))
          {
            pair->isDouble = 1;
            pair->vdouble = strtod(str, &p);
          }
          else
          {
            pair->isDouble = 0;
            pair->vint = strtoll(str, &p, 10);
          }
          if (p != str + len)
          {
            result = JSONBadParameter;
            goto clean0;
          }
        }
        break;
      case JSONTrue:
        pair->vint = 1;
        break;
      case JSONFalse:
        pair->vint = 0;
        break;
      case JSONNull:
        pair->str = NULL;
        break;
      case JSONArray:
      case JSONObject:
        {
          JSONPair_t *child;

          result = __json_parse(pair->jsonType, &child, pair->value, pair->valueLength);
          if (result != JSONSuccess && result != JSONNotFound)
          {
            goto clean0;
          }
          result = JSONSuccess;
          pair->children = child;
        }
        break;
    }
  } ;

  *__pair = prev;
  return result;
  
clean0:
  json_pairs_free(prev);
  *__pair = NULL;
  return result;
}

JSONStatus_t json_parse(JSONPair_t **__pair, const char *json, size_t length)
{
  return __json_parse(JSONInvalid, __pair, json, length);
}

/* only the first 36 ascii characters need an escape */
static char const *character_escape[] = {
	"\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007", /*  0-7  */
	"\\b"    ,     "\\t",     "\\n", "\\u000b",     "\\f",     "\\r", "\\u000e", "\\u000f", /*  8-f  */
	"\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017", /* 10-17 */
	"\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f", /* 18-1f */
	"\x20"   , "\x21"   , "\\\""   , "\x23"   , "\x24"   , "\x25"   , "\x26"   , "\x27"   , /* 20-27 */
	"\x28"   , "\x29"   , "\x2a"   , "\x2b"   , "\x2c"   , "\x2d"   , "\x2e"   , "\x2f"   , /* 28-2f */
	"\x30"   , "\x31"   , "\x32"   , "\x33"   , "\x34"   , "\x35"   , "\x36"   , "\x37"   , /* 30-37 */
	"\x38"   , "\x39"   , "\x3a"   , "\x3b"   , "\x3c"   , "\x3d"   , "\x3e"   , "\x3f"   , /* 38-3f */
	"\x40"   , "\x41"   , "\x42"   , "\x43"   , "\x44"   , "\x45"   , "\x46"   , "\x47"   , /* 40-47 */
	"\x48"   , "\x49"   , "\x4a"   , "\x4b"   , "\x4c"   , "\x4d"   , "\x4e"   , "\x4f"   , /* 48-4f */
	"\x50"   , "\x51"   , "\x52"   , "\x53"   , "\x54"   , "\x55"   , "\x56"   , "\x57"   , /* 50-57 */
	"\x58"   , "\x59"   , "\x5a"   , "\x5b"   , "\\\\"   , "\x5d"   , "\x5e"   , "\x5f"   , /* 58-5f */
	"\x60"   , "\x61"   , "\x62"   , "\x63"   , "\x64"   , "\x65"   , "\x66"   , "\x67"   , /* 60-67 */
	"\x68"   , "\x69"   , "\x6a"   , "\x6b"   , "\x6c"   , "\x6d"   , "\x6e"   , "\x6f"   , /* 68-6f */
	"\x70"   , "\x71"   , "\x72"   , "\x73"   , "\x74"   , "\x75"   , "\x76"   , "\x77"   , /* 70-77 */
	"\x78"   , "\x79"   , "\x7a"   , "\x7b"   , "\x7c"   , "\x7d"   , "\x7e"   , "\\u007f", /* 78-7f */
	"\\u0080", "\\u0081", "\\u0082", "\\u0083", "\\u0084", "\\u0085", "\\u0086", "\\u0087", /* 80-87 */
	"\\u0088", "\\u0089", "\\u008a", "\\u008b", "\\u008c", "\\u008d", "\\u008e", "\\u008f", /* 88-8f */
	"\\u0090", "\\u0091", "\\u0092", "\\u0093", "\\u0094", "\\u0095", "\\u0096", "\\u0097", /* 90-97 */
	"\\u0098", "\\u0099", "\\u009a", "\\u009b", "\\u009c", "\\u009d", "\\u009e", "\\u009f", /* 98-9f */
	"\\u00a0", "\\u00a1", "\\u00a2", "\\u00a3", "\\u00a4", "\\u00a5", "\\u00a6", "\\u00a7", /* a0-a7 */
	"\\u00a8", "\\u00a9", "\\u00aa", "\\u00ab", "\\u00ac", "\\u00ad", "\\u00ae", "\\u00af", /* a8-af */
	"\\u00b0", "\\u00b1", "\\u00b2", "\\u00b3", "\\u00b4", "\\u00b5", "\\u00b6", "\\u00b7", /* b0-b7 */
	"\\u00b8", "\\u00b9", "\\u00ba", "\\u00bb", "\\u00bc", "\\u00bd", "\\u00be", "\\u00bf", /* b8-bf */
	"\\u00c0", "\\u00c1", "\\u00c2", "\\u00c3", "\\u00c4", "\\u00c5", "\\u00c6", "\\u00c7", /* c0-c7 */
	"\\u00c8", "\\u00c9", "\\u00ca", "\\u00cb", "\\u00cc", "\\u00cd", "\\u00ce", "\\u00cf", /* c8-cf */
	"\\u00d0", "\\u00d1", "\\u00d2", "\\u00d3", "\\u00d4", "\\u00d5", "\\u00d6", "\\u00d7", /* d0-d7 */
	"\\u00d8", "\\u00d9", "\\u00da", "\\u00db", "\\u00dc", "\\u00dd", "\\u00de", "\\u00df", /* d8-df */
	"\\u00e0", "\\u00e1", "\\u00e2", "\\u00e3", "\\u00e4", "\\u00e5", "\\u00e6", "\\u00e7", /* e0-e7 */
	"\\u00e8", "\\u00e9", "\\u00ea", "\\u00eb", "\\u00ec", "\\u00ed", "\\u00ee", "\\u00ef", /* e8-ef */
	"\\u00f0", "\\u00f1", "\\u00f2", "\\u00f3", "\\u00f4", "\\u00f5", "\\u00f6", "\\u00f7", /* f0-f7 */
	"\\u00f8", "\\u00f9", "\\u00fa", "\\u00fb", "\\u00fc", "\\u00fd", "\\u00fe", "\\u00ff", /* f8-ff */
};

static int __json_pad_print(JSON_PRINTER_f printer, 
  httpd_buf_t *b, int depth, const char *pad)
{
  int k=0;
  
  if (depth == -1)
    return 0;

  while (depth--)
  {
    int n = printer(b, "%s", pad);
    if (n < 0) return n;
    k += n;
  }

  return k;
}

int json_printer_default(httpd_buf_t *b, const char *fmt, ...)
{
  size_t n;
  va_list args;

  HTTPD_UNUSED(b);

  va_start(args, fmt);
  n = vprintf(fmt, args);
  va_end(args);
  return n;
}

static int 
__json_print(JSON_PRINTER_f printer, httpd_buf_t *b, 
  JSONPair_t *pair, int depth, const char *pad)
{
  size_t n, k=0;
  
  while (pair != NULL)
  {
    n = __json_pad_print(printer, b, depth,pad);
    if (n < 0) return n;
    k += n;

    if (pair->keyLength)
    {
      n = printer(b, "\"%.*s\":",pair->keyLength,pair->key);
      if (n < 0) return n;
      k += n;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        return -1;
        
      case JSONString:
        if (pair->valueLength)
        {
          n = printer(b,"%s","\"");
          if (n < 0) return n;
          k += n;

          for (int i=0;i<pair->valueLength;i++)
          {
            n = printer(b,"%s",character_escape[(int)pair->value[i]]);
            if (n < 0) return n;
            k += n;
          }

          n = printer(b,"%s","\"");
          if (n < 0) return n;
          k += n;
        }
        else
        {
          n = printer(b,"\"%s\"","");
          if (n < 0) return n;
          k += n;
        }
        break;
        
      case JSONNumber:
        if (pair->isDouble)
        {
          n = printer(b, "%lf",pair->vdouble);
          if (n < 0) return n;
          k += n;
        }
        else
        {
          n = printer(b, "%lld",pair->vint);
          if (n < 0) return n;
          k += n;
        }
        break;
        
      case JSONTrue:
        n = printer(b, "%s", "true");
        if (n < 0) return n;
        k += n;
        break;
        
      case JSONFalse:
        n = printer(b, "%s", "false");
        if (n < 0) return n;
        k += n;
        break;
        
      case JSONNull:
        n = printer(b, "%s", "null");
        if (n < 0) return n;
        k += n;
        break;
        
      case JSONArray:
        n = printer(b, "%s", "[");
        if (n < 0) return n;
        k += n;

        if (depth != -1)
        {
          n = printer(b, "%s", "\n");
          if (n < 0) return n;
          k += n;
        }

        n = __json_print(printer, b, pair->children, 
          depth != -1 ? depth+1 : -1, pad);
        if (n < 0) return n;
        k += n;
        
        n = __json_pad_print(printer, b, depth,pad);
        if (n < 0) return n;
        k += n;

        n = printer(b, "%s", "]");
        if (n < 0) return n;
        k += n;
        break;
        
      case JSONObject:
        n = printer(b,"%s","{");
        if (n < 0) return n;
        k += n;

        if (depth != -1)
        {
          n = printer(b, "%s", "\n");
          if (n < 0) return n;
          k += n;
        }

        n = __json_print(printer, b, pair->children, 
          depth != -1 ? depth+1 : -1, pad);
        if (n < 0) return n;
        k += n;
        
        n = __json_pad_print(printer, b, depth,pad);
        if (n < 0) return n;
        k += n;

        n = printer(b, "%s", "}");
        if (n < 0) return n;
        k += n;
        break;
    }

    if (pair->siblings != NULL)
    {
      n = printer(b, "%s", ",");
      if (n < 0) return n;
      k += n;
    }
    if (depth != -1)
    {
      n = printer(b, "%s", "\n");
      if (n < 0) return n;
      k += n;
    }

    pair = pair->siblings;
  }
 
  return k;
}

JSONStatus_t json_print(JSONPair_t *pair, int depth, const char *pad)
{
  int n;
  httpd_buf_t b;
  
  n = json_printer_default(&b, "%s", "{");
  if (n < 0) return JSONPartial;
  if (depth != -1)
  {
    n = json_printer_default(&b, "%s", "\n");
    if (n < 0) return JSONPartial;
  }
  n = __json_print(json_printer_default, &b,  
    pair, depth != -1 ? depth+1 : -1, pad);
  if (n < 0) return JSONPartial;

  n = json_printer_default(&b, "%s", "}");
  if (n < 0) return JSONPartial;
  
  return JSONSuccess;
}

static inline int 
json_printer_buffer(httpd_buf_t *b, const char *fmt, ...)
{
  size_t n;
  va_list args;

  va_start(args, fmt);
  n = vsnprintf(b->p, b->len, fmt, args);
  va_end(args);
  b->p += n, b->len -= n;
  return n;
}

int json_tostr(JSONPair_t *pair, char *buf,
  size_t length, int depth, const char *pad)
{
  int k=0;
  httpd_buf_t b = {.buf = buf, .len = length, .p = buf};
  int n;

  n = json_printer_buffer(&b, "%s", "{");
  if (n <= 0) return -1;
  k += n;

  if (depth != -1)
  {
    n = json_printer_buffer(&b, "%s", "\n");
    if (n <= 0) return -1;
    k += n;
  }

  n = __json_print(json_printer_buffer, &b, 
        pair, depth != -1 ? depth+1 : -1, pad);
  if (n <= 0) return -1;
  k += n;

  n = json_printer_buffer(&b, "%s", "}");
  if (n <= 0) return -1;
  k += n;

  return k;
}

int json_to_printer(JSON_PRINTER_f printer, httpd_buf_t *b, 
  JSONPair_t *pair, int depth, const char *pad)
{
  int n, k=0;

  n = printer(b, "%s", "{");
  if (n < 0) return -1;
  k += n;
  
  if (depth != -1)
  {
    n = printer(b, "%s", "\n");
    if (n < 0) return -1;
    k += n;
  }

  n = __json_print(printer, b,
        pair, depth != -1 ? depth+1 : -1, pad);
  if (n < 0) return -1;
  k += n;

  n = printer(b, "%s", "}");
  if (n < 0) return -1;
  k += n;

  return k;
}

static int __json_pad_length(int depth, const char *pad)
{
  if (depth == -1)
    return 0;

  return depth * strlen(pad);
}

static int __json_cal_length(JSONPair_t *pair, int depth, const char *pad)
{
  int n = 0;
  char temp[256];
  
  while (pair != NULL)
  {
    n += __json_pad_length(depth,pad);
    if (pair->keyLength)
    {
      n += 3 + pair->keyLength;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        goto clean0;
      case JSONString:
        n += 2 + pair->valueLength;
        break;
      case JSONNumber:
        if (pair->isDouble)
        {
          n += snprintf(temp, sizeof temp, "%lf", pair->vdouble);
        }
        else
        {
          n += snprintf(temp, sizeof temp, "%lld",pair->vint);
        }
        break;
      case JSONTrue:
        n += 4;
        break;
      case JSONFalse:
        n += 5;
        break;
      case JSONNull:
        n += 4;
        break;
      case JSONArray:
        n += 1;
        if (depth != -1)
        {
          n += 1;
        }

        n += __json_cal_length(pair->children, depth != -1 ? depth+1 : -1, pad);
        if (n < 0)
          goto clean0;
        
        n += __json_pad_length(depth,pad);
        n += 1;
        break;
      case JSONObject:
        n += 1;
        if (depth != -1)
        {
          n += 1;
        }

        n += __json_cal_length(pair->children, depth != -1 ? depth+1 : -1, pad);
        if (n < 0)
          goto clean0;
        
        n += __json_pad_length(depth,pad);
        n += 1;
        break;
    }

    if (pair->siblings != NULL)
    {
      n += 1;
    }
    if (depth != -1)
    {
      n += 1;
    }

    pair = pair->siblings;
  }
 
clean0:
  return n;
}

int json_cal_length(JSONPair_t *pair, int depth, const char *pad)
{
  int n = 0;

  n += 1;
  if (depth != -1)
  {
    n += 1;
  }
  
  n += __json_cal_length(pair, depth != -1 ? depth+1 : -1, pad);

  n += 1;
  return n;
}

JSONPair_t *json_find_bykey(JSONPair_t *pair, const char *key, 
  size_t length)
{
  while (pair != NULL)
  {
    if (pair->keyLength == length
      && !strncmp(pair->key, key, length))
      return pair;
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_t *json_find_bykey_head(JSONPair_t *pair, const char *key, 
  size_t length)
{
  while (pair != NULL)
  {
    if (pair->keyLength >= length
      && !memcmp(pair->key, key, length))
      return pair;
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_t *json_find_bykey_head_tail(JSONPair_t *pair, const char *key, 
  size_t headLen, const char *tailKey, size_t tailKeyLen)
{
  while (pair != NULL)
  {
    if (pair->keyLength >= headLen + tailKeyLen
      && !memcmp(pair->key, key, headLen)
      && !memcmp(pair->key+pair->keyLength-tailKeyLen, tailKey, tailKeyLen))
    {
      return pair;
    }
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_t *json_find_bykey_head_offset(JSONPair_t *pair, const char *key, 
  size_t headLen, size_t startOffset, const char *startKey, size_t startKeyLen)
{
  while (pair != NULL)
  {
    if (pair->keyLength >= headLen + startKeyLen
      && pair->keyLength >= startOffset + startKeyLen
      && !memcmp(pair->key, key, headLen)
      && !memcmp(pair->key+startOffset, startKey, startKeyLen))
    {
      return pair;
    }
    pair = pair->siblings;
  }
  return NULL;
}

