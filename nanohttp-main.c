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

static int
simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{
  log_debug("logging in user=\"%s\" password=\"%s\"\n", user, password);

  if (strcmp(user, "bob")) {

    log_error("user \"%s\" unkown\n", user);
    return 0;
  }

  if (strcmp(password, "builder")) {

    log_error("wrong password\n");
    return 0;
  }

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
    "</table>";

static void
secure_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

  r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  if (r == H_OK)
  {
    log_info("secure_service received, redirect to index.html");
    r = http_output_stream_write_string(conn->out, nanohttp_index_html_body);
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
      r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
      do
      {
        r = http_output_stream_write_string(conn->out, nanohttp_index_html_head);
        if (r != NULL) break;
        r = http_output_stream_write_string(conn->out, "<body>");
        if (r != NULL) break;
        r = http_output_stream_write_string(conn->out, nanohttp_index_html_body);
        if (r != NULL) break;
        r = http_output_stream_write_string(conn->out, "</body></html>");
      } while(0);
    }
    else if (strcmp("data/setmib.json", path))
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
static void
data_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

  if (strcmp("/data/setmib.json", req->path))
  {
    return root_service(conn, req);
  }

  if (req->query->key != NULL)
  {
    char buf[128];
    int n;
    unsigned char *query = NULL;
    JSONPair_t *pair,*p;

    query = decode_url((const uint8_t*)req->query->key, 0);
    if (query == NULL)
    {
      log_error("Failed to parse query");
      r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
      goto finished;
    }

    log_debug("decoded query is : %s", query);

    pair = json_parse((const char *)query, strlen((const char *)query));
    if (pair == NULL)
    {
      free(query);
      log_error("Failed to parse json");
      r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
      goto finished;
    }

    p = json_find_bykey(pair, "id", 2);
    if (p == NULL || p->jsonType != JSONNumber)
    {
      free(query);
      json_pairs_free(pair);
      log_error("Bad id in json");
      r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
      goto finished;
    }

    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    herror_release(r);

    n = snprintf(buf, sizeof buf, "{\"id\":%d,\"err\":[{\"id\":\"0.0\",\"reason\":"
      "\"For testing received error message from our product.\"}]}", (int)p->vint);
    r = http_output_stream_write(conn->out, (unsigned char *)buf, n);

    free(query);
    json_pairs_free(pair);
  }
  else
  {
    r = httpd_send_header(conn, 204, HTTP_STATUS_204_REASON_PHRASE);
  }

finished:  
  herror_release(r);
  return;
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
  JSONPair_t *p = json_parse(buffer,bufferLength);

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

  json_test();
  
  nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);

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

  if ((status = httpd_run()) != H_OK)
  {
    fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
    herror_release(status);
    goto error1;
  }

  httpd_destroy();
  return 0;
    
error0:
  httpd_destroy();
error1:
  return 1;
}
