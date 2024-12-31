#include "nanohttp-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "nanohttp-logging.h"
#include "nanohttp-server.h"
#include "nanohttp-json.h"
#include "nanohttp-file.h"
#include "nanohttp-form.h"
#include "nanohttp-urlencode.h"
#include "nanohttp-base64.h"
#include "nanohttp-user.h"

static int
simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{
  httpd_user_t *auth_user;
  log_debug("logging in user=\"%s\" password=\"%s\"\n", user, password);

  auth_user = nanohttp_users_match(user, strlen(user), password, strlen(password));
  if (auth_user == NULL) {
    log_error("authenticate failed\n");
    return 0;
  }

  req->userLevel = auth_user->type;
  return 1;
}

/**********************************************************************************
*  ADD JSON FILE HERE
**********************************************************************************/
static void
default_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  char buf[REQUEST_MAX_PATH_SIZE+256+1];

  if (!strcmp(req->path, "/device.json"))
  {
    httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    http_output_stream_write_string(conn->out,
      "{"
        "'shortAddr':10,"
        "'channel':12"
      "}");
  }
  else
  {
    snprintf(buf, sizeof buf, "Resource \"%s\" not found", req->path);
    httpd_send_not_found(conn, buf);
  }
  return;
}

static void
headers_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  hpair_t *walker;
  char buf[512];
  size_t len;

  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Request headers</title>"
      "</head>"
      "<body>"
        "<h1>Request headers</h1>"
        "<ul>");

  for (walker=req->header; walker; walker=walker->next)
  {
    len = snprintf(buf, 512, "<li>%s: %s</li>", walker->key, walker->value);
    http_output_stream_write(conn->out, (unsigned char *)buf, len);
  }

  http_output_stream_write_string(conn->out,
        "</ul>"
      "</body>"
    "</html>");
}

static void
mime_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_not_implemented(conn, "mime_service");
}

static size_t 
__multipart_cb_headers_complete(multipartparser *p)
{
  if (p->arg == NULL)
  {
    char buf[256];
    char *file = "uploads/gw.bin";

    if (p->value_length)
    {
      if (p->field_length == 19 && !strncmp("Content-Disposition",p->field,19))
      {
        char *f = strstr(p->value,"filename");
        char *end = p->value + p->value_length;

        /*"Content-Disposition: form-data; name=\"File\"; filename=\"nanoHttpd-2.10.tar.xz\"\r\nContent-Type: application/x-xz\r\n\r\ny7zXZ"*/
        if (f != NULL)
        {
          for (f += 8;f < end;f++)
          {
            if (*f == '"')
              break;
          }
          if (f < end)
          {
            char *e;
            f++;
            while(f <end && isspace((int)*f))
            {
              f++;
            }
            e = f;
            while (e < end && *e != '"')
              e++;
            if (e <= end && e-f < sizeof(buf)-1)
            {
              snprintf(buf,sizeof(buf),"%s/%.*s","uploads",(int)(e-f), f);
              file = buf;
            }
          }
        }
      }
    }

    p->arg = nanohttp_file_open_for_write(file);
    if (p->arg == NULL)
    {
      log_error("Not able to open the file %s.", file);
      return -1;
    }
  }
  return 0;
}

static size_t
__multipart_cb_data(multipartparser *p, const char* data, size_t size)
{
  return nanohttp_file_write(p->arg, data, size) != size;
}

static size_t
__multipart_cb_header_field(multipartparser *p, const char* data, size_t size)
{
  if (p->content_disposition_parsed)
    return 0;

  p->field_length+=snprintf(p->field+p->field_length, sizeof(p->field)-p->field_length, "%.*s", (int)size, data);
  return 0;
}

static size_t
__multipart_cb_header_value(multipartparser *p, const char* data, size_t size)
{
  if (p->field_length == 19 && !strncmp("Content-Disposition",p->field,19))
  {
    p->content_disposition_parsed = 1;
  }
  p->value_length+=snprintf(p->value+p->value_length, sizeof(p->value)-p->value_length, "%.*s", (int)size, data);
  return 0;
}

