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
#include "nanohttp-defs.h"
#include "nanohttp-ctype.h"

#define NG_ATOI_USE_WHILE 1

static const ng_uint64_t __ng_cache_aligned __offsets[32] = {
  '0'*UINT64_C(0),
  '0'*UINT64_C(1),
  '0'*UINT64_C(11),
  '0'*UINT64_C(111),
  '0'*UINT64_C(1111),
  '0'*UINT64_C(11111),
  '0'*UINT64_C(111111),
  '0'*UINT64_C(1111111),
  '0'*UINT64_C(11111111),
  '0'*UINT64_C(111111111),
  '0'*UINT64_C(1111111111),
  '0'*UINT64_C(11111111111),
  '0'*UINT64_C(111111111111),
  '0'*UINT64_C(1111111111111),
  '0'*UINT64_C(11111111111111),
  '0'*UINT64_C(111111111111111),
  '0'*UINT64_C(1111111111111111),
  '0'*UINT64_C(11111111111111111),
  '0'*UINT64_C(111111111111111111),
  '0'*UINT64_C(1111111111111111111),
  '0'*UINT64_C(11111111111111111111)};

#define _(i,j) (j##u)*((ng_uint32_t)(1e##i))
#define __(i) { _(i,0),_(i,1),_(i,2),_(i,3),_(i,4),_(i,5),_(i,6),_(i,7),_(i,8),_(i,9) }
static ng_uint32_t __ng_cache_aligned uint32_offsets[11][10] = {
  {0,0,0,0,0,0,0,0,0,0},
  __(0),__(1),__(2),__(3),__(4),__(5),__(6),__(7),__(8),__(9)
};
#undef _
#undef __


#define _(i,j) (j##ull)*((ng_uint64_t)(1e##i))
#define __(i) { _(i,0),_(i,1),_(i,2),_(i,3),_(i,4),_(i,5),_(i,6),_(i,7),_(i,8),_(i,9) }
static ng_uint64_t uint64_offsets[21][10] = {
  {0,0,0,0,0,0,0,0,0,0},
  __( 0),__( 1),__( 2),__( 3),__( 4),__( 5),__( 6),__( 7),__( 8),__( 9),
  __(10),__(11),__(12),__(13),__(14),__(15),__(16),__(17),__(18),__(19)
};
#undef _
#undef __

#if NG_ATOI_USE_WHILE
//convert char *s to an unsigned 64bit integer
//len is the number of numeric characters
//s does not require the trailing '\0'
ng_uint64_t ng_atou64(const char *str, ng_uint8_t len, const char **end)
{
  ng_uint64_t value = 0;
  ng_uint8_t c;

  if (len > NG_UINT64_STR_LEN_MAX)
  {
    if (end)
      *end = str;
    return value;
  } 

  for (ng_uint8_t n = len; n > 0; n--)
  {
    c = str[len-n];
    if (!__ng_isdigit(c))
    {
      if (end)
        *end = str + len - n;
      return value;
    }
    value += uint64_offsets[n][c-'0'];
  }

  if (end)
    *end = str + len;
  return value;
}

//convert char *s to an unsigned 32bit integer
//len is the number of numeric characters
//s does not require the trailing '\0'
ng_uint32_t ng_atou32(const char *str, ng_uint8_t len, const char **end)
{
  ng_uint32_t value = 0;
  ng_uint8_t c;

  for (ng_uint8_t n = len; n > 0; n--)
  {
    c = str[len-n];
    if (!__ng_isdigit(c))
    {
      if (end)
        *end = str + len - n;
      return value;
    }
    value += uint32_offsets[n][c-'0'];
  }

  if (end)
    *end = str + len;
  return value;
}

#else

//convert char *s to an unsigned 64bit integer
//len is the number of numeric characters
//s does not require the trailing '\0'
ng_uint64_t ng_atou64(const char *str, ng_uint8_t len, const char **end)
{
  ng_size_t value = 0;
  ng_uint8_t c;

  if (len > NG_UINT64_STR_LEN_MAX)
  {
    if (end)
      *end = str;
    return value;
  } 

#define _VA(n) do { \
  c = str[len-n]; \
  if (!__ng_isdigit(c)) \
  { \
    if (end) \
      *end = str + len - n; \
    return value; \
  } \
  uint64_offsets[n][c-'0']; \
}while(0)

  switch (len) { // handle up to 20 digits, assume we're 64-bit
      case 20:    value += _VA(20);// FALL THROUGH
      case 19:    value += _VA(19);// FALL THROUGH
      case 18:    value += _VA(18);// FALL THROUGH
      case 17:    value += _VA(17);// FALL THROUGH
      case 16:    value += _VA(16);// FALL THROUGH
      case 15:    value += _VA(15);// FALL THROUGH
      case 14:    value += _VA(14);// FALL THROUGH
      case 13:    value += _VA(13);// FALL THROUGH
      case 12:    value += _VA(12);// FALL THROUGH
      case 11:    value += _VA(11);// FALL THROUGH
      case 10:    value += _VA(10);// FALL THROUGH
      case  9:    value += _VA( 9);// FALL THROUGH
      case  8:    value += _VA( 8);// FALL THROUGH
      case  7:    value += _VA( 7);// FALL THROUGH
      case  6:    value += _VA( 6);// FALL THROUGH
      case  5:    value += _VA( 5);// FALL THROUGH
      case  4:    value += _VA( 4);// FALL THROUGH
      case  3:    value += _VA( 3);// FALL THROUGH
      case  2:    value += _VA( 2);// FALL THROUGH
      case  1:    value += _VA( 1);// FALL THROUGH
      default:    break;
  }
