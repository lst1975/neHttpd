 #*************************************************************************************
 #          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 #               TDMA Time-Sensitive-Network Wifi V1.0.1
 # Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 #*************************************************************************************
 #
 # Permission is hereby granted, http_free of charge, to any person obtaining a copy of
 # this software and associated documentation files (the "Software"), to deal in
 # the Software without restriction, including without limitation the rights to
 # use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 # the Software, and to permit persons to whom the Software is furnished to do so,
 # subject to the following conditions:
 #
 # THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 # COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 # NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 # NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 # DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 # UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 # ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 # MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 # EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 # OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 # FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 # IN THE SOFTWARE.
 #
 #*************************************************************************************
 #                              
 #                    https://github.com/lst1975/TDMA-ng-Wifi
 #                              
 #*************************************************************************************
 #
 #************************************************************************************
 #                               ngRTOS Kernel V2.0.1
 # Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 #*************************************************************************************
 #
 # Permission is hereby granted, http_free of charge, to any person obtaining a copy of
 # this software and associated documentation files (the "Software"), to deal in
 # the Software without restriction, including without limitation the rights to
 # use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 # the Software, and to permit persons to whom the Software is furnished to do so,
 # subject to the following conditions:
 #
 # THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 # COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 # NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 # NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 # DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 # UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 # ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 # MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 # EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 # OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 # FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 # IN THE SOFTWARE.
 #
 #************************************************************************************
 #                              https://github.com/lst1975/ngRTOS
 #                              https://github.com/lst1975/neHttpd
 #*************************************************************************************
 #

STRIP ?= strip
 
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
	nanohttp-ring.o \
	nanohttp-mem.o \
	nanohttp-system.o \
	nanohttp-time.o \
	nanohttp-ctype.o \
	nanohttp-atoi.o \
	nanohttp-itoa.o \
	nanohttp-buffer.o \
	nanohttp-signal.o \
	nanohttp-utils.o \
	nanohttp-inet.o \
	nanohttp-urlencode.o

depends:= $(objects:.o=.d)
sources:= $(objects:.o=.c)

.PHONY: all clean

all: httpd

-include $(depends)

CFLAGS = -Wall -O3 -g3 -I. -Wno-attributes
LDFLAGS = 

# Check if __CYGWIN__ is defined and set a Makefile variable
CYGWIN_CHECK := $(shell echo | gcc -dM -E - | grep -q "__CYGWIN__" && echo yes || echo no)
MINGW64_CHECK := $(shell echo | gcc -dM -E - | grep -q "__MINGW64__" && echo yes || echo no)
MINGW32_CHECK := $(shell echo | gcc -dM -E - | grep -q "__MINGW32__" && echo yes || echo no)

ifeq ($(CYGWIN_CHECK),yes)
LDLIB = -lws2_32 -lc
else
ifeq ($(MINGW64_CHECK),yes)
LDLIB = -lws2_32 -lc
else
ifeq ($(MINGW32_CHECK),yes)
LDLIB = -lws2_32 -lc
else
LDLIB = -lpthread -lc
endif
endif
endif

release:  $(objects)
	$(CC) $^ -o httpd $(LDLIB)
	$(STRIP) httpd

httpd: $(objects)
	$(CC) $(LDFLAGS) $^ -o $@  $(LDLIB)

# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $*.d -MP

clean: 
	rm -f $(objects) *.o.d *.d httpd httpd.exe