static herror_t
__post_service(httpd_conn_t *conn, struct hrequest_t *req, 
  const char *file)
{
  herror_t r = NULL;
  if (req->method == HTTP_REQUEST_POST)
  {
    multipartparser p;
    multipartparser_callbacks settings;
    multipartparser_callbacks_init(&settings);
    settings.on_data = __multipart_cb_data;
    settings.on_header_field = __multipart_cb_header_field;
    settings.on_header_value = __multipart_cb_header_value;
    settings.on_headers_complete = __multipart_cb_headers_complete;

    if (!req->content_type || !req->content_type->params
      || strcmp(req->content_type->params->key, "boundary")
      || !req->content_type->params->value)
    {
      r = herror_new("post_service", FILE_ERROR_READ, 
        "Failed to read stream %s", req->path);
      return r;
    }

    multipartparser_init(&p, NULL, req->content_type->params->value);
    while (http_input_stream_is_ready(req->in))
    {
      unsigned char buffer[1024];
      size_t len = http_input_stream_read(req->in, buffer, 1024);
      if (len == -1)
      {
        r = herror_new("post_service", FILE_ERROR_READ, 
          "Failed to read stream %s", req->path);
        break;
      }

      if (len != multipartparser_execute(&p, &settings, (const char *)buffer, len))
      {
        r = herror_new("post_service", FILE_ERROR_WRITE, 
          "Failed to read stream %s", req->path);
        break;
      }
    }

    nanohttp_file_close(p.arg);
  }
  else
  {
    r = httpd_send_not_implemented(conn, "post_service");
  }

  if (r != NULL)
  {
    log_error("%s", herror_message(r));
  }
  return r;
}

static void
post_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = __post_service(conn, req, "config/gw.bin");
  if (r == NULL)
  {
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  }
  else
  {
    r = httpd_send_header(conn, 500, HTTP_STATUS_500_REASON_PHRASE);
  }

  herror_release(r);
}

static herror_t
__root_service_read(void *arg, const char *buf, size_t length)
{
  httpd_conn_t *conn=(httpd_conn_t *)arg;
  return http_output_stream_write(conn->out, (const unsigned char *)buf, length);
}

/*
"<html>"
  "<head>"
    "<title>nanoHTTP server examples</title>"
  "</head>"
  "<body>"
    "<h1>nanoHTTP server examples</h1>"
    "<ul>"
      "<li><a href=\"/\">Simple service</a></li>"
      "<li><a href=\"/secure\">Secure service</a> (try: bob/builder)</li>"
      "<li><a href=\"/headers\">Request headers</a></li>"
        "<li><a href=\"/mime\">MIME service</a></li>"
        "<li>"
        "<form action=\"/post\" method=\"post\" enctype=\"multipart/form-data\">"
          "<fieldset>"
            "<legend>Upload file</legend>"
            "<input name=\"Text\" type=\"text\" value=\"test-field\"? "
            "<input name=\"File\" type=\"file\" accept=\"text/\*\"> "
            "<input value=\"Submit\" type=\"submit\">"
          "</fieldset>"
        "</form>"
        "</li>"
      "<li><a href=\"/not_existent\">The default service</a></li>"
      "<li><a href=\"/nhttp\">Admin page</a> (try -NHTTPDadmin on the command line)</li>"
    "</ul>"
  "</body>"
"</html>");
*/

static const char *nanohttp_index_html_body =
    "<div class=\"workingBusy\"></div>"
    "<table class=\"main-layout\">"
      "<tr class='head'>"
        "<td class=\"table-head left\">"
          "<img class=\"logo\" src=\"\">"
        "</td>"
        "<td class=\"table-head right\">"
          "<span class=\"title\"></span>"
          "<a class=\"login\"><img src=\"\"></a>"
        "</td>"
      "</tr>"
      "<tr class='main'>"
        "<td class=\"table-left\">"
          "<div class=\"table-left-container\">"
          "</div>"
        "</td>"
        "<td class=\"table-right\">"
          "<div class=\"panel_container\">"
          "</div>"
        "</td>"
    "	</tr>"
    "</table>"
    "<script>\n"
      "var userjs = $('html > head > script.userjs');\n"
      "if (userjs.length) userjs.remove();\n"
    "</script>";

static herror_t
__send_menu_js(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = H_OK;
  do
  {
    if (req->userLevel == _N_http_user_type_ADMIN)
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_ADMIN);
    else if (req->userLevel == _N_http_user_type_SUPER)
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_SUPER);
    else
      r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_JS_MENU_GUEST);
    if (r != H_OK) break;
  } 
  while(0);
  
  return r;
}
static herror_t
__send_root_html(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = H_OK;

  do
  {
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    if (r != H_OK) break;

    r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL1);
    if (r != H_OK) break;

    r = __send_menu_js(conn, req);
    if (r != H_OK) break;
    
    r = http_output_stream_write_buffer(conn->out, nanohttp_index_html_head_DECL2);
    if (r != H_OK) break;

    r = http_output_stream_write_string(conn->out, "<body>");
    if (r != H_OK) break;
    r = http_output_stream_write_string(conn->out, nanohttp_index_html_body);
    if (r != H_OK) break;
    r = http_output_stream_write_string(conn->out, "</body></html>");
  } 
  while(0);

  return r;
}

