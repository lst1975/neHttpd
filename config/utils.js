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