#undef _VA

  if (end)
    *end = str + len;
  return value;
}

//convert char *s to an unsigned 32bit integer
//len is the number of numeric characters
//s does not require the trailing '\0'
ng_uint32_t ng_atou32(const char *str, ng_uint8_t len, const char **end)
{
  ng_uint32_t value = 0;
  ng_uint8_t c;

  if (len > NG_UINT32_STR_LEN_MAX)
  {
    if (end)
      *end = str;
    return value;
  } 
  
#define _VA(n) do { \
  c = str[len-n]; \
  if (!__ng_isdigit(c)) \
  { \
    if (end) \
      *end = str + len - n; \
    return value; \
  } \
  uint32_offsets[n][c-'0']; \
}while(0)
  switch (len) { // handle up to 10 digits, assume we're 32-bit
    case 10:    value += _VA(10);// FALL THROUGH
    case  9:    value += _VA( 9);// FALL THROUGH
    case  8:    value += _VA( 8);// FALL THROUGH
    case  7:    value += _VA( 7);// FALL THROUGH
    case  6:    value += _VA( 6);// FALL THROUGH
    case  5:    value += _VA( 5);// FALL THROUGH
    case  4:    value += _VA( 4);// FALL THROUGH
    case  3:    value += _VA( 3);// FALL THROUGH
    case  2:    value += _VA( 2);// FALL THROUGH
    case  1:    value += _VA( 1);// FALL THROUGH
    default:    break;
  }
  if (end)
    *end = str + len;
  return value - (ng_uint32_t)(__offsets[len]);
#undef _VA
}
#endif

//convert char *s to an unsigned 64bit integer
ng_uint64_t ng_atou64_s(const char *s, const char **end)
{
  ng_uint64_t ret = s[0];
  ng_uint8_t pos = 1;
  while(s[pos])
  {
    if (!__ng_isdigit(s[pos]) || pos > NG_UINT64_STR_LEN_MAX)
    {
      if (end)
        *end = s + pos;
      return ret-__offsets[pos];
    }
    ret = ret*10 + s[pos++];
  }

  if (end)
    *end = s + pos;
  return ret-__offsets[pos];
}

//convert char *s to an unsigned 32bit integer
ng_uint32_t ng_atou32_s(const char *s, const char **end)
{
  ng_uint32_t ret = s[0];
  ng_uint8_t pos = 1;
  
  while(s[pos])
  {
    if (!__ng_isdigit(s[pos]) || pos > NG_UINT32_STR_LEN_MAX)
    {
      if (end)
        *end = s + pos;
      return ret-__offsets[pos];
    }
    ret = ret*10 + s[pos++];
  }

  if (end)
    *end = s + pos;
  return ret-(ng_uint32_t)(__offsets[pos]);
}

#define _(i,j) (j*(1ull<<(4*i)))
#define __(i) \
  { _(i, 0),_(i, 1),_(i, 2),_(i, 3),_(i, 4),_(i, 5),_(i, 6),_(i, 7), \
    _(i, 8),_(i, 9),_(i,10),_(i,11),_(i,12),_(i,13),_(i,14),_(i,15) }
static ng_uint64_t __ng_cache_aligned uint64_offsets_hex[17][16] =
{
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  __( 0),__( 1),__( 2),__( 3),__( 4),__( 5),__( 6),__( 7),
  __( 8),__( 9),__(10),__(11),__(12),__(13),__(14),__(15)
};
#undef __
#undef _

ng_uint64_t ng_htou64(const char *str, ng_uint8_t len, const char **end)
{
  ng_uint64_t value = 0;
  
  if (len > 16)
  {
    if (end)
      *end = str;
    return 0;
  }
  
  for (ng_uint8_t n = len; n > 0; n--)
  {
    int c = __ng_hex2int(str[len-n]);
    if (c == 0xFF)
    {
      if (end)
        *end = &str[len-n];
      return value;
    }
    value += uint64_offsets_hex[n][c];
  }

  if (end)
    *end = str+len;
  return value;
}

