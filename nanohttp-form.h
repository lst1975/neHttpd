#ifndef MULTIPARTPARSER_H
#define MULTIPARTPARSER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct _multipartparser multipartparser;
typedef struct _multipartparser_callbacks multipartparser_callbacks;

typedef size_t (*multipart_cb) (multipartparser*);
typedef size_t (*multipart_data_cb) (multipartparser*, const char* data, size_t size);

struct _multipartparser {
    /** PRIVATE **/
    char        boundary[70];
    int         boundary_length;
    int         index;
    uint16_t    state;

    /** PUBLIC **/
    void* data;
    void* arg;
};

struct _multipartparser_callbacks {
    multipart_cb      on_body_begin;
    multipart_cb      on_part_begin;
    multipart_data_cb on_header_field;
    multipart_data_cb on_header_value;
    multipart_cb      on_headers_complete;
    multipart_data_cb on_data;
    multipart_cb      on_part_end;
    multipart_cb      on_body_end;
};

void multipartparser_init(multipartparser* parser, void *arg, const char* boundary);

void multipartparser_callbacks_init(multipartparser_callbacks* callbacks);

size_t multipartparser_execute(
    multipartparser* parser, 
    multipartparser_callbacks* callbacks,
    const char* data,
    size_t size);

#ifdef __cplusplus
}
#endif
#endif // MULTIPARTPARSER_H