static void
secure_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

  r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  if (r == H_OK)
  {
    log_info("secure_service received, redirect to index.html");
    do {
      r = http_output_stream_write_string(conn->out, nanohttp_index_html_body);
      if (r != H_OK) break;
      r = __send_menu_js(conn, req);
      if (r != H_OK) break;
    }
    while (0);
  }
  else
  {
    r = http_output_stream_write_string(conn->out,
      "<html>"
        "<head>"
          "<title>Secure ressource!</title>"
        "</head>"
        "<body>"
          "<h1>Authenticated access!!!</h1>"
        "</body>"
      "</html>");
  }

  herror_release(r);
  return;
}

extern const char *nanohttp_index_html_head;
#define favorICON "AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAABAAACMuAAAjLgAAAAAAAAAAAAAAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAA" \
"NAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAAwAAwwMABFSEgAQTRIAAggLAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAAcgDQATWB" \
"MACi8PAAAACwAAAAwAAQcMABFPEgARTxIAAQcLAAAADAAAAA0AAAANAAAADQAAAA0AAAAMACSpIAA072kANvmWADb5lAAz7WMAIZwcAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAqwisANfR6ADb6mQAy6YYAJaw7ABhzFQAy610ANvmVADb5l" \
"QAz610AGnkWAAAADAAAAA0AAAANAAAADAAioB4ANvmTADf/ywA3/8QAN//FADf/ygA194kAHYYZAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAAwAJrAiADb7nwA3/8sANvvDAC/c0AAmsbwAL9yKADf/xQA3/8UAN//FADf/yAA08W0ABRgOAAAADQAAAA0AAAAL" \
"ADPsXgA3/8oANvqaAC7YPgAv3EMANvujADf/yAAx5lIAAAAKAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAEFDQA08W0AN//MADb5kQAszTcAJrSEACav2AA08s0AN/64ADHkTgAx5E4AN/66ADb9rwAkpyEAAAAMAAAADQADDw4ANfaFADf/yAAx5EkAAAAIAAAACAAy6lc" \
"AN//LADT0eQAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAAsAK8kuADf9tQA3/rsALtY3AAAABQAejkoALdPMADf/ygA083QAAw4LAByCFQA2+6AAN/66ACm+KwAAAAsAAAANAAcjDwA194oAN//FAC7ZPAAAAAoAAAAKADHjSgA3/8kANfaAAAAADQAAAA0AAAANAAAADQ" \
"AAAA0AAAANAAszDwA19XwAN//MADT0eAAIKA4AAAAJACvLTwA1+MkANv2vACnAKQAAAAsAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAA0AAAANAAAADQAAAAsALtc6ADf+vAA3/bIAKsYsA" \
"AAACgAYcBQANvmOADf9zgAv240ABRoPAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAANAAAADQAAAA0AAAANABRgEwA1+IoAN//LADTwaQAAAAwAAAAKADHkTAA3/8IAM+zPACe3jwADEQ4A" \
"AAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAAAA0AAAANAAAADQAAAAoAMOFHADf/wwA2/KgAJrIiAAAACwAfkxoANvqcADf9ywAt0NQAKsSNAAYcDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwA" \
"AAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAA0AAAAMAB2HGAA2+pcAN//JADPrWwAAAAsAAAALADLrWgA3/8YAM+zCACzN0wAx5YkABhsOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAA" \
"AACgAw4UsAN//JADX2gAAAAA0AAAANAAAADQAAAAsAMulUADf/xwA2+p0AIJUbAAAACwAlryIANvyoADf/wAAv3YIAMeXJADX1hwAFGA4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAAAA0AA" \
"AAMACOlHwA2+6MAN//FADHlTAAAAAoAAAAMADTwaQA3/8sANfiIAC7WSwA2+sgANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAAsAM+5jADf/ywA1+JAAGHAVAAAACgAq" \
"xCsAN/2yADf+vQAu1TgAL9xBADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAALACi9KAA2/K4AN/6/AC/cPwAAAAoACCQOADX0dwA3/8wANfV9AAAADAAv3UMAN//HADX" \
"3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAEEg0ANPJyADf/zAA19oEADkQQAAAACgAt1DYAN/66ADf9tQAsyy8AAAAIAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb" \
"8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19YAAAAALACzOMgA3/bcAN/24ACzPMwAAAAoAElMRADX3hgA3/8wANPJuAAIJDQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1" \
"z0AAAAKAAAACgAw4UsAN//JADX1fwANPxAANfaAADf/zAA083MABRYNAAAACgAw30MAN/7BADb8rAAnuCUAAAALAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/skAM+5+AC/a" \
"PgA3/r8ANv2vACm/KAAAAAoAG30WADb5lAA3/8oAM+1gAAAACwAAAA0AAAAKAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMeJLADX3ygAu2IwANfiOADf/ygAz72QAAAALAAAACwAy5lAAN//" \
"GADb7oQAioB0AAAAMAAAADQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw3ksAMN/MAC3TvAA3/8MANvukACSnHwAAAAsAIp4dADb7oAA3/8YAMudRAAAACwAAAA0AAAANAAAACgAv3U" \
"QAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKACrFTQAryNAAMunNADf/xQAy6VUAAAALAAAACwAz7V8AN//KADb5lAAbfxcAAAAMAAAADQAAAA0AAAAKAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7n" \
"wA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAJKdPACzPzgA3/csANvqXAB2IGAAAAAsAJ7clADb8qwA3/sEAMN9EAAAACgAAAA0AAAANAAAADQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeK" \
"ADf/xQAu1z0AAAAKAAAACgAmsE4AM+7KADf/wAAw4UcAAAAKAAEGDQA08m4AN//MADX3hwASVhIAAAANAAAADQAAAA0AAAANAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAMADPtbAA3/sw" \
"ANPGUABVjFAAAAAoAK8ovADf9tQA3/rsALtU3AAAACwAAAA0AAAANAAAADQAAAA0AAAAKAC/dRAA3/8cANfeHAAQWDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByMPADX3igA3/8UAL9w8AAAABwAqwykAN/2wADX0zwAntocAAAALAAs1DwA19XwAN//MADT0eQ" \
"AIKA4AAAANAAAADQAAAA0AAAANAAAADQAAAAoAL9tBADf/xwA1+IwACzcOAAAACwAeixcANvujADf+ugApvSoAAAALAAAADQABBw0ANfaCADf/ygAz610AG34YADT0eQA3/8oALdDUACOjpAAdhyAAL91DADf+vQA3/bMAKsUsAAAACwAAAA0AAAANAAAADQAAAA0AAAANA" \
"AAACwApwCwAN/23ADf9tAAu1z0AGHEWADHkTgA3/sAANvynACCVHQAAAAwAAAANAAAACwAx5E4AN//EADf+vQA2+6cAN//FADb6twAms7wAKsPTADPsrAA3/rcAN//JADPwZwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAovDwA083QAN//KADf+ugA2+6MA" \
"N/6/ADf/xQAz7F0AAAAMAAAADQAAAA0AAAANABNZEgAz7WIANvysADf+uwA2+6EAMN9GACGaOgAv2ZkANvq6ADf9sgA08nMAHYkYAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAB6KGQA08W0AN/ytADf+uwA2/KcAM+xfABViEwAAAAwAAAANAAAADQA" \
"AAA0AAAAMAAcfDQAioiAAKcArAB6OGwABBAwAAAALABlzFgAovSoAJrAkAA07DwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAAgmDQAjoyAAKcAsACCWHAAEFAwAAAAMAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAAAAsAAAAMAA" \
"AADQAAAA0AAAAMAAAACwAAAAsAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAAAAsAAAAMAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AA" \
"AANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="

