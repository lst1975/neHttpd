#include "nanohttp-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nanohttp-logging.h"
#include "nanohttp-server.h"
#include "nanohttp-json.h"
#include "nanohttp-file.h"
#include "nanohttp-form.h"

static int
simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{
  fprintf(stdout, "logging in user=\"%s\" password=\"%s\"\n", user, password);

  if (strcmp(user, "bob")) {

    fprintf(stderr, "user \"%s\" unkown\n", user);
    return 0;
  }

  if (strcmp(password, "builder")) {

    fprintf(stderr, "wrong password\n");
    return 0;
  }

  return 1;
}

static void
secure_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Secure ressource!</title>"
      "</head>"
      "<body>"
        "<h1>Authenticated access!!!</h1>"
      "</body>"
    "</html>");

  return;
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
__multipart_data_cb(multipartparser *p, const char* data, size_t size)
{
  return nanohttp_file_write(p->arg, data, size) != size;
}

static herror_t
post_service(httpd_conn_t *conn, struct hrequest_t *req, 
  const char *file)
{
  herror_t r = NULL;
  if (req->method == HTTP_REQUEST_POST)
  {
    void *arg;
    multipartparser p;
    multipartparser_callbacks settings;
    multipartparser_callbacks_init(&settings);
    settings.on_data = __multipart_data_cb;

    if (!req->content_type || !req->content_type->params
      || strcmp(req->content_type->params->key, "boundary")
      || !req->content_type->params->value)
    {
      r = herror_new("post_service", FILE_ERROR_READ, 
        "Failed to read stream %s", req->path);
      return r;
    }

    arg = nanohttp_file_open(file);
    if (arg == NULL)
    {
      return herror_new("nanohttp_file_write", FILE_ERROR_OPEN, 
        "Not able to open the file %s.", file);
    }
    
    multipartparser_init(&p, arg, req->content_type->params->value);
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

    nanohttp_file_close(arg);
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
post_service_wia(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r = post_service(conn, req, "config/gw.bin");
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

static void
root_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  herror_t r;

  if (strstr(req->path, "..") != NULL)
  {
    return default_service(conn, req);
  }
  else
  {
    r = httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    if (r == NULL)
    {
      const char *path = req->path+1;
      if (!strcmp("/", req->path))
        path = "config/index.html";
      r = nanohttp_file_read(path, __root_service_read, conn);
      if (r != NULL)
      {
        char buf[1024];
        size_t n = snprintf(buf, sizeof buf, "Failed to readfile index.html: %s", herror_message(r));
        log_error("%s", buf);
        herror_release(r);
        r = http_output_stream_write(conn->out, (const unsigned char *)buf, n-1);
        herror_release(r);
      }
    }
    else
    {
      herror_release(r);
      r = http_output_stream_write_string(conn->out,
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
                    "<input name=\"File\" type=\"file\" accept=\"text/*\"> "
                    "<input value=\"Submit\" type=\"submit\">"
                  "</fieldset>"
                "</form>"
                "</li>"
              "<li><a href=\"/not_existent\">The default service</a></li>"
              "<li><a href=\"/nhttp\">Admin page</a> (try -NHTTPDadmin on the command line)</li>"
            "</ul>"
          "</body>"
        "</html>");
      herror_release(r);
    }
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

  if ((status = httpd_register("/", root_service, "ROOT")) != H_OK)
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

  if ((status = httpd_register("/headers", 
    headers_service, "HEAD")) != H_OK)
  {
    fprintf(stderr, "Cannot register headers service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register("/mime", mime_service, "MIME")) != H_OK)
  {
    fprintf(stderr, "Cannot register MIME service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register("/post/wia.bin", post_service_wia, "POST-GW")) != H_OK)
  {
    fprintf(stderr, "Cannot register POST service (%s)\n", 
      herror_message(status));
    herror_release(status);
    goto error1;
  }

  if ((status = httpd_register_default("/error", default_service)) != H_OK)
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
