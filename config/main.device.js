/*** 
 * get browser type and browser version and language 
 * @param write22 
 * @returns {{}} 
 */  
function MAIN_DEVICE_display(p)
{
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet(p, "data/device.json", "GET", null, 
    function(data,err){
      if (!err)
        p.loader.load_config(p,p.container,data);
    });
}
