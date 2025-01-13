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
#include "nanohttp-channel.h"
#include "nanohttp-logging.h"
#include "nanohttp-mem.h"
#include "nanohttp-ctype.h"
#include "nanohttp-atoi.h"

#define NG_MAX_INSTANCE 8192

static ng_instance_s *ng_instance_table[NG_MAX_INSTANCE]={0};
static ng_list_head_s ng_instance_list = NG_LIST_HEAD_INIT(ng_instance_list);
static int ng_instance_num   = 0;

static void ng_system_instance_deinit(ng_instance_s *t);

static int
ng_system_instance_getid(void)
{
  for (int i=0; i<NG_MAX_INSTANCE;i++)
    if (ng_instance_table[i]==NULL)
      return i;
  return ng_ERR_EOVERFLOW;
}

static ng_result_t
ng_system_instance_getbyid(int id, ng_instance_s **t)
{
  if (id >=NG_MAX_INSTANCE)
  {
    log_error("Bad instance id %d.", id);
    return ng_ERR_EINVAL;
  }
  
  *t = ng_instance_table[id];
  if (*t == NULL)
  {
    log_error("Failed to find instance %d.", id);
    return ng_ERR_ENOENT;
  }

  return ng_ERR_NONE;
}

ng_instance_cfg_s *
ng_system_instance_cfg_byid(int id)
{
  ng_instance_s *t;
  if (ng_system_instance_getbyid(id, &t) == ng_ERR_NONE)
    return &t->cfg;
  return NULL;
}

ng_instance_cfg_s *
ng_system_instance_cfg(ng_instance_s *t)
{
  return t != NULL ? &t->cfg : NULL;
}

static ng_result_t
ng_system_instance_init(ng_instance_s *t,
  ng_devtab_s **devs, int n)
{
  ng_result_t r = ng_ERR_NONE;

  for (int i=0; i<NG_DEV_PRIORITY_MAX;i++)
  {
    ng_INIT_LIST_HEAD(&t->dev_list[i]);
  }
  __ng_list_poison_entry(&t->link);
  t->cfg.connections = NG_CONNECTIONS_MAX;
  t->cfg.files       = NG_FILES_MAX;
  t->log         = NULL;

  for (int i=0; i<n;i++)
  {
    ng_devtab_s *d = devs[i];
    r = ng_dev_register(d, t);
    if (r != ng_ERR_NONE)
    {
      log_error("failed to register device %pS.", &d->name);
      goto clean0;
    }
  }
  return ng_ERR_NONE;
  
clean0:
  ng_system_instance_deinit(t);
  return r;
}

static void
ng_system_instance_deinit(ng_instance_s *t)
{
  for (int i=0; i<NG_DEV_PRIORITY_MAX;i++)
  {
    ng_devtab_s *d;
    ng_list_head_s *head;
    head = &t->dev_list[i];
    while (!ng_list_empty(head))
    {
      ng_list_get_from_head(head,d,ng_devtab_s,link);
      NG_ASSERT(d != NULL);
      if (d->alloc)
        http_free(d);
    }
  }
}

static void 
ng_dev_destroy(ng_instance_s *t)
{
  for (int i=0; i<NG_DEV_PRIORITY_MAX;i++)
  {
    ng_devtab_s *dp;
    ng_list_head_s *head;

    head = &t->dev_list[i];
    ng_list_for_each_entry(dp,ng_devtab_s,head,link)
    {
      if (!dp->started)
        continue;
      dp->free(dp);
      dp->started = ng_FALSE;
    }
  }
  return;
}

static ng_result_t 
ng_dev_start(ng_instance_s *t)
{
  ng_result_t r;

  for (int i=0; i<NG_DEV_PRIORITY_MAX;i++)
  {
    ng_devtab_s *dp;
    ng_list_head_s *head;

    head = &t->dev_list[i];
    ng_list_for_each_entry(dp,ng_devtab_s,head,link)
    {
      r = dp->init(dp);
      if (r != ng_ERR_NONE)
      {
        log_error("Failed to init device %pS.", &dp->name);
        goto err;
      }
      log_error("Init device %pS: OK.", &dp->name);
      dp->started = ng_TRUE;
    }
  }
  return ng_ERR_NONE;
  
err:  
  ng_dev_destroy(t);
  return r;
}

