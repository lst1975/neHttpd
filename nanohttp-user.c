#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "nanohttp-json.h"
#include "nanohttp-file.h"
#include "nanohttp-user.h"
#include "nanohttp-logging.h"
#include "nanohttp-base64.h"

static ng_list_head_s users = NG_LIST_HEAD_INIT(users);

static int __nanohttp_users2file(void);

#define __USER_FILE "data/users.json"

static herror_t 
__file_user(void *arg, const char *buf, size_t length) 
{
  httpd_buf_t *t = (httpd_buf_t *)arg;
  memcpy((char *)t->data + t->len, buf, length);
  t->len += length;
  return NULL;
}

void nanohttp_users_free(void)
{
  httpd_user_t *entry;

  while(!ng_list_empty(&users))
  {
    entry = ng_list_first_entry(&users,httpd_user_t,link);
    ng_list_del(&entry->link);
    free(entry);
  }
}

static int __nanohttp_users_init__one(JSONPair_t *p)
{
  int err = -1;

  httpd_user_t *entry;
  JSONPair_t *usr, *pwd, *type;
  if (p->jsonType != JSONObject)
    goto clean2;
  usr = json_find_bykey(p->children,  "username", 8);
  pwd = json_find_bykey(p->children,  "password", 8);
  type = json_find_bykey(p->children, "level", 5);
  if (!usr || !pwd || !type)
    goto clean2;
  usr = json_find_bykey(usr->children,  "value", 5);
  pwd = json_find_bykey(pwd->children,  "value", 5);
  type = json_find_bykey(type->children,"value", 5);
  if (!usr || !pwd || !type)
    goto clean2;
  entry = malloc(sizeof(*entry)+usr->valueLength+pwd->valueLength);
  if (entry == NULL)
    goto clean2;
  if (type->valueLength == 13 && !strncmp(type->value, "Administrator", 13))
    entry->type = _N_http_user_type_ADMIN;
  else if (type->valueLength == 5 && !strncmp(type->value, "Guest", 5))
    entry->type = _N_http_user_type_GUEST;
  else if (type->valueLength == 10 && !strncmp(type->value, "SupperUser", 10))
    entry->type = _N_http_user_type_GUEST;
  else
  {
    free(entry);
    goto clean2;
  }
  entry->name.data = ((char *)(entry+1));
  entry->name.len  = usr->valueLength;
  memcpy(entry->name.data, usr->value, entry->name.len);

  entry->pswd.data = ((char *)(entry+1))+usr->valueLength;
  entry->pswd.len  = pwd->valueLength;
  memcpy(entry->pswd.data, pwd->value, entry->pswd.len);
  ng_list_add_tail(&entry->link, &users);
  err = 0;

clean2:
  return err;
}

int nanohttp_users_init(void)
{
  int err = -1;
  herror_t r;
  JSONPair_t *pair, *p, *s;
  JSONStatus_t result;
  httpd_buf_t tmp, _b, *b = &_b;
  
  b->len = nanohttp_file_size(__USER_FILE);
  if (b->len == 0)
  {
    goto clean0;
  }
  tmp.data = b->data = malloc(b->len);
  if (b->data == NULL)
  {
    goto clean0;
  }
  tmp.len = 0;

  r = nanohttp_file_read_all(__USER_FILE, __file_user, &tmp);
  if (r != NULL)
  {
    log_verbose("%s", herror_message(r));
    herror_release(r);
    goto clean1;
  }
  
  log_debug("%.*s\n", (int)b->len, b->data);

  result = json_parse(&pair, (const char *)b->data, b->len);
  if (result != JSONSuccess)
  {
    goto clean1;
  }

  p = json_find_bykey(pair, "AccountConfiguration", 20);
  if (p == NULL || p->jsonType != JSONObject)
  {
    goto clean2;
  }

  /* read Supperuser */
  s = json_find_bykey(p->children, "supperuser", 10);
  if (s == NULL || s->jsonType != JSONObject)
  {
    goto clean2;
  }
  if (0 > __nanohttp_users_init__one(s))
    goto clean2;

  /* read Administrators or Guests */
  p = json_find_bykey(p->children, "users", 5);
  if (p == NULL || p->jsonType != JSONObject)
  {
    goto clean2;
  }
  p = json_find_bykey(p->children, "value", 5);
  if (p == NULL || p->jsonType != JSONArray)
  {
    goto clean2;
  }
  for (p=p->children;p!=NULL;p=p->siblings)
  {
    if (0 > __nanohttp_users_init__one(p))
      goto clean2;
  }
  err = 0;

clean2:
  json_pairs_free(pair);
clean1:
  free(b->data);
  b->data = NULL;
clean0:
  return err;
}

