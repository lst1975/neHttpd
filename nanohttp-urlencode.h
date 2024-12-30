#ifndef LUAURLENCODE_LUA_URLENCODE_H_
#define LUAURLENCODE_LUA_URLENCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "nanohttp-common.h"

extern const uint8_t url_unreserved[256];

int decode_url(httpd_buf_t *b, const uint8_t* input, int len);
int encode_url(httpd_buf_t *b, const uint8_t* input, int len);
void test_encode_url(void);
void test_decode_url(void);
int utf8_char_lenth(const unsigned char *data, int len);
size_t calculate_buffer_size(const char *utf8, size_t len);
void convert_utf8_to_unicode_escape(const char *utf8, size_t len, httpd_buf_t *buffer);

#ifdef __cplusplus
}
#endif

#endif

