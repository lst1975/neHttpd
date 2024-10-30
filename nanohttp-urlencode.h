#ifndef LUAURLENCODE_LUA_URLENCODE_H_
#define LUAURLENCODE_LUA_URLENCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdint.h>

uint8_t* decode_url(const uint8_t* input, int len);
uint8_t* encode_url(const uint8_t* input, int len);

#ifdef __cplusplus
}
#endif

#endif

