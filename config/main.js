function nanoAjaxGet(file,type,data,cb)
{
  return $.ajax({
			type: type,
			url: file,
			data: data,
			context: null,
			async: true,
			success: function(data) {
                if ((/\.json$/).test(file) && gmtIsString(data))			
				{
				    try{
					  data = JSON.parse(data);
					}
					catch(ex)
					{
					  console.log(ex.message);
				      cb(null, 1);
				      return;
					}
				}
				cb(data, 0);
			},

			error: function(data) {
				cb(null, 1);
			}
		});
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

function gmtToolbarScroll(scroll, left)
{
    return scroll.niceScroll({
      cursorcolor:"rgba(128,128,128,0.8)",
      cursorborder:"1px solid rgba(128,128,128,0.5)",
      railhoffset:{left:left,top:0},
      enableobserver:true,

      horizrailenabled:false,
      disablemutationobserver:false});
}

var panel_CONTAINER = null;

var ____wrapper;
var ____container;
var ____niceScroll;
var __p = null;
function __load_page(p,data){
	if (p.isVisible)
	{
		return;
	}
	if (__p)
	{
        __p.hide(__p);
		__p.close(__p);
		__p.page_wrapper.remove();
	}

	p.page_wrapper = $("<div id='"+p.name+"'/>").appendTo(____wrapper);
	p.page_wrapper.html((p.css||"")+(p.js||""));
	p.load(p,data);

	p.show(p);
	__p = p;
}

function __start_icon(____toolbar){
    var scroll = $('<div class="left-icon-scroll"></div>').appendTo(____toolbar);
    var wrap = $('<div class="left-icon-wrap"></div>').appendTo(scroll);
    var container = $('<div class="left-icon-container"></div>').appendTo(wrap);
    $('<div class="table-left-gap"/>').appendTo(container);

    var leftIcon=[
      {name: "DeviceInfo", lang:gmtLangBuild(["DeviceInfo"],1), ln:"config/system.png", isSys: 0},
      {name: "Config", lang:gmtLangBuild(["Config"],1), ln:"config/config.png", isSys: 0},
      {name: "SystemUpgrade", lang:gmtLangBuild(["SystemUpgrade"],1), ln:"config/upgrade.png", isSys: 0},
    ];
    for (var i=0; i<leftIcon.length;i++)
    {
        $('<div class="table-left-container '+leftIcon[i].name+'">'
          +'<table class="main-icon"><tr><td class="left"><img class="'+leftIcon[i].name+'" src="'+leftIcon[i].ln+'"></img></td><td class="right"><div class="text">'+leftIcon[i].lang+'</div></td></tr></table>'
          +'</div>').appendTo(container);
    };

	var z = ____toolbar.find(".table-left-container.SystemUpgrade");
	z.AlloyFinger({
		"tap":function(e){
			__start_page(page_UPGRADE);
		}
	});

	z = ____toolbar.find(".table-left-container.Config");
	z.AlloyFinger({
		"tap":function(e){
			__start_page(page_DEVICE);
		}
	});

	z = ____toolbar.find(".table-left-container.DeviceInfo");
	z.AlloyFinger({
		"tap":function(e){
			__start_page(page_SYSTEM);
		}
	});
}

function __start_page(page){
	__load_page(page);
	____niceScroll = panel_CONTAINER.niceScroll({
	  cursorcolor:"rgba(128,128,128,0.8)",
	  cursorborder:"1px solid rgba(128,128,128,0.5)",
	  railhoffset:{left:0},
	  enableobserver:true,
	  disablemutationobserver:false});
}

function window_resize(){
}
$(document).ready(function(){
    ____table = $(document.body).find(".main-layout");
    ____container = ____table.find(".table-right");
    ____toolbar   = ____table.find(".table-left .table-left-container");
    ____wrapper = ____container.children(".panel_container");
    ____wrapper.css({"overflow-x": "hidden"});
    panel_CONTAINER = ____wrapper;
    
    var title = gmtLangBuild(["Title"],0);
    $(document).find("html > head > title").text(title);
    $(document).find("html > head > meta#MetaDescription").attr("content",title);
    ____table.find(".table-head.right .title").text(title);
    __start_icon(____toolbar);
})
