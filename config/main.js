function page_common(name, type, display, alwaysActive, action)
{
  this.name    = name;
  this.type    = type;
  this.page_wrapper = null;
  this.container    = null;
  this.intervalID   = -1;
  this.is_phone    = false;
  this.is_vertical = false;
  this.load    = function(data){
              this.data = data;
              this.loader = new cfgOperation(this, alwaysActive, action);
              this.display(this);
              this.inited = true;
              this.isVisible = true;
            };
  this.display = display;
  this.header  = null;
  this.footer  = null;
  this.bgcolor = null;
  this.txtcolor= null;
  this.inited  = false;
  this.content = null;
  this.page    = null;
  this.ajax = null;
  this.ajaxRefresh = null;
  this.resize  = function(){
    if (!this.inited || !this.isVisible)
        return;
  };
  this.toggle = function(){
      if (!this.inited)
        return;
      this.page_wrapper.toggle();
    };

  this.show = function(){this.isVisible = true};
  this.hide = function(){this.isVisible = false};

  this.close = function(){
      if (this.intervalID != -1)
      {
        clearInterval(this.intervalID);
        this.intervalID = -1;
      }
      if (this.ajax) {
        this.ajax.abort();
        this.ajax = null;
      }
      if (this.ajaxRefresh) {
        this.ajaxRefresh.abort();
        this.ajaxRefresh = null;
      }
      if (this.page_wrapper)
      {
        this.page_wrapper.empty();
      }
    };
  this.initVisible = function(b){
      this.isVisible = b;
  };

  this.lang= function(){};
  this.getLang = function(){return this.lang;};
  this.changeLang= function(lang){this.lang = lang;};
  this.theme= function(){};
  this.changeTheme = function(p,t){};

  this.getTheme = function(){return this.theme;};
  this.priv       = null;
  this.click      = null;
  this.dblclick   = null;
  this.progress   = null;
  this.parent     = null;
  this.children   = [];

  this.finger=[];
  this.productFilter=null;
  this.isVisible= false;

  this.css= "";
  this.js=  "";
};

var page_SYSTEM  = new page_common("gmt_SYSTEM","normal",MAIN_SYSTEM_display,false,"data/setmib.json");
var page_DEVICE  = new page_common("gmt_DEVICE","normal",MAIN_DEVICE_display,false,"data/setmib.json");
var page_UPGRADE = new page_common("gmt_UPGRADE","normal",MAIN_UPGRADE_display,false);
var page_USERS = new page_common("gmt_USERS","normal",MAIN_USERS_display,false,"data/setmib.json");

function pagesClose()
{
  __p = null;
  page_SYSTEM.hide();
  page_DEVICE.hide();
  page_UPGRADE.hide();
  page_USERS.hide();
  page_SYSTEM.close();
  page_DEVICE.close();
  page_UPGRADE.close();
  page_USERS.close();
}

var loginWindow = 
    '<div id="id01" class="__login modal">'
      +'<div class="modal-content animate" action="config/secure" method="get">'
        +'<div class="container">'
          +'<label for="uname"><b>Username</b></label>'
          +'<input class="name" type="text" value="" placeholder="Enter Username" name="uname" required>'

          +'<label for="psw"><b>Password</b></label>'
          +'<input class="password" type="password" value="" placeholder="Enter Password" name="psw" required>'
            
          +'<button type="submit">Login</button>'
          +'<label>'
            +'<input type="checkbox" checked="checked" name="remember"> Remember me'
          +'</label>'
        +'</div>'

        +'<div class="container" style="background-color:#f1f1f1">'
          +'<button type="button" onclick="$(\'.modal\').hide()" class="cancelbtn">Cancel</button>'
        +'</div>'
      +'</div>'
    +'</div>';

var panel_CONTAINER = null;

var ____wrapper;
var ____container;
var ____workingBusy;
var ____login;
var ____auth=null;
var __p = null;

