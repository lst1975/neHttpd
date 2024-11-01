var ___mibid={
  id:0,
  e:{}
};
function changeSubmitButtonState(button, n)
{
  n ? button.addClass("active") : button.removeClass("active");
}

function changeConfigItem(__ev, __getVal, __setErr)
{
  var ov = __ev.data("osrc");
  var ot = __ev.data("stat");
  var i = __ev.attr("index");
  var v = __getVal(__ev);
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

function load_group(stat, section, o, index)
{
  for (var b in o)
  {
    var n = o[b];
    if (gmtIsObject(n))
    {
      var val;
      if (n.index && section.children("legend").length)
      {
        section.children("legend").text(n.value);
      }
      var group = $('<div class="input-group">').appendTo(section);
      if (n.writable) 
        group.addClass("writable");
      var label = n.label || n;
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
          val.attr("id",b+index);
          val.data("osrc", n);
          val.data("stat", stat);
          val.on("change", function(e){
              $(this).removeClass("error")
              $(this).attr("title","");
              changeConfigItem($(this), 
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
          val.on("change", function(e){
              $(this).removeClass("error")
              $(this).attr("title","");
              changeConfigItem($(this), 
                function(__ev){return __ev.val();},
                function(__ev,__reason){
                  __ev.addClass("error");
                  __ev.attr("title",__reason||"");
                });
            });
          break;
        case "bool":
          var html=
                '<label class=switch>'
                  +'<input id=iosToggle type=checkbox />'
                  +'<div class=switch-btn></div>'
                +'</label>';
          var e = $(html).appendTo(group);
          val = e.children("input");
          if (n.writable) stat.w++, val.addClass("writable");
          val.attr("id",b+index);
          val.attr("cfgType", n.type);
          val.attr("index", index+"."+n.id);
          val.data("osrc", n);
          val.data("stat", stat);
          val.prop('disabled', true);
          val.prop('checked', n.value ? true : false);
          val.prop('disabled', false);
          val.on("change", function(e){
              $(this).removeClass("error")
              $(this).attr("title","");
              changeConfigItem($(this), 
                function(__ev){return __ev.is(':checked');},
                function(__ev,__reason){
                  __ev.addClass("error");
                  __ev.attr("title",__reason||"");
                });
           });
          break;
        case "group":
          var sel = $("<img class='arrow down'></img>").appendTo(group);
          var div = $("<div class='sub-section'></div>").appendTo(section);
          sel.data("subSection",div);
          sel.attr("src", getIconSvg("config/arrow-down.png"));
          function toggleArrow(img){
            if (img.hasClass("down"))
            {
              img.removeClass("down").addClass("up");
              img.attr("src", getIconSvg("config/arrow-up.png"));
              img.data("subSection").show();
              img.parent().addClass("active");
            }
            else
            {
              img.removeClass("up").addClass("down");
              img.attr("src", getIconSvg("config/arrow-down.png"));
              img.data("subSection").hide();
              img.parent().removeClass("active");
            }
          }
          group.AlloyFinger({
            "tap":function(e){
              toggleArrow($(this).children("img"));
            }
          });
          sel.attr("sub", b);
          div.addClass(b).hide();
          group.addClass("hasSubsection").css("cursor", "pointer");
          load_group(stat, div,n, index+"."+n.id);
          break;
        case "list":
          var sel = $("<img class='arrow down'></img>").appendTo(group);
          var div = $("<div class='sub-section'></div>").appendTo(section);
          sel.data("subSection",div);
          sel.attr("src", getIconSvg("config/more.png"));
          function toggleList(img){
            if (img.hasClass("down"))
            {
              img.removeClass("down").addClass("up");
              img.data("subSection").show();
              img.parent().addClass("active");
            }
            else
            {
              img.removeClass("up").addClass("down");
              img.data("subSection").hide();
              img.parent().removeClass("active");
            }
          };
          group.AlloyFinger({
            "tap":function(e){
              toggleList($(this).children("img"));
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
            load_group(stat, list,n.value[i],index+"."+n.id+"."+i+(n.index ? ":"+n.value[i][n.index].value:""));
          }
          break;
      }
    }
  }
}

function load_config(div, cfg)
{
  var stat = {w:0,c:0,values:{}};
  for (var a in cfg)
  {
    var o = cfg[a];
    if (gmtIsObject(o))
    {
      var label = o.label || a;
      div.append("<h1>"+label+"</h1>");
      var section = $('<div class="section">').appendTo(div);
      load_group(stat, section, o, o.id);
    }
  }
  if (stat.w)
  {
    var button = $("<button class=''>"+gmtLangBuild(["SubmitConfig"],0)+"</button>").appendTo(div);
    div.data("stat",stat);
    div.data("submit",button);
    stat.submit = button;
    button.data("stat",stat);
    button.AlloyFinger({
      "tap":function(e){
        ____workingBusy.show();
        var d = $(this).data("stat");
        if (!d.c) return;
        var ___id = ___mibid.id++;
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
        nanoAjaxGet("data/setmib.json", "GET", y, function(data, err){
            //TODO
            ____workingBusy.hide();
            if (err)
            {
              delete ___mibid.e[___id];
              console.log("Failed to submit your configurations:"+err);
            }
            else
            {
              var x=data;
              if (!x.hasOwnProperty("id"))
              {
                delete ___mibid.e[___id];
                return;
              }
              if (x.id != ___id || !___mibid.e[x.id])
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
              delete ___mibid.e[___id];
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
        ___mibid.e[___id] = 1;
      }
    });
  }
}
