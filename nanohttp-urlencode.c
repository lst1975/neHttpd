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
#include "nanohttp-urlencode.h"
#include "nanohttp-logging.h"

static ng_uint8_t utf8_len(const ng_uint8_t* str, int inlen);

/*
 Alphanumeric characters: A-Z, a-z, 0-9
 Special characters: -, _, ., ~
 Reserved characters: !, ', (, ), *, ;, :, @, &, =, +, $, ,, /, ?, #
 */
static const ng_uint8_t url_unreserved[256] = {
  /* 0*/  /* 1*/  /* 2*/  /* 3*/  /* 4*/  /* 5*/  /* 6*/  /* 7*/  /* 0x00-0x0F */
     0,      0,      0,      0,      0,      0,      0,      0,
  /* 8*/  /*\t*/  /*\n*/  /* b*/  /* c*/  /*\r*/  /* e*/  /* f*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*10*/  /*11*/  /*12*/  /*13*/  /*14*/  /*15*/  /*16*/  /*17*/  /* 0x10-0x1F */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*18*/  /*19*/  /*1a*/  /*1b*/  /*1c*/  /*1d*/  /*1e*/  /*1f*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*  */  /* !*/  /* "*/  /* #*/  /* $*/  /* %*/  /* &*/  /* '*/  /* 0x20-0x2F */
     0,      1,      0,      1,      1,      0,      1,      1,
  /* (*/  /* )*/  /* **/  /* +*/  /* ,*/  /* -*/  /* .*/  /* /*/ 
     1,      1,      1,      1,      1,      1,      1,      1,
  /* 0*/  /* 1*/  /* 2*/  /* 3*/  /* 4*/  /* 5*/  /* 6*/  /* 7*/  /* 0x30-0x3F */
     1,      1,      1,      1,      1,      1,      1,      1,
  /* 8*/  /* 9*/  /* :*/  /* ;*/  /* <*/  /* =*/  /* >*/  /* ?*/ 
     1,      1,      1,      1,      0,      1,      0,      1,
  /* @*/  /* A*/  /* B*/  /* C*/  /* D*/  /* E*/  /* F*/  /* G*/  /* 0x40-0x4F */
     1,      1,      1,      1,      1,      1,      1,      1,
  /* H*/  /* I*/  /* J*/  /* K*/  /* L*/  /* M*/  /* N*/  /* O*/ 
     1,      1,      1,      1,      1,      1,      1,      1,
  /* P*/  /* Q*/  /* R*/  /* S*/  /* T*/  /* U*/  /* V*/  /* W*/  /* 0x50-0x5F */
     1,      1,      1,      1,      1,      1,      1,      1,
  /* X*/  /* Y*/  /* Z*/  /* [*/  /* \*/  /* ]*/  /* ^*/  /* _*/ 
     1,      1,      1,      0,      0,      0,      0,      1,
  /* `*/  /* a*/  /* b*/  /* c*/  /* d*/  /* e*/  /* f*/  /* g*/  /* 0x60-0x6F */
     0,      1,      1,      1,      1,      1,      1,      1,
  /* h*/  /* i*/  /* j*/  /* k*/  /* l*/  /* m*/  /* n*/  /* o*/ 
     1,      1,      1,      1,      1,      1,      1,      1,
  /* p*/  /* q*/  /* r*/  /* s*/  /* t*/  /* u*/  /* v*/  /* w*/  /* 0x70-0x7F */
     1,      1,      1,      1,      1,      1,      1,      1,
  /* x*/  /* y*/  /* z*/  /* {*/  /* |*/  /* }*/  /* ~*/  /*7f*/ 
     1,      1,      1,      0,      0,      0,      1,      0,
  /*80*/  /*81*/  /*82*/  /*83*/  /*84*/  /*85*/  /*86*/  /*87*/  /* 0x80-0x8F */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*88*/  /*89*/  /*8a*/  /*8b*/  /*8c*/  /*8d*/  /*8e*/  /*8f*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*90*/  /*91*/  /*92*/  /*93*/  /*94*/  /*95*/  /*96*/  /*97*/  /* 0x90-0x9F */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*98*/  /*99*/  /*9a*/  /*9b*/  /*9c*/  /*9d*/  /*9e*/  /*9f*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*a0*/  /*a1*/  /*a2*/  /*a3*/  /*a4*/  /*a5*/  /*a6*/  /*a7*/  /* 0xA0-0xAF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*a8*/  /*a9*/  /*aa*/  /*ab*/  /*ac*/  /*ad*/  /*ae*/  /*af*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*b0*/  /*b1*/  /*b2*/  /*b3*/  /*b4*/  /*b5*/  /*b6*/  /*b7*/  /* 0xB0-0xBF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*b8*/  /*b9*/  /*ba*/  /*bb*/  /*bc*/  /*bd*/  /*be*/  /*bf*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*c0*/  /*c1*/  /*c2*/  /*c3*/  /*c4*/  /*c5*/  /*c6*/  /*c7*/  /* 0xC0-0xCF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*c8*/  /*c9*/  /*ca*/  /*cb*/  /*cc*/  /*cd*/  /*ce*/  /*cf*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*d0*/  /*d1*/  /*d2*/  /*d3*/  /*d4*/  /*d5*/  /*d6*/  /*d7*/  /* 0xD0-0xDF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*d8*/  /*d9*/  /*da*/  /*db*/  /*dc*/  /*dd*/  /*de*/  /*df*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*e0*/  /*e1*/  /*e2*/  /*e3*/  /*e4*/  /*e5*/  /*e6*/  /*e7*/  /* 0xE0-0xEF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*e8*/  /*e9*/  /*ea*/  /*eb*/  /*ec*/  /*ed*/  /*ee*/  /*ef*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
  /*f0*/  /*f1*/  /*f2*/  /*f3*/  /*f4*/  /*f5*/  /*f6*/  /*f7*/  /* 0xF0-0xFF */
     0,      0,      0,      0,      0,      0,      0,      0,
  /*f8*/  /*f9*/  /*fa*/  /*fb*/  /*fc*/  /*fd*/  /*fe*/  /*ff*/ 
     0,      0,      0,      0,      0,      0,      0,      0,
};

int is_url_char_unreserved(ng_uint8_t c)
{
  return !url_unreserved[c];
}

struct _str_enc{
  union {
    struct {
      union{
        const char cptr[0];
        char buf1[1];
        char buf4[4];
        char buf8[6];
        ng_uint8_t  v1;
        ng_uint32_t v4;
      };
      ng_uint16_t len;
    };
    ng_uint64_t v8;
  };
};