function nanoAjaxGet(context,file,method,data,cb,headers)
{
  return $.ajax({
      type: method,
      url: file,
      data: data,
      context: context,
      async: true,
      success: function(data) {
        if ((/\.json$/).test(file))      
        {
          try {
            if (data)
              data = JSON.parse(data);
            else
              data = {};
          }
          catch(ex)
          {
            console.log(ex.message);
            cb.call(this, null, 1);
            return;
          }
        }
        cb.call(this, data, 0);
      },

      error: function(data) {
        cb.call(this, null, 1);
      },

      beforeSend: function(xhr) {
        var hdr = headers || (____auth ? {Authorization:____auth} :{});
        if (hdr)
        {
          for (p in hdr)
            if (hdr.hasOwnProperty(p))
              xhr.setRequestHeader(p, hdr[p]);
        }
      }
    });
}

function __load_page(p,data,forced){
  if (__p)
  {
    __p.hide();
    __p.close();
    __p.page_wrapper.remove();
  }

  if (!forced && p.isVisible)
  {
    __p = p;
    return;
  }

  p.page_wrapper = $("<div class='panel_page' id='"+p.name+"'/>").appendTo(____wrapper);
  p.page_wrapper.html((p.css||"")+(p.js||""));
  p.container = $("<div class='cfg_container'>").appendTo(p.page_wrapper);

  p.load(data);
  p.show();
  __p = p;
}

function __load_sub_page(name,type,display,data,alwaysActive,action){
  var p = new page_common(name,type,display,alwaysActive,action);
  var parent = __p;
  if (parent)
  {
    parent.hide();
    parent.toggle();
  }
  p.page_wrapper = ____wrapper;
  p.container = $("<div class='cfg_container'>").appendTo(p.page_wrapper);
  p.load(data);
  p.show();
  p.parent = parent;
  __p = p;
  return p;
}
function __close_sub_page(p){
  if (p.parent)
  {
    p.parent.show();
    p.parent.toggle();
    __p = p.parent;
  }
  p.hide();
  p.page_wrapper = null;
  p.close();
  p.container.remove();
  delete p;
}
function getIconSvgWithColor(n, color)
{
  return __icons[n] ? getSvgDataURI(__icons[n].replace(/fill="[^\"]+"/, 'fill="#'+color+'"')) : n;
}

function __start_icon(____toolbar){
  var scroll = $('<div class="left-icon-scroll"></div>').appendTo(____toolbar);
  var wrap = $('<div class="left-icon-wrap"></div>').appendTo(scroll);
  var container = $('<div class="left-icon-container"></div>').appendTo(wrap);
  $('<div class="table-left-gap"/>').appendTo(container);

  for (var i=0; i<___leftIcon.length;i++)
  {
      $('<div class="table-left-container '+___leftIcon[i].name+'">'
        +'<table class="main-icon"><tr><td class="left"><img class="'+___leftIcon[i].name+'" svg="'+___leftIcon[i].ln+'" src="'
          +getIconSvgWithColor(___leftIcon[i].ln, "#ffffff")+'"></img></td><td class="right"><div class="text">'
          +___leftIcon[i].lang+'</div></td></tr></table>'
        +'</div>').appendTo(container);
  };

  var choosedColor = "0a7ea1";
  function toolbarChangeICON(el){
    var img = el.find("img");
    var svgName = img.attr("svg");
    var choosed = ____toolbar.find(".table-left-container.choosed");
    if (choosed.length)
    {
      var _txt = choosed.find(".text");
      var _img = choosed.find("img");
      var _svgName = _img.attr("svg");
      _img.attr("src", getIconSvgWithColor(_svgName, "000000"));
      _txt.css("color", _txt.attr("oldColor") || "#000000");
      choosed.removeClass("choosed");
    }
    img.attr("src", getIconSvgWithColor(svgName, choosedColor));
    var txt = el.find(".text");
    if (!txt.attr("oldColor"))
      txt.attr("oldColor", txt.css("color"));
    txt.css("color", "#"+choosedColor);
    el.addClass("choosed");
  };
  
  var z = ____toolbar.find(".table-left-container.SystemUpgrade");
  if (z.length)
    z.AlloyFinger({
      "tap":function(e){
        toolbarChangeICON($(this));
        __start_page(page_UPGRADE);
      }
    });

  z = ____toolbar.find(".table-left-container.Config");
  if (z.length)
    z.AlloyFinger({
      "tap":function(e){
        toolbarChangeICON($(this));
        __start_page(page_DEVICE);
      }
    });

  z = ____toolbar.find(".table-left-container.DeviceInfo");
  if (z.length)
    z.AlloyFinger({
      "tap":function(e){
        toolbarChangeICON($(this));
        __start_page(page_SYSTEM);
      }
    });

  toolbarChangeICON(z);

  z = ____toolbar.find(".table-left-container.Users");
  z.AlloyFinger({
    "tap":function(e){
      toolbarChangeICON($(this));
      __start_page(page_USERS);
    }
  });

}