ng_result_t
ng_system_instance_add(ng_devtab_s **devs, int n)
{
  int id;
  ng_result_t r;
  ng_instance_s *t;

  id = ng_system_instance_getid();
  if (id < 0)
  {
    r = id;
    goto clean0;
  }
  
  t = http_malloc(sizeof(*t));
  if (t == NULL)
  {
    log_error("failed to malloc ng_instance_s.");
    r = ng_ERR_ENOMEM;
    goto clean0;
  }
  ng_bzero(t, sizeof(*t));
  t->inst = id;
  ng_list_add_tail(&t->link, &ng_instance_list);
  ng_instance_table[id] = t;
  ng_instance_num++;
  
  r = ng_system_instance_init(t, devs, n);
  if (r != ng_ERR_NONE)
    goto clean1;

  r = ng_dev_start(t);
  if (r != ng_ERR_NONE)
    goto clean2;
  
  return ng_ERR_NONE;
  
clean2:
  ng_system_instance_deinit(t);
clean1:
  ng_instance_num--;
  ng_instance_table[id] = NULL;
  ng_list_del(&t->link);
  http_free(t);
clean0:
  return r;
}

ng_result_t
ng_system_instance_del(int id)
{
  ng_result_t r;
  ng_instance_s *t;

  r = ng_system_instance_getbyid(id, &t);
  if (r != ng_ERR_NONE)
    return r;

  ng_dev_destroy(t);
  ng_system_instance_deinit(t);
  
  ng_instance_num--;
  ng_instance_table[id] = NULL;
  ng_list_del(&t->link);
  http_free(t);
  return ng_ERR_NONE;
}

ng_result_t
ng_system_init(void)
{
  ng_bzero(ng_instance_table, sizeof(ng_instance_table));
  ng_INIT_LIST_HEAD(&ng_instance_list);
  ng_instance_num = 0;
  return ng_ERR_NONE;
}

void
ng_system_deinit(void)
{
  ng_list_head_s *head = &ng_instance_list;

  while (!ng_list_empty(head))
  {
    ng_instance_s *t;
    t = ng_list_first_entry_or_null(head,ng_instance_s,link);
    NG_ASSERT(t != NULL);
    ng_system_instance_del(t->inst);
  }

  NG_ASSERT(ng_instance_num == 0);
  return;
}

#define CHANNELS  680

/* announce: register a device */
ng_result_t ng_dev_register(ng_devtab_s *dp, ng_instance_s *t)
{
  ng_devtab_s *d;

  d = (ng_devtab_s *)http_malloc(dp->size);
  if (d == NULL)
  {
    log_error("Failed to malloc device %pS.", &dp->name);
    return ng_ERR_ENOMEM;
  }
  ng_memcpy(d, dp, sizeof(ng_devtab_s));
  d->instance = t;
  d->alloc = ng_TRUE;
  
  ng_INIT_LIST_HEAD(&dp->channels);
  ng_list_add_head(&dp->link, &t->dev_list[dp->priority]);
  
  return ng_ERR_NONE;
}

/* announce: register a device */
ng_result_t ng_dev_unregister(ng_devtab_s *dp)
{
  ng_list_del(&dp->link);
  if (dp->alloc)
    http_free(dp);
  return ng_ERR_NONE;
}

static ng_devtab_s *
__ng_dev_find(ng_instance_s *t, const char *name, int len)
{
  for (int i = 0; i < NG_DEV_PRIORITY_MAX; i++)
  {
    ng_devtab_s  *dp;
    ng_list_head_s *head = &t->dev_list[i];
    ng_list_for_each_entry(dp, ng_devtab_s, head, link)
    {
      if (ng_block_isequal(&dp->name,name,len))
        return dp;
    }
  }
  return NULL;
}