static const struct _str_enc url_str_enc[256] = {
  /* 0   1   2   3   4   5   6   7   0x00-0x0F */
  {.len=3,.buf4={ "%00"}},    {.len=3,.buf4={ "%01"}},    
  {.len=3,.buf4={ "%02"}},    {.len=3,.buf4={ "%03"}}, 
  {.len=3,.buf4={ "%04"}},    {.len=3,.buf4={ "%05"}},    
  {.len=3,.buf4={ "%06"}},    {.len=3,.buf4={ "%07"}},
  /* 8  \t  \n   b   c  \r   e   f*/ 
  {.len=3,.buf4={ "%08"}},    {.len=3,.buf4={ "%09"}},    
  {.len=3,.buf4={ "%0A"}},    {.len=3,.buf4={ "%0B"}},
  {.len=3,.buf4={ "%0C"}},    {.len=3,.buf4={ "%0D"}},    
  {.len=3,.buf4={ "%0E"}},    {.len=3,.buf4={ "%0F"}},
  /*10  11  12  13  14  15  16  17   0x10-0x1F */
  {.len=3,.buf4={ "%10"}},    {.len=3,.buf4={ "%11"}},    
  {.len=3,.buf4={ "%12"}},    {.len=3,.buf4={ "%13"}},
  {.len=3,.buf4={ "%14"}},    {.len=3,.buf4={ "%15"}},    
  {.len=3,.buf4={ "%16"}},    {.len=3,.buf4={ "%17"}},
  /*18  19  1a  1b  1c  1d  1e  1f*/ 
  {.len=3,.buf4={ "%18"}},    {.len=3,.buf4={ "%19"}},    
  {.len=3,.buf4={ "%1A"}},    {.len=3,.buf4={ "%1B"}},
  {.len=3,.buf4={ "%1C"}},    {.len=3,.buf4={ "%1D"}},    
  {.len=3,.buf4={ "%1E"}},    {.len=3,.buf4={ "%1F"}},
  /*SPC  !   "   #   $   %   &   '   0x20-0x2F */
  {.len=3,.buf4={ "%20"}},    {.len=1,.buf1={   "!"}},    
  {.len=3,.buf4={ "%22"}},    {.len=1,.buf1={   "#"}},
  {.len=1,.buf1={   "$"}},    {.len=3,.buf4={ "%25"}},    
  {.len=1,.buf1={   "&"}},    {.len=1,.buf1={   "'"}},
  /* (   )   *   +   ,   -   .   /*/ 
  {.len=1,.buf1={   "("}},    {.len=1,.buf1={   ")"}},    
  {.len=1,.buf1={   "*"}},    {.len=1,.buf1={   "+"}},
  {.len=1,.buf1={   ","}},    {.len=1,.buf1={   "-"}},    
  {.len=1,.buf1={   "."}},    {.len=1,.buf1={   "/"}},
  /* 0   1   2   3   4   5   6   7   0x30-0x3F */
  {.len=1,.buf1={   "0"}},    {.len=1,.buf1={   "1"}},    
  {.len=1,.buf1={   "2"}},    {.len=1,.buf1={   "3"}},
  {.len=1,.buf1={   "4"}},    {.len=1,.buf1={   "5"}},    
  {.len=1,.buf1={   "6"}},    {.len=1,.buf1={   "7"}},
  /* 8   9   :   ;   <   =   >   ?*/ 
  {.len=1,.buf1={   "8"}},    {.len=1,.buf1={   "9"}},    
  {.len=1,.buf1={   ":"}},    {.len=1,.buf1={   ";"}},
  {.len=3,.buf4={ "%3C"}},    {.len=1,.buf1={   "="}},    
  {.len=3,.buf4={ "%3E"}},    {.len=1,.buf1={   "?"}},
  /* @   A   B   C   D   E   F   G   0x40-0x4F */
  {.len=1,.buf1={   "@"}},    {.len=1,.buf1={   "A"}},    
  {.len=1,.buf1={   "B"}},    {.len=1,.buf1={   "C"}},
  {.len=1,.buf1={   "D"}},    {.len=1,.buf1={   "E"}},    
  {.len=1,.buf1={   "F"}},    {.len=1,.buf1={   "G"}},
  /* H   I   J   K   L   M   N   O*/ 
  {.len=1,.buf1={   "H"}},    {.len=1,.buf1={   "I"}},    
  {.len=1,.buf1={   "J"}},    {.len=1,.buf1={   "K"}},
  {.len=1,.buf1={   "L"}},    {.len=1,.buf1={   "M"}},    
  {.len=1,.buf1={   "N"}},    {.len=1,.buf1={   "O"}},
  /* P   Q   R   S   T   U   V   W   0x50-0x5F */
  {.len=1,.buf1={   "P"}},    {.len=1,.buf1={   "Q"}},    
  {.len=1,.buf1={   "R"}},    {.len=1,.buf1={   "S"}},
  {.len=1,.buf1={   "T"}},    {.len=1,.buf1={   "U"}},    
  {.len=1,.buf1={   "V"}},    {.len=1,.buf1={   "W"}},
  /* X   Y   Z   [   \   ]   ^   _*/ 
  {.len=1,.buf1={   "X"}},    {.len=1,.buf1={   "Y"}},    
  {.len=1,.buf1={   "Z"}},    {.len=3,.buf4={ "%5B"}},
  {.len=3,.buf4={ "%5C"}},    {.len=3,.buf4={ "%5D"}},    
  {.len=3,.buf4={ "%5E"}},    {.len=1,.buf1={   "_"}},
  /* `   a   b   c   d   e   f   g   0x60-0x6F */
  {.len=3,.buf4={ "%60"}},    {.len=1,.buf1={   "a"}},    
  {.len=1,.buf1={   "b"}},    {.len=1,.buf1={   "c"}},
  {.len=1,.buf1={   "d"}},    {.len=1,.buf1={   "e"}},    
  {.len=1,.buf1={   "f"}},    {.len=1,.buf1={   "g"}},
  /* h   i   j   k   l   m   n   o*/ 
  {.len=1,.buf1={   "h"}},    {.len=1,.buf1={   "i"}},    
  {.len=1,.buf1={   "j"}},    {.len=1,.buf1={   "k"}},
  {.len=1,.buf1={   "l"}},    {.len=1,.buf1={   "m"}},    
  {.len=1,.buf1={   "n"}},    {.len=1,.buf1={   "o"}},
  /* p   q   r   s   t   u   v   w   0x70-0x7F */
  {.len=1,.buf1={   "p"}},    {.len=1,.buf1={   "q"}},    
  {.len=1,.buf1={   "r"}},    {.len=1,.buf1={   "s"}},
  {.len=1,.buf1={   "t"}},    {.len=1,.buf1={   "u"}},    
  {.len=1,.buf1={   "v"}},    {.len=1,.buf1={   "w"}},
  /* x   y   z   {   |   }   ~  7f*/ 
  {.len=1,.buf1={   "x"}},    {.len=1,.buf1={   "y"}},    
  {.len=1,.buf1={   "z"}},    {.len=3,.buf4={ "%7B"}},
  {.len=3,.buf4={ "%7C"}},    {.len=3,.buf4={ "%7D"}},    
  {.len=1,.buf1={   "~"}},    {.len=3,.buf4={ "%7F"}},
  /*80  81  82  83  84  85  86  87   0x80-0x8F */
  {.len=6,.buf8={"%C2%80"}}, {.len=6,.buf8={"%C2%81"}}, 
  {.len=6,.buf8={"%C2%82"}}, {.len=6,.buf8={"%C2%83"}},
  {.len=6,.buf8={"%C2%84"}}, {.len=6,.buf8={"%C2%85"}}, 
  {.len=6,.buf8={"%C2%86"}}, {.len=6,.buf8={"%C2%87"}},
  /*88  89  8a  8b  8c  8d  8e  8f*/ 
  {.len=6,.buf8={"%C2%88"}}, {.len=6,.buf8={"%C2%89"}}, 
  {.len=6,.buf8={"%C2%8A"}}, {.len=6,.buf8={"%C2%8B"}},
  {.len=6,.buf8={"%C2%8C"}}, {.len=6,.buf8={"%C2%8D"}}, 
  {.len=6,.buf8={"%C2%8E"}}, {.len=6,.buf8={"%C2%8F"}},
  /*90  91  92  93  94  95  96  97   0x90-0x9F */
  {.len=6,.buf8={"%C2%90"}}, {.len=6,.buf8={"%C2%91"}}, 
  {.len=6,.buf8={"%C2%92"}}, {.len=6,.buf8={"%C2%93"}},
  {.len=6,.buf8={"%C2%94"}}, {.len=6,.buf8={"%C2%95"}}, 
  {.len=6,.buf8={"%C2%96"}}, {.len=6,.buf8={"%C2%97"}},
  /*98  99  9a  9b  9c  9d  9e  9f*/ 
  {.len=6,.buf8={"%C2%98"}}, {.len=6,.buf8={"%C2%99"}}, 
  {.len=6,.buf8={"%C2%9A"}}, {.len=6,.buf8={"%C2%9B"}},
  {.len=6,.buf8={"%C2%9C"}}, {.len=6,.buf8={"%C2%9D"}}, 
  {.len=6,.buf8={"%C2%9E"}}, {.len=6,.buf8={"%C2%9F"}},
  /*a0  a1  a2  a3  a4  a5  a6  a7   0xA0-0xAF */
  {.len=6,.buf8={"%C2%A0"}}, {.len=6,.buf8={"%C2%A1"}}, 
  {.len=6,.buf8={"%C2%A2"}}, {.len=6,.buf8={"%C2%A3"}},
  {.len=6,.buf8={"%C2%A4"}}, {.len=6,.buf8={"%C2%A5"}}, 
  {.len=6,.buf8={"%C2%A6"}}, {.len=6,.buf8={"%C2%A7"}},
  /*a8  a9  aa  ab  ac  ad  ae  af*/ 
  {.len=6,.buf8={"%C2%A8"}}, {.len=6,.buf8={"%C2%A9"}}, 
  {.len=6,.buf8={"%C2%AA"}}, {.len=6,.buf8={"%C2%AB"}},
  {.len=6,.buf8={"%C2%AC"}}, {.len=6,.buf8={"%C2%AD"}}, 
  {.len=6,.buf8={"%C2%AE"}}, {.len=6,.buf8={"%C2%AF"}},
  /*b0  b1  b2  b3  b4  b5  b6  b7   0xB0-0xBF */
  {.len=6,.buf8={"%C2%B0"}}, {.len=6,.buf8={"%C2%B1"}}, 
  {.len=6,.buf8={"%C2%B2"}}, {.len=6,.buf8={"%C2%B3"}},
  {.len=6,.buf8={"%C2%B4"}}, {.len=6,.buf8={"%C2%B5"}}, 
  {.len=6,.buf8={"%C2%B6"}}, {.len=6,.buf8={"%C2%B7"}},
  /*b8  b9  ba  bb  bc  bd  be  bf*/ 
  {.len=6,.buf8={"%C2%B8"}}, {.len=6,.buf8={"%C2%B9"}}, 
  {.len=6,.buf8={"%C2%BA"}}, {.len=6,.buf8={"%C2%BB"}},
  {.len=6,.buf8={"%C2%BC"}}, {.len=6,.buf8={"%C2%BD"}}, 
  {.len=6,.buf8={"%C2%BE"}}, {.len=6,.buf8={"%C2%BF"}},
  /*c0  c1  c2  c3  c4  c5  c6  c7   0xC0-0xCF */
  {.len=6,.buf8={"%C3%80"}}, {.len=6,.buf8={"%C3%81"}}, 
  {.len=6,.buf8={"%C3%82"}}, {.len=6,.buf8={"%C3%83"}},
  {.len=6,.buf8={"%C3%84"}}, {.len=6,.buf8={"%C3%85"}}, 
  {.len=6,.buf8={"%C3%86"}}, {.len=6,.buf8={"%C3%87"}},
  /*c8  c9  ca  cb  cc  cd  ce  cf*/ 
  {.len=6,.buf8={"%C3%88"}}, {.len=6,.buf8={"%C3%89"}}, 
  {.len=6,.buf8={"%C3%8A"}}, {.len=6,.buf8={"%C3%8B"}},
  {.len=6,.buf8={"%C3%8C"}}, {.len=6,.buf8={"%C3%8D"}}, 
  {.len=6,.buf8={"%C3%8E"}}, {.len=6,.buf8={"%C3%8F"}},
  /*d0  d1  d2  d3  d4  d5  d6  d7   0xD0-0xDF */
  {.len=6,.buf8={"%C3%90"}}, {.len=6,.buf8={"%C3%91"}}, 
  {.len=6,.buf8={"%C3%92"}}, {.len=6,.buf8={"%C3%93"}},
  {.len=6,.buf8={"%C3%94"}}, {.len=6,.buf8={"%C3%95"}}, 
  {.len=6,.buf8={"%C3%96"}}, {.len=6,.buf8={"%C3%97"}},
  /*d8  d9  da  db  dc  dd  de  df*/ 
  {.len=6,.buf8={"%C3%98"}}, {.len=6,.buf8={"%C3%99"}}, 
  {.len=6,.buf8={"%C3%9A"}}, {.len=6,.buf8={"%C3%9B"}},
  {.len=6,.buf8={"%C3%9C"}}, {.len=6,.buf8={"%C3%9D"}}, 
  {.len=6,.buf8={"%C3%9E"}}, {.len=6,.buf8={"%C3%9F"}},
  /*e0  e1  e2  e3  e4  e5  e6  e7   0xE0-0xEF */
  {.len=6,.buf8={"%C3%A0"}}, {.len=6,.buf8={"%C3%A1"}}, 
  {.len=6,.buf8={"%C3%A2"}}, {.len=6,.buf8={"%C3%A3"}},
  {.len=6,.buf8={"%C3%A4"}}, {.len=6,.buf8={"%C3%A5"}}, 
  {.len=6,.buf8={"%C3%A6"}}, {.len=6,.buf8={"%C3%A7"}},
  /*e8  e9  ea  eb  ec  ed  ee  ef*/ 
  {.len=6,.buf8={"%C3%A8"}}, {.len=6,.buf8={"%C3%A9"}}, 
  {.len=6,.buf8={"%C3%AA"}}, {.len=6,.buf8={"%C3%AB"}},
  {.len=6,.buf8={"%C3%AC"}}, {.len=6,.buf8={"%C3%AD"}}, 
  {.len=6,.buf8={"%C3%AE"}}, {.len=6,.buf8={"%C3%AF"}},
  /*f0  f1  f2  f3  f4  f5  f6  f7   0xF0-0xFF */
  {.len=6,.buf8={"%C3%B0"}}, {.len=6,.buf8={"%C3%B1"}}, 
  {.len=6,.buf8={"%C3%B2"}}, {.len=6,.buf8={"%C3%B3"}},
  {.len=6,.buf8={"%C3%B4"}}, {.len=6,.buf8={"%C3%B5"}}, 
  {.len=6,.buf8={"%C3%B6"}}, {.len=6,.buf8={"%C3%B7"}},
  /*f8  f9  fa  fb  fc  fd  fe  ff*/ 
  {.len=6,.buf8={"%C3%B8"}}, {.len=6,.buf8={"%C3%B9"}}, 
  {.len=6,.buf8={"%C3%BA"}}, {.len=6,.buf8={"%C3%BB"}},
  {.len=6,.buf8={"%C3%BC"}}, {.len=6,.buf8={"%C3%BD"}}, 
  {.len=6,.buf8={"%C3%BE"}}, {.len=6,.buf8={"%C3%BF"}}
};

