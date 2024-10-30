var ___mibid={
  id:0,
  e:{}
};
function changeSubmitButtonState(button, n)
{
  n ? button.addClass("active") : button.removeClass("active");
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
      group.append("<label class='group' for='"+b+"'>"+label+"</label>");
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
          val.data("osrc", n);
          val.data("stat", stat);
          val.on("change", function(e){
              var ov = $(this).data("osrc");
              var ot = $(this).data("stat");
              var i = $(this).attr("index");
              if (!ov.value || ov.value != $(this).val())
              {
                if (!ot.values.hasOwnProperty(i))
                  ot.c++;
                ot.values[i] = { 
                  v : $(this).val(),
                  o : $(this),
                  f : function(z,v){
                      var zv = z.data("osrc");
                      zv.value = v;
                    }
                };
              }
              else
              {
                if (ot.values.hasOwnProperty(i))
                  ot.c--;
                delete ot.values[i];
              }
              changeSubmitButtonState(ot.submit, ot.c);
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
          val.attr("id",b);
          val.attr("cfgType", n.type);
          val.attr("index", index+"."+n.id);
          val.data("osrc", n);
          val.data("stat", stat);
          val.on("change", function(e){
              var ov = $(this).data("osrc");
              var ot = $(this).data("stat");
              var i = $(this).attr("index");
              if (!ov.value || ov.value != $(this).val())
              {
                if (!ot.values.hasOwnProperty(i))
                  ot.c++;
                ot.values[i] = { 
                  v : $(this).val(),
                  o : $(this),
                  f : function(z,v){
                      var zv = z.data("osrc");
                      zv.value = v;
                    }
                };
              }
              else
              {
                if (ot.values.hasOwnProperty(i))
                  ot.c--;
                delete ot.values[i];
              }
              changeSubmitButtonState(ot.submit, ot.c);
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
          val.attr("id",b);
          val.attr("cfgType", n.type);
          val.attr("index", index+"."+n.id);
          val.data("osrc", n);
          val.data("stat", stat);
          val.prop('disabled', true);
          val.prop('checked', n.value ? true : false);
          val.prop('disabled', false);
          val.on("change", function(e){
              var ov = $(this).data("osrc");
              var ot = $(this).data("stat");
              var isChecked = $(this).is(':checked');
              var i = $(this).attr("index");
              if (ov.value != isChecked)
              {
                if (!ot.values.hasOwnProperty(i))
                  ot.c++;
                ot.values[i] = { 
                  v : isChecked,
                  o : $(this),
                  f : function(z,v){
                      var zv = z.data("osrc");
                      zv.value = v;
                    }
                };
              }
              else
              {
                if (ot.values.hasOwnProperty(i))
                  ot.c--;
                delete ot.values[i];
              }
              changeSubmitButtonState(ot.submit, ot.c);
            });
          break;
        case "group":
          var sel = $("<img class='arrow down'></img>").appendTo(group);
          var div = $("<div class='sub-section'></div>").appendTo(section);
          sel.data("subSection",div);
          sel.attr("src", "config/arrow-down.png");
          function toggleArrow(img){
            if (img.hasClass("down"))
            {
              img.removeClass("down").addClass("up");
              img.attr("src", "config/arrow-up.png");
              img.data("subSection").show();
              img.parent().addClass("active");
            }
            else
            {
              img.removeClass("up").addClass("down");
              img.attr("src", "config/arrow-down.png");
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
          sel.attr("src", "config/more.png");
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
          }
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
            load_group(stat, list,n.value[i],index+"."+n.id);
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
        var d = $(this).data("stat");
        if (!d.c) return;
        var ___id = ___mibid.id++;
        var k = {
            id:___id,
            values:{}
          };
        for (m in d.values)
        {
          if (d.values.hasOwnProperty(m))
          {
            k.values[m] = d.values[m].v;
          }
        }
        var y = JSON.stringify(k);
        nanoAjaxGet("config/setmib.json", "GET", y, function(data, err){
            //TODO
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