static const httpd_buf_t __http_user_super={.cptr="SupperUser",.len=10};
static const httpd_buf_t __http_user_admin={.cptr="Administrator",.len=13};
static const httpd_buf_t __http_user_guest={.cptr="Guest",.len=5};
const httpd_buf_t *
__nanohttp_level2string(int level)
{
  switch (level)
  {
    case _N_http_user_type_SUPER:
      return &__http_user_super;
    case _N_http_user_type_ADMIN:
      return &__http_user_admin;
    case _N_http_user_type_GUEST:
      return &__http_user_guest;
    default:
      return NULL;
  }
}
static int
__nanohttp_string2level(const char *level, int levelLen)
{
  if (levelLen == __http_user_super.len && !memcmp(level, __http_user_super.data, levelLen))
    return _N_http_user_type_SUPER;
  if (levelLen == __http_user_admin.len && !memcmp(level, __http_user_admin.data, levelLen))
    return _N_http_user_type_ADMIN;
  if (levelLen == __http_user_guest.len && !memcmp(level, __http_user_guest.data, levelLen))
    return _N_http_user_type_GUEST;

  return _N_http_user_type_NONE;
}

#if 0
static const httpd_buf_t __http_user_crypt={.cptr="neHttpd2025",.len=11};

int
nanohttp_pswd_enc(httpd_buf_t *b, const char *pswd, int len)
{
  unsigned char *p;

  b->size = BASE64_ENCODE_OUT_SIZE(len);
  b->ptr = (unsigned char *)malloc(b->size + len);
  if (b->ptr == NULL)
    return -1;
  p = b->ptr + b->size;
  memcpy(p, pswd, len);
  for (int i=0;i<len;i++)
  {
    p[i] ^= __http_user_crypt.cptr[i%__http_user_crypt.len];
  }
  b->len = b64Encode(p, len, b->ptr, b->size);
  return 0;
}

int
nanohttp_pswd_dec(httpd_buf_t *b, const char *pswd, int len)
{
  unsigned char *p;

  b->size = BASE64_DECODE_OUT_SIZE(len);
  b->ptr = malloc(b->size + len);
  if (b->ptr == NULL)
    return -1;
  p = b->ptr + b->size;
  memcpy(p, pswd, len);
  p[len] = '\0';
  
  b->len = b64Decode(p, len, b->ptr, b->size);
  for (int i=0;i<b->len;i++)
  {
    b->ptr[i] ^= __http_user_crypt.cptr[i%__http_user_crypt.len];
  }
  return 0;
}
#endif

httpd_user_t * 
nanohttp_users_match(const char *name, int nameLen, 
  const char *pswd, int pswdLen)
{
  httpd_user_t *entry;
  
  ng_list_for_each_entry(entry, httpd_user_t, &users, link)
  {
    if (entry->name.len != nameLen)
      continue;
    if (memcmp(name, entry->name.cptr, nameLen))
      continue;
    if (pswd)
    {
      if (entry->pswd.len != pswdLen)
        continue;
      if (memcmp(entry->pswd.cptr, pswd, pswdLen))
        continue;
    }
    return entry;
  }
  
  return NULL;
}

