/*
 * coreJSON v3.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
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

#include "nanohttp-json.h"

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
    free(pair->str);
    pair->str = NULL;
  }
  free(pair);
}

#define json_MIN(a,b) ((a)<(b) ? (a) : (b))

JSONPair_t *json_parse(const char *json, size_t length)
{
  size_t start = 0, next = 0;
  JSONStatus_t result;
  JSONPair_t *prev = NULL;
  JSONPair_t *cur = NULL;

  result = JSON_Validate(json, length);
  if (result != JSONSuccess)
  {
    return NULL;
  }

  while (result != JSONNotFound)
  {
    JSONPair_t *pair = malloc(sizeof(*pair));
    if (pair == NULL)
      goto clean0;

    memset(pair,0,sizeof(*pair));

    result = JSON_Iterate(json, length, &start, &next, pair);
    if (result != JSONSuccess)
    {
      free(pair);
      if (result == JSONNotFound)
        break;
      goto clean0;
    }
    else
    {
      if ((pair->key == NULL || pair->keyLength == 0)
        && (pair->jsonType != JSONArray && pair->jsonType != JSONObject))
      {
        free(pair);
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

          child = json_parse(pair->value, pair->valueLength);
          if (child == NULL)
          {
            goto clean0;
          }
          pair->children = child;
        }
        break;
    }
  } ;

  return prev;
  
clean0:
  json_pairs_free(prev);
  return NULL;
}

static int __json_pad_put(char *buf, int len, int depth, const char *pad)
{
  char *p = buf;

  if (depth == -1)
    return 0;
  
  while (depth--)
  {
    int n = snprintf(p, len, "%s", pad);
    p += n;
    len -= n;
  }

  return p - buf;
}

static void __json_pad_print(int depth, const char *pad)
{
  if (depth == -1)
    return;

  while (depth--)
  {
    printf("%s", pad);
  }
}

static int __json_tostr(JSONPair_t *pair, char *buf,
  size_t length, int depth, const char *pad)
{
  char *p = buf;
  int n, r=-1;
  
  while (pair != NULL)
  {
    n = __json_pad_put(p,length,depth,pad);
    p+=n,length-=n;
    if (pair->keyLength)
    {
      n = snprintf(p,length,"\"%.*s\":",pair->keyLength,pair->key);
      p+=n,length-=n;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        goto clean0;
      case JSONString:
        n = snprintf(p,length,"\"%.*s\"",pair->valueLength,pair->value);
        p+=n,length-=n;
        break;
      case JSONNumber:
        if (pair->isDouble)
        {
          n = snprintf(p,length,"%lf",pair->vdouble);
          p+=n,length-=n;
        }
        else
        {
          n = snprintf(p,length,"%lld",pair->vint);
          p+=n,length-=n;
        }
        break;
      case JSONTrue:
        n = snprintf(p,length,"true");
        p+=n,length-=n;
        break;
      case JSONFalse:
        n = snprintf(p,length,"false");
        p+=n,length-=n;
        break;
      case JSONNull:
        n = snprintf(p,length,"null");
        p+=n,length-=n;
        break;
      case JSONArray:
        n = snprintf(p,length,"[");
        p+=n,length-=n;
        if (depth != -1)
        {
          n = snprintf(p,length,"\n");
          p+=n,length-=n;
        }

        n = __json_tostr(pair->children, p, length, depth != -1 ? depth+1 : -1, pad);
        if (n == -1)
          goto clean0;
        p+=n,length-=n;
        
        n = __json_pad_put(p,length,depth,pad);
        p+=n,length-=n;
        n = snprintf(p,length,"]");
        p+=n,length-=n;
        break;
      case JSONObject:
        n = snprintf(p,length,"{");
        p+=n,length-=n;
        if (depth != -1)
        {
          n = snprintf(p,length,"\n");
          p+=n,length-=n;
        }

        n = __json_tostr(pair->children, p, length, depth != -1 ? depth+1 : -1, pad);
        if (n == -1)
          goto clean0;
        p+=n,length-=n;
        
        n = __json_pad_put(p,length,depth,pad);
        p+=n,length-=n;
        n = snprintf(p,length,"}");
        p+=n,length-=n;
        break;
    }

    if (pair->siblings != NULL)
    {
      n = snprintf(p,length,",");
      p+=n,length-=n;
    }
    if (depth != -1)
    {
      n = snprintf(p,length,"\n");
      p+=n,length-=n;
    }

    pair = pair->siblings;
  }

  r = p - buf;

clean0:
  return r;
}

JSONStatus_t json_tostr(JSONPair_t *pair, char *buf,
  size_t length, int depth, const char *pad)
{
  char *p = buf;
  int n;
  JSONStatus_t r = JSONSuccess;

  n = snprintf(p,length,"{");
  p+=n,length-=n;
  if (depth != -1)
  {
    n = snprintf(p,length,"\n");
    p+=n,length-=n;
  }

  n = __json_tostr(pair, p, length, depth != -1 ? depth+1 : -1, pad);
  if (n != -1)
    p+=n,length-=n;

  n = snprintf(p,length,"}");
  p+=n,length-=n;

  return r;
}

static JSONStatus_t __json_print(JSONPair_t *pair, int depth, const char *pad)
{
  JSONStatus_t r = JSONSuccess;
  
  while (pair != NULL)
  {
    __json_pad_print(depth,pad);
    if (pair->keyLength)
    {
      printf("\"%.*s\":",pair->keyLength,pair->key);
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        goto clean0;
      case JSONString:
        printf("\"%.*s\"",pair->valueLength,pair->value);
        break;
      case JSONNumber:
        if (pair->isDouble)
        {
          printf("%lf",pair->vdouble);
        }
        else
        {
          printf("%lld",pair->vint);
        }
        break;
      case JSONTrue:
        printf("true");
        break;
      case JSONFalse:
        printf("false");
        break;
      case JSONNull:
        printf("null");
        break;
      case JSONArray:
        printf("[");
        if (depth != -1)
        {
          printf("\n");
        }

        r = __json_print(pair->children, depth != -1 ? depth+1 : -1, pad);
        if (r != JSONSuccess)
          goto clean0;
        
        __json_pad_print(depth,pad);
        printf("]");
        break;
      case JSONObject:
        printf("{");
        if (depth != -1)
        {
          printf("\n");
        }

        r = __json_print(pair->children, depth != -1 ? depth+1 : -1, pad);
        if (r != JSONSuccess)
          goto clean0;
        
        __json_pad_print(depth,pad);
        printf("}");
        break;
    }

    if (pair->siblings != NULL)
    {
      printf(",");
    }
    if (depth != -1)
    {
      printf("\n");
    }

    pair = pair->siblings;
  }
 
clean0:
  return r;
}

JSONStatus_t json_print(JSONPair_t *pair, int depth, const char *pad)
{
  JSONStatus_t r;
  printf("{");
  if (depth != -1)
  {
    printf("\n");
  }
  r = __json_print(pair, depth != -1 ? depth+1 : -1, pad);

  printf("}");
  return r;
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