static void
root_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = NULL;

  if (strstr(req->path, "..") != NULL)
  {
    return default_service(conn, req);
  }
  else
  {
    const char *path = req->path+1;
    if (!strcmp("/", req->path))
    {
      r = __send_root_html(conn, req);
    }
    else if (!strcmp("favicon.ico", path))
    {
      unsigned char *bf=NULL;
      do
      {
        int len;
        bf = (unsigned char *)malloc(B64_DECLEN(sizeof(favorICON)-1)+1);
        if (bf == NULL) break;
#if __configUseStreamBase64
        ng_buffer_s in, out;
        in.cptr = favorICON;
        in.len  = sizeof(favorICON)-1;
        out.ptr = bf;
        len = b64Decode(&in, &out);
        if (len < 0)
        {
          log_error("b64Decode failed");
          break;
        }
#else      
        len = base64_decode_string((const unsigned char *)favorICON, bf);
#endif
        if (httpd_set_header(conn, HEADER_CONTENT_TYPE, "image/png")) break;

        r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
        if (r != NULL) break;
        r = http_output_stream_write(conn->out, bf, len);
        if (r != NULL) break;
      } 
      while(0);
      
      if (bf != NULL)
        free(bf);
    }
    else if (strcmp("data/setmib.json", path)
      && strcmp("data/add.json", req->path)
      && strcmp("data/del.json", req->path)
      && strcmp("data/save.json", req->path)
#if !__NHTTP_TEST      
      && strcmp("data/deivce.json", req->path)
      && strcmp("data/users.json", req->path)
      && strcmp("data/system.json", req->path)
#endif      
      )
    {
      log_debug("Try to open the file %s.", path);

      void *file = nanohttp_file_open_for_read(path);
      if (file == NULL)
      {
        // If the file does not exist
        log_error("Not able to open the file %s.", path);
        r = httpd_send_header(conn, 404, HTTP_STATUS_404_REASON_PHRASE);
      }
      else
      {
        r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
        if (r == NULL)
        {
          r = nanohttp_file_read(file, __root_service_read, conn);
          if (r != NULL)
          {
            char buf[1024];
            size_t n = snprintf(buf, sizeof buf, 
              "Failed to readfile %s: %s", path, herror_message(r));
            log_error("%s", buf);
            herror_release(r);
            r = http_output_stream_write(conn->out, (const unsigned char *)buf, n-1);
          }
        }
        nanohttp_file_close(file);
      }
    }
    else
    {
      return default_service(conn, req);
    }
  }
  
  herror_release(r);
}