#define _(h) (h)
static const char *int_to_hex_upper[256] = {
  _("00"),_("01"),_("02"),_("03"),_("04"),_("05"),_("06"),_("07"),
  _("08"),_("09"),_("0A"),_("0B"),_("0C"),_("0D"),_("0E"),_("0F"),
  _("10"),_("11"),_("12"),_("13"),_("14"),_("15"),_("16"),_("17"),
  _("18"),_("19"),_("1A"),_("1B"),_("1C"),_("1D"),_("1E"),_("1F"),
  _("20"),_("21"),_("22"),_("23"),_("24"),_("25"),_("26"),_("27"),
  _("28"),_("29"),_("2A"),_("2B"),_("2C"),_("2D"),_("2E"),_("2F"),
  _("30"),_("31"),_("32"),_("33"),_("34"),_("35"),_("36"),_("37"),
  _("38"),_("39"),_("3A"),_("3B"),_("3C"),_("3D"),_("3E"),_("3F"),
  _("40"),_("41"),_("42"),_("43"),_("44"),_("45"),_("46"),_("47"),
  _("48"),_("49"),_("4A"),_("4B"),_("4C"),_("4D"),_("4E"),_("4F"),
  _("50"),_("51"),_("52"),_("53"),_("54"),_("55"),_("56"),_("57"),
  _("58"),_("59"),_("5A"),_("5B"),_("5C"),_("5D"),_("5E"),_("5F"),
  _("60"),_("61"),_("62"),_("63"),_("64"),_("65"),_("66"),_("67"),
  _("68"),_("69"),_("6A"),_("6B"),_("6C"),_("6D"),_("6E"),_("6F"),
  _("70"),_("71"),_("72"),_("73"),_("74"),_("75"),_("76"),_("77"),
  _("78"),_("79"),_("7A"),_("7B"),_("7C"),_("7D"),_("7E"),_("7F"),
  _("80"),_("81"),_("82"),_("83"),_("84"),_("85"),_("86"),_("87"),
  _("88"),_("89"),_("8A"),_("8B"),_("8C"),_("8D"),_("8E"),_("8F"),
  _("90"),_("91"),_("92"),_("93"),_("94"),_("95"),_("96"),_("97"),
  _("98"),_("99"),_("9A"),_("9B"),_("9C"),_("9D"),_("9E"),_("9F"),
  _("A0"),_("A1"),_("A2"),_("A3"),_("A4"),_("A5"),_("A6"),_("A7"),
  _("A8"),_("A9"),_("AA"),_("AB"),_("AC"),_("AD"),_("AE"),_("AF"),
  _("B0"),_("B1"),_("B2"),_("B3"),_("B4"),_("B5"),_("B6"),_("B7"),
  _("B8"),_("B9"),_("BA"),_("BB"),_("BC"),_("BD"),_("BE"),_("BF"),
  _("C0"),_("C1"),_("C2"),_("C3"),_("C4"),_("C5"),_("C6"),_("C7"),
  _("C8"),_("C9"),_("CA"),_("CB"),_("CC"),_("CD"),_("CE"),_("CF"),
  _("D0"),_("D1"),_("D2"),_("D3"),_("D4"),_("D5"),_("D6"),_("D7"),
  _("D8"),_("D9"),_("DA"),_("DB"),_("DC"),_("DD"),_("DE"),_("DF"),
  _("E0"),_("E1"),_("E2"),_("E3"),_("E4"),_("E5"),_("E6"),_("E7"),
  _("E8"),_("E9"),_("EA"),_("EB"),_("EC"),_("ED"),_("EE"),_("EF"),
  _("F0"),_("F1"),_("F2"),_("F3"),_("F4"),_("F5"),_("F6"),_("F7"),
  _("F8"),_("F9"),_("FA"),_("FB"),_("FC"),_("FD"),_("FE"),_("FF")
};
#undef _
#define _(h) (h)
static const char *int_to_hex_lower[256] = {
  _("00"),_("01"),_("02"),_("03"),_("04"),_("05"),_("06"),_("07"),
  _("08"),_("09"),_("0a"),_("0b"),_("0c"),_("0d"),_("0e"),_("0f"),
  _("10"),_("11"),_("12"),_("13"),_("14"),_("15"),_("16"),_("17"),
  _("18"),_("19"),_("1a"),_("1b"),_("1c"),_("1d"),_("1e"),_("1f"),
  _("20"),_("21"),_("22"),_("23"),_("24"),_("25"),_("26"),_("27"),
  _("28"),_("29"),_("2a"),_("2b"),_("2c"),_("2d"),_("2e"),_("2f"),
  _("30"),_("31"),_("32"),_("33"),_("34"),_("35"),_("36"),_("37"),
  _("38"),_("39"),_("3a"),_("3b"),_("3c"),_("3d"),_("3e"),_("3f"),
  _("40"),_("41"),_("42"),_("43"),_("44"),_("45"),_("46"),_("47"),
  _("48"),_("49"),_("4a"),_("4b"),_("4c"),_("4d"),_("4e"),_("4f"),
  _("50"),_("51"),_("52"),_("53"),_("54"),_("55"),_("56"),_("57"),
  _("58"),_("59"),_("5a"),_("5b"),_("5c"),_("5d"),_("5e"),_("5f"),
  _("60"),_("61"),_("62"),_("63"),_("64"),_("65"),_("66"),_("67"),
  _("68"),_("69"),_("6a"),_("6b"),_("6c"),_("6d"),_("6e"),_("6f"),
  _("70"),_("71"),_("72"),_("73"),_("74"),_("75"),_("76"),_("77"),
  _("78"),_("79"),_("7a"),_("7b"),_("7c"),_("7d"),_("7e"),_("7f"),
  _("80"),_("81"),_("82"),_("83"),_("84"),_("85"),_("86"),_("87"),
  _("88"),_("89"),_("8a"),_("8b"),_("8c"),_("8d"),_("8e"),_("8f"),
  _("90"),_("91"),_("92"),_("93"),_("94"),_("95"),_("96"),_("97"),
  _("98"),_("99"),_("9a"),_("9b"),_("9c"),_("9d"),_("9e"),_("9f"),
  _("a0"),_("a1"),_("a2"),_("a3"),_("a4"),_("a5"),_("a6"),_("a7"),
  _("a8"),_("a9"),_("aa"),_("ab"),_("ac"),_("ad"),_("ae"),_("af"),
  _("b0"),_("b1"),_("b2"),_("b3"),_("b4"),_("b5"),_("b6"),_("b7"),
  _("b8"),_("b9"),_("ba"),_("bb"),_("bc"),_("bd"),_("be"),_("bf"),
  _("c0"),_("c1"),_("c2"),_("c3"),_("c4"),_("c5"),_("c6"),_("c7"),
  _("c8"),_("c9"),_("ca"),_("cb"),_("cc"),_("cd"),_("ce"),_("cf"),
  _("d0"),_("d1"),_("d2"),_("d3"),_("d4"),_("d5"),_("d6"),_("d7"),
  _("d8"),_("d9"),_("da"),_("db"),_("dc"),_("dd"),_("de"),_("df"),
  _("e0"),_("e1"),_("e2"),_("e3"),_("e4"),_("e5"),_("e6"),_("e7"),
  _("e8"),_("e9"),_("ea"),_("eb"),_("ec"),_("ed"),_("ee"),_("ef"),
  _("f0"),_("f1"),_("f2"),_("f3"),_("f4"),_("f5"),_("f6"),_("f7"),
  _("f8"),_("f9"),_("fa"),_("fb"),_("fc"),_("fd"),_("fe"),_("ff")
};
#undef _

