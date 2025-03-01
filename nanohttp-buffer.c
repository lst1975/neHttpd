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

#include "nanohttp-config.h"
#include "nanohttp-buffer.h"
#include "nanohttp-mem.h"
#include "nanohttp-logging.h"

void
ng_free_data_buffer(ng_buffer_s *data)
{
  if (data->buf != NULL)
  {
    ng_free(data->buf);
    data->buf = NULL;
    data->len = 0;
  }
}

void
ng_free_data_block(ng_block_s *block)
{
  if (block->buf != NULL)
  {
    ng_free(block->buf);
    block->buf = NULL;
    block->len = 0;
  }
}

int
ng_dup_data_block(ng_block_s *block, const ng_block_s *n, int free_old)
{
  if (n->data != NULL && n->len)
  {
    void *p = ng_malloc(n->len);
    if (p == NULL)
    {
      log_fatal("ng_dup_data_block failed.");
      return -1;
    }
    if (free_old)
      ng_free_data_block(block);
    ng_memcpy(p, n->data, n->len);
    block->data = p;
    block->len  = n->len;
  }

  return 0;
}

int
ng_dup_data_block_str(ng_block_s *block, const ng_block_s *n, int free_old)
{
  void *p = ng_malloc(n->len+1);
  if (p == NULL)
    return -1;
  if (free_old)
    ng_free_data_block(block);
  ng_memcpy(p, n->data, n->len);
  block->data = p;
  block->len  = n->len;
  block->buf[block->len] = '\0';
  return 0;
}

#define _(x) { .len = sizeof(#x)-1, .cptr=#x }
const ng_block_s __ng_cache_aligned __ng_uint8_string[] = {
  _(  0), _(  1), _(  2), _(  3), _(  4), _(  5), _(  6), _(  7),
  _(  8), _(  9), _( 10), _( 11), _( 12), _( 13), _( 14), _( 15),
  _( 16), _( 17), _( 18), _( 19), _( 20), _( 21), _( 22), _( 23),
  _( 24), _( 25), _( 26), _( 27), _( 28), _( 29), _( 30), _( 31),
  _( 32), _( 33), _( 34), _( 35), _( 36), _( 37), _( 38), _( 39),
  _( 40), _( 41), _( 42), _( 43), _( 44), _( 45), _( 46), _( 47),
  _( 48), _( 49), _( 50), _( 51), _( 52), _( 53), _( 54), _( 55),
  _( 56), _( 57), _( 58), _( 59), _( 60), _( 61), _( 62), _( 63),
  _( 64), _( 65), _( 66), _( 67), _( 68), _( 69), _( 70), _( 71),
  _( 72), _( 73), _( 74), _( 75), _( 76), _( 77), _( 78), _( 79),
  _( 80), _( 81), _( 82), _( 83), _( 84), _( 85), _( 86), _( 87),
  _( 88), _( 89), _( 90), _( 91), _( 92), _( 93), _( 94), _( 95),
  _( 96), _( 97), _( 98), _( 99), _(100), _(101), _(102), _(103),
  _(104), _(105), _(106), _(107), _(108), _(109), _(110), _(111),
  _(112), _(113), _(114), _(115), _(116), _(117), _(118), _(119),
  _(120), _(121), _(122), _(123), _(124), _(125), _(126), _(127),
  _(128), _(129), _(130), _(131), _(132), _(133), _(134), _(135),
  _(136), _(137), _(138), _(139), _(140), _(141), _(142), _(143),
  _(144), _(145), _(146), _(147), _(148), _(149), _(150), _(151),
  _(152), _(153), _(154), _(155), _(156), _(157), _(158), _(159),
  _(160), _(161), _(162), _(163), _(164), _(165), _(166), _(167),
  _(168), _(169), _(170), _(171), _(172), _(173), _(174), _(175),
  _(176), _(177), _(178), _(179), _(180), _(181), _(182), _(183),
  _(184), _(185), _(186), _(187), _(188), _(189), _(190), _(191),
  _(192), _(193), _(194), _(195), _(196), _(197), _(198), _(199),
  _(200), _(201), _(202), _(203), _(204), _(205), _(206), _(207),
  _(208), _(209), _(210), _(211), _(212), _(213), _(214), _(215),
  _(216), _(217), _(218), _(219), _(220), _(221), _(222), _(223),
  _(224), _(225), _(226), _(227), _(228), _(229), _(230), _(231),
  _(232), _(233), _(234), _(235), _(236), _(237), _(238), _(239),
  _(240), _(241), _(242), _(243), _(244), _(245), _(246), _(247),
  _(248), _(249), _(250), _(251), _(252), _(253), _(254), _(255)
};

