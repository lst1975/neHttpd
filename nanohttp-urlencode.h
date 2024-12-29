#ifndef LUAURLENCODE_LUA_URLENCODE_H_
#define LUAURLENCODE_LUA_URLENCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "nanohttp-common.h"

int decode_url(httpd_buf_t *b, const uint8_t* input, int len);
int encode_url(httpd_buf_t *b, const uint8_t* input, int len);
uint8_t utf8_len(const uint8_t* str, int inlen);
void test_encode_url(void);
void test_decode_url(void);

#ifdef __cplusplus
}
#endif

#endif