int ng_u64toh(ng_uint64_t v, char *str, ng_uint8_t len, int is_upper, char seporator)
{
  NG_ASSERT((!seporator && len >= 16) || (seporator && len >= 23));

  int start = 0;
  ng_uint8_t  *p, *e;
  ng_uint16_t *o;
  const char **int_to_hex = is_upper ? int_to_hex_upper : int_to_hex_lower;
  
  v = ng_htonll(v);
  p = (ng_uint8_t  *)&v;
  e = p + 8;
  o = (ng_uint16_t *)str;
  while (p < e)
  {
    ng_uint8_t c = *p++;
    if (!c && !start)
      continue;
    start = 1;
    *o++ = *(const ng_uint16_t *)int_to_hex[c];
    if (seporator)
    {
      *(char *)o = seporator;
      o = (ng_uint16_t *)((char *)o+1);
    }
  }

  return ((char *)o) - str;
}

int ng_u32toh(ng_uint32_t v, char *str, ng_uint8_t len, int is_upper, char seporator)
{
  NG_ASSERT((!seporator && len >= 8) || (seporator && len >= 11));

  int start = 0;
  ng_uint8_t  *p, *e;
  ng_uint16_t *o;
  const char **int_to_hex = is_upper ? int_to_hex_upper : int_to_hex_lower;
  
  v = ng_htonl(v);
  p = (ng_uint8_t  *)&v;
  e = p + 4;
  o = (ng_uint16_t *)str;
  while (p < e)
  {
    ng_uint8_t c = *p++;
    if (!c && !start)
      continue;
    start = 1;
    *o++ = *(const ng_uint16_t *)int_to_hex[c];
    if (seporator)
    {
      *(char *)o = seporator;
      o = (ng_uint16_t *)((char *)o+1);
    }
  }

  return ((char *)o) - str;
}


