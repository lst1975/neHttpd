#include "nanohttp-urlencode.h"
#include "nanohttp-logging.h"

static uint8_t utf8_len(const uint8_t* str, int inlen);
static const uint8_t xdigit[16] = "0123456789ABCDEF";

/*
 Alphanumeric characters: A-Z, a-z, 0-9
 Special characters: -, _, ., ~
 Reserved characters: !, ', (, ), *, ;, :, @, &, =, +, $, ,, /, ?, #
 */
#if 0
static const int url_unreserved[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x00-0x0F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x10-0x1F */
  0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1, /* 0x20-0x2F */
  1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1, /* 0x30-0x3F */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x40-0x4F */
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, /* 0x50-0x5F */
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x60-0x6F */
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0, /* 0x70-0x7F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xC0-0xCF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xD0-0xDF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xE0-0xEF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};
#endif

struct _str_enc{
  long len;
  const char *cptr;
};

static const struct _str_enc url_str_enc[256] = {
  {.len=3,.cptr="%00"},
  {.len=3,.cptr="%01"},
  {.len=3,.cptr="%02"},
  {.len=3,.cptr="%03"},
  {.len=3,.cptr="%04"},
  {.len=3,.cptr="%05"},
  {.len=3,.cptr="%06"},
  {.len=3,.cptr="%07"},
  {.len=3,.cptr="%08"},
  {.len=3,.cptr="%09"},
  {.len=3,.cptr="%0A"},
  {.len=3,.cptr="%0B"},
  {.len=3,.cptr="%0C"},
  {.len=3,.cptr="%0D"},
  {.len=3,.cptr="%0E"},
  {.len=3,.cptr="%0F"},
  {.len=3,.cptr="%10"},
  {.len=3,.cptr="%11"},
  {.len=3,.cptr="%12"},
  {.len=3,.cptr="%13"},
  {.len=3,.cptr="%14"},
  {.len=3,.cptr="%15"},
  {.len=3,.cptr="%16"},
  {.len=3,.cptr="%17"},
  {.len=3,.cptr="%18"},
  {.len=3,.cptr="%19"},
  {.len=3,.cptr="%1A"},
  {.len=3,.cptr="%1B"},
  {.len=3,.cptr="%1C"},
  {.len=3,.cptr="%1D"},
  {.len=3,.cptr="%1E"},
  {.len=3,.cptr="%1F"},
  {.len=3,.cptr="%20"},
  {.len=1,.cptr="!"},
  {.len=3,.cptr="%22"},
  {.len=1,.cptr="#"},
  {.len=1,.cptr="$"},
  {.len=3,.cptr="%25"},
  {.len=1,.cptr="&"},
  {.len=1,.cptr="'"},
  {.len=1,.cptr="("},
  {.len=1,.cptr=")"},
  {.len=1,.cptr="*"},
  {.len=1,.cptr="+"},
  {.len=1,.cptr=","},
  {.len=1,.cptr="-"},
  {.len=1,.cptr="."},
  {.len=1,.cptr="/"},
  {.len=1,.cptr="0"},
  {.len=1,.cptr="1"},
  {.len=1,.cptr="2"},
  {.len=1,.cptr="3"},
  {.len=1,.cptr="4"},
  {.len=1,.cptr="5"},
  {.len=1,.cptr="6"},
  {.len=1,.cptr="7"},
  {.len=1,.cptr="8"},
  {.len=1,.cptr="9"},
  {.len=1,.cptr=":"},
  {.len=1,.cptr=";"},
  {.len=3,.cptr="%3C"},
  {.len=1,.cptr="="},
  {.len=3,.cptr="%3E"},
  {.len=1,.cptr="?"},
  {.len=1,.cptr="@"},
  {.len=1,.cptr="A"},
  {.len=1,.cptr="B"},
  {.len=1,.cptr="C"},
  {.len=1,.cptr="D"},
  {.len=1,.cptr="E"},
  {.len=1,.cptr="F"},
  {.len=1,.cptr="G"},
  {.len=1,.cptr="H"},
  {.len=1,.cptr="I"},
  {.len=1,.cptr="J"},
  {.len=1,.cptr="K"},
  {.len=1,.cptr="L"},
  {.len=1,.cptr="M"},
  {.len=1,.cptr="N"},
  {.len=1,.cptr="O"},
  {.len=1,.cptr="P"},
  {.len=1,.cptr="Q"},
  {.len=1,.cptr="R"},
  {.len=1,.cptr="S"},
  {.len=1,.cptr="T"},
  {.len=1,.cptr="U"},
  {.len=1,.cptr="V"},
  {.len=1,.cptr="W"},
  {.len=1,.cptr="X"},
  {.len=1,.cptr="Y"},
  {.len=1,.cptr="Z"},
  {.len=3,.cptr="%5B"},
  {.len=3,.cptr="%5C"},
  {.len=3,.cptr="%5D"},
  {.len=3,.cptr="%5E"},
  {.len=1,.cptr="_"},
  {.len=3,.cptr="%60"},
  {.len=1,.cptr="a"},
  {.len=1,.cptr="b"},
  {.len=1,.cptr="c"},
  {.len=1,.cptr="d"},
  {.len=1,.cptr="e"},
  {.len=1,.cptr="f"},
  {.len=1,.cptr="g"},
  {.len=1,.cptr="h"},
  {.len=1,.cptr="i"},
  {.len=1,.cptr="j"},
  {.len=1,.cptr="k"},
  {.len=1,.cptr="l"},
  {.len=1,.cptr="m"},
  {.len=1,.cptr="n"},
  {.len=1,.cptr="o"},
  {.len=1,.cptr="p"},
  {.len=1,.cptr="q"},
  {.len=1,.cptr="r"},
  {.len=1,.cptr="s"},
  {.len=1,.cptr="t"},
  {.len=1,.cptr="u"},
  {.len=1,.cptr="v"},
  {.len=1,.cptr="w"},
  {.len=1,.cptr="x"},
  {.len=1,.cptr="y"},
  {.len=1,.cptr="z"},
  {.len=3,.cptr="%7B"},
  {.len=3,.cptr="%7C"},
  {.len=3,.cptr="%7D"},
  {.len=1,.cptr="~"},
  {.len=3,.cptr="%7F"},
  {.len=6,.cptr="%C2%80"},
  {.len=6,.cptr="%C2%81"},
  {.len=6,.cptr="%C2%82"},
  {.len=6,.cptr="%C2%83"},
  {.len=6,.cptr="%C2%84"},
  {.len=6,.cptr="%C2%85"},
  {.len=6,.cptr="%C2%86"},
  {.len=6,.cptr="%C2%87"},
  {.len=6,.cptr="%C2%88"},
  {.len=6,.cptr="%C2%89"},
  {.len=6,.cptr="%C2%8A"},
  {.len=6,.cptr="%C2%8B"},
  {.len=6,.cptr="%C2%8C"},
  {.len=6,.cptr="%C2%8D"},
  {.len=6,.cptr="%C2%8E"},
  {.len=6,.cptr="%C2%8F"},
  {.len=6,.cptr="%C2%90"},
  {.len=6,.cptr="%C2%91"},
  {.len=6,.cptr="%C2%92"},
  {.len=6,.cptr="%C2%93"},
  {.len=6,.cptr="%C2%94"},
  {.len=6,.cptr="%C2%95"},
  {.len=6,.cptr="%C2%96"},
  {.len=6,.cptr="%C2%97"},
  {.len=6,.cptr="%C2%98"},
  {.len=6,.cptr="%C2%99"},
  {.len=6,.cptr="%C2%9A"},
  {.len=6,.cptr="%C2%9B"},
  {.len=6,.cptr="%C2%9C"},
  {.len=6,.cptr="%C2%9D"},
  {.len=6,.cptr="%C2%9E"},
  {.len=6,.cptr="%C2%9F"},
  {.len=6,.cptr="%C2%A0"},
  {.len=6,.cptr="%C2%A1"},
  {.len=6,.cptr="%C2%A2"},
  {.len=6,.cptr="%C2%A3"},
  {.len=6,.cptr="%C2%A4"},
  {.len=6,.cptr="%C2%A5"},
  {.len=6,.cptr="%C2%A6"},
  {.len=6,.cptr="%C2%A7"},
  {.len=6,.cptr="%C2%A8"},
  {.len=6,.cptr="%C2%A9"},
  {.len=6,.cptr="%C2%AA"},
  {.len=6,.cptr="%C2%AB"},
  {.len=6,.cptr="%C2%AC"},
  {.len=6,.cptr="%C2%AD"},
  {.len=6,.cptr="%C2%AE"},
  {.len=6,.cptr="%C2%AF"},
  {.len=6,.cptr="%C2%B0"},
  {.len=6,.cptr="%C2%B1"},
  {.len=6,.cptr="%C2%B2"},
  {.len=6,.cptr="%C2%B3"},
  {.len=6,.cptr="%C2%B4"},
  {.len=6,.cptr="%C2%B5"},
  {.len=6,.cptr="%C2%B6"},
  {.len=6,.cptr="%C2%B7"},
  {.len=6,.cptr="%C2%B8"},
  {.len=6,.cptr="%C2%B9"},
  {.len=6,.cptr="%C2%BA"},
  {.len=6,.cptr="%C2%BB"},
  {.len=6,.cptr="%C2%BC"},
  {.len=6,.cptr="%C2%BD"},
  {.len=6,.cptr="%C2%BE"},
  {.len=6,.cptr="%C2%BF"},
  {.len=6,.cptr="%C3%80"},
  {.len=6,.cptr="%C3%81"},
  {.len=6,.cptr="%C3%82"},
  {.len=6,.cptr="%C3%83"},
  {.len=6,.cptr="%C3%84"},
  {.len=6,.cptr="%C3%85"},
  {.len=6,.cptr="%C3%86"},
  {.len=6,.cptr="%C3%87"},
  {.len=6,.cptr="%C3%88"},
  {.len=6,.cptr="%C3%89"},
  {.len=6,.cptr="%C3%8A"},
  {.len=6,.cptr="%C3%8B"},
  {.len=6,.cptr="%C3%8C"},
  {.len=6,.cptr="%C3%8D"},
  {.len=6,.cptr="%C3%8E"},
  {.len=6,.cptr="%C3%8F"},
  {.len=6,.cptr="%C3%90"},
  {.len=6,.cptr="%C3%91"},
  {.len=6,.cptr="%C3%92"},
  {.len=6,.cptr="%C3%93"},
  {.len=6,.cptr="%C3%94"},
  {.len=6,.cptr="%C3%95"},
  {.len=6,.cptr="%C3%96"},
  {.len=6,.cptr="%C3%97"},
  {.len=6,.cptr="%C3%98"},
  {.len=6,.cptr="%C3%99"},
  {.len=6,.cptr="%C3%9A"},
  {.len=6,.cptr="%C3%9B"},
  {.len=6,.cptr="%C3%9C"},
  {.len=6,.cptr="%C3%9D"},
  {.len=6,.cptr="%C3%9E"},
  {.len=6,.cptr="%C3%9F"},
  {.len=6,.cptr="%C3%A0"},
  {.len=6,.cptr="%C3%A1"},
  {.len=6,.cptr="%C3%A2"},
  {.len=6,.cptr="%C3%A3"},
  {.len=6,.cptr="%C3%A4"},
  {.len=6,.cptr="%C3%A5"},
  {.len=6,.cptr="%C3%A6"},
  {.len=6,.cptr="%C3%A7"},
  {.len=6,.cptr="%C3%A8"},
  {.len=6,.cptr="%C3%A9"},
  {.len=6,.cptr="%C3%AA"},
  {.len=6,.cptr="%C3%AB"},
  {.len=6,.cptr="%C3%AC"},
  {.len=6,.cptr="%C3%AD"},
  {.len=6,.cptr="%C3%AE"},
  {.len=6,.cptr="%C3%AF"},
  {.len=6,.cptr="%C3%B0"},
  {.len=6,.cptr="%C3%B1"},
  {.len=6,.cptr="%C3%B2"},
  {.len=6,.cptr="%C3%B3"},
  {.len=6,.cptr="%C3%B4"},
  {.len=6,.cptr="%C3%B5"},
  {.len=6,.cptr="%C3%B6"},
  {.len=6,.cptr="%C3%B7"},
  {.len=6,.cptr="%C3%B8"},
  {.len=6,.cptr="%C3%B9"},
  {.len=6,.cptr="%C3%BA"},
  {.len=6,.cptr="%C3%BB"},
  {.len=6,.cptr="%C3%BC"},
  {.len=6,.cptr="%C3%BD"},
  {.len=6,.cptr="%C3%BE"},
  {.len=6,.cptr="%C3%BF"}
};

/*
 * http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf - page 94
 *
 * Table 3-7. Well-Formed UTF-8 Byte Sequences
 *
 * +--------------------+------------+-------------+------------+-------------+
 * | Code Points        | First Byte | Second Byte | Third Byte | Fourth Byte |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0000..U+007F     | 00..7F     |             |            |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0080..U+07FF     | C2..DF     | 80..BF      |            |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+0800..U+0FFF     | E0         | A0..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+1000..U+CFFF     | E1..EC     | 80..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+D000..U+D7FF     | ED         | 80..9F      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+E000..U+FFFF     | EE..EF     | 80..BF      | 80..BF     |             |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+10000..U+3FFFF   | F0         | 90..BF      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+40000..U+FFFFF   | F1..F3     | 80..BF      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 * | U+100000..U+10FFFF | F4         | 80..8F      | 80..BF     | 80..BF      |
 * +--------------------+------------+-------------+------------+-------------+
 */

/* Return 0 - success,  >0 - index(1 based) of first error char */
int utf8_char_lenth(const unsigned char *data, int len)
{
  int bytes;
  const unsigned char byte1 = data[0];

  if (byte1 <= 0x7F) 
  {
    /* 00..7F */
    bytes = 1;
  } 
  else if (len >= 2 && byte1 >= 0xC2 && byte1 <= 0xDF &&
          (signed char)data[1] <= (signed char)0xBF) 
  {
    /* C2..DF, 80..BF */
    bytes = 2;
  } 
  else if (len >= 3) 
  {
    const unsigned char byte2 = data[1];

    /* Is byte2, byte3 between 0x80 ~ 0xBF */
    const int byte2_ok = (signed char)byte2 <= (signed char)0xBF;
    const int byte3_ok = (signed char)data[2] <= (signed char)0xBF;

    if (byte2_ok && byte3_ok &&
         /* E0, A0..BF, 80..BF */
        ((byte1 == 0xE0 && byte2 >= 0xA0) ||
         /* E1..EC, 80..BF, 80..BF */
         (byte1 >= 0xE1 && byte1 <= 0xEC) ||
         /* ED, 80..9F, 80..BF */
         (byte1 == 0xED && byte2 <= 0x9F) ||
         /* EE..EF, 80..BF, 80..BF */
         (byte1 >= 0xEE && byte1 <= 0xEF))) 
    {
      bytes = 3;
    } 
    else if (len >= 4) 
    {
      /* Is byte4 between 0x80 ~ 0xBF */
      const int byte4_ok = (signed char)data[3] <= (signed char)0xBF;

      if (byte2_ok && byte3_ok && byte4_ok &&
               /* F0, 90..BF, 80..BF, 80..BF */
              ((byte1 == 0xF0 && byte2 >= 0x90) ||
               /* F1..F3, 80..BF, 80..BF, 80..BF */
               (byte1 >= 0xF1 && byte1 <= 0xF3) ||
               /* F4, 80..8F, 80..BF, 80..BF */
               (byte1 == 0xF4 && byte2 <= 0x8F))) 
      {
        bytes = 4;
      } 
      else 
      {
        bytes = 1;
      }
    } 
    else 
    {
      bytes = 1;
    }
  }
  else 
  {
    bytes = 1;
  }

  return bytes;
}

int encode_url(httpd_buf_t *b, const uint8_t* input, int len) 
{
  uint8_t* encoded;

  if (!len) len = strlen((const char *)input);
  b->ptr = encoded = (uint8_t*)malloc(sizeof(uint8_t) * len * 6);
  if (encoded == NULL)
  {
    log_fatal("Failed to malloc.");
    return -1;
  }

  int in_cursor = 0;
  int out_cursor = 0;

  while (in_cursor < len) 
  {
    uint8_t charlen;
    charlen = utf8_char_lenth(&input[in_cursor], len - in_cursor);
    if (charlen == 1)
    {
      const struct _str_enc *enc;
      const uint8_t c = input[in_cursor++];
      enc = &url_str_enc[c];
      switch (enc->len)
      {
        case 1:
          encoded[out_cursor++] = enc->cptr[0];
          break;
        case 3:
          encoded[out_cursor++] = enc->cptr[0];
          encoded[out_cursor++] = enc->cptr[1];
          encoded[out_cursor++] = enc->cptr[2];
          break;
        case 6:
          *(uint32_t *)&encoded[out_cursor]  = *(const uint32_t *)&enc->cptr[0];
          out_cursor += 4;
          *(uint16_t *)&encoded[out_cursor]  = *(const uint16_t *)&enc->cptr[4];
          out_cursor += 2;
          break;
      }
    }
    else
    {
      int in_end = in_cursor + charlen;
      while (in_cursor < in_end)
      {
        const uint8_t c = input[in_cursor++];
        encoded[out_cursor++] = '%';
        encoded[out_cursor++] = xdigit[((c)>>4)&0xF];
        encoded[out_cursor++] = xdigit[((c)>>0)&0xF];;
      }
    }
  }

  b->len = out_cursor;
  return b->len;
}

#define __ 256
static const int hexval[256] = {
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 00-0F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 10-1F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 20-2F */
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,__,__,__,__,__,__, /* 30-3F */
  __,10,11,12,13,14,15,__,__,__,__,__,__,__,__,__, /* 40-4F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 50-5F */
  __,10,11,12,13,14,15,__,__,__,__,__,__,__,__,__, /* 60-6F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 70-7F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 80-8F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 90-9F */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* A0-AF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* B0-BF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* C0-CF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* D0-DF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* E0-EF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* F0-FF */
};
#undef __

static const uint8_t url_str_dec_len[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  2,2,2,2,0,2,2,2,0,0,0,0,0,0,0,0,
  0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int decode_url(httpd_buf_t *b, const uint8_t* input, int len) 
{
  uint8_t *decoded;
  int in_cursor = 0;
  int out_cursor = 0;

  if (!len) len = strlen((const char *)input);
  b->ptr = decoded = (uint8_t*)malloc(sizeof(uint8_t) * len);
  if (decoded == NULL)
  {
    log_fatal("Failed to malloc.");
    return -1;
  }
  
  while (in_cursor < len) 
  {
    uint8_t charlen;
    uint8_t c;
    uint8_t v1,v1raw;
    uint8_t v2,v2raw;


    c = input[in_cursor++];
    if (c != '%') {
      decoded[out_cursor++] = c;
      continue;
    }
    
    if (len - in_cursor < 2)
    {
      decoded[out_cursor++] = c;
      continue;
    }
    charlen = url_str_dec_len[input[in_cursor]];
    if (!charlen)
    {
      charlen = utf8_len(&input[in_cursor-1], len - in_cursor+1);
      if (!charlen)
      {
        decoded[out_cursor++] = c;
        continue;
      }
      if (charlen == 1)
      {
        v1raw = input[in_cursor++];
        v2raw = input[in_cursor++];
        if (v1raw == 0x30 && v2raw == 0x30) {
          decoded[out_cursor++] = '\0';
          continue;
        }
        v1 = hexval[v1raw];
        v2 = hexval[v2raw];
        if ((v1 | v2) != 0xFF) 
        {
          decoded[out_cursor++] = (v1 << 4) | v2;
          continue;
        }
        else
        {
          decoded[out_cursor++] = '%';
          decoded[out_cursor++] = v1raw;
          decoded[out_cursor++] = v2raw;
          continue;
        }
      }
      else
      {
        int in_end = in_cursor + charlen;
        while (in_cursor < in_end)
        {
          decoded[out_cursor++] = c;
          c = input[in_cursor++];
        }
        continue;
      }
    }
    else if (charlen == 1 || charlen == 2)
    {
      v1raw = input[in_cursor++];
      v2raw = input[in_cursor++];
      if (v1raw == 0x30 && v2raw == 0x30) {
        decoded[out_cursor++] = '\0';
        continue;
      }
      v1 = hexval[v1raw];
      v2 = hexval[v2raw];
      if ((v1 | v2) != 0xFF) 
      {
        decoded[out_cursor++] = (v1 << 4) | v2;
        continue;
      }
      else
      {
        decoded[out_cursor++] = '%';
        decoded[out_cursor++] = v1raw;
        decoded[out_cursor++] = v2raw;
        continue;
      }
    }
    else if (charlen == 5)
    {
      if (len - in_cursor < 5)
      {
        decoded[out_cursor++] = c;
        in_cursor++;
        continue;
      }
      if (input[in_cursor+2]=='%')
      {
        v1raw = input[in_cursor++];
        v2raw = input[in_cursor++];
        v1 = hexval[v1raw];
        v2 = hexval[v2raw];
        if (v1 != 0xFF && v2 != 0xFF) 
        {
          in_cursor++;
          uint8_t _v1raw = input[in_cursor++];
          uint8_t _v2raw = input[in_cursor++];
          uint8_t v3 = hexval[_v1raw];
          uint8_t v4 = hexval[_v2raw];
          if (v3 != 0xFF && v4 != 0xFF)
          {
            decoded[out_cursor++] = (((v2 << 2) | v3) << 4) | v4;
            continue;
          }
          else
          {
            decoded[out_cursor++] = '%';
            decoded[out_cursor++] = v1raw;
            decoded[out_cursor++] = v2raw;
            decoded[out_cursor++] = '%';
            decoded[out_cursor++] = _v1raw;
            decoded[out_cursor++] = _v2raw;
            continue;
          }
        }
        else
        {
          decoded[out_cursor++] = '%';
          decoded[out_cursor++] = v1raw;
          decoded[out_cursor++] = v2raw;
          continue;
        }
      }
    }

    decoded[out_cursor++] = c;
  }
  
  b->len = out_cursor;
  return b->len;
}

#define UTF8_LEAD(c) ((uint8_t)(c) < 0x80 || ((uint8_t)(c) > 0xC1 && (uint8_t)(c) < 0xF5))
#define UTF8_TRAIL(c) (((uint8_t)(c) & 0xC0) == 0x80)

#define __ 0xFF
/*
 * 0x00: 0
 * 0x01-0xC1: 1
 * 0xF5-: 1
 */
static const uint8_t utf8_immediate_len[256] = {
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00-0x0F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x10-0x1F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x20-0x2F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x30-0x3F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x40-0x4F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x50-0x5F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x60-0x6F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x70-0x7F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x80-0x8F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x90-0x9F */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xA0-0xAF */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xB0-0xBF */
   1, 1,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xC0-0xCF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xD0-0xDF */
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, /* 0xE0-0xEF */
  __,__,__,__,__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xF0-0xFF */
};
#undef __

/*
 * 0xC2-0xDF: 2
 * 0xE0-0xEF: 3
 * 0xF0-0xF4: 4
 */
static const uint8_t utf8_count_len[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x00-0x0F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x10-0x1F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x20-0x2F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x30-0x3F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x40-0x4F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x50-0x5F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x60-0x6F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x70-0x7F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
  0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xC0-0xCF */
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xD0-0xDF */
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* 0xE0-0xEF */
  4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};

static uint8_t utf8_len(const uint8_t* str, int inlen) {
  if (inlen < 2)
  {
    return inlen;
  }
  else
  {
    const uint8_t lead = *str;

    const uint8_t immediate_len = utf8_immediate_len[lead];
    if (immediate_len != 0xFF) {
      return immediate_len;
    }

    const uint8_t count = utf8_count_len[lead];
    uint8_t trail = *(++str);

    if (count == 3) 
    {
      if (inlen < count)
        return 1;
      if ((lead == 0xE0 && 0xA0 > trail) 
        || (lead == 0xED && trail > 0x9F)) 
      {
        return 1;
      }
    } 
    else if (count == 4) 
    {
      if (inlen < count)
        return 1;
      if ((lead == 0xF0 && 0x90 > trail) 
        || (lead == 0xF4 && trail > 0x8F)) 
      {
        return 1;
      }
    }

    uint8_t size = 1;
    for (; size < count; ++size) 
    {
      if (!UTF8_TRAIL(trail)) 
      {
        return size;
      }
      trail = *(++str);
    }
    return size;
  }
}

// Function to calculate the size of the output buffer
size_t calculate_buffer_size(const char *utf8, size_t len) 
{
  size_t size = 1;  // Start with 1 for the null terminator
  unsigned char *ptr = (unsigned char *)utf8;
  const unsigned char *end = ptr + len;
  while (ptr < end) {
    if (*ptr <= 0x7F) {
      // 1-byte character (ASCII)
      size += 6;  // Each escape sequence \uXXXX requires 6 characters
      ptr++;
    } else if ((*ptr & 0xE0) == 0xC0) {
      // 2-byte character
      size += 6;
      ptr += 2;  // Move past 2-byte character
    } else if ((*ptr & 0xF0) == 0xE0) {
      // 3-byte character
      size += 6;
      ptr += 3;  // Move past 3-byte character
    } else if ((*ptr & 0xF8) == 0xF0) {
      // 4-byte character
      size += 6;
      ptr += 4;  // Move past 4-byte character
    }
  }
  return size;
}

#include <stdio.h>

// Function to convert UTF-8 to Unicode escape sequence and store in buffer
void convert_utf8_to_unicode_escape(const char *utf8, 
  size_t len, httpd_buf_t *buffer) 
{
  unsigned char *ptr = (unsigned char *)utf8;
  const unsigned char *end = ptr + len;
  unsigned int codepoint = 0;
  char *buf_ptr = (char *)buffer->ptr;

  while (ptr < end) {
    if (*ptr <= 0x7F) {
      // 1-byte character (ASCII)
      codepoint = *ptr;
      sprintf(buf_ptr, "\\u%04X", codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xE0) == 0xC0) {
      // 2-byte character
      codepoint = *ptr & 0x1F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      sprintf(buf_ptr, "\\u%04X", codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xF0) == 0xE0) {
      // 3-byte character
      codepoint = *ptr & 0x0F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      sprintf(buf_ptr, "\\u%04X", codepoint);
      buf_ptr += 6;
      ptr++;
    } else if ((*ptr & 0xF8) == 0xF0) {
      // 4-byte character
      codepoint = *ptr & 0x07;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      codepoint <<= 6;
      ptr++;
      codepoint |= *ptr & 0x3F;
      sprintf(buf_ptr, "\\u%04X", codepoint);
      buf_ptr += 6;
      ptr++;
    }
    else {
      *buf_ptr++ = *ptr++;
    }
  }

  buffer->len = buf_ptr - (char *)buffer->ptr;
}

#define _TE_INPUT  "c\x01\xff\xfe\xfd"
#define _TE_OUTPUT "c%01%C3%BF%C3%BE%C3%BD"

static void __test_encode_url(const char *output, int outlen, 
  const char *input, int inlen)
{
  httpd_buf_t b;
  if (0 > encode_url(&b, (const uint8_t *)output, outlen))
  {
    log_verbose("test encode_url FAILED.");
    return;
  }
  if (b.len != inlen
    || memcmp(input, b.ptr, b.len))
  {
    log_verbose("test encode_url FAILED.");
    free(b.ptr);
    return;
  }
  free(b.ptr);
  log_verbose("test encode_url OK.");
}

/* "北京" */
#define _TE_INPUT1  "\u5317\u4eac"
#define _TE_OUTPUT1 "%E5%8C%97%E4%BA%AC"
static void test_encode_url1(void)
{
  __test_encode_url(_TE_INPUT, sizeof(_TE_INPUT)-1,
    _TE_OUTPUT, sizeof(_TE_OUTPUT)-1);
}
static void test_encode_url2(void)
{
  __test_encode_url(_TE_INPUT1, sizeof(_TE_INPUT1)-1,
    _TE_OUTPUT1, sizeof(_TE_OUTPUT1)-1);
}

static void test_encode_url3(void)
{
  for (int i=128;i<256;i++)
  {
    const struct _str_enc *e = &url_str_enc[i];
    char output[1]={i};
    __test_encode_url(output, 1, e->cptr, e->len);
  }
}

static void 
__test_decode_url(const char *output, int outlen, 
  const char *input, int inlen)
{
  httpd_buf_t b;
  if (0 > decode_url(&b, (const uint8_t *)output, outlen))
  {
    log_verbose("test decode_url FAILED.");
    return;
  }
  if (b.len != inlen
    || memcmp(input, b.ptr, b.len))
  {
    log_verbose("test decode_url FAILED.");
    free(b.ptr);
    return;
  }
  log_verbose("test decode_url OK.");
  log_verbose("%.*s", b.len, b.ptr);
  free(b.ptr);
}

static void test_decode_url1(void)
{
  __test_decode_url(_TE_OUTPUT1, sizeof(_TE_OUTPUT1)-1, 
    _TE_INPUT1, sizeof(_TE_INPUT1)-1);
}

static void test_decode_url2(void)
{
  __test_decode_url(_TE_OUTPUT, sizeof(_TE_OUTPUT)-1, 
    _TE_INPUT, sizeof(_TE_INPUT)-1);
}

static void test_decode_url3(void)
{
  for (int i=128;i<256;i++)
  {
    const struct _str_enc *e = &url_str_enc[i];
    char output[1]={i};
    __test_decode_url(e->cptr, e->len, output, 1);
  }
}

void test_encode_url(void)
{
  test_encode_url1();
  test_encode_url2();
  test_encode_url3();
}

void test_decode_url(void)
{
  test_decode_url1();
  test_decode_url2();
  test_decode_url3();
}

