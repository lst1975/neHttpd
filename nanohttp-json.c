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
#include "nanohttp-config.h"
#include "nanohttp-json.h"
#include "nanohttp-user.h"
#include "nanohttp-logging.h"
#include "dtoa-ryu/ryu_parse.h"

/** @cond DO_NOT_DOCUMENT */

/* A compromise to satisfy both MISRA and CBMC */
typedef union
{
  char c;
  ng_uint8_t u;
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
             ng_size_t * start,
             ng_size_t max )
{
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

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
static ng_size_t countHighBits( ng_uint8_t c )
{
  ng_uint8_t n = c;
  ng_size_t i = 0;

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
static ng_bool_t shortestUTF8( ng_size_t length,
              ng_uint32_t value )
{
  ng_bool_t ret = ng_FALSE;
  ng_uint32_t min = 0U, max = 0U;

  NG_ASSERT( ( length >= 2U ) && ( length <= 4U ) );

  switch( length )
  {
    case 2:
      min = ( ng_uint32_t ) 1 << 7U;
      max = ( ( ng_uint32_t ) 1 << 11U ) - 1U;
      break;

    case 3:
      min = ( ng_uint32_t ) 1 << 11U;
      max = ( ( ng_uint32_t ) 1 << 16U ) - 1U;
      break;

    default:
      min = ( ng_uint32_t ) 1 << 16U;
      max = 0x10FFFFU;
      break;
  }

  if( ( value >= min ) && ( value <= max ) &&
    ( ( value < 0xD800U ) || ( value > 0xDFFFU ) ) )
  {
    ret = ng_TRUE;
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
static ng_bool_t skipUTF8MultiByte( const char * buf,
                 ng_size_t * start,
                 ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U, bitCount = 0U, j = 0U;
  ng_uint32_t value = 0U;
  char_ c = { 0 };

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;
  NG_ASSERT( i < max );
  NG_ASSERT( !isascii_( buf[ i ] ) );

  c.c = buf[ i ];

  if( ( c.u > 0xC1U ) && ( c.u < 0xF5U ) )
  {
    bitCount = countHighBits( c.u );
    value = ( ( ng_uint32_t ) c.u ) & ( ( ( ng_uint32_t ) 1 << ( 7U - bitCount ) ) - 1U );

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

    if( ( j == 0U ) && ( shortestUTF8( bitCount, value ) == ng_TRUE ) )
    {
      *start = i + 1U;
      ret = ng_TRUE;
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
static ng_bool_t skipUTF8( const char * buf,
            ng_size_t * start,
            ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  if( *start < max )
  {
    if( isascii_( buf[ *start ] ) )
    {
      *start += 1U;
      ret = ng_TRUE;
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
static ng_uint8_t hexToInt( char c )
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
static ng_bool_t skipOneHexEscape( const char * buf,
                ng_size_t * start,
                ng_size_t max,
                ng_uint16_t * outValue )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U, end = 0U;
  ng_uint16_t value = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  NG_ASSERT( outValue != NULL );

  i = *start;
#define HEX_ESCAPE_LENGTH  ( 6U )   /* e.g., \u1234 */
  /* MISRA Ref 14.3.1 [Configuration dependent invariant] */
  /* More details at: https://github.com/FreeRTOS/coreJSON/blob/main/MISRA.md#rule-143 */
  /* coverity[misra_c_2012_rule_14_3_violation] */
  end = ( i <= ( NG_SIZE_MAX - HEX_ESCAPE_LENGTH ) ) ? ( i + HEX_ESCAPE_LENGTH ) : NG_SIZE_MAX;

  if( ( end < max ) && ( buf[ i ] == '\\' ) && ( buf[ i + 1U ] == 'u' ) )
  {
    for( i += 2U; i < end; i++ )
    {
      ng_uint8_t n = hexToInt( buf[ i ] );

      if( n == NOT_A_HEX_CHAR )
      {
        break;
      }

      value = ( value << 4U ) | n;
    }
  }

  if( ( i == end ) && ( value > 0U ) )
  {
    ret = ng_TRUE;
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

static ng_bool_t skipHexEscape( const char * buf,
               ng_size_t * start,
               ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U;
  ng_uint16_t value = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( skipOneHexEscape( buf, &i, max, &value ) == ng_TRUE )
  {
    if( isHighSurrogate( value ) )
    {
      if( ( skipOneHexEscape( buf, &i, max, &value ) == ng_TRUE ) &&
        ( isLowSurrogate( value ) ) )
      {
        ret = ng_TRUE;
      }
    }
    else if( isLowSurrogate( value ) )
    {
      /* premature low surrogate */
    }
    else
    {
      ret = ng_TRUE;
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t skipEscape( const char * buf,
            ng_size_t * start,
            ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

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
        ret = ng_TRUE;
        break;

      default:

        /* a control character: (NUL,SPACE) */
        if( iscntrl_( c ) )
        {
          i += 2U;
          ret = ng_TRUE;
        }

        break;
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t skipString( const char * buf,
            ng_size_t * start,
            ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( buf[ i ] == '"' ) )
  {
    i++;

    while( i < max )
    {
      if( buf[ i ] == '"' )
      {
        ret = ng_TRUE;
        i++;
        break;
      }

      if( buf[ i ] == '\\' )
      {
        if( skipEscape( buf, &i, max ) != ng_TRUE )
        {
          break;
        }
      }
      /* An unescaped control character is not allowed. */
      else if( iscntrl_( buf[ i ] ) )
      {
        break;
      }
      else if( skipUTF8( buf, &i, max ) != ng_TRUE )
      {
        break;
      }
      else
      {
        /* MISRA 15.7 */
      }
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t strnEq( const char * a,
          const char * b,
          ng_size_t n )
{
  ng_size_t i = 0U;

  NG_ASSERT( ( a != NULL ) && ( b != NULL ) );

  for( i = 0; i < n; i++ )
  {
    if( a[ i ] != b[ i ] )
    {
      break;
    }
  }

  return ( i == n ) ? ng_TRUE : ng_FALSE;
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
static ng_bool_t skipLiteral( const char * buf,
             ng_size_t * start,
             ng_size_t max,
             const char * literal,
             ng_size_t length )
{
  ng_bool_t ret = ng_FALSE;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  NG_ASSERT( literal != NULL );

  if( ( *start < max ) && ( length <= ( max - *start ) ) )
  {
    ret = strnEq( &buf[ *start ], literal, length );
  }

  if( ret == ng_TRUE )
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
static ng_bool_t skipAnyLiteral( const char * buf,
              ng_size_t * start,
              ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;

#define skipLit_( x ) \
  ( skipLiteral( buf, start, max, ( x ), ( sizeof( x ) - 1UL ) ) == ng_TRUE )

  if( skipLit_( "true" ) )
  {
    ret = ng_TRUE;
  }
  else if( skipLit_( "false" ) )
  {
    ret = ng_TRUE;
  }
  else if( skipLit_( "null" ) )
  {
    ret = ng_TRUE;
  }
  else
  {
    ret = ng_FALSE;
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
static ng_bool_t skipDigits( const char * buf,
            ng_size_t * start,
            ng_size_t max,
            ng_int32_t * outValue )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U, saveStart = 0U;
  ng_int32_t value = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  saveStart = *start;

  for( i = *start; i < max; i++ )
  {
    if( !isdigit_( buf[ i ] ) )
    {
      break;
    }

    if( ( outValue != NULL ) && ( value > -1 ) )
    {
      ng_int8_t n = ( ng_int8_t ) hexToInt( buf[ i ] );

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
    ret = ng_TRUE;
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
              ng_size_t * start,
              ng_size_t max )
{
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( buf[ i ] == '.' ) )
  {
    i++;

    if( skipDigits( buf, &i, max, NULL ) == ng_TRUE )
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
              ng_size_t * start,
              ng_size_t max )
{
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  if( ( i < max ) && ( ( buf[ i ] == 'e' ) || ( buf[ i ] == 'E' ) ) )
  {
    i++;

    if( ( i < max ) && ( ( buf[ i ] == '-' ) || ( buf[ i ] == '+' ) ) )
    {
      i++;
    }

    if( skipDigits( buf, &i, max, NULL ) == ng_TRUE )
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
static ng_bool_t skipNumber( const char * buf,
            ng_size_t * start,
            ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

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
      ret = ng_TRUE;
      i++;
    }
    else
    {
      ret = skipDigits( buf, &i, max, NULL );
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t skipAnyScalar( const char * buf,
               ng_size_t * start,
               ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;

  if( skipString( buf, start, max ) == ng_TRUE )
  {
    ret = ng_TRUE;
  }
  else if( skipAnyLiteral( buf, start, max ) == ng_TRUE )
  {
    ret = ng_TRUE;
  }
  else if( skipNumber( buf, start, max ) == ng_TRUE )
  {
    ret = ng_TRUE;
  }
  else
  {
    ret = ng_FALSE;
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
static ng_bool_t skipSpaceAndComma( const char * buf,
                 ng_size_t * start,
                 ng_size_t max )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  skipSpace( buf, start, max );
  i = *start;

  if( ( i < max ) && ( buf[ i ] == ',' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    if( ( i < max ) && !isCloseBracket_( buf[ i ] ) )
    {
      ret = ng_TRUE;
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
                ng_size_t * start,
                ng_size_t max )
{
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  while( i < max )
  {
    if( skipAnyScalar( buf, &i, max ) != ng_TRUE )
    {
      break;
    }

    if( skipSpaceAndComma( buf, &i, max ) != ng_TRUE )
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
                 ng_size_t * start,
                 ng_size_t max )
{
  ng_size_t i = 0U;
  ng_bool_t comma = ng_FALSE;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

  i = *start;

  while( i < max )
  {
    if( skipString( buf, &i, max ) != ng_TRUE )
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

    if( skipAnyScalar( buf, &i, max ) != ng_TRUE )
    {
      break;
    }

    comma = skipSpaceAndComma( buf, &i, max );
    *start = i;

    if( comma != ng_TRUE )
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
             ng_size_t * start,
             ng_size_t max,
             char mode )
{
  NG_ASSERT( isOpenBracket_( mode ) );

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
static JSONStatus_e skipCollection( const char * buf,
                  ng_size_t * start,
                  ng_size_t max )
{
  JSONStatus_e ret = JSONPartial;
  char c, stack[ JSON_MAX_DEPTH ];
  ng_int16_t depth = -1;
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );

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

          if( ( skipSpaceAndComma( buf, &i, max ) == ng_TRUE ) &&
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
JSONStatus_e JSON_Validate( const char * buf,
              ng_size_t max )
{
  JSONStatus_e ret;
  ng_size_t i = 0U;

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
      if( skipAnyScalar( buf, &i, max ) == ng_TRUE )
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
static ng_bool_t nextValue( const char * buf,
             ng_size_t * start,
             ng_size_t max,
             ng_size_t * value,
             ng_size_t * valueLength )
{
  ng_bool_t ret = ng_TRUE;
  ng_size_t i = 0U, valueStart = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  NG_ASSERT( ( value != NULL ) && ( valueLength != NULL ) );

  i = *start;
  valueStart = i;

  if( skipAnyScalar( buf, &i, max ) == ng_TRUE )
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
    ret = ng_FALSE;
  }

  if( ret == ng_TRUE )
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
static ng_bool_t nextKeyValuePair( const char * buf,
                ng_size_t * start,
                ng_size_t max,
                ng_size_t * key,
                ng_size_t * keyLength,
                ng_size_t * value,
                ng_size_t * valueLength )
{
  ng_bool_t ret = ng_TRUE;
  ng_size_t i = 0U, keyStart = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( max > 0U ) );
  NG_ASSERT( ( key != NULL ) && ( keyLength != NULL ) );
  NG_ASSERT( ( value != NULL ) && ( valueLength != NULL ) );

  i = *start;
  keyStart = i;

  if( skipString( buf, &i, max ) == ng_TRUE )
  {
    *key = keyStart + 1U;
    *keyLength = i - keyStart - 2U;
  }
  else
  {
    ret = ng_FALSE;
  }

  if( ret == ng_TRUE )
  {
    skipSpace( buf, &i, max );

    if( ( i < max ) && ( buf[ i ] == ':' ) )
    {
      i++;
      skipSpace( buf, &i, max );
    }
    else
    {
      ret = ng_FALSE;
    }
  }

  if( ret == ng_TRUE )
  {
    ret = nextValue( buf, &i, max, value, valueLength );
  }

  if( ret == ng_TRUE )
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
static ng_bool_t objectSearch( const char * buf,
              ng_size_t max,
              const char * query,
              ng_size_t queryLength,
              ng_size_t * outValue,
              ng_size_t * outValueLength )
{
  ng_bool_t ret = ng_FALSE;

  ng_size_t i = 0U, key = 0U, keyLength = 0U, value = 0U, valueLength = 0U;

  NG_ASSERT( ( buf != NULL ) && ( query != NULL ) );
  NG_ASSERT( ( outValue != NULL ) && ( outValueLength != NULL ) );

  skipSpace( buf, &i, max );

  if( ( i < max ) && ( buf[ i ] == '{' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    while( i < max )
    {
      if( nextKeyValuePair( buf, &i, max, &key, &keyLength,
                  &value, &valueLength ) != ng_TRUE )
      {
        break;
      }

      if( ( queryLength == keyLength ) &&
        ( strnEq( query, &buf[ key ], keyLength ) == ng_TRUE ) )
      {
        ret = ng_TRUE;
        break;
      }

      if( skipSpaceAndComma( buf, &i, max ) != ng_TRUE )
      {
        break;
      }
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t arraySearch( const char * buf,
             ng_size_t max,
             ng_uint32_t queryIndex,
             ng_size_t * outValue,
             ng_size_t * outValueLength )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U, value = 0U, valueLength = 0U;
  ng_uint32_t currentIndex = 0U;

  NG_ASSERT( buf != NULL );
  NG_ASSERT( ( outValue != NULL ) && ( outValueLength != NULL ) );

  skipSpace( buf, &i, max );

  if( ( i < max ) && ( buf[ i ] == '[' ) )
  {
    i++;
    skipSpace( buf, &i, max );

    while( i < max )
    {
      if( nextValue( buf, &i, max, &value, &valueLength ) != ng_TRUE )
      {
        break;
      }

      if( currentIndex == queryIndex )
      {
        ret = ng_TRUE;
        break;
      }

      if( ( skipSpaceAndComma( buf, &i, max ) != ng_TRUE ) ||
        ( currentIndex == NG_UINT32_MAX ) )
      {
        break;
      }

      currentIndex++;
    }
  }

  if( ret == ng_TRUE )
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
static ng_bool_t skipQueryPart( const char * buf,
               ng_size_t * start,
               ng_size_t max,
               ng_size_t * outLength )
{
  ng_bool_t ret = ng_FALSE;
  ng_size_t i = 0U;

  NG_ASSERT( ( buf != NULL ) && ( start != NULL ) && ( outLength != NULL ) );
  NG_ASSERT( max > 0U );

  i = *start;

  while( ( i < max ) &&
       !isSeparator_( buf[ i ] ) &&
       !isSquareOpen_( buf[ i ] ) )
  {
    i++;
  }

  if( i > *start )
  {
    ret = ng_TRUE;
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
static JSONStatus_e multiSearch( const char * buf,
                 ng_size_t max,
                 const char * query,
                 ng_size_t queryLength,
                 ng_size_t * outValue,
                 ng_size_t * outValueLength )
{
  JSONStatus_e ret = JSONSuccess;
  ng_size_t i = 0U, start = 0U, queryStart = 0U, value = 0U, length = max;

  NG_ASSERT( ( buf != NULL ) && ( query != NULL ) );
  NG_ASSERT( ( outValue != NULL ) && ( outValueLength != NULL ) );
  NG_ASSERT( ( max > 0U ) && ( queryLength > 0U ) );

  while( i < queryLength )
  {
    ng_bool_t found = ng_FALSE;

    if( isSquareOpen_( query[ i ] ) )
    {
      ng_int32_t queryIndex = -1;
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
        ( ng_uint32_t ) queryIndex, &value, &length );
    }
    else
    {
      ng_size_t keyLength = 0;

      queryStart = i;

      if( ( skipQueryPart( query, &i, queryLength, &keyLength ) != ng_TRUE ) ||
        /* catch an empty key part or a trailing separator */
        ( i == ( queryLength - 1U ) ) )
      {
        ret = JSONBadParameter;
        break;
      }

      found = objectSearch( &buf[ start ], length, 
        &query[ queryStart ], keyLength, &value, &length );
    }

    if( found == ng_FALSE )
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
 * @return an enum of JSONTypes_e
 */
static JSONTypes_e getType(char c)
{
  JSONTypes_e t;

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
JSONStatus_e JSON_SearchConst( const char * buf,
                 ng_size_t max,
                 const char * query,
                 ng_size_t queryLength,
                 const char ** outValue,
                 ng_size_t * outValueLength,
                 JSONTypes_e * outType )
{
  JSONStatus_e ret;
  ng_size_t value = 0U;

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
    JSONTypes_e t = getType( buf[ value ] );

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
JSONStatus_e JSON_SearchT( char * buf,
               ng_size_t max,
               const char * query,
               ng_size_t queryLength,
               char ** outValue,
               ng_size_t * outValueLength,
               JSONTypes_e * outType )
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
static JSONStatus_e iterate( const char * buf,
               ng_size_t max,
               ng_size_t * start,
               ng_size_t * next,
               ng_size_t * outKey,
               ng_size_t * outKeyLength,
               ng_size_t * outValue,
               ng_size_t * outValueLength )
{
  JSONStatus_e ret = JSONNotFound;
  ng_bool_t found = ng_FALSE;

  NG_ASSERT( ( buf != NULL ) && ( max > 0U ) );
  NG_ASSERT( ( start != NULL ) && ( next != NULL ) );
  NG_ASSERT( ( outKey != NULL ) && ( outKeyLength != NULL ) );
  NG_ASSERT( ( outValue != NULL ) && ( outValueLength != NULL ) );

  if( *start < max )
  {
    switch( buf[ *start ] )
    {
      case '[':
      found = nextValue( buf, next, max, outValue, outValueLength );

      if( found == ng_TRUE )
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

  if( found == ng_TRUE )
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
JSONStatus_e JSON_Iterate(const char *buf,
               ng_size_t  max,
               ng_size_t *start,
               ng_size_t *next,
               JSONPair_s *outPair)
{
  JSONStatus_e ret;
  ng_size_t key = 0U, keyLength = 0U, value = 0U, valueLength = 0U;

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
    JSONTypes_e t = getType( buf[ value ] );

    if( t == JSONString )
    {
      /* strip the surrounding quotes */
      value++;
      valueLength -= 2U;
    }

    outPair->key.cptr = (key == 0U ) ? NULL : &buf[key];
    outPair->key.len  = keyLength;
    outPair->val.cptr = &buf[ value ];
    outPair->val.len  = valueLength;
    outPair->jsonType = t;
  }

  return ret;
}

static char *json_types[] =
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
const char *json_type2str(JSONTypes_e type)
{
  return (type >= JSONTypes_MAX) ? "<unknown>" : json_types[type];
}

void json_pairs_free(JSONPair_s *pair)
{
  if (pair == NULL)
    return;
  if (pair->children)
    json_pairs_free(pair->children);
  if (pair->siblings)
    json_pairs_free(pair->siblings);
  if (pair->jsonType == JSONString && pair->str != NULL)
  {
    ng_free(pair->str);
    pair->str = NULL;
  }
  ng_free(pair);
}

#define json_MIN(a,b) ((a)<(b) ? (a) : (b))

static JSONStatus_e __json_parse(JSONTypes_e parent_jsonType, 
  JSONPair_s **__pair, const char *json, ng_size_t length)
{
  ng_size_t start = 0, next = 0;
  JSONStatus_e result;
  JSONPair_s *prev = NULL;
  JSONPair_s *cur = NULL;

  result = JSON_Validate(json, length);
  if (result != JSONSuccess)
  {
    *__pair = NULL;
    return result;
  }

  while (result != JSONNotFound)
  {
    JSONPair_s *pair = ng_malloc(sizeof(*pair));
    if (pair == NULL)
    {
      result = JSONBadParameter;
      goto clean0;
    }
    ng_memset(pair,0,sizeof(*pair));

    result = JSON_Iterate(json, length, &start, &next, pair);
    if (result != JSONSuccess)
    {
      ng_free(pair);
      if (result == JSONNotFound)
      {
        result = JSONSuccess;
        break;
      }
      goto clean0;
    }
    else
    {
      if ((pair->key.cptr == NULL || pair->key.len == 0)
        && (pair->jsonType != JSONArray 
            && pair->jsonType != JSONObject
            && parent_jsonType != JSONArray))
      {
        ng_free(pair);
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
          if (NULL != ng_memchr(pair->val.cptr, '.', pair->val.len))
          {
            pair->isDouble = 1;
            if (s2d_n(pair->val.cptr, pair->val.len, &pair->vdouble) != SUCCESS)
            {
              result = JSONBadParameter;
              goto clean0;
            }
          }
          else
          {
            const char *p = NULL;
            pair->isDouble = 0;
            pair->vint = ng_atoi64(pair->val.cptr, pair->val.len, &p);
            if (p != pair->val.cptr + pair->val.len)
            {
              result = JSONBadParameter;
              goto clean0;
            }
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
          JSONPair_s *child;

          result = __json_parse(pair->jsonType, &child, pair->val.cptr, pair->val.len);
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

JSONStatus_e json_parse(JSONPair_s **__pair, const char *json, ng_size_t length)
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

static int 
__json_pad_print(JSON_PRINTER_f printer, ng_buffer_s *b, 
  int depth, const char *pad, int pad_length)
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

int json_printer_default(ng_buffer_s *b, const char *fmt, ...)
{
  va_list args;

  HTTPD_UNUSED(b);

  va_start(args, fmt);
  _vnanohttp_log_printf(NANOHTTP_LOG_INFO, fmt, args);
  va_end(args);
  return 0;
}

static int 
__json_print(JSON_PRINTER_f printer, ng_buffer_s *b, 
  JSONPair_s *pair, int depth, const char *pad, 
  int pad_length)
{
  ng_size_t n, k=0;
  
  while (pair != NULL)
  {
    n = __json_pad_print(printer, b, depth, pad, pad_length);
    if (n < 0) return n;
    k += n;

    if (pair->key.len)
    {
      n = printer(b, "\"%pS\":", &pair->key);
      if (n < 0) return n;
      k += n;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        return -1;
        
      case JSONString:
        if (pair->val.len)
        {
          n = printer(b,"%s","\"");
          if (n < 0) return n;
          k += n;

          for (int i=0;i<pair->val.len;i++)
          {
            n = printer(b, "%s", 
              character_escape[(int)pair->val.cptr[i]]);
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
          n = printer(b, "%lf", pair->vdouble);
          if (n < 0) return n;
          k += n;
        }
        else
        {
          n = printer(b, "%lld", pair->vint);
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
          depth != -1 ? depth+1 : -1, pad, pad_length);
        if (n < 0) return n;
        k += n;
        
        n = __json_pad_print(printer, b, depth, 
          pad, pad_length);
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
          depth != -1 ? depth+1 : -1, pad, pad_length);
        if (n < 0) return n;
        k += n;
        
        n = __json_pad_print(printer, b, depth, 
          pad, pad_length);
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

JSONStatus_e 
json_print(JSONPair_s *pair, int depth, 
  const char *pad, int pad_length)
{
  int n;
  ng_buffer_s b;
  
  n = json_printer_default(&b, "%s", "{");
  if (n < 0) return JSONPartial;
  if (depth != -1)
  {
    n = json_printer_default(&b, "%s", "\n");
    if (n < 0) return JSONPartial;
  }
  n = __json_print(json_printer_default, &b, pair, 
    depth != -1 ? depth + 1 : -1, pad, pad_length);
  if (n < 0) return JSONPartial;

  n = json_printer_default(&b, "%s", "}");
  if (n < 0) return JSONPartial;
  
  return JSONSuccess;
}

static inline int 
json_printer_buffer(ng_buffer_s *b, const char *fmt, ...)
{
  ng_size_t n;
  va_list args;

  va_start(args, fmt);
  n = ng_vsnprintf(b->p, b->len, fmt, args);
  va_end(args);
  b->p += n, b->len -= n;
  return n;
}

int 
json_tostr(JSONPair_s *pair, char *buf, ng_size_t length, 
  int depth, const char *pad, int pad_length)
{
  int k=0;
  ng_buffer_s b = {.buf = buf, .len = length, .p = buf};
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

  n = __json_print(json_printer_buffer, &b, pair, 
    depth != -1 ? depth+1 : -1, pad, pad_length);
  if (n <= 0) return -1;
  k += n;

  n = json_printer_buffer(&b, "%s", "}");
  if (n <= 0) return -1;
  k += n;

  return k;
}

int 
json_to_printer(JSON_PRINTER_f printer, 
  ng_buffer_s *b, JSONPair_s *pair, int depth, 
  const char *pad, int pad_length)
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

  n = __json_print(printer, b, pair, 
    depth != -1 ? depth+1 : -1, pad, pad_length);
  if (n < 0) return -1;
  k += n;

  n = printer(b, "%s", "}");
  if (n < 0) return -1;
  k += n;

  return k;
}

static int 
__json_pad_length(int depth, const char *pad, 
  int padLen)
{
  if (depth == -1)
    return 0;

  return depth * padLen;
}

static int 
__json_cal_length(JSONPair_s *pair, int depth, 
  const char *pad, int padLen)
{
  int n = 0;
  char temp[256];
  
  while (pair != NULL)
  {
    n += __json_pad_length(depth, pad, padLen);
    if (pair->key.len)
    {
      n += 3 + pair->key.len;
    }

    switch (pair->jsonType)
    {
      default:
      case JSONInvalid:
        goto clean0;
        
      case JSONString:
        n += 2 + pair->val.len;
        break;
        
      case JSONNumber:
        if (pair->isDouble)
        {
          n += ng_snprintf(temp, sizeof temp, 
            "%lf", pair->vdouble);
        }
        else
        {
          n += ng_snprintf(temp, sizeof temp, 
            "%lld", pair->vint);
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

        n += __json_cal_length(pair->children, 
          depth != -1 ? depth+1 : -1, pad, padLen);
        if (n < 0)
          goto clean0;
        
        n += __json_pad_length(depth , pad, padLen);
        n += 1;
        break;
        
      case JSONObject:
        n += 1;
        if (depth != -1)
        {
          n += 1;
        }

        n += __json_cal_length(pair->children, 
          depth != -1 ? depth+1 : -1, pad, padLen);
        if (n < 0)
          goto clean0;
        
        n += __json_pad_length(depth, pad, padLen);
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

int 
json_cal_length(JSONPair_s *pair, int depth, 
  const char *pad, int padLen)
{
  int n = 0;
  
  n += 1;
  if (depth != -1)
  {
    n += 1;
  }
  
  n += __json_cal_length(pair, 
    depth != -1 ? depth+1 : -1, pad, padLen);

  n += 1;
  return n;
}

JSONPair_s *
json_find_bykey(JSONPair_s *pair, const char *key, 
  ng_size_t length)
{
  while (pair != NULL)
  {
    if (pair->key.len == length
      && !ng_memcmp(pair->key.cptr, key, length))
      return pair;
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_s *
json_find_bykey_head(JSONPair_s *pair, const char *key, 
  ng_size_t length)
{
  while (pair != NULL)
  {
    if (pair->key.len >= length
      && !ng_memcmp(pair->key.cptr, key, length))
      return pair;
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_s *
json_find_bykey_head_tail(JSONPair_s *pair, 
  const char *key, ng_size_t headLen, const char *tailKey, 
  ng_size_t tailKeyLen)
{
  while (pair != NULL)
  {
    if (pair->key.len >= headLen + tailKeyLen
      && !ng_memcmp(pair->key.cptr, key, headLen)
      && !ng_memcmp(pair->key.cptr+pair->key.len-tailKeyLen, 
            tailKey, tailKeyLen))
    {
      return pair;
    }
    pair = pair->siblings;
  }
  return NULL;
}

JSONPair_s *
json_find_bykey_head_offset(JSONPair_s *pair, 
  const char *key, ng_size_t headLen, ng_size_t startOffset, 
  const char *startKey, ng_size_t startKeyLen)
{
  while (pair != NULL)
  {
    if (pair->key.len >= headLen + startKeyLen
      && pair->key.len >= startOffset + startKeyLen
      && !ng_memcmp(pair->key.cptr, key, headLen)
      && !ng_memcmp(pair->key.cptr+startOffset, 
            startKey, startKeyLen))
    {
      return pair;
    }
    pair = pair->siblings;
  }
  return NULL;
}

