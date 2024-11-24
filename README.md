# neHttpd

Embedded HTTP Server for Web Configuraion Framwork

# Compile
   make

# Run
   ```
   ./httpd
   http://localhost:8080
   ```
   ![index](https://github.com/user-attachments/assets/f0f44f4e-dd66-4afe-befa-99ee6f6a71e1)

# Upload
   The uploaded file will be stored in the fixed positon of "/uploads"
# Get and Set
  1. Get configuration from server data structure, please refer to /data/system.json
   ```
   {
     "device": {
       "name": {
         "type":"string",
         "range":[8,128],
         "value":"aaa-To-Be-Filled-By-O-E-M",
         "writable":true,
         "label": "Host Name",
         "id": 0
       },
       "arch": {
         "type":"string",
         "range":[3,128],
         "value":"X86_64",
         "writable":false,
         "label": "Architecture",
         "id": 1
       },
       .
       .
       .
   }
   ```
  2. Set configuration to server data structure
   ```
  {
     "id": 1,
     "values": {
       "0.7.1:wifi0.1": "1.1.1.2",
       "0.3": "Dark"
     }
  }
   ```
 3. Returned data structure for setting configuration to server data structure
   ```
    {
       "id":1,
       "err":[
          {
            "id":"0.7.1:wifi0.1",
            "reason":"For testing received error message from our product."
          }
       ]
    }
   ```
# Features
   Only JSON file for configuraion is transmitted between Browser and Backend neHttpd.
   Nearly all work are create JSON data, neHttpd framework will finished remained job
   automatically.

   1. automatically convert between JSON and C structure
   2. automatically web page configuration elements, use following JSON data:
   ```
   {
     "device": {
       "name": {
         "type":"string",
         "range":[8,128],
         "value":"aaa-To-Be-Filled-By-O-E-M",
         "writable":true,
         "label": "Host Name",
         "id": 0
       },
       "arch": {
         "type":"string",
         "range":[3,128],
         "value":"X86_64",
         "writable":false,
         "label": "Architecture",
         "id": 1
       },
       .
       .
       .
   }
   ```
   3. please refer to /data/system.json.
   4. automatically validate input value
   5. automatically generate submitted JSON, id is a.x.b.z, like SNMP mib.
   6. if some configuration items are wrong, those relative input items will be marked by red border.
   7. strict security policy.
   8. fast and small, meet most ebedded environment.
   9. simple and easy to use.
      
   ![cfg1](https://github.com/user-attachments/assets/7cf45960-d231-409a-9a7d-db38280a1f4c)

   add or delete item for list

   ![cfg](https://github.com/user-attachments/assets/87bb6ed9-9e7a-41f6-9ef1-7645cf88e341)

# Data Type
          "array"  : options: [ *, *, ...]
          "url"    : [http[s]://][<user>:<password>@]<location>[:<port>]  
          "mac"    : 01-23-45-67-89-AB or 0123456789AB or 01:23:45:67:89:AB  
          "ipv4"   : 192.168.1.1
          "ipv6"   : 2001:db8:0:0:0:0:2:1|::1|::ffff:192.0.2.128|2001:db8::567:1  
          "number" : [-+]?[0-9]*|[-+]?0(x|X)[0-9A-F]+
          "string" : [\x20-\x7E]
          "float"  : [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
          "bool"   : true or false
          "group"  : V (expand)
          "group+" : > (expand)
          "list"   : ... (expand)
          "list+"  : + - ... (+:add, -:delete, ...:expand) 

# Copying
   ```
/*************************************************************************************
 *                       neHttpd For Embedded Product
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 *************************************************************************************
 *                              
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
   ```
