#ifndef __nanohttp_user_h
#define __nanohttp_user_h

#include "nanohttp-list.h"

typedef struct {
  union{
    void *data;
    const char *cptr;
  };
  size_t len;
} httpd_buf_t;

#define _N_http_user_type_NONE   0
#define _N_http_user_type_SUPER  1
#define _N_http_user_type_ADMIN  2
#define _N_http_user_type_NORMAL 3

#define _N_http_user_NAME_MINLEN 8
#define _N_http_user_NAME_MAXLEN 128
#define _N_http_user_PSWD_MINLEN 6
#define _N_http_user_PSWD_MAXLEN 32

typedef struct {
  httpd_buf_t name;
  httpd_buf_t pswd;
  int type;
  ng_list_head_s link;
} httpd_user_t;

void nanohttp_users_free(void);
int nanohttp_users_init(void);
httpd_user_t *nanohttp_users_match(const char *name, int nameLen, 
  const char *pswd, int pswdLen);
httpd_user_t *nanohttp_users_add(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen);
httpd_user_t *nanohttp_users_update(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen);

#endif