function __start_page(page){
  __load_page(page);
}

function __start_main(){
  ____login = $(loginWindow).appendTo(document.body);
  ____workingBusy = $(document.body).children(".workingBusy");
  ____table = $(document.body).find(".main-layout");
  ____header   = ____table.find(".head");
  ____main = ____table.find(".main");
  ____container = ____table.find(".table-right");
  ____toolbar   = ____table.find(".table-left .table-left-container");
  ____wrapper = ____container.children(".panel_container");
  panel_CONTAINER = ____wrapper;
  window_resize();

  var title = gmtLangBuild(["Title"],0);
  $(document).find("html > head > title").text(title);
  $(document).find("html > head > meta#MetaDescription").attr("content",title);
  $(document).find("html > head > link[rel=icon]").attr("href",__icons["config/favicon.ico"]);
  ____table.find(".table-head.right .title").text(title);

  ____table.find(".table-head.right .login > img").attr("src", getIconSvg("config/login.png"));
  ____table.find(".table-head.left .logo").attr("src", __icons["config/logo.png"]);
  //____header.css("background-image","url("+__icons["bg.jpg"]+")");
  ____header.css("background-image","linear-gradient(90deg, #020024 0%, #090979 35%, #00d4ff 100%)");

  __start_icon(____toolbar);
  __start_page(page_SYSTEM);

  ____header.find(".login > img").AlloyFinger({
    "tap":function(e){
      ____login.show();
    }
  });

  ____login.find("button[type=submit]").AlloyFinger({
    "tap":function(e){
      var auth = 'Basic ' 
            + btoa(unescape(encodeURIComponent(____login.find("input.name").val() + ':' 
            + ____login.find("input.password").val())));
      nanoAjaxGet(____login, "/secure", "GET", null, 
        function(data, err){
          if (!err)
          {
            ____auth = auth;
            ____login.hide();
            pagesClose();
            $(document.body).html(data);
            $(document).ready(function(){
              __start_main();
            });
          }
          else
          {
            gmMessageBox("Confirm", null, gmtLangBuild(["AuthFailed"],1));
          }
        },
        {
          'Authorization' : auth
        });
    }
  });

  
  function window_resize(){
    ____workingBusy.height($(window).height());
    ____workingBusy.width($(window).width());
    ____table.height($(window).height());
    ____toolbar.height($(window).height());
    ____main.height($(window).height());
    ____wrapper.height($(window).height()-____header.outerHeight());
  }
  $(window).resize(function() {
    window_resize();
    if (__p)
      __p.resize(__p);
  });
}

$(document).ready(function(){
  $(document).find("html > head > link[rel=icon]").attr("href",__icons["config/favicon.ico"]);
  var login = $(document.body).children(".__login");
  login.show();
  login.find("button[type=submit]").AlloyFinger({
    "tap":function(e){
      ____auth = 'Basic ' 
            + btoa(unescape(encodeURIComponent(login.find("input.name").val() + ':' 
            + login.find("input.password").val())));
      nanoAjaxGet(login, "/secure", "GET", null, 
        function(data, err){
          if (!err)
          {
            pagesClose();
            $(document.body).html(data);
            $(document).ready(function(){
              __start_main();
            });
          }
          else
          {
            gmMessageBox("Confirm", null, gmtLangBuild(["AuthFailed"],1));
            ____auth = null;
          }
        },
        {
          'Authorization' : ____auth
        });
      }
  })
});