static const struct _str_enc url_hex_char[256] = {
	{.len=3,.buf4={"%00"}},{.len=3,.buf4={"%01"}},{.len=3,.buf4={"%02"}},{.len=3,.buf4={"%03"}},
	{.len=3,.buf4={"%04"}},{.len=3,.buf4={"%05"}},{.len=3,.buf4={"%06"}},{.len=3,.buf4={"%07"}},
	{.len=3,.buf4={"%08"}},{.len=3,.buf4={"%09"}},{.len=3,.buf4={"%0A"}},{.len=3,.buf4={"%0B"}},
	{.len=3,.buf4={"%0C"}},{.len=3,.buf4={"%0D"}},{.len=3,.buf4={"%0E"}},{.len=3,.buf4={"%0F"}},
	{.len=3,.buf4={"%10"}},{.len=3,.buf4={"%11"}},{.len=3,.buf4={"%12"}},{.len=3,.buf4={"%13"}},
	{.len=3,.buf4={"%14"}},{.len=3,.buf4={"%15"}},{.len=3,.buf4={"%16"}},{.len=3,.buf4={"%17"}},
	{.len=3,.buf4={"%18"}},{.len=3,.buf4={"%19"}},{.len=3,.buf4={"%1A"}},{.len=3,.buf4={"%1B"}},
	{.len=3,.buf4={"%1C"}},{.len=3,.buf4={"%1D"}},{.len=3,.buf4={"%1E"}},{.len=3,.buf4={"%1F"}},
	{.len=3,.buf4={"%20"}},{.len=3,.buf4={"%21"}},{.len=3,.buf4={"%22"}},{.len=3,.buf4={"%23"}},
	{.len=3,.buf4={"%24"}},{.len=3,.buf4={"%25"}},{.len=3,.buf4={"%26"}},{.len=3,.buf4={"%27"}},
	{.len=3,.buf4={"%28"}},{.len=3,.buf4={"%29"}},{.len=3,.buf4={"%2A"}},{.len=3,.buf4={"%2B"}},
	{.len=3,.buf4={"%2C"}},{.len=3,.buf4={"%2D"}},{.len=3,.buf4={"%2E"}},{.len=3,.buf4={"%2F"}},
	{.len=3,.buf4={"%30"}},{.len=3,.buf4={"%31"}},{.len=3,.buf4={"%32"}},{.len=3,.buf4={"%33"}},
	{.len=3,.buf4={"%34"}},{.len=3,.buf4={"%35"}},{.len=3,.buf4={"%36"}},{.len=3,.buf4={"%37"}},
	{.len=3,.buf4={"%38"}},{.len=3,.buf4={"%39"}},{.len=3,.buf4={"%3A"}},{.len=3,.buf4={"%3B"}},
	{.len=3,.buf4={"%3C"}},{.len=3,.buf4={"%3D"}},{.len=3,.buf4={"%3E"}},{.len=3,.buf4={"%3F"}},
	{.len=3,.buf4={"%40"}},{.len=3,.buf4={"%41"}},{.len=3,.buf4={"%42"}},{.len=3,.buf4={"%43"}},
	{.len=3,.buf4={"%44"}},{.len=3,.buf4={"%45"}},{.len=3,.buf4={"%46"}},{.len=3,.buf4={"%47"}},
	{.len=3,.buf4={"%48"}},{.len=3,.buf4={"%49"}},{.len=3,.buf4={"%4A"}},{.len=3,.buf4={"%4B"}},
	{.len=3,.buf4={"%4C"}},{.len=3,.buf4={"%4D"}},{.len=3,.buf4={"%4E"}},{.len=3,.buf4={"%4F"}},
	{.len=3,.buf4={"%50"}},{.len=3,.buf4={"%51"}},{.len=3,.buf4={"%52"}},{.len=3,.buf4={"%53"}},
	{.len=3,.buf4={"%54"}},{.len=3,.buf4={"%55"}},{.len=3,.buf4={"%56"}},{.len=3,.buf4={"%57"}},
	{.len=3,.buf4={"%58"}},{.len=3,.buf4={"%59"}},{.len=3,.buf4={"%5A"}},{.len=3,.buf4={"%5B"}},
	{.len=3,.buf4={"%5C"}},{.len=3,.buf4={"%5D"}},{.len=3,.buf4={"%5E"}},{.len=3,.buf4={"%5F"}},
	{.len=3,.buf4={"%60"}},{.len=3,.buf4={"%61"}},{.len=3,.buf4={"%62"}},{.len=3,.buf4={"%63"}},
	{.len=3,.buf4={"%64"}},{.len=3,.buf4={"%65"}},{.len=3,.buf4={"%66"}},{.len=3,.buf4={"%67"}},
	{.len=3,.buf4={"%68"}},{.len=3,.buf4={"%69"}},{.len=3,.buf4={"%6A"}},{.len=3,.buf4={"%6B"}},
	{.len=3,.buf4={"%6C"}},{.len=3,.buf4={"%6D"}},{.len=3,.buf4={"%6E"}},{.len=3,.buf4={"%6F"}},
	{.len=3,.buf4={"%70"}},{.len=3,.buf4={"%71"}},{.len=3,.buf4={"%72"}},{.len=3,.buf4={"%73"}},
	{.len=3,.buf4={"%74"}},{.len=3,.buf4={"%75"}},{.len=3,.buf4={"%76"}},{.len=3,.buf4={"%77"}},
	{.len=3,.buf4={"%78"}},{.len=3,.buf4={"%79"}},{.len=3,.buf4={"%7A"}},{.len=3,.buf4={"%7B"}},
	{.len=3,.buf4={"%7C"}},{.len=3,.buf4={"%7D"}},{.len=3,.buf4={"%7E"}},{.len=3,.buf4={"%7F"}},
	{.len=3,.buf4={"%80"}},{.len=3,.buf4={"%81"}},{.len=3,.buf4={"%82"}},{.len=3,.buf4={"%83"}},
	{.len=3,.buf4={"%84"}},{.len=3,.buf4={"%85"}},{.len=3,.buf4={"%86"}},{.len=3,.buf4={"%87"}},
	{.len=3,.buf4={"%88"}},{.len=3,.buf4={"%89"}},{.len=3,.buf4={"%8A"}},{.len=3,.buf4={"%8B"}},
	{.len=3,.buf4={"%8C"}},{.len=3,.buf4={"%8D"}},{.len=3,.buf4={"%8E"}},{.len=3,.buf4={"%8F"}},
	{.len=3,.buf4={"%90"}},{.len=3,.buf4={"%91"}},{.len=3,.buf4={"%92"}},{.len=3,.buf4={"%93"}},
	{.len=3,.buf4={"%94"}},{.len=3,.buf4={"%95"}},{.len=3,.buf4={"%96"}},{.len=3,.buf4={"%97"}},
	{.len=3,.buf4={"%98"}},{.len=3,.buf4={"%99"}},{.len=3,.buf4={"%9A"}},{.len=3,.buf4={"%9B"}},
	{.len=3,.buf4={"%9C"}},{.len=3,.buf4={"%9D"}},{.len=3,.buf4={"%9E"}},{.len=3,.buf4={"%9F"}},
	{.len=3,.buf4={"%A0"}},{.len=3,.buf4={"%A1"}},{.len=3,.buf4={"%A2"}},{.len=3,.buf4={"%A3"}},
	{.len=3,.buf4={"%A4"}},{.len=3,.buf4={"%A5"}},{.len=3,.buf4={"%A6"}},{.len=3,.buf4={"%A7"}},
	{.len=3,.buf4={"%A8"}},{.len=3,.buf4={"%A9"}},{.len=3,.buf4={"%AA"}},{.len=3,.buf4={"%AB"}},
	{.len=3,.buf4={"%AC"}},{.len=3,.buf4={"%AD"}},{.len=3,.buf4={"%AE"}},{.len=3,.buf4={"%AF"}},
	{.len=3,.buf4={"%B0"}},{.len=3,.buf4={"%B1"}},{.len=3,.buf4={"%B2"}},{.len=3,.buf4={"%B3"}},
	{.len=3,.buf4={"%B4"}},{.len=3,.buf4={"%B5"}},{.len=3,.buf4={"%B6"}},{.len=3,.buf4={"%B7"}},
	{.len=3,.buf4={"%B8"}},{.len=3,.buf4={"%B9"}},{.len=3,.buf4={"%BA"}},{.len=3,.buf4={"%BB"}},
	{.len=3,.buf4={"%BC"}},{.len=3,.buf4={"%BD"}},{.len=3,.buf4={"%BE"}},{.len=3,.buf4={"%BF"}},
	{.len=3,.buf4={"%C0"}},{.len=3,.buf4={"%C1"}},{.len=3,.buf4={"%C2"}},{.len=3,.buf4={"%C3"}},
	{.len=3,.buf4={"%C4"}},{.len=3,.buf4={"%C5"}},{.len=3,.buf4={"%C6"}},{.len=3,.buf4={"%C7"}},
	{.len=3,.buf4={"%C8"}},{.len=3,.buf4={"%C9"}},{.len=3,.buf4={"%CA"}},{.len=3,.buf4={"%CB"}},
	{.len=3,.buf4={"%CC"}},{.len=3,.buf4={"%CD"}},{.len=3,.buf4={"%CE"}},{.len=3,.buf4={"%CF"}},
	{.len=3,.buf4={"%D0"}},{.len=3,.buf4={"%D1"}},{.len=3,.buf4={"%D2"}},{.len=3,.buf4={"%D3"}},
	{.len=3,.buf4={"%D4"}},{.len=3,.buf4={"%D5"}},{.len=3,.buf4={"%D6"}},{.len=3,.buf4={"%D7"}},
	{.len=3,.buf4={"%D8"}},{.len=3,.buf4={"%D9"}},{.len=3,.buf4={"%DA"}},{.len=3,.buf4={"%DB"}},
	{.len=3,.buf4={"%DC"}},{.len=3,.buf4={"%DD"}},{.len=3,.buf4={"%DE"}},{.len=3,.buf4={"%DF"}},
	{.len=3,.buf4={"%E0"}},{.len=3,.buf4={"%E1"}},{.len=3,.buf4={"%E2"}},{.len=3,.buf4={"%E3"}},
	{.len=3,.buf4={"%E4"}},{.len=3,.buf4={"%E5"}},{.len=3,.buf4={"%E6"}},{.len=3,.buf4={"%E7"}},
	{.len=3,.buf4={"%E8"}},{.len=3,.buf4={"%E9"}},{.len=3,.buf4={"%EA"}},{.len=3,.buf4={"%EB"}},
	{.len=3,.buf4={"%EC"}},{.len=3,.buf4={"%ED"}},{.len=3,.buf4={"%EE"}},{.len=3,.buf4={"%EF"}},
	{.len=3,.buf4={"%F0"}},{.len=3,.buf4={"%F1"}},{.len=3,.buf4={"%F2"}},{.len=3,.buf4={"%F3"}},
	{.len=3,.buf4={"%F4"}},{.len=3,.buf4={"%F5"}},{.len=3,.buf4={"%F6"}},{.len=3,.buf4={"%F7"}},
	{.len=3,.buf4={"%F8"}},{.len=3,.buf4={"%F9"}},{.len=3,.buf4={"%FA"}},{.len=3,.buf4={"%FB"}},
	{.len=3,.buf4={"%FC"}},{.len=3,.buf4={"%FD"}},{.len=3,.buf4={"%FE"}},{.len=3,.buf4={"%FF"}},
};

