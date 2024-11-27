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
        p.loader.load_config(p,p.container,data);
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
      if (!err)
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
    });
}