#define ___CFG_TEST "{\"err\":0}"

#define ___USER_PFX_STR "2.0."
#define ___USER_PFX_LEN 4 

#define __CFG_ret0(x) "{\"id\":%d,\"err\":[{\"id\":\"" x "\",\"reason\":\""
#define CFG_ret1 __CFG_ret0("%.*s")
#define CFG_ret3 "\"}]}"
#define CFG_RET0(msg) __CFG_ret0("") msg CFG_ret3
#define CFG_RET1(msg) CFG_ret1 msg CFG_ret3

static void
data_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

#if __NHTTP_TEST      
  if (!strcmp("/data/system.json", req->path)
    || !strcmp("/data/device.json", req->path)
    || !strcmp("/data/users.json", req->path)
    )
  {
    return root_service(conn, req);
  }
#endif

  if (req->query != NULL)
  {
    char buf[126]={0};
    int n, len, id;
    unsigned char *query;
    unsigned char *data = NULL;
    JSONPair_t *pair,*p;
    JSONStatus_t result;
    httpd_buf_t in;

    data = (unsigned char *)hpairnode_get(req->query, "data");
    if (data == NULL)
    {
      log_error("No query data");
      goto finished;
    }
    
    log_debug("try to decode %s\n",(char *)data);
    
    /* decode_url malloced new data */
    if (0 > decode_url(&in, (const uint8_t*)data, strlen((char *)data)))
    {
      log_error("Failed to parse query");
      goto finished;
    }
    
    log_debug("decoded query is : %.*s", in.len, in.cptr);

    query = (unsigned char *)malloc(B64_DECLEN(in.len) + 1);
    if (query == NULL)
    {
      log_error("Failed to malloc key");
      goto finished;
    }
    
    {
#if __configUseStreamBase64
      ng_buffer_s out;
      out.ptr = query;
      len = b64Decode(&in, &out);
      if (len < 0)
      {
        free(in.ptr);
        log_error("b64Decode failed");
        goto finished;
      }
#else      
      len = base64_decode_string(in.ptr, query);
#endif
      free(in.ptr);
      log_debug("decoded query is : %s", query);
    }

    result = json_parse(&pair, (const char *)query, len);
    if (result != JSONSuccess)
    {
      log_error("Failed to parse json");
      goto finished;
    }

    p = json_find_bykey(pair, "id", 2);
    if (p == NULL || p->jsonType != JSONNumber)
    {
      log_error("Bad id in json");
      goto finished;
    }
    id = (int)p->vint;
    
    if (!strcmp("/data/setmib.json", req->path))
    {
      // Process del operation
      int err, usrLen;

      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }

      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONObject)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Mailformed Request."), id);
        goto finished;
      }
      
      for (p=p->children; p != NULL; p = p->siblings)
      {
        if (p->keyLength >= ___USER_PFX_LEN 
          && !memcmp(p->key, ___USER_PFX_STR, ___USER_PFX_LEN))
        {
          const char *colon;
          
          if (req->userLevel != _N_http_user_type_SUPER)
          {
            // Process set operation
            n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
            goto finished;
          }
          
          colon = memchr(p->key, ':', p->keyLength);
          if (colon != NULL && colon - p->key > ___USER_PFX_LEN)
          {
            const char *usr;
            
            usr = colon + 1;
            usrLen = p->keyLength - (usr - p->key) - 2;
            if ((usrLen < _N_http_user_NAME_MINLEN
              || usrLen > _N_http_user_NAME_MAXLEN))
            {
              n = snprintf(buf, sizeof buf, CFG_RET1("Invalid username length."), id, 
                    p->keyLength, p->key);
              goto finished;
            }
            if (!memcmp(p->key+p->keyLength-2, ".0", 2))
            {
              n = snprintf(buf, sizeof buf, CFG_RET1("This user does not exists."), id, 
                    p->keyLength, p->key);
            }
            else 
            {
              if (!memcmp(p->key+p->keyLength-2, ".1", 2))
              {
                err = nanohttp_users_update(usr, usrLen, p->value, p->valueLength, NULL, 0);
              }
              else if (!memcmp(p->key+p->keyLength-2, ".2", 2))
              {
                err = nanohttp_users_update(usr, usrLen, NULL, 0, p->value, p->valueLength);
              }
              else
              {
                err = _N_http_user_error_SYS;
              }
              switch (err)
              {
                case _N_http_user_error_NONE:
                  n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
                  break;
                case _N_http_user_error_PERM:
                  n = snprintf(buf, sizeof buf, CFG_RET1("Superuser can not be deleted."), id, 
                        (int)p->keyLength, p->key);
                  goto finished;
                case _N_http_user_error_EXIST:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("This user does not exists."), id, 
                        p->keyLength, p->key);
                  goto finished;
                case _N_http_user_error_VNAME:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("The length of username is invalid: [%d,%d]."), id, 
                        p->keyLength, p->key,
                        _N_http_user_NAME_MINLEN, _N_http_user_NAME_MAXLEN);
                  goto finished;
                case _N_http_user_error_INVAL:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("Invalid account type."), id, 
                        p->keyLength, p->key);
                  goto finished;
                case _N_http_user_error_SYS:
                default:
                  n = snprintf(buf, sizeof buf, 
                        CFG_RET1("System error."), id, 
                        p->keyLength, p->key);
                  goto finished;
              }
            }
          }
          else
          {
            n = snprintf(buf, sizeof buf, CFG_RET1("Invalid parameter."), id, 
                  p->keyLength, p->key);
            goto finished;
          }
        }
        else
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, 
            CFG_RET1("For testing received error message from our product."), 
            id, p->keyLength, p->key);
        }
      }
    }
    else if (!strcmp("/data/save.json", req->path))
    {
      if (req->userLevel > _N_http_user_type_ADMIN)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Process save operation
      n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }
    else if (!strcmp("/data/add.json", req->path))
    {
      int err;
      JSONPair_t *usr, *pwd, *type;
      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONObject)
      {
        goto finished;
      }
      usr = json_find_bykey_head_tail(p->children,  
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.0", 3);
      pwd = json_find_bykey_head_tail(p->children,  
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.1", 3);
      type = json_find_bykey_head_tail(p->children, 
              ___USER_PFX_STR, ___USER_PFX_LEN, ":.2", 3);
      if (usr != NULL || pwd != NULL)
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        if (!usr || !pwd)
        {
          if (usr)
            n = snprintf(buf, sizeof buf, CFG_RET1("No password."), id, 
                  (int)usr->keyLength-1, usr->key);
          else
            n = snprintf(buf, sizeof buf, CFG_RET0("No username."), id);
          goto finished;
        }
        err = nanohttp_users_add(
          usr->value, 
          usr->valueLength,
          pwd->value, 
          pwd->valueLength,
          type ? type->value : NULL,
          type ? type->valueLength : 0
          );
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_PERM:
            n = snprintf(buf, sizeof buf, CFG_RET1("Superuser can not be deleted."), id, 
                  (int)usr->keyLength, usr->key);
            goto finished;
          case _N_http_user_error_EXIST:
            n = snprintf(buf, sizeof buf, CFG_RET1("This user already exists."), id, 
                  (int)usr->keyLength, usr->key);
            goto finished;
          case _N_http_user_error_VNAME:
            n = snprintf(buf, sizeof buf, CFG_RET1("The length of username is invalid: [%d,%d]."), id, 
                  (int)usr->keyLength, usr->key, 
                  _N_http_user_NAME_MINLEN, _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_VPSWD:
            n = snprintf(buf, sizeof buf, CFG_RET1("The length of username is invalid: [%d,%d]."), id, 
                  (int)pwd->keyLength, pwd->key, 
                  _N_http_user_PSWD_MINLEN, _N_http_user_PSWD_MAXLEN);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = snprintf(buf, sizeof buf, CFG_RET1("System error."), id, 
                  (int)usr->keyLength, usr->key);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), 
            id);
          goto finished;
        }
        n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (!strcmp("/data/del.json", req->path))
    {
      // Process del operation
      int err, usrLen;
      const char *colon, *usr;
      // Process add operation
      p = json_find_bykey(pair, "value", 5);
      if (p != NULL && p->jsonType != JSONString)
      {
        goto finished;
      }

      if (!memcmp(p->value, ___USER_PFX_STR, ___USER_PFX_LEN))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }

        colon = memchr(p->value, ':', p->valueLength);
        if (colon == NULL || colon-p->value < ___USER_PFX_LEN + 1)
        {
          n = snprintf(buf, sizeof buf, CFG_RET1("This user does not exists."), id, 
                (int)p->valueLength, p->value);
          goto finished;
        }
        usr = colon + 1;
        usrLen = p->valueLength - (usr - p->value);
        if ((usrLen < _N_http_user_NAME_MINLEN
          || usrLen > _N_http_user_NAME_MAXLEN))
        {
          n = snprintf(buf, sizeof buf, CFG_RET1("Invalid username length."), id, 
                (int)p->valueLength, p->value);
          goto finished;
        }
        
        err = nanohttp_users_del(usr, usrLen);
        switch (err)
        {
          case _N_http_user_error_NONE:
            n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
            break;
          case _N_http_user_error_EXIST:
            n = snprintf(buf, sizeof buf, CFG_RET1("This user does not exists."), id, 
                  (int)p->valueLength, p->value);
            goto finished;
          case _N_http_user_error_VNAME:
            n = snprintf(buf, sizeof buf, CFG_RET1("The length of username is invalid: [%d,%d]."), id, 
                  (int)p->valueLength, p->value,
                  _N_http_user_NAME_MINLEN, _N_http_user_NAME_MAXLEN);
            goto finished;
          case _N_http_user_error_PERM:
            n = snprintf(buf, sizeof buf, CFG_RET1("Superuser can not be deleted."), id, 
                  (int)p->valueLength, p->value);
            goto finished;
          case _N_http_user_error_SYS:
          default:
            n = snprintf(buf, sizeof buf, CFG_RET1("System error."), id, 
                  (int)p->valueLength, p->value);
            goto finished;
        }
      }
      else
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
      }
    }
    else if (!strcmp("/data/template.json", req->path))
    {
      // Generate template.json
      p = json_find_bykey(pair, "value", 5);
      if (p == NULL || p->jsonType != JSONString)
      {
        goto finished;
      }
      if (p->valueLength == 3 && !strncmp(p->value,"2.0",3))
      {
        if (req->userLevel != _N_http_user_type_SUPER)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        snprintf(req->path, sizeof(req->path)-1, "/data/templateUser.json");
      }
      else if (p->valueLength > 2 && !memcmp(p->value,"1.",2))
      {
        if (req->userLevel > _N_http_user_type_ADMIN)
        {
          // Process set operation
          n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
          goto finished;
        }
        snprintf(req->path, sizeof(req->path)-1, "/data/templateInterface.json");
      }
      else
      {
        n = snprintf(buf, sizeof buf, CFG_RET0("Bad Request."), id);
        goto finished;
      }
      
      free(query);
      json_pairs_free(pair);
      root_service(conn, req);
      return;
    }
    else if (!strcmp("/data/users.json", req->path))
    {
      if (req->userLevel != _N_http_user_type_SUPER)
      {
        // Process set operation
        n = snprintf(buf, sizeof buf, CFG_RET0("Authorization Failed."), id);
        goto finished;
      }
      // Generate template.json
      n = snprintf(buf, sizeof buf, "{\"id\":%d}", id);
    }