/*
 * http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf - page 94
 *
 * Table 3-7. Well-Formed UTF-8 Byte Sequences
 *
 * +--------------------+------------+-------------+------------+-------------+
 * | Code Points        | First Byte | Second Byte | Third Byte | Fourth Byte |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0000..U+007F     | 00..7F     |             |            |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0080..U+07FF     | C2..DF     | 80..BF      |            |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0800..U+0FFF     | E0         | A0..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+1000..U+CFFF     | E1..EC     | 80..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+D000..U+D7FF     | ED         | 80..9F      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+E000..U+FFFF     | EE..EF     | 80..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+10000..U+3FFFF   | F0         | 90..BF      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+40000..U+FFFFF   | F1..F3     | 80..BF      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+100000..U+10FFFF | F4         | 80..8F      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 */

/* Return 0 - success,  >0 - index(1 based) of first error char */
int utf8_char_lenth(const unsigned char *data, int len)
{
  int bytes;
  const unsigned char byte1 = data[0];

  if (byte1 <= 0x7F) 
  {
    /* 00..7F */
    bytes = 1;
  } 
  else if (len >= 2 && byte1 >= 0xC2 && byte1 <= 0xDF &&
          (signed char)data[1] <= (signed char)0xBF) 
  {
    /* C2..DF, 80..BF */
    bytes = 2;
  } 
  else if (len >= 3) 
  {
    const unsigned char byte2 = data[1];

    /* Is byte2, byte3 between 0x80 ~ 0xBF */
    const int byte2_ok = (signed char)byte2 <= (signed char)0xBF;
    const int byte3_ok = (signed char)data[2] <= (signed char)0xBF;

    if (byte2_ok && byte3_ok &&
         /* E0, A0..BF, 80..BF */
        ((byte1 == 0xE0 && byte2 >= 0xA0) ||
         /* E1..EC, 80..BF, 80..BF */
         (byte1 >= 0xE1 && byte1 <= 0xEC) ||
         /* ED, 80..9F, 80..BF */
         (byte1 == 0xED && byte2 <= 0x9F) ||
         /* EE..EF, 80..BF, 80..BF */
         (byte1 >= 0xEE && byte1 <= 0xEF))) 
    {
      bytes = 3;
    } 
    else if (len >= 4) 
    {
      /* Is byte4 between 0x80 ~ 0xBF */
      const int byte4_ok = (signed char)data[3] <= (signed char)0xBF;

      if (byte2_ok && byte3_ok && byte4_ok &&
               /* F0, 90..BF, 80..BF, 80..BF */
              ((byte1 == 0xF0 && byte2 >= 0x90) ||
               /* F1..F3, 80..BF, 80..BF, 80..BF */
               (byte1 >= 0xF1 && byte1 <= 0xF3) ||
               /* F4, 80..8F, 80..BF, 80..BF */
               (byte1 == 0xF4 && byte2 <= 0x8F))) 
      {
        bytes = 4;
      } 
      else 
      {
        bytes = 1;
      }
    } 
    else 
    {
      bytes = 1;
    }
  }
  else 
  {
    bytes = 1;
  }

  return bytes;
}

