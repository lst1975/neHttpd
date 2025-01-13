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
function MAIN_UPGRADE_display(p)
{
    p.container.remove();
    p.container = null;
    
    // https://developer.mozilla.org/zh-CN/docs/Web/Media/Formats/Image_types
    const fileTypes = [
      "image/apng",
      "image/bmp",
      "image/gif",
      "image/jpeg",
      "image/pjpeg",
      "image/png",
      "image/svg+xml",
      "image/tiff",
      "image/webp",
      "image/vnd.microsoft.icon",
      "image/x-icon",
      "text/plain",
      "text/html",
      "text/xml",
      "text/javascript",
      "text/css",
      "font/woff",
      "font/woff2",
      "application/atom+xml",
      "application/x-sh",
      "application/xml",
      "application/atom+xml",
      "application/json",
      "application/vnd.rar",
      "application/x-tar",
      "application/x-bzip2",
      "application/x-bzip",
      "application/x-gzip",
      "application/gzip",
      "application/x-7z-compressed",
      "application/zip",
      "application/x-zip-compressed",
      "application/octet-stream",//.bin application/octet-stream is the default value for all other cases. An unknown file type should use this type. Browsers are particularly careful when manipulating these files to protect users from software vulnerabilities and possible dangerous behavior.
    ];

    function validFileType(file) {
      //return fileTypes.includes(file.type);
      return 1;
    }
    var form = $('<form action="/post" method="post" enctype="multipart/form-data"></form>').appendTo(p.page_wrapper);
    var __input = "<fieldset>"
                    +"<legend>"+gmtLangBuild(["UploadFile"],0)+"</legend>"
                    +"<div class='txtBorder'><div class='progress'>0%</div></div>"
                    +"<div class='input'>"
                      +"<label class='button label'>"+gmtLangBuild(["ChooseFile"],0)+"</label>"
                      +"<input class='file' name='File' type='file' accept='*' multiple>"
                      +"<span class='name'></span>"
                      +"<input class='hidden_submit' value='Submit' type='submit'>"
                      +"<label class='button submit'>"+gmtLangBuild(["StartUpload"],0)+"</label>"
                   +"</div>"
                  +"</fieldset>";
    var input = $(__input).appendTo(form);
    var txtBorder = input.find('.txtBorder');
    var txt = txtBorder.find('.progress');
    var submit= input.find(".submit");
    var div= input.find(".input");
    var file= input.find(".file");
    var label= input.find(".label");
    var name= input.find(".name");
    
    name.css({
      "margin-left": "40px",
      "font-style":"italic"
    });
    input.find(".button").css({
      border:"1px solid gray",
      "border-radius":"5px",
      cursor:"pointer",
      padding:"3px",
      "font-size":"80%",
      "opacity":".8",
    }).hover(function(){
      $(this).css({"opacity":"1","color":"white","background-color":"#2eb9e3"});
    },function(){
      $(this).css({"opacity":".8","color":"black","background-color":"transparent"});
    });
    file.hide();
    input.find(".hidden_submit").hide();
    
    label.AlloyFinger({
        "tap":function(e){
            file.trigger("click");
        }
    });
    submit.css({
        "color":"gray",
        "margin-left": "auto",
        "pointer-events":"none"
    });
    div.css({
        "width":"100%",
        "display":"flex",
    });
    form.css({
        "width":"80%",
        "max-width":"600px",
        "margin": "100px auto 0 auto"
    });
    input.css({
        "height":"100%",
        "width":"100%",
    });
    txtBorder.css({
        "height":"30px",
        "line-height": "30px",
        "width":"100%",
        "text-align":"center",
        "margin":"10px 0",
        "border": "dashed 1px lightgray"
    });
    txt.css({
        "height":"100%",
        "width":"100%",
    });

    submit.AlloyFinger({
        "tap":function(e){
            uploadFile(submit.attr("filename")) //calling uploadFile with passing file name as an argument
        }
    });
    submit.attr("disabled", true);
    input.on("change", function(e){
        if (!e.target.files.length)
          return;
        var file = e.target.files[0] //getting file and [0] this means if user has selected multiples files then get first one only
        if (!validFileType(file))
        {
          name.text(gmtLangBuild(["WrongFileType"],0)+file.type);
          return;
        }
        //if file is selected
        var fileName = file.name //getting selected file name
        if (fileName.length >= 12) {
            //if filename length is greater or equal to 12 the split the name and add...
            let splitName = fileName.split('.')
            fileName = splitName[0].substring(0, 12) + '... .' + splitName[1]
        }
        name.text(fileName);
        submit.css({
            "color":"black",
            "pointer-events":"all"
        });
        submit.attr("disabled", false);
        submit.attr("filename", fileName);
        txt.css({
          "background-color":"#2eb9e3",
          "background":"linear-gradient(90deg, #0ff, #2eb9e3 0%, transparent 0)",
        });
        txt.text("0%");
    });

    function uploadFile(fileName) {
      var xhr = new XMLHttpRequest() //creating a new xml obj (AJAX)
      xhr.open('POST', 'post/wia.bin') //sending post request to the specified URL/File
      xhr.upload.addEventListener('progress', ({ loaded, total }) => {
        var fileLoaded = Math.floor((loaded / total) * 100) //getting percentage of loaded file size
        var fileSize;
        
        //if file size is less than 1024 then add only KB else convert size into KB to MB
        if (loaded < 1024)
          fileSize = loaded + 'B';
        else if (loaded < 1024 * 1024)
          fileSize = (loaded / (1024)).toFixed(2) + 'KB';
        else if (loaded < 1024 * 1024 * 1024)
          fileSize = (loaded / (1024 * 1024)).toFixed(2) + 'MB';
        else 
          fileSize = (loaded / (1024 * 1024 * 1024)).toFixed(2) + 'GB';
          
        txt.css({
          "background-color":"#2eb9e3",
          "background":"linear-gradient(90deg, #0ff, #2eb9e3 "+fileLoaded+"%, transparent 0)",
        });
        txt.text(fileLoaded+"%");
        name.html(fileName+"&nbsp;&#8226;&nbsp;"+gmtLangBuild([loaded === total ? 
          "Uploaded" : "Uploading"],0)+"&nbsp;&#8226;&nbsp;" +fileSize);
      })
      // Set up the callback function to handle the response
      xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
          // When the request is complete
          if (xhr.status === 200) {
            // Check if the status is 200 OK
            console.log('Request succeeded:', xhr.responseText);
          } 
          else {
            console.log('Request failed with status:', xhr.status);
          }
          gmMessageBox("Confirm", null, 
            gmtLangBuild(["SystemUpgrade", xhr.status === 200 ? "Ok" : "Fail", "FullStop"],1));
        }
      };
      if (____auth)
        xhr.setRequestHeader("Authorization", ____auth);
      var formData = new FormData(form[0]); //formData is an object to easily send form data
      xhr.send(formData); //sending form data to php
    }
}
