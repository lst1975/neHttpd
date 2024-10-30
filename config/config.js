function load_group(stat, section, o, index)
{
  for (var b in o)
  {
    var n = o[b];
    if (gmtIsObject(n))
    {
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
          $(html).appendTo(group).attr("index", index+"."+n.id);
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
          var e = $(html).appendTo(group);
          if (n.writable) stat.w++, e.addClass("writable");
          e.attr("id",b);
          e.attr("cfgType", n.type);
          e.attr("index", index+"."+n.id);
          break;
        case "bool":
          var html=
                '<label class=switch>'
                  +'<input id=iosToggle type=checkbox />'
                  +'<div class=switch-btn></div>'
                +'</label>';
          var e = $(html).appendTo(group);
          if (n.writable) stat.w++, e.addClass("writable");
          e.attr("id",b);
          e.attr("cfgType", n.type);
          e.attr("index", index+"."+n.id);
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
  var stat = {w:0,c:0};
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
    $("<span></span><button class=''>"+gmtLangBuild(["SubmitConfig"],0)+"</button>").appendTo(div);
    div.data("stat",stat);
  
  }
}