static int
__nanohttp_user2json_super(httpd_user_t *entry, char *b, int len, int count)
{
  const httpd_buf_t *lp = __nanohttp_level2string(entry->type);
  if (lp == NULL)
    return -1;

  int n = snprintf(b, len,
  		"\"username\": {"
  		"  \"type\":\"string\","
  		"  \"range\":[%u,%u],"
  		"  \"value\":\"%.*s\","
  		"  \"writable\":false,"
  		"  \"label\": \"User Name\","
  		"  \"id\": 0"
  		"},"
  		"\"password\": {"
  		"  \"type\":\"string\","
  		"  \"range\":[%u,%u],"
  		"  \"value\":\"%.*s\","
  		"  \"writable\":false,"
  		"  \"label\": \"Password\","
  		"  \"id\": 1"
  		"},"
  		"\"level\": {"
  		"  \"type\":\"string\","
  		"  \"range\":\"[10,10]\","
  		"  \"value\":\"%.*s\","
  		"  \"writable\":false,"
  		"  \"label\": \"User Level\","
  		"  \"id\": 2"
  		"}%s",
  		_N_http_user_NAME_MINLEN,
  		_N_http_user_NAME_MAXLEN,
      (int)entry->name.len, entry->name.cptr,
      _N_http_user_PSWD_MINLEN,
      _N_http_user_PSWD_MAXLEN,
      (int)entry->pswd.len, entry->pswd.cptr,
      (int)lp->len, lp->cptr,
      count ? ",":""
    );

  if (n >= len - 1)
    return - 1;
  
  return n;
}
static int
__nanohttp_user2json(httpd_user_t *entry, char *b, int len, int count)
{
  const httpd_buf_t *lp = __nanohttp_level2string(entry->type);
  if (lp == NULL)
    return -1;

  int n = snprintf(b, len,
  		"{\"username\": {"
  		"  \"type\":\"string\","
  		"  \"range\":[%u,%u],"
  		"  \"value\":\"%.*s\","
  		"  \"writable\":false,"
  		"  \"label\": \"User Name\","
  		"  \"id\": 0"
  		"},"
  		"\"password\": {"
  		"  \"type\":\"string\","
  		"  \"range\":[%u,%u],"
  		"  \"value\":\"%.*s\","
  		"  \"writable\":true,"
  		"  \"label\": \"Password\","
  		"  \"id\": 1"
  		"},"
  		"\"level\": {"
  		"  \"type\":\"array\","
  		"  \"range\":[\"Administrator\", \"Guest\"],"
  		"  \"value\":\"%.*s\","
  		"  \"writable\":true,"
  		"  \"label\": \"User Level\","
  		"  \"id\": 2"
  		"}}%s",
  		_N_http_user_NAME_MINLEN,
  		_N_http_user_NAME_MAXLEN,
      (int)entry->name.len, entry->name.cptr,
      _N_http_user_PSWD_MINLEN,
      _N_http_user_PSWD_MAXLEN,
      (int)entry->pswd.len, entry->pswd.cptr,
      (int)lp->len, lp->cptr,
      count ? ",":""
    );

  if (n >= len - 1)
    return - 1;
  
  return n;
}

static inline int 
json_printer_file(httpd_buf_t *b, const char *fmt, ...)
{
  size_t n;
  va_list args;

  va_start(args, fmt);
  n = vfprintf(b->data, fmt, args);
  va_end(args);
  return n;
}

static int
__nanohttp_users2file(void)
{
  int n, count=0, err=-1;
  httpd_user_t *entry;
  char buf[8192];
  char *p = buf;
  int len = sizeof(buf);
  JSONStatus_t result;
  JSONPair_t *json;
  void *fp;
  httpd_buf_t b;

  n = snprintf(p, len, "%s", "{\"AccountConfiguration\":{");
  p += n, len -= n;
  
  ng_list_for_each_entry(entry, httpd_user_t, &users, link)
  {
    if (entry->type == _N_http_user_type_SUPER)
    {
      n = snprintf(p, len, "%s", 
        "\"supperuser\":{\"label\":\"SupperUser\",\"type\":\"group\",\"id\":0,");
      p += n, len -= n;
      n = __nanohttp_user2json_super(entry, p, len, 0);
      if (n == -1)
        goto clean0;
      p += n, len -= n;
      n = snprintf(p, len, "%s", 
        "},\"users\":{\"type\":\"list+\",\"label\":\"Accounts\",\"id\":0,"
        "\"range\":[1,100],\"writable\":true,\"index\":\"username\",\"value\":[");
      p += n, len -= n;
    }
    else
    {
      count++;
      n = __nanohttp_user2json(entry, p, len, count);
      if (n == -1)
        goto clean0;
      p += n, len -= n;
    }
  }

  if (count)
  {
    p--;
    len++;
  }
  
  n = snprintf(p, len, "%s", 
    "]},\"label\":\"Account Configuration\",\"id\":2}}");
  p += n, len -= n;

  len = p -buf;

  log_debug("%.*s\n", (int)len, buf);
  
  result = JSON_Validate(buf, len);
  if( result != JSONSuccess )
  {
    goto clean0;
  }
  result = json_parse(&json,buf, len);
  if( result != JSONSuccess )
  {
    goto clean0;
  }

  json_print(json,0,"  ");
  
  fp = nanohttp_file_open_for_write(__USER_FILE);
  if (fp == NULL)
  {
    goto clean1;
  }

  b.data = fp;
  len = json_to_printer(json_printer_file, &b, json, 0, "  ");
  if (len <= 0)
  {
    goto clean2;
  }
  err = 0;
  
clean2:
  nanohttp_file_close(fp);
clean1:
  json_pairs_free(json);
clean0:
  return err;
}

