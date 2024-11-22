var labelConv = {
  "Memory":"Memory Profile",
  "Host Name":"Host Name",
};

function toggleArrow(el, up, down)
{
  if (el.hasClass("down"))
  {
    el.removeClass("down").addClass("up");
    //img.attr("src", getIconSvg("config/arrow-up.png"));
    if (up) el.html(up);
    el.data("subSection").show();
    el.parent().addClass("active");
  }
  else
  {
    el.removeClass("up").addClass("down");
    //img.attr("src", getIconSvg("config/arrow-down.png"));
    if (down) el.html(down);
    el.data("subSection").hide();
    el.parent().removeClass("active");
  }
}
function changeSubmitButtonState(button, n)
{
  n ? button.addClass("active") : button.removeClass("active");
}

function cfgOperation()
{
  this.___mibid={
    id:0,
    e:{}
  };

  this.changeConfigItem = function(__ev, __getVal, __setErr)
  {
    var ov = __ev.data("osrc");
    var ot = __ev.data("stat");
    var i  = __ev.attr("index");
    var v  = __getVal(__ev);
    if ((ov.type != "bool" && !ov.value) || ov.value != v)
    {
      var isValOk;
      if (!ot.values.hasOwnProperty(i))
        ot.c++;
      switch (ov.type){
        case "ipv4":
          isValOk = function(z,v){
              v = v.trim();
              if(gmtIsString(v) && (/^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$/).test(v))
                return null;
              return "Format Error! Example: 192.168.1.1."
            };
          break;
        case "string":
          isValOk = function(z,v){
              var zv = z.data("osrc");
              if (zv.range && gmtIsArray(zv.range))
              {
                v = v.trim();
                if (gmtIsString(v) && v.length >= zv.range[0] && v.length <= zv.range[1])
                  return null;
                return "Length Error! Length: " + JSON.stringify(zv.range);
              };
              return null;
            };
          break;
        case "number":
        case "float":
          isValOk = function(z,v){
              var zv = z.data("osrc");
              if (zv.range && gmtIsArray(zv.range))
              {
                if (Number(v) >= zv.range[0] && Number(v) <= zv.range[1])
                  return null;
                return "Range Error! Range: " + JSON.stringify(zv.range);
              };
              return null;
            };
          break;
        case "bool":
        default:
          isValOk = function(){
             return null;
            };
          break;
      }
      ot.values[i] = { 
        v : v,
        o : __ev,
        e : __setErr,
        f : function(z,v){
            var zv = z.data("osrc");
            zv.value = v;
          },
        isValOk : isValOk
      };
    }
    else
    {
      if (ot.values.hasOwnProperty(i))
        ot.c--;
      delete ot.values[i];
    }
    changeSubmitButtonState(ot.submit, ot.c);
  }

  this.load_group = function(stat, section, o, index)
  {
    for (var b in o)
    {
      if (!o.hasOwnProperty(b))
        continue;
      var n = o[b];
      if (gmtIsObject(n))
      {
        var val;
        var group = $('<div class="input-group">').appendTo(section);
        if (n.writable) 
          group.addClass("writable");
        var label = labelConv[n.label || n] || n.label || n;
        group.append("<label class='group' for='"+(b+index)+"'>"+label+"</label>");
        switch (n.type)
        {
          case "array":
            var html = '<select id="'+b+'">';
            for (var i=0;i<n.range.length;i++)
            {
              html += '<option value="'+n.range[i]+'">'+n.range[i]+'</option>';
            }
            html += '</select>';
            val = $(html).appendTo(group).attr("index", index+"."+n.id);
            if (n.value) val.val(n.value);
            val.attr("id",b+index);
            val.data("osrc", n);
            val.data("stat", stat);
            val.data("cfg", this);
            val.on("change", function(e){
                $(this).removeClass("error")
                $(this).attr("title","");
                $(this).data("cfg").changeConfigItem($(this), 
                  function(__ev){return __ev.val();},
                  function(__ev,__reason){
                    __ev.addClass("error");
                    __ev.attr("title",__reason||"");
                  });
              });
            if (n.writable) stat.w++;
            break;
          case "ipv4":
          case "number":
          case "string":
          case "float":
            var html="";
            if (n.writable)
            {
              html += '<input type="text" value="'+n.value+'" placeholder="">';
            }
            else
            {
              html += '<div>'+n.value+'</div>';
            }
            val = $(html).appendTo(group);
            if (n.writable) stat.w++, val.addClass("writable");
            val.attr("id",b+index);
            val.attr("cfgType", n.type);
            val.attr("index", index+"."+n.id);
            val.data("osrc", n);
            val.data("stat", stat);
            val.data("cfg", this);
            val.on("change", function(e){
                $(this).removeClass("error")
                $(this).attr("title","");
                $(this).data("cfg").changeConfigItem($(this), 
                  function(__ev){
                    var ov = __ev.data("osrc");
                    if (ov.type == "number" || ov.type == "float")
                      return Number(__ev.val());
                    else
                      return __ev.val();
                  },
                  function(__ev,__reason){
                    __ev.addClass("error");
                    __ev.attr("title",__reason||"");
                  });
              });
            break;
          case "bool":
            var html,e;
  		      if (n.writable)
  		      {
  			      html=
  				    	'<label class=switch>'
  					      +'<input id=iosToggle type=checkbox />'
  					      +'<div class=switch-btn></div>'
  					    +'</label>';
  			      e = $(html).appendTo(group);
  			      val = e.children("input");
  		      }
  		      else
  		      {
  			      html=
  				    	'<label class=switch>'
  				    	+'</label>';
  			      e = $(html).appendTo(group);
  			      val = e;
  		      }
            if (n.writable) stat.w++, val.addClass("writable");
            val.attr("id",b+index);
            val.attr("cfgType", n.type);
            val.attr("index", index+"."+n.id);
            val.data("osrc", n);
            val.data("stat", stat);
            if (n.writable) 
              val.prop('checked', n.value ? true : false);
            else
            {
              val.html(n.value ? "&#10004;" : "&#10008;");
              val.css("color",n.value ? "green":"#ad5454");
            }
            val.prop('disabled', !n.writable);
  		      if (n.writable)
  		      {
              val.data("cfg", this);
  			      val.on("change", function(e){
  				      $(this).removeClass("error")
  				      $(this).attr("title","");
  				      $(this).data("cfg").changeConfigItem($(this), 
  				    	function(__ev){return __ev.is(':checked');},
  				    	function(__ev,__reason){
  				    	  __ev.addClass("error");
  				    	  __ev.attr("title",__reason||"");
  				    	});
  			       });
  		      }
            break;
          case "group":
            var sel = $("<span class='arrow char down'></span>").appendTo(group);
            var div = $("<div class='sub-section'></div>").appendTo(section);
            sel.data("subSection",div);
            sel.html("&#11206;");
            //sel.attr("src", getIconSvg("config/arrow-down.png"));
            group.AlloyFinger({
              "tap":function(e){
                toggleArrow($(this).children("span.arrow"), "&#11205;", "&#11206;");
              }
            });
            sel.attr("sub", b);
            div.addClass(b).hide();
            group.addClass("hasSubsection").css("cursor", "pointer");
            this.load_group(stat, div,n, index+"."+n.id);
            break;
          case "list":
            var sel = $("<span class='arrow down'>...</span>").appendTo(group);
            var div = $("<div class='sub-section'/>").appendTo(section);
            sel.data("subSection",div);
            //sel.attr("src", getIconSvg("config/more.png"));
            group.AlloyFinger({
              "tap":function(e){
                toggleArrow($(this).children("span.arrow"));
              }
            });
            sel.attr("sub", b);
            div.addClass(b).hide();
            group.addClass("hasSubsection").css("cursor", "pointer");
            if (!gmtIsArray(n.value))
              break;
            for (var i=0;i<n.value.length;i++)
            {
              var list = $('<fieldset class="list sub-section"><legend/></fieldset>').appendTo(div);
       	      if (n.index)
       	      {
       	        list.children("legend").text(n.value[i][n.index].value);
       	      }
              this.load_group(stat, list,n.value[i],index+"."+n.id+"."+i+(n.index ? ":"+n.value[i][n.index].value:""));
            }
            break;
        }
      }
    }
  }

  this.load_config = function(div, cfg)
  {
    var stat = {w:0,c:0,values:{}};
    for (var a in cfg)
    {
      if (!cfg.hasOwnProperty(a))
        continue;
      var o = cfg[a];
      if (gmtIsObject(o))
      {
        var label = labelConv[o.label || a] || o.label || a;
        div.append("<h1>"+label+"</h1>");
        var section = $('<div class="section">').appendTo(div);
        this.load_group(stat, section, o, o.id);
      }
    }
    if (stat.w)
    {
      var wrap = $("<div/>").appendTo(div);
      var save = $("<button class='save'>"+gmtLangBuild(["SaveConfig"],0)+"</button>").appendTo(wrap);
      var button = $("<button class='commit'>"+gmtLangBuild(["SubmitConfig"],0)+"</button>").appendTo(wrap);
      wrap.css("display","flex").css("justify-content","space-between");
      save.css("margin","0").css("margin-left","20px");
      button.css("margin","0").css("margin-right","20px");
      div.data("stat",stat);
      div.data("submit",button);
      stat.submit = button;
      button.data("stat",stat);
      save.data("stat",stat);
      changeSubmitButtonState(save,1);
      save.data("cfg",this);
      save.AlloyFinger({
        "tap":function(e){
          ____workingBusy.show();
          var d = $(this).data("stat");
          var cfg = $(this).data("cfg");
          var ___id = cfg.___mibid.id++;
          var k = {
              id:___id,
              values:{"0.9999999":1}
            };
          var y = JSON.stringify(k);
          nanoAjaxGet($(this).data("cfg"), "data/setmib.json", "GET", y, function(data, err){
              //TODO
              ____workingBusy.hide();
              if (err)
              {
                delete this.___mibid.e[___id];
                console.log("Failed to submit your configurations:"+err);
              }
              else
              {
                var x=data;
                if (!x.hasOwnProperty("id"))
                {
                  delete this.___mibid.e[___id];
                  return;
                }
                if (x.id != ___id || !this.___mibid.e[x.id])
                {
                  return;
                }
                if (x.err && gmtIsArray(x.err))
                {
                  alert("Failed");
                }
                delete this.___mibid.e[___id];
              }
            });
          cfg.___mibid.e[___id] = 1;
        }
    	});
      button.data("cfg",this);
      button.AlloyFinger({
        "tap":function(e){
          ____workingBusy.show();
          var d = $(this).data("stat");
          var cfg = $(this).data("cfg");
          if (!d.c) return;
          var ___id = cfg.___mibid.id++;
          var k = {
              id:___id,
              values:{}
            };
          
          var hasError=0;
          for (m in d.values)
          {
            if (d.values.hasOwnProperty(m))
            {
              var p = d.values[m];
              var reason = p.isValOk(p.o,p.v);
              if (reason)
              {
                hasError++;
                p.e(p.o,reason);
              }
            }
          }
          if (hasError)
          {
            ____workingBusy.hide();
            return;
          }
          for (m in d.values)
          {
            if (d.values.hasOwnProperty(m))
            {
              k.values[m] = d.values[m].v;
            }
          }
          var y = JSON.stringify(k);
          nanoAjaxGet($(this).data("cfg"), "data/setmib.json", "GET", y, function(data, err){
              //TODO
              ____workingBusy.hide();
              if (err)
              {
                delete this.___mibid.e[___id];
                console.log("Failed to submit your configurations:"+err);
              }
              else
              {
                var x=data;
                if (!x.hasOwnProperty("id"))
                {
                  delete this.___mibid.e[___id];
                  return;
                }
                if (x.id != ___id || !this.___mibid.e[x.id])
                {
                  return;
                }
                if (x.err && gmtIsArray(x.err))
                {
                  for (var i=0;i<x.err.length;i++)
                  {
                    var g=x.err[i];
                    if (!gmtIsObject(g))
                      continue;
                    if (g.id && d.values.hasOwnProperty(g.id))
                    {
                      var p = d.values[g.id];
                      p.e(p.o,g.reason);
                    }
                  }
                }
                delete this.___mibid.e[___id];
                d.c = 0;
                for (m in d.values)
                {
                  if (d.values.hasOwnProperty(m))
                  {
                    var p = d.values[m];
                    p.f(p.o,p.v);
                  }
                }
                d.values={};
                changeSubmitButtonState(button,0);
              }
            });
          cfg.___mibid.e[___id] = 1;
        }
      });
    }
  }
}
