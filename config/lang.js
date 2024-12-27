/*** 
 * get browser type and browser version and language 
 * @param write22 
 * @returns {{}} 
 */  
function getBrowserVersion(write22){  
  var browser = { name: "unknown"};  
  var userAgent = navigator.userAgent.toLowerCase();  
  var lang22=navigator.language;  
  var s;  
  
  s = userAgent.match(/msie ([\d.]+)/);
  if (s)
    browser.ie = s[1];
  else
  {
    s = userAgent.match(/firefox\/([\d.]+)/);
    if (s)
      browser.firefox = s[1];
    else
    {
      s = userAgent.match(/chrome\/([\d.]+)/);
      if (s)
        browser.chrome = s[1];
      else
      {
        s = userAgent.match(/opera.([\d.]+)/);
        if (s)
          browser.opera = s[1];
        else
        {
          s = userAgent.match(/version\/([\d.]+).*safari/);
          if (s)
            browser.safari = s[1];
        }
      }
    }

  }

  var version = "";  
  if (browser.ie) {  
    version =  browser.ie;  
    browser.name='IE';  
    if(!browser.lang ||browser.lang==undefined){  

      lang22=navigator.browserLanguage;  
      browser.systemLanguage=navigator.systemLanguage ;  
      browser.userLanguage=navigator.userLanguage;  
    }  
  }  
  else  
  if (browser.firefox) {  
    version = browser.firefox;  

    browser.mozilla=browser.firefox;  
    browser.name='firefox';  
  }  
  else  

  if (browser.chrome) {  
    version = browser.chrome;  
    browser.name='chrome';  

  }  
  else  
  if (browser.opera) {  
    version =  browser.opera;  
    browser.name='opera';  
  }  
  else  
  if (browser.safari) {  
    version =  browser.safari;  
    browser.name='Safari';  
  }  
  else {  
    version = 'unknown browser';  
  }  
  browser.ver=version;  
  if(lang22 && lang22!=undefined)  
  {  
    browser.lang=lang22.toLowerCase();  
  }  

  return browser;  
}  

var gmtBrowser = getBrowserVersion(true);

var gmtLangZH = {
  ___name: "ZH",
  Device: "设备",
  Title: "嵌入式HTTP服务器和网页配置平台",
  Information: "信息",
  System: "系统",
  Upgrade: "升级",
  DeviceInfo: "设备信息",
  Config: "设备配置",
  SystemUpgrade: "系统升级",
  UploadFile: "上传升级文件",
  ChooseFile: "选择文件",
  StartUpload: "开始上传",
  Uploading: "正在上传",
  Uploaded: "上传完毕",
  WrongFileType: "错误的文件类型：",
  SubmitConfig: "提交配置",
  SaveConfig:"保存配置",
  Return:"返回",
  Confirm:"确定",
  Cancel:"取消",
  ItemDelMsg:"你是否确定删除这元素？",
  Expand:"展开",
  Collapse:"缩回",
  Yes:"是",
  No:"否",
  Add:"添加",
  Del:"删除",
  Detail:"详细配置",
  CfgFailed:"配置发生错误：",
  AddOk:"添加成功。",
  SaveOk:"保存成功。",
  Ok:"成功",
  Fail:"失败",
  FullStop:"。",
  Users:"用户管理",
};

var gmtLangEN = {
  ___name: "ZHEN",
  Device: "Device",
  Title: "Embedded Http Server for Web Configuraion",
  Information: "Information",
  System: "System",
  Upgrade: "Upgrade",
  DeviceInfo: "Information",
  Config: "Configuration",
  SystemUpgrade: "Upgrade",
  UploadFile: "Upload File",
  ChooseFile: "Choose File",
  StartUpload: "Submit",
  Uploading: "Uploading",
  Uploaded: "Uploaded",
  WrongFileType: "Wrong File Type:",
  SubmitConfig: "Submit",
  SaveConfig:"Save",
  Return:"Return",
  Confirm:"Confirm",
  Cancel:"Cancel",
  ItemDelMsg:"Are you sure to delete this element?",
  Expand:"Expand",
  Collapse:"Collapse",
  Detail:"Detail",
  Yes:"Yes",
  No:"No",
  Add:"Add",
  Del:"Delete",
  CfgFailed:"Configuration Failed:",
  AddOk:"Added Successfully.",
  SaveOk:"Saved Successfully.",
  Ok:"Successfully.",
  Fail:"Failed.",
  FullStop:".",
  Users:"Accounts",
};

var gmtLang = gmtBrowser.lang == "zh-cn" ? gmtLangZH : gmtLangEN;
var langDefault = gmtLang.___name;

var langObjects = {
  "EN": gmtLangEN,
  "ZH": gmtLangZH,
};
function setSystemLanguage(name)
{
  var t = langObjects[name];
  if (t)
  {
    gmtLang = t;

  }
}
function getSystemLanguageByname(name)
{
  return langObjects[name] || gmtLang;
}
function gmtLangBuild(__names, cap, __lang)
{
  var str = "";
  var lang = __lang || gmtLang;
  var names = gmtIsString(__names) ? [__names] : __names;
  for (var i=0;i<names.length;i++)
  {
    if (lang == gmtLangZH)
    	str += lang[names[i]];
    else
    {
    	str += (!i?"":" ") + lang[names[i]];
    }
  }
  if (lang == gmtLangEN && cap)
  {
	  if (cap == 1)
	    return str.slice(0,1).toUpperCase() + str.slice(1);
	  else
	  {

  		var g = str.split(" "),s="";
  		for (var i=0;i<g.length;i++)
  		{
  			s += (!i?"":" ") + g[i].slice(0,1).toUpperCase() + g[i].slice(1);
  		}
  		return s;
	  }
  }

  return str;
}