int 
nanohttp_users_add(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen)
{
  int type;
  httpd_user_t *entry;

  if (nameLen < _N_http_user_NAME_MINLEN 
    || nameLen > _N_http_user_NAME_MAXLEN)
  {
    return _N_http_user_error_VNAME;
  }
  
  if (pswdLen < _N_http_user_PSWD_MINLEN 
    || pswdLen > _N_http_user_PSWD_MAXLEN)
  {
    return _N_http_user_error_VPSWD;
  }
  
  type = __nanohttp_string2level(level, levelLen);
  if (type == _N_http_user_type_NONE)
  {
    level    = __http_user_guest.cptr;
    levelLen = __http_user_guest.len;
    type     = _N_http_user_type_GUEST;
  }
  
  entry = nanohttp_users_match(name, nameLen, NULL, 0);
  if (entry != NULL)
    return _N_http_user_error_EXIST;
  entry = malloc(sizeof(*entry)+nameLen+pswdLen);
  if (entry == NULL)
    return _N_http_user_error_SYS;

  entry->type = type;
  
  entry->name.data = ((char *)(entry+1));
  entry->name.len  = nameLen;
  memcpy(entry->name.data, name, entry->name.len);
  
  entry->pswd.data = ((char *)(entry+1))+nameLen;
  entry->pswd.len  = pswdLen;
  memcpy(entry->pswd.data, pswd, pswdLen);

  ng_list_add_tail(&entry->link, &users);

  if (__nanohttp_users2file() < 0)
  {
    ng_list_del(&entry->link);
    free(entry);
    return _N_http_user_error_SYS;
  }
  
  return _N_http_user_error_NONE;
}

int 
nanohttp_users_update(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen)
{
  int type;
  httpd_user_t *entry, *old;

  if (!pswdLen && !levelLen)
  {
    return _N_http_user_error_INVAL;
  }
  if (nameLen < _N_http_user_NAME_MINLEN 
    || nameLen > _N_http_user_NAME_MAXLEN)
  {
    return _N_http_user_error_VNAME;
  }
  
  if (pswdLen && (pswdLen < _N_http_user_PSWD_MINLEN 
    || pswdLen > _N_http_user_PSWD_MAXLEN))
  {
    return _N_http_user_error_VPSWD;
  }
  
  old = nanohttp_users_match(name, nameLen, NULL, 0);
  if (old == NULL)
    return _N_http_user_error_EXIST;

  if (levelLen)
  {
    type = __nanohttp_string2level(level, levelLen);
    if (type == _N_http_user_type_NONE)
    {
      return _N_http_user_error_INVAL;
    }
  }
  else
  {
    type = old->type;
  }

  entry = malloc(sizeof(*entry)+nameLen+pswdLen);
  if (entry == NULL)
    return _N_http_user_error_SYS;

  entry->type = type;

  entry->name.data = ((char *)(entry+1));
  entry->name.len  = nameLen;
  memcpy(entry->name.data, name, entry->name.len);
  
  entry->pswd.data = ((char *)(entry+1))+nameLen;
  entry->pswd.len  = pswdLen;
  memcpy(entry->pswd.data, pswd, pswdLen);

  ng_list_add_after(&entry->link, &old->link);
  ng_list_del(&old->link);
  free(old);
  
  if (__nanohttp_users2file() < 0)
  {
    ng_list_del(&entry->link);
    free(entry);
    return _N_http_user_error_SYS;
  }
  
  return _N_http_user_type_NONE;
}

int 
nanohttp_users_del(const char *name, int nameLen)
{
  httpd_user_t *entry;

  if (nameLen < _N_http_user_NAME_MINLEN 
    || nameLen > _N_http_user_NAME_MAXLEN)
  {
    if (nameLen != 4 || memcmp(name, "bob2", 4))
      return _N_http_user_error_VNAME;
  }
  
  entry = nanohttp_users_match(name, nameLen, NULL, 0);
  if (entry == NULL)
    return _N_http_user_error_EXIST;

  ng_list_del(&entry->link);
  
  if (__nanohttp_users2file() < 0)
    return _N_http_user_error_SYS;
  else
    return _N_http_user_type_NONE;
}

