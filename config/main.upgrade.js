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
                      +"<input class='file' name='File' type='file' accept='*'>"
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
        var file = e.target.files[0] //getting file and [0] this means if user has selected multiples files then get first one only
        if (file) {
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
        }
    });

    function uploadFile(fileName) {
      var xhr = new XMLHttpRequest() //creating a new xml obj (AJAX)
      xhr.open('POST', 'post/wia.bin') //sending post request to the specified URL/File
      xhr.upload.addEventListener('progress', ({ loaded, total }) => {
        var fileLoaded = Math.floor((loaded / total) * 100) //getting percentage of loaded file size
        var fileTotal = Math.floor(total / 1000) // getting file size in KB from bytes
        var fileSize;
        
        //if file size is less than 1024 then add only KB else convert size into KB to MB
        if (!fileTotal)
          fileSize = total + 'B';
        else 
        {
          fileTotal < 1024
            ? (fileSize = fileTotal + 'KB')
            : (fileSize = (loaded / (1024 * 1024)).toFixed(2) + 'MB');
        }
        txt.css({
          "background-color":"#2eb9e3",
          "background":"linear-gradient(90deg, #0ff, #2eb9e3 "+fileLoaded+"%, transparent 0)",
        });
        txt.text(fileLoaded+"%");
        name.html(fileName+"&nbsp;&#8226;&nbsp;"+gmtLangBuild([loaded === total ? "Uploaded" : "Uploading"],0)+"&nbsp;&#8226;&nbsp;" +fileSize);
      })
      if (____auth)
        xhr.setRequestHeader("Authorization", ____auth);
      var formData = new FormData(form[0]); //formData is an object to easily send form data
      xhr.send(formData); //sending form data to php
    }
}
