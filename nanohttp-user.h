#ifndef __nanohttp_user_h
#define __nanohttp_user_h

#include "nanohttp-list.h"
#include "nanohttp-common.h"

#define _N_http_user_type_NONE   0
#define _N_http_user_type_SUPER  1
#define _N_http_user_type_ADMIN  2
#define _N_http_user_type_GUEST  3

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

#define _N_http_user_error_NONE  0
#define _N_http_user_error_EXIST 1
#define _N_http_user_error_SYS   2
#define _N_http_user_error_VNAME 4
#define _N_http_user_error_VPSWD 5
#define _N_http_user_error_INVAL 6

void nanohttp_users_free(void);
int nanohttp_users_init(void);
httpd_user_t *nanohttp_users_match(const char *name, int nameLen, 
  const char *pswd, int pswdLen);
int nanohttp_users_add(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen);
int nanohttp_users_update(const char *name, int nameLen, 
  const char *pswd, int pswdLen, const char *level, int levelLen);
int nanohttp_users_del(const char *name, int nameLen);
const httpd_buf_t *__nanohttp_level2string(int level);

#endif
