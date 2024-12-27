objects = \
	nanohttp-main.o \
	nanohttp-admin.o \
	nanohttp-base64.o \
	nanohttp-common.o \
	nanohttp-error.o \
	nanohttp-logging.o \
	nanohttp-client.o \
	nanohttp-mime.o \
	nanohttp-request.o \
	nanohttp-response.o \
	nanohttp-server.o \
	nanohttp-socket.o \
	nanohttp-stream.o \
	nanohttp-json.o \
	nanohttp-ssl.o \
	nanohttp-file.o \
	nanohttp-form.o \
	nanohttp-url.o \
	nanohttp-user.o \
	nanohttp-urlencode.o

all: $(objects)
	$(CC) $^ -o httpd -lpthread -lc

# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem
$(objects): %.o: %.c
	$(CC) -Wall -O0 -g3 -c $^ -o $@

clean: 
	rm -f $(objects) httpd