finished:  
    free(query);
    json_pairs_free(pair);
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    herror_release(r);
    if (*buf)
    {
      r = http_output_stream_write(conn->out, (unsigned char *)buf, n);
      herror_release(r);
    }
    return;
  }
  else
  {
    if (!strcmp("/data/system.json", req->path))
    {
      // Generate system.json
    }
    else
    {
      r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
    }
    herror_release(r);
    return;
  }
}

static void
file_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  return root_service(conn, req);
}

void json_show( const char * json,
           size_t length )
{
  size_t start = 0, next = 0;
  JSONPair_t pair = { 0 };
  JSONStatus_t result;

  result = JSON_Validate( json, length );
  if( result == JSONSuccess )
  {
    result = JSON_Iterate( json, length, &start, &next, &pair );
  }

  while( result == JSONSuccess )
  {
    if( pair.key != NULL )
    {
      printf( "key: %.*s\t", ( int ) pair.keyLength, pair.key );
    }

    printf( "value: (%s) %.*s\n", json_type2str(pair.jsonType),
            ( int ) pair.valueLength, pair.value );

    result = JSON_Iterate( json, length, &start, &next, &pair );
  }
}

int json_test(void)
{
  // Variables used in this example.
  JSONStatus_t result;
  char buffer[] = "{\"foo\":\"abc\",\"bar\":{\"foo\":\"xyz\"},"
    "\"ar\":[{\"a1\":0,\"a2\":true},{\"a1\":1,\"a2\":false}]}";
  size_t bufferLength = sizeof( buffer ) - 1;
  char queryKey[] = "bar.foo";
  size_t queryKeyLength = sizeof( queryKey ) - 1;
  char * value;
  size_t valueLength;

  // Calling JSON_Validate() is not necessary if the document is 
  // guaranteed to be valid.
  result = JSON_Validate( buffer, bufferLength );

  if( result == JSONSuccess )
  {
    result = JSON_Search( buffer, bufferLength, queryKey, queryKeyLength,
                          &value, &valueLength );
  }

  if( result == JSONSuccess )
  {
    // The pointer "value" will point to a location in the "buffer".
    char save = value[ valueLength ];
    // After saving the character, set it to a null byte for printing.
    value[ valueLength ] = '\0';
    // "Found: bar.foo -> xyz" will be printed.
    printf( "Found: %s -> %s\n", queryKey, value );
    // Restore the original character.
    value[ valueLength ] = save;
  }
  
  json_show(buffer,strlen(buffer));

  printf("\n\n");
  JSONPair_t *p;
  result = json_parse(&p, buffer,bufferLength);

  //json_print(p,0,"  ");
  //printf("\n\n");
  //json_print(p,-1,"  ");
  //printf("\n\n");

  char buf[1024];
  int m = json_cal_length(p,0,"  ");
  printf("%d=====\n\n",m);
  json_tostr(p,buf,sizeof(buf),0,"  ");
  printf("%lu=====\n\n",strlen(buf));
  printf("%s\n\n",buf);

  int n = json_cal_length(p,-1,"  ");
  printf("%d=====\n\n",n);
  json_tostr(p,buf,sizeof(buf),-1,"  ");
  printf("%lu=====\n\n",strlen(buf));
  printf("%s\n\n",buf);

  json_pairs_free(p);

  return 0;
}