/* open: call a device open and return a channel */
ng_channel_s *
ng_open(const char *fmt, ...)
{
  int             inst,n;
  ng_result_t     r;
  ng_devtab_s    *dp;
  ng_channel_s   *ch;
  const char     *p;
  ng_instance_s  *t;
  httpd_buf_t     s;
  char           *buffer;
  ng_block_s      _name;
  ng_block_s     *name;
  va_list args;

#define __BUF_SZ 1024
  buffer = http_malloc(__BUF_SZ);
  if (buffer == NULL)
  {
    log_error("Failed to malloc temp buffer.");
    goto clean0;
  }
  _name.buf = buffer;
  va_start(args, fmt);
  _name.len = ng_vsnprintf(buffer, __BUF_SZ, fmt, args);
  va_end(args);
  name = &_name;

  BUF_SIZE_INIT(&s, name->buf, name->len);

  p = ng_memchr(BUF_CUR_PTR(&s), '/', BUF_REMAIN(&s));
  if (p == NULL)
  {
    log_error("Bad name %pS, need '/'.", name);
    goto err0;
  }

  n     = p - BUF_CUR_PTR(&s);
  if (!ng_str_isdigit_s(BUF_CUR_PTR(&s), n))
    goto err0;
  
  inst = ng_atoui(BUF_CUR_PTR(&s), n);
  r = ng_system_instance_getbyid(inst, &t);
  if (r != ng_ERR_NONE)
    goto err0;

  BUF_GO(&s,n+1);
  
  p = ng_memchr(BUF_CUR_PTR(&s), '/', BUF_REMAIN(&s));
  if (p == NULL)
  {
    log_error("Bad name %pS, need '/'.", name);
    goto err0;
  }

  n     = p - BUF_CUR_PTR(&s);
  dp = __ng_dev_find(t, BUF_CUR_PTR(&s), n);
  if (dp == NULL)
  {
    log_error("Failed to find device %pS.", name);
    goto err0;
  }
  BUF_GO(&s,n+1);
  
  ch = (ng_channel_s *)http_malloc(sizeof(*ch));
  if (ch == NULL) 
  {
    log_error("Failed to malloc channel %pS.", name);
    goto err0;
  }
  
  ch->dp   = dp;
  ch->type = 0;

  r = (*dp->open)(ch, &s.b);
  if (r != ng_ERR_NONE) 
  {
    log_error("Device refused to open %pS.", name);
    goto err1;
  }

  http_free(buffer);
  ng_list_add_tail(&ch->link, &dp->channels);
  return ch;
#undef __BUF_SZ
  
err1:  
  http_free(ch);
err0:  
  log_error("Failed to open channel %pS.", name);
  http_free(buffer);
clean0:
  return NULL;
}

/* close: close a channel and return associated channel */
ng_result_t ng_close(ng_channel_s *ch)
{
  ng_result_t r;

  if (ch == NULL) 
    return ng_ERR_NONE;
  
  r = (*ch->dp->close)(ch);
  ng_list_del(&ch->link);
  http_free(ch);
  return r;
}

/* write: stub for the driver */
int ng_write(ng_channel_s *ch, ng_mchain_s *mc)
{
  if (ch == NULL) 
    return ng_ERR_EINVAL;
  return (*ch->dp->write)(ch, mc);
}

/* read: stub for the driver */
int ng_read(ng_channel_s *ch, ng_mchain_s *mc)
{
  if (ch == NULL) 
    return ng_ERR_EINVAL;
  return (*ch->dp->read)(ch, mc);
}

/* ioctl: stub for the driver */
ng_result_t ng_ioctl(ng_channel_s *ch, int cmd, void *arg)
{
  if (ch == NULL) 
    return ng_ERR_EINVAL;
  return (*ch->dp->ioctl)(ch, cmd, arg);
}
