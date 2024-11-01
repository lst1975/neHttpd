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
   3. please refer to /config/system.json
   4. automatically validate input value
   5. automatically generate submitted JSON, id is a.x.b.z, like SNMP mib.
   6. if some configuration items are wrong, those relative input items will be marked by red border.
      
   ![overview](https://github.com/user-attachments/assets/a748ba6b-4085-4be5-bcb7-81caa9be568b)

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