int encode_url(ng_buffer_s *b, const ng_uint8_t *input, int len) 
{
  int in_cursor;
  int out_cursor;
  ng_uint8_t *encoded;

  if (!len) 
    len = ng_strlen((const char *)input);
  
  encoded = (ng_uint8_t*)ng_malloc(sizeof(ng_uint8_t) * len * 6);
  if (encoded == NULL)
  {
    log_fatal("Failed to ng_malloc.");
    return -1;
  }
  b->ptr = encoded;

  in_cursor = 0;
  out_cursor = 0;
  while (in_cursor < len) 
  {
    ng_uint8_t charlen;
    charlen = utf8_char_lenth(&input[in_cursor], len - in_cursor);
    if (charlen == 1)
    {
      const ng_uint8_t c = input[in_cursor++];
      const struct _str_enc *enc=&url_str_enc[c];
      switch (enc->len)
      {
        case 1:
          *(ng_uint8_t  *)&encoded[out_cursor]  = enc->v1;
          out_cursor += 1;
          break;
          
        case 3:
          *(ng_uint32_t *)&encoded[out_cursor]  = enc->v4;
          out_cursor += 3;
          break;
          
        case 6:
          *(ng_uint64_t *)&encoded[out_cursor]  = enc->v8;
          out_cursor += 6;
          break;
          
        default:
          NG_ASSERT(0);
          break;
      }
    }
    else
    {
      int in_end = in_cursor + charlen;
      while (in_cursor < in_end)
      {
        *(ng_uint32_t *)&encoded[out_cursor] = url_hex_char[input[in_cursor++]].v4;
        out_cursor += 3;
      }
    }
  }

  b->len = out_cursor;
  return b->len;
}