int
main(int argc, char **argv)
{
  herror_t status;

  nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);

  test_encode_url();
  test_decode_url();
  json_test();
  
  if (httpd_init(argc, argv))
  {
    fprintf(stderr, "Cannot init httpd\n");
    goto error0;
  }

  if ((status = httpd_register_secure("/", root_service, 
    simple_authenticator, "ROOT")) != H_OK)
  {
    fprintf(stderr, "Cannot register service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register(_nanoConfig_HTTPD_FILE_SERVICE, 
    file_service, "FILE")) != H_OK)
  {
    fprintf(stderr, "Cannot register service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_secure("/secure", secure_service, 
    simple_authenticator, "SECURE")) != H_OK)
  {
    fprintf(stderr, "Cannot register secure service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_secure("/headers", headers_service, 
    simple_authenticator, "HEAD")) != H_OK)
  {
    fprintf(stderr, "Cannot register headers service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_secure("/mime", mime_service, 
    simple_authenticator, "MIME")) != H_OK)
  {
    fprintf(stderr, "Cannot register MIME service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_secure("/post/wia.bin", post_service, 
    simple_authenticator, "POST")) != H_OK)
  {
    fprintf(stderr, "Cannot register POST service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register("/favicon.ico", root_service, 
    "FAVICON")) != H_OK)
  {
    fprintf(stderr, "Cannot register default service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_secure("/data/", data_service, 
    simple_authenticator, "DATA")) != H_OK)
  {
    fprintf(stderr, "Cannot register DATA service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_default_secure("/error", default_service, 
    simple_authenticator)) != H_OK)
  {
    fprintf(stderr, "Cannot register default service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if (nanohttp_users_init() < 0)
  {
    fprintf(stderr, "Cannot init users.\n");
    goto error1;
  }

  if ((status = httpd_run()) != H_OK)
  {
    fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
    herror_release(status);
    goto error1;
  }

  nanohttp_users_free();
  httpd_destroy();
  return 0;
    
error0:
  httpd_destroy();
error1:
  return 1;
}
