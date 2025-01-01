/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
function __gmMessageBox(order, done, fail, msg, cb, arg)
{
  $.MessageBox({
    buttonDone  : gmtLangBuild([done],1),
    buttonFail  : fail ? gmtLangBuild([fail],1) : null,
    buttonsOrder: order || null,  // String
    message     : msg,
  }).done(function(){
    if (cb) cb(0, arg);
  }).fail(function(){
    if (cb) cb(1, arg);
  });              
}
function gmMessageBox(done, fail, msg, cb, arg)
{
  __gmMessageBox(null, done, fail, msg, cb, arg);
}

function gmtIsArray(obj)
{
  if (Array.isArray)
    return Array.isArray(obj);
  else
    return typeof(obj) === "array";
}
function gmtIsString(obj)
{
  return typeof(obj) === "string";
}
function gmtIsUndefined(obj)
{
  return typeof(obj) === "undefined";

}
function gmtIsFunction(obj)
{
  return typeof(obj) === "function";

}
function gmtIsObject(obj)
{
  return typeof(obj) === "object";

}
function gmtHasClass(ele,c)
{
  for (var i=0;i<c.length;i++)

  {
    if (!ele.hasClass(c[i]))
      return false;

  }
  return true;
}
function gmtHasClassAny(ele,c)
{
  for (var i=0;i<c.length;i++)
  {
    if (ele.hasClass(c[i]))
      return true;
  }
  return false;
}
function gmtSetClass(eles,c)
{
  for (var i=0;i<eles.length;i++)
  {
    if (eles[i])
      eles[i].css(c);
  }
}
function gmtTime()
{
  var d = new Date();
  return d.getTime();
}
function gmtSetIcon(img,icon,color)
{
  img.attr("saveColor", color);
  img.attr("src", getIconInline(icon+(img.hasClass("fill")?"Fill":""), color));
}
function gmtToggleIcon(img,icon)
{
  img.toggleClass("fill");
  gmtSetIcon(img,icon,img.attr("saveColor"));
}
function gmtEventStop(evt)
{
  evt.preventDefault();
  evt.stopPropagation();

  evt.___finished = true;
}
function gmtHoverOpacity(ele, op1, op2)
{
  ele.hover(
    function(){
      $(this).css("opacity", op1);
    },
    function(){
      $(this).css("opacity", op2);
    });
}

function isTouchDevice() {
  return 'ontouchstart' in document.documentElement;
}

var sysConfig = { isTouchDevice: isTouchDevice()};

function toUnicodeEscapeSequence(str) {
  return Array.from(str)
    .map(char => {
      const codePoint = char.codePointAt(0); // Get Unicode code point
      return `\\u${codePoint.toString(16).padStart(4, '0')}`; // Format as \uXXXX
    })
    .join(''); // Join the array into a single string
}
