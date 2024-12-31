#ifndef __nanohttp_buffer_h
#define __nanohttp_buffer_h

typedef struct {
  union{
    void *data;
    const char *cptr;
    unsigned char *ptr;
    char *buf;
  };
  char *p;
  size_t size;
  size_t len;
  size_t nbytes;
} httpd_buf_t;

static inline void BUF_GO(httpd_buf_t *b, size_t len)
{
  b->p   += len;
  b->len += len;
}

static inline char *BUF_CUR_PTR(httpd_buf_t *b)
{
  return b->p;
}

static inline size_t BUF_REMAIN(httpd_buf_t *b)
{
  return b->size - b->len;
}

static inline void BUF_SIZE_INIT(httpd_buf_t *b, char *buf, size_t size)
{
  b->p      = buf;
  b->data   = buf;
  b->len    = 0;
  b->size   = size;
  b->nbytes = 0;
}

#endif
