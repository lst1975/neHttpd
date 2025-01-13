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
#ifndef __NGRTOS_CHANLLE_H__
#define __NGRTOS_CHANLLE_H__

#include "nanohttp-defs.h"
#include "nanohttp-list.h"
#include "nanohttp-error.h"

typedef int ng_result_t;

typedef struct ng_devtab ng_devtab_s;
typedef struct ng_channel ng_channel_s;
typedef struct ng_mchain ng_mchain_s;

typedef struct __ng_instance ng_instance_s;
typedef struct __ng_instance_cfg ng_instance_cfg_s;

#define NG_CONNECTIONS_MAX 0xffffffff
#define NG_FILES_MAX 0xffffffff

#define NG_DEV_PRIORITY_Realtime       0
#define NG_DEV_PRIORITY_Veryhigh       1
#define NG_DEV_PRIORITY_high           2
#define NG_DEV_PRIORITY_normal_above   3
#define NG_DEV_PRIORITY_normal         4
#define NG_DEV_PRIORITY_normal_below   5
#define NG_DEV_PRIORITY_low_above      6
#define NG_DEV_PRIORITY_low            7
#define NG_DEV_PRIORITY_MAX            8

#define NG_DEV_NAME_LEN_MAX 64

struct __ng_instance_cfg{
  uint32_t connections;
  uint32_t files;
};

struct __ng_cache_aligned __ng_instance{
  ng_instance_cfg_s cfg;
  void *log;
  ng_list_head_s link;
  ng_list_head_s dev_list[NG_DEV_PRIORITY_MAX];
  uint32_t inst;
};

#define NG_INSTACE2ID(t) ((ng_instance_s*)t)->inst

struct ng_mchain{
  size_t size;
  size_t count;
  void **m;
  ng_list_head_s link;
};

struct ng_channel {
  ng_list_head_s  link;
  ng_devtab_s	   *dp;
  ng_mchain_s    *cur;
  uint32_t        type:8;
  uint32_t        pad1:8;
  uint32_t        pad2:16;
  uint32_t        pad3;
  void	         *priv;
};

struct ng_dev_stats{
  ng_ulong_t errors;
  ng_ulong_t bytes;
  ng_ulong_t packets;
  ng_ulong_t overflow;
};
typedef struct ng_dev_stats ng_dev_stats_s;

#define NG_DEV_MBUF_MAX  8

struct __ng_cache_aligned ng_devtab {
  ng_block_s      name;
  ng_list_head_s  channels;
  ng_list_head_s  link;
  ng_result_t   (*init )(ng_devtab_s *dev);
  void          (*free )(ng_devtab_s *dev);
  ng_result_t   (*open )(ng_channel_s *ch, const ng_block_s *name);
  ng_result_t   (*close)(ng_channel_s *ch);
  int	          (*read )(ng_channel_s *ch, ng_mchain_s *mc);
  int	          (*write)(ng_channel_s *ch, ng_mchain_s *mc);
  ng_result_t   (*ioctl)(ng_channel_s *ch, int cmd, void *arg);
  uint32_t        alloc:1;
  uint32_t        started:1;
  uint32_t        size:16;
  uint32_t        priority:14;
  uint16_t        flag;
  uint16_t        pad;
  void           *log;
  const char     *ver;
  void           *cmd;
  void           *instance;
  char	          priv[0];
};

#define NG_DEV2PRIV(dev, t) ((t *)((dev)->priv))
#define NG_PRIV2DEV(p) container_of(p, ng_devtab_s, priv);

#define NG_CONCAT2(a,b) a ## b
#define NG_DEV_DEFNAME(name)  ____ ## name ## dev
#define NG_DEV_DECL(name)  ng_devtab_s NG_DEV_DEFNAME(name)
#define NG_GET_DEV_PRIV(name) (&NG_DEV_DEFNAME(name).priv)
#define NG_DEF_DEV(_name, ver, pri, type, command) \
  NG_DEV_DECL(name) = { \
    .name  = DECL_CONST_STR(#_name), \
    .link  = NG_LIST_ENTRY_INIT(), \
    .init  = NG_CONCAT2(_name, _init),  \
    .free  = NG_CONCAT2(_name, _deinit),  \
    .open  = NG_CONCAT2(_name, _open),  \
    .close = NG_CONCAT2(_name, _close), \
    .read  = NG_CONCAT2(_name, _read),  \
    .write = NG_CONCAT2(_name, _write), \
    .ioctl = NG_CONCAT2(_name, _ioctl), \
    .alloc = ng_FALSE, \
    .started = ng_FALSE, \
    .flag  = 0, \
    .instance = NULL, \
    .priority = pri, \
    .size  = RTE_CACHE_LINE_ROUNDUP(sizeof(ng_devtab_s)+sizeof(type)), \
    .log = NULL, \
    .cmd = command, \
    .ver = ver, \
  }

#define DEV2PRIV(dp,t) (t*)(dp+1)

#define NG_DEV_OP_INIT   0
#define NG_DEV_OP_DEINIT 1

ng_result_t ng_dev_register(ng_devtab_s *dp, ng_instance_s *t);
ng_result_t ng_dev_unregister(ng_devtab_s *dp);

ng_channel_s *ng_open(const char *fmt, ...);
ng_result_t ng_close(ng_channel_s *ch);
ng_result_t ng_ioctl(ng_channel_s *ch, int cmd, void *arg);
int ng_read(ng_channel_s *ch, ng_mchain_s *mc);
int ng_write(ng_channel_s *ch, ng_mchain_s *mc);

ng_result_t ng_system_instance_add(ng_devtab_s **devs, int n);
ng_result_t ng_system_instance_del(int id);
ng_instance_cfg_s *ng_system_instance_cfg_byid(int id);
ng_instance_cfg_s *ng_system_instance_cfg(ng_instance_s *t);

ng_result_t ng_system_init(void);
void ng_system_deinit(void);

void ng_mchain_free(ng_mchain_s *mc);

#endif

