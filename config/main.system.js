/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
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
 **************************************************************************************
 *                              
 *                    https://github.com/lst1975/TDMA-ng-Wifi
 *                              
 **************************************************************************************
 */
/*************************************************************************************
 *                               ngRTOS Kernel V2.0.1
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
 *                              https://github.com/lst1975/ngRTOS
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
 */
/*** 
 * get browser type and browser version and language 
 * @param write22 
 * @returns {{}} 
 */  
function MAIN_SYSTEM_display(p)
{
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet(p, "data/system.json", "GET", null, 
    function(data,err){
      if (!err)
      {
        p.loader.load_config(p,p.container,data);
        if (p.intervalID != -1)
        {
          clearInterval(p.intervalID);
          p.intervalID = -1;
        }
        p.intervalID = setInterval(function(){
          //__load_page(page_SYSTEM, null, true)
          p.ajaxRefresh = nanoAjaxGet(p, "data/system.json", "GET", null, 
            function(data,err){
              if (!err)
              {
                p.loader.eachIndex(data, function(idx, val){
                    if (val != undefined)
                    {
                      var el = p.container.find(idx);
                      if (el.length)
                      {
                        el.data("setVal")(el, val);
                      }
                      else
                      {
                        return;
                      }
                    }
                  });
              }
            })
        }, 10000)
      }
    });
}

function MAIN_TEMPLATE_display(p)
{
  var addListItem = p.data;
  var loader = addListItem.loader;
  var ___id = loader.___mibid.id++;
  var kd = {
      id:___id,
      value : addListItem.index
    };
  var y = JSON.stringify(kd);
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet(p, "data/template.json", "GET", "data="+Base64.encode(y), 
    function(data,err){
      if (!err && !data.err)
      {
        for (var a in data)
        {
          if (!data.hasOwnProperty(a))
            continue;
          var o = data[a];
          if (gmtIsObject(o))
          {
            var n = p.data.n;
            o.id = p.data.index+'.'+n.value.length+(n.index ? ":"+o[n.index].value:"");
          }
        }
        p.loader.load_config(p,p.container,data);
      }
      else
        gmMessageBox("Confirm", null, gmtLangBuild(["TemplateFailed"],1));
    });
}

function MAIN_USERS_display(p)
{
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet(p, "data/users.json", "GET", null, 
    function(data,err){
      if (!err && !data.err)
        p.loader.load_config(p,p.container,data);
      else
        gmMessageBox("Confirm", null, gmtLangBuild(["TemplateFailed"],1));
    });
}