#define __ 256
static const int hexval[256] = {
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 00-0F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 10-1F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 20-2F */
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,__,__,__,__,__,__, /* 30-3F */
  __,10,11,12,13,14,15,__,__,__,__,__,__,__,__,__, /* 40-4F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 50-5F */
  __,10,11,12,13,14,15,__,__,__,__,__,__,__,__,__, /* 60-6F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 70-7F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 80-8F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 90-9F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* A0-AF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* B0-BF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* C0-CF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* D0-DF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* E0-EF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* F0-FF */
};
#undef __

static const ng_uint8_t url_str_dec_len[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  2,2,2,2,0,2,2,2,0,0,0,0,0,0,0,0,
  0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int decode_url(ng_buffer_s *b, const ng_uint8_t* input, int len) 
{
  ng_uint8_t *decoded;
  int in_cursor = 0;
  int out_cursor = 0;

  if (!len) len = ng_strlen((const char *)input);
  b->ptr = decoded = (ng_uint8_t*)ng_malloc(sizeof(ng_uint8_t) * len);
  if (decoded == NULL)
  {
    log_fatal("Failed to ng_malloc.");
    return -1;
  }
  
  while (in_cursor < len) 
  {
    ng_uint8_t charlen;
    ng_uint8_t c;
    ng_uint8_t v1,v1raw;
    ng_uint8_t v2,v2raw;


    c = input[in_cursor++];
    if (c != '%') {
      decoded[out_cursor++] = c;
      continue;
    }
    
    if (len - in_cursor < 2)
    {
      decoded[out_cursor++] = c;
      continue;
    }
    charlen = url_str_dec_len[input[in_cursor]];
    if (!charlen)
    {
      charlen = utf8_len(&input[in_cursor-1], len - in_cursor+1);
      if (!charlen)
      {
        decoded[out_cursor++] = c;
        continue;
      }
      if (charlen == 1)
      {
        v1raw = input[in_cursor++];
        v2raw = input[in_cursor++];
        if (v1raw == 0x30 && v2raw == 0x30) {
          decoded[out_cursor++] = '\0';
          continue;
        }
        v1 = hexval[v1raw];
        v2 = hexval[v2raw];
        if ((v1 | v2) != 0xFF) 
        {
          decoded[out_cursor++] = (v1 << 4) | v2;
          continue;
        }
        else
        {
          decoded[out_cursor++] = '%';
          decoded[out_cursor++] = v1raw;
          decoded[out_cursor++] = v2raw;
          continue;
        }
      }
      else
      {
        int in_end = in_cursor + charlen;
        while (in_cursor < in_end)
        {
          decoded[out_cursor++] = c;
          c = input[in_cursor++];
        }
        continue;
      }
    }
    else if (charlen == 1 || charlen == 2)
    {
      v1raw = input[in_cursor++];
      v2raw = input[in_cursor++];
      if (v1raw == 0x30 && v2raw == 0x30) {
        decoded[out_cursor++] = '\0';
        continue;
      }
      v1 = hexval[v1raw];
      v2 = hexval[v2raw];
      if ((v1 | v2) != 0xFF) 
      {
        decoded[out_cursor++] = (v1 << 4) | v2;
        continue;
      }
      else
      {
        decoded[out_cursor++] = '%';
        decoded[out_cursor++] = v1raw;
        decoded[out_cursor++] = v2raw;
        continue;
      }
    }
    else if (charlen == 5)
    {
      if (len - in_cursor < 5)
      {
        decoded[out_cursor++] = c;
        in_cursor++;
        continue;
      }
      if (input[in_cursor+2]=='%')
      {
        v1raw = input[in_cursor++];
        v2raw = input[in_cursor++];
        v1 = hexval[v1raw];
        v2 = hexval[v2raw];
        if (v1 != 0xFF && v2 != 0xFF) 
        {
          in_cursor++;
          ng_uint8_t _v1raw = input[in_cursor++];
          ng_uint8_t _v2raw = input[in_cursor++];
          ng_uint8_t v3 = hexval[_v1raw];
          ng_uint8_t v4 = hexval[_v2raw];
          if (v3 != 0xFF && v4 != 0xFF)
          {
            decoded[out_cursor++] = (((v2 << 2) | v3) << 4) | v4;
            continue;
          }
          else
          {
            decoded[out_cursor++] = '%';
            decoded[out_cursor++] = v1raw;
            decoded[out_cursor++] = v2raw;
            decoded[out_cursor++] = '%';
            decoded[out_cursor++] = _v1raw;
            decoded[out_cursor++] = _v2raw;
            continue;
          }
        }
        else
        {
          decoded[out_cursor++] = '%';
          decoded[out_cursor++] = v1raw;
          decoded[out_cursor++] = v2raw;
          continue;
        }
      }
    }

    decoded[out_cursor++] = c;
  }
  
  b->len = out_cursor;
  return b->len;
}

#define UTF8_LEAD(c) ((ng_uint8_t)(c) < 0x80 || ((ng_uint8_t)(c) > 0xC1 && (ng_uint8_t)(c) < 0xF5))
#define UTF8_TRAIL(c) (((ng_uint8_t)(c) & 0xC0) == 0x80)

#define __ 0xFF
/*
 * 0x00: 0
 * 0x01-0xC1: 1
 * 0xF5-: 1
 */
static const ng_uint8_t utf8_immediate_len[256] = {
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00-0x0F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x10-0x1F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x20-0x2F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x30-0x3F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x40-0x4F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x50-0x5F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x60-0x6F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x70-0x7F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x80-0x8F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x90-0x9F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xA0-0xAF */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xB0-0xBF */
   1, 1,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xC0-0xCF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xD0-0xDF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xE0-0xEF */
  __,__,__,__,__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xF0-0xFF */
};
#undef __

/*
 * 0xC2-0xDF: 2
 * 0xE0-0xEF: 3
 * 0xF0-0xF4: 4
 */
static const ng_uint8_t utf8_count_len[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x00-0x0F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x10-0x1F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x20-0x2F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x30-0x3F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x40-0x4F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x50-0x5F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x60-0x6F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x70-0x7F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
  0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xC0-0xCF */
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xD0-0xDF */
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* 0xE0-0xEF */
  4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};

static ng_uint8_t utf8_len(const ng_uint8_t* str, int inlen) {
  if (inlen < 2)
  {
    return inlen;
  }
  else
  {
    const ng_uint8_t lead = *str;

    const ng_uint8_t immediate_len = utf8_immediate_len[lead];
    if (immediate_len != 0xFF) {
      return immediate_len;
    }

    const ng_uint8_t count = utf8_count_len[lead];
    ng_uint8_t trail = *(++str);

    if (count == 3) 
    {
      if (inlen < count)
        return 1;
      if ((lead == 0xE0 && 0xA0 > trail) 
        || (lead == 0xED && trail > 0x9F)) 
      {
        return 1;
      }
    } 
    else if (count == 4) 
    {
      if (inlen < count)
        return 1;
      if ((lead == 0xF0 && 0x90 > trail) 
        || (lead == 0xF4 && trail > 0x8F)) 
      {
        return 1;
      }
    }

    ng_uint8_t size = 1;
    for (; size < count; ++size) 
    {
      if (!UTF8_TRAIL(trail)) 
      {
        return size;
      }
      trail = *(++str);
    }
    return size;
  }
}

// Function to calculate the size of the output buffer
ng_size_t calculate_buffer_size(const char *utf8, ng_size_t len) 
{
  ng_size_t size = 1;  // Start with 1 for the null terminator
  unsigned char *ptr = (unsigned char *)utf8;
  const unsigned char *end = ptr + len;
  while (ptr < end) {
    if (*ptr <= 0x7F) {
      // 1-byte character (ASCII)
      size += 6;  // Each escape sequence \uXXXX requires 6 characters
      ptr++;
    } else if ((*ptr & 0xE0) == 0xC0) {
      // 2-byte character
      size += 6;
      ptr += 2;  // Move past 2-byte character
    } else if ((*ptr & 0xF0) == 0xE0) {
      // 3-byte character
      size += 6;
      ptr += 3;  // Move past 3-byte character
    } else if ((*ptr & 0xF8) == 0xF0) {
      // 4-byte character
      size += 6;
      ptr += 4;  // Move past 4-byte character
    }
  }
  return size;
}

#include <stdio.h>

#if 0
static inline void __char_to_escape(char *buf_ptr, ng_uint8_t c)
{
  sprintf(buf_ptr, "\\u%04X", c);
}
#else
static inline void __char_to_escape(char *buf_ptr, ng_uint8_t c)
{
  *(ng_uint32_t*)buf_ptr = *(const ng_uint32_t*)"\\u00";
  const struct _str_enc *uh = &url_hex_char[c];
  *(ng_uint16_t *)&buf_ptr[4] = *(ng_uint16_t *)(uh->buf4+1);
}
#endif

// Function to convert UTF-8 to Unicode escape sequence and store in buffer
void convert_utf8_to_unicode_escape(const char *utf8, 
  ng_size_t len, ng_buffer_s *buffer) 
{
  const ng_uint8_t *ptr = (const ng_uint8_t *)utf8;
  const ng_uint8_t *end = ptr + len;
  ng_uint8_t codepoint = 0;
  char *buf_ptr = (char *)buffer->ptr;

  while (ptr < end) {
    if (*ptr <= 0x7F) {
      // 1-byte character (ASCII)
      codepoint = *ptr;
      __char_to_escape(buf_ptr, codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xE0) == 0xC0) {
      // 2-byte character
      codepoint = *ptr & 0x1F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      __char_to_escape(buf_ptr, codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xF0) == 0xE0) {
      // 3-byte character
      codepoint = *ptr & 0x0F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      __char_to_escape(buf_ptr, codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xF8) == 0xF0) {
      // 4-byte character
      codepoint = *ptr & 0x07;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      __char_to_escape(buf_ptr, codepoint);
      buf_ptr += 6;
      ptr++;
    }
    else {
      *buf_ptr++ = *ptr++;
    }
  }

  buffer->len = buf_ptr - (char *)buffer->ptr;
}

#if __NHTTP_TEST_URLENCODE

#define _TE_INPUT  "c\x01\xff\xfe\xfd"
#define _TE_OUTPUT "c%01%C3%BF%C3%BE%C3%BD"

static void __test_encode_url(const char *output, int outlen, 
  const char *input, int inlen)
{
  ng_buffer_s b;
  if (0 > encode_url(&b, (const ng_uint8_t *)output, outlen))
  {
    log_verbose("test encode_url FAILED.");
    return;
  }
  if (b.len != inlen
    || ng_memcmp(input, b.ptr, b.len))
  {
    log_verbose("test encode_url FAILED.");
    ng_free(b.ptr);
    return;
  }
  ng_free(b.ptr);
  log_verbose("test encode_url OK.");
}


/* "北京" */
#define _TE_INPUT1  "\u5317\u4eac"
#define _TE_OUTPUT1 "%E5%8C%97%E4%BA%AC"
static void test_encode_url1(void)
{
  __test_encode_url(_TE_INPUT, sizeof(_TE_INPUT)-1,
    _TE_OUTPUT, sizeof(_TE_OUTPUT)-1);
}
static void test_encode_url2(void)
{
  __test_encode_url(_TE_INPUT1, sizeof(_TE_INPUT1)-1,
    _TE_OUTPUT1, sizeof(_TE_OUTPUT1)-1);
}

static void test_encode_url3(void)
{
  for (int i=128;i<256;i++)
  {
    const struct _str_enc *e = &url_str_enc[i];
    char output[1]={i};
    __test_encode_url(output, 1, e->cptr, e->len);
  }
}

static void 
__test_decode_url(const char *output, int outlen, 
  const char *input, int inlen)
{
  ng_buffer_s b;
  if (0 > decode_url(&b, (const ng_uint8_t *)output, outlen))
  {
    log_verbose("test decode_url FAILED.");
    return;
  }
  if (b.len != inlen
    || ng_memcmp(input, b.ptr, b.len))
  {
    log_verbose("test decode_url FAILED.");
    ng_free(b.ptr);
    return;
  }
  log_verbose("test decode_url OK.");
  log_verbose("%pS.", &b);
  ng_free_data_block(&b);
}

static void test_decode_url1(void)
{
  __test_decode_url(_TE_OUTPUT1, sizeof(_TE_OUTPUT1)-1, 
    _TE_INPUT1, sizeof(_TE_INPUT1)-1);
}

static void test_decode_url2(void)
{
  __test_decode_url(_TE_OUTPUT, sizeof(_TE_OUTPUT)-1, 
    _TE_INPUT, sizeof(_TE_INPUT)-1);
}

static void test_decode_url3(void)
{
  for (int i=128;i<256;i++)
  {
    const struct _str_enc *e = &url_str_enc[i];
    char output[1]={i};
    __test_decode_url(e->cptr, e->len, output, 1);
  }
}

int test_encode_url(void)
{
  test_encode_url1();
  test_encode_url2();
  test_encode_url3();
  return 0;
}

int test_decode_url(void)
{
  test_decode_url1();
  test_decode_url2();
  test_decode_url3();
  return 0;
}
#else
int test_encode_url(void)
{
  return 0;
}

int test_decode_url(void)
{
  return 0;
}
#endif
