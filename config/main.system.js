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
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet(p, "data/template.json", "GET", null, 
    function(data,err){
      if (!err)
        p.loader.load_config(p,p.container,data);
    });
}

