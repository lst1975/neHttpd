/*** 
 * get browser type and browser version and language 
 * @param write22 
 * @returns {{}} 
 */  
function MAIN_SYSTEM_display(p)
{
  if (p.ajax) p.ajax.abort();
  p.ajax = nanoAjaxGet("data/system.json", "GET", null, 
    function(data,err){
      if (!err)
        load_config(p.container,data);
    });
}

var page_SYSTEM = {
  name    : "gmt_SYSTEM",
  page_wrapper: null,
  container: null,
  
  is_phone : false,
  is_vertical : false,
  load    : function(p){
              p.display(p);
              p.inited = true;
              p.isVisible = true;
            },
  display : MAIN_SYSTEM_display,
  header  : null,
  footer  : null,
  bgcolor : null,
  txtcolor: null,
  inited  : false,
  content : null,
  page    : null,
  wrnode  : null,
  resize  : function(p){
    if (!p.inited || !p.isVisible)
        return;
  },
  thumnail_seq  : 0,
  thumnail_node : function(p){},
  toggle : function(p){},

  show : function(p){p.isVisible = true},
  hide : function(p){p.isVisible = false},

  close : function(p){
      if (p.ajax) {
        p.ajax.abort();
        p.ajax = null;
      }
      if (p.page_wrapper)
      {
        p.page_wrapper.empty();
      }
    },
  initVisible : function(p,b){
      p.isVisible = b;
  },

  lang: function(p){},
  getLang : function(p){return p.lang;},
  changeLang: function(p, lang){p.lang = lang;},
  theme: function(p){},
  changeTheme : function(p,t){},

  getTheme : function(p){return p.theme;},
  priv: null,
  click      : null,
  dblclick   : null,
  progress:null,
  parent   : null,
  children:[],

  finger:[],
  productFilter:null,
  isVisible: false,

  css: "",
  js:  "",
};
