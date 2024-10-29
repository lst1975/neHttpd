function load_group(section, o)
{
  for (var b in o)
  {
    var n = o[b];
    if (gmtIsObject(n))
    {
      var group = $('<div class="input-group'+(n.writtable?" writable":"")+'">').appendTo(section);
      var label = n.label || n;
      group.append("<label for='"+b+"'>"+label+"</label>");
      switch (n.type)
      {
        case "array":
          var html = '<select id="'+b+'">';
          for (var i=0;i<n.range.length;i++)
          {
            html += '<option value="'+n.range[i]+'">'+n.range[i]+'</option>';
          }
          html += '</select>';
          group.append(html);
          break;
        case "number":
        case "string":
        case "float":
          var html="";
          if (n.writtable)
          {
            html += '<input type="text" value="'+n.value+'" placeholder="">';
          }
          else
          {
             html += '<div>'+n.value+'</div>';
          }
          var e = $(html).appendTo(group);
          if (n.writtable) e.addClass("writable");
          e.attr("id",b);
          e.attr("cfgType", n.type);
          break;
        case "group":
          var sel = $("<img class='arrow down'></img>").appendTo(group);
          var div = $("<div class='sub-section'></div>").appendTo(section);
          sel.data("subSection",div);
          sel.attr("src", "config/arrow-down.png");
        	sel.AlloyFinger({
        		"tap":function(e){
        			if ($(this).hasClass("down"))
              {
                $(this).removeClass("down").addClass("up");
                $(this).attr("src", "config/arrow-up.png");
                $(this).data("subSection").show();
              }
              else
              {
                $(this).removeClass("up").addClass("down");
                $(this).attr("src", "config/arrow-down.png");
                $(this).data("subSection").hide();
              }
        		}
        	});
          sel.attr("sub", b);
          div.addClass(b).hide();
          load_group(div,n);
          break;
      }
    }
  }
}

function load_config(div, cfg)
{
  for (var a in cfg)
  {
    var o = cfg[a];
    if (gmtIsObject(o))
    {
      var label = o.label || a;
      div.append("<h1>"+label+"</h1>");
      var section = $('<div class="section">').appendTo(div);
      load_group(section, o);
    }
  }
}