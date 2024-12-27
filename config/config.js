var labelConv = {
  "Memory":"Memory Profile",
  "Host Name":"Host Name",
};

function valid8_Validation(buffer) 
{
  if (gmtIsString(buffer))
  {
    const utf8EncodeText = new TextEncoder();
    buffer = utf8EncodeText.encode(buffer);
  }
  /*
  Maximum length allowed. Can be set to 5 or 6 (or to 1, 2 or 3)
   */

   var maxBytes = 4;


  /*
  Allow Unicode surrogates (0xD800-0xDFFF)
   */
  var surrogates = true;

  var code, i, j, len, mask, mode, n;
  mode = 0;
  for (i = j = 0, len = buffer.length; j < len; i = ++j) {
    n = buffer[i];
    if (mode) {
      if (0x80 !== (0xC0 & n)) {
        return;
      }
      code = code << 6 | n & 0x3F;
      if (--mode) {
        continue;
      }
      if (maxBytes < 5 && code > 0x0010FFFF) {
        return;
      }
      if (!surrogates && (0xD800 <= code && code <= 0xDFFF)) {
        return;
      }
      if (!(code >> mask)) {
        return;
      }
      continue;
    }
    if (!(n & 0x80)) {
      continue;
    }
    if (n === 0xFF || n === 0xFE || n === 0xC0 || n === 0xC1) {
      return;
    }
    if (!(n & 0x40)) {
      return;
    }
    mode = 1;
    mask = 0x20;
    while (n & mask) {
      mask >>= 1;
      mode++;
    }
    if (mode >= maxBytes) {
      return;
    }
    code = n & mask - 1;
    mask = 5 * mode + 1;
  }
  
  if (mode) {
    return false;
  }
  return true;
};
var Base64 = {
    _keyStr: "ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
        "abcdefghijklmnopqrstuvwxyz0123456789+/=",
    encode: function (e) {
        let t = "";
        let n, r, i, s, o, u, a;
        let f = 0;
        e = Base64._utf8_encode(e);
        while (f < e.length) {
            n = e.charCodeAt(f++);
            r = e.charCodeAt(f++);
            i = e.charCodeAt(f++);
            s = n >> 2;
            o = (n & 3) << 4 | r >> 4;
            u = (r & 15) << 2 | i >> 6;
            a = i & 63;
            if (isNaN(r)) {
                u = a = 64
            } else if (isNaN(i)) {
                a = 64
            }
            t = t +
                this._keyStr.charAt(s) +
                this._keyStr.charAt(o) +
                this._keyStr.charAt(u) +
                this._keyStr.charAt(a)
        }
        return t
    },
    decode: function (e) {
        let t = "";
        let n, r, i;
        let s, o, u, a;
        let f = 0;
        e = e.replace(/[^A-Za-z0-9\+\/\=]/g, "");
        while (f < e.length) {
            s = this._keyStr.indexOf(e.charAt(f++));
            o = this._keyStr.indexOf(e.charAt(f++));
            u = this._keyStr.indexOf(e.charAt(f++));
            a = this._keyStr.indexOf(e.charAt(f++));
            n = s << 2 | o >> 4;
            r = (o & 15) << 4 | u >> 2;
            i = (u & 3) << 6 | a;
            t = t + String.fromCharCode(n);
            if (u != 64) {
                t = t + String.fromCharCode(r)
            }
            if (a != 64) {
                t = t + String.fromCharCode(i)
            }
        }
        t = Base64._utf8_decode(t);
        return t
    },
    _utf8_encode: function (e) {
        e = e.replace(/\r\n/g, "\n");
        let t = "";
        for (let n = 0; n < e.length; n++) {
            let r = e.charCodeAt(n);
            if (r < 128) {
                t += String.fromCharCode(r)
            } else if (r > 127 && r < 2048) {
                t +=
                    String.fromCharCode(r >> 6 | 192);
                t +=
                    String.fromCharCode(r & 63 | 128)
            } else {
                t +=
                    String.fromCharCode(r >> 12 | 224);
                t +=
                    String.fromCharCode(r >> 6 & 63 | 128);
                t +=
                    String.fromCharCode(r & 63 | 128)
            }
        }
        return t
    },
    _utf8_decode: function (e) {
        let t = "";
        let n = 0;
        let r = c1 = c2 = 0;
        while (n < e.length) {
            r = e.charCodeAt(n);
            if (r < 128) {
                t += String.fromCharCode(r);
                n++
            } else if (r > 191 && r < 224) {
                c2 = e.charCodeAt(n + 1);
                t += String.fromCharCode(
                    (r & 31) << 6 | c2 & 63);

                n += 2
            } else {
                c2 = e.charCodeAt(n + 1);
                c3 = e.charCodeAt(n + 2);
                t += String.fromCharCode(
                    (r & 15) << 12 | (c2 & 63)
                    << 6 | c3 & 63);
                n += 3
            }
        }
        return t
    }
}
function toggleArrow(el, up, down)
{
  if (el.hasClass("down"))
  {
    el.removeClass("down").addClass("up");
    //img.attr("src", getIconSvg("config/arrow-up.png"));
    if (up) {
      el.html(up);
      el.attr("title", gmtLangBuild(["Collapse"],1));
    }
    el.data("subSection").show();
    el.parent().addClass("active");
  }
  else
  {
    el.removeClass("up").addClass("down");
    //img.attr("src", getIconSvg("config/arrow-down.png"));
    if (down) {
      el.html(down);
      el.attr("title", gmtLangBuild(["Expand"],1));
    }
    el.data("subSection").hide();
    el.parent().removeClass("active");
  }
}
function changeSubmitButtonState(button, n)
{
  n ? button.addClass("active") : button.removeClass("active");
}

function cfgOperation(page, alwaysActive, action)
{
  this.___mibid={
    id:0,
    e:{}
  };
  this.stat = {w:0,c:0,values:{}};
  this.action = action || "data/setmib.json";
  this.subAction = action || "data/setmib.json";
  this.alwaysActive = alwaysActive;
  this.page = page;
  this.changeConfigItem = function(__ev, __getVal, __setErr)
  {
    var ov = __ev.data("osrc");
    var ot = __ev.data("stat");
    var i  = __ev.attr("index");
    var v  = __getVal(__ev);
    if ((ov.type != "bool" && !ov.value) || ov.value != v)
    {
      var formatErr  = "Format Error! Example: ";
      var exampleV4  = "192.168.1.1.";
      var exampleV6  = "2001:db8:0:0:0:0:2:1/::1/::ffff:192.0.2.128/2001:db8::567:1.";
      var exampleMac = "01-23-45-67-89-AB or 0123456789AB or 01:23:45:67:89:AB";
      var isValOk;
      switch (ov.type){
        case "url":
           isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if((/^|(http(s)?:\/\/)([\w-]+\.)+[\w-]+([\w- ;,.\/?%&=]*)/).test(v))
                  return null;
              }
              return "Format Error! Example: [http[s]://][<user>:<password>@]<location>[:<port>]";
            };
          break;
         
        case "mac":
          isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if (v.length < 12 || v.length > 17)
                  return formatErr + exampleMac;
                if((/^([0-9A-Fa-f]{2}:){5}([0-9A-Fa-f]{2})$/).test(v)
                  || (/^([0-9A-Fa-f]{2}-){5}([0-9A-Fa-f]{2})$/).test(v)
                  || (/^([0-9A-Fa-f]{2}\.){5}([0-9A-Fa-f]{2})$/).test(v)
                  || (/^([0-9A-Fa-f]{2}\ ){5}([0-9A-Fa-f]{2})$/).test(v)
                  || (/^([0-9A-Fa-f]{2}){6}$/).test(v))
                  return null;
              }
              return formatErr + exampleMac;
            };
          break;
         
        case "ipv6":
          isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if (v.length < 2 || v.length > 45)
                  return formatErr + exampleV4;
                if((/^([\da-fA-F]{1,4}:){6}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^::([\da-fA-F]{1,4}:){0,4}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:):([\da-fA-F]{1,4}:){0,3}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){2}:([\da-fA-F]{1,4}:){0,2}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){3}:([\da-fA-F]{1,4}:){0,1}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){4}:((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){7}[\da-fA-F]{1,4}$|^:((:[\da-fA-F]{1,4}){1,6}|:)$|^[\da-fA-F]{1,4}:((:[\da-fA-F]{1,4}){1,5}|:)$|^([\da-fA-F]{1,4}:){2}((:[\da-fA-F]{1,4}){1,4}|:)$|^([\da-fA-F]{1,4}:){3}((:[\da-fA-F]{1,4}){1,3}|:)$|^([\da-fA-F]{1,4}:){4}((:[\da-fA-F]{1,4}){1,2}|:)$|^([\da-fA-F]{1,4}:){5}:([\da-fA-F]{1,4})?$|^([\da-fA-F]{1,4}:){6}:$/).test(v))
                  return null;
              }
              return formatErr + exampleV6;
            };
          break;

        case "ipv4":
          isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if (v.length < 7 || v.length > 15)
                  return formatErr + exampleV4;
                if((/^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$/).test(v))
                  return null;
              }
              return formatErr + exampleV4;
            };
          break;
        case "string":
          isValOk = function(z,v){
              if (!gmtIsString(v))
                return "Format Error! Not string";
              if (!(/^[\x20-\x7E]+$/).test(v)
                && !valid8_Validation(v))
                return "Format Error! Non-printable character";
              var zv = z.data("osrc");
              if (zv.range && gmtIsArray(zv.range))
              {
                v = v.trim();
                if (v.length >= zv.range[0] && v.length <= zv.range[1])
                  return null;
                return "Length Error! Length: " + JSON.stringify(zv.range);
              }
            };
          break;
        case "number":
           isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if(!(/^[-+]?[0-9]*$/).test(v) && !(/^[-+]?0(x|X)[0-9A-F]+$/).test(v))
                  return "Format Error! neither digit nor hex";
              }
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
        case "float":
          isValOk = function(z,v){
              if (gmtIsString(v))
              {
                v = v.trim();
                if(!(/^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$/).test(v))
                  return "Format Error!";
              }
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

      var reason = isValOk(__ev,v);
      if (reason)
      {
        __setErr(__ev,reason);
      }
      
      switch (ov.type){
        case "float":
        case "number":
          v=Number(v);
          break;
        case "mac":
          v=v.replace(/-/g, "").replace(/:/g, "");
          break;
        case "string":
          //v = Base64.encode(v);
          break;
        default:
          break;
      }
      if (!ot.values.hasOwnProperty(i))
        ot.c++;
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
    if (!this.alwaysActive)
      changeSubmitButtonState(ot.submit, ot.c);
  }

  this.__eachIndex = function(o, index, cbIdx)
  {
    for (var b in o)
    {
      if (!o.hasOwnProperty(b))
        continue;
      var n = o[b];
      if (gmtIsObject(n))
      {
        var localIndex = this.makeIndex(index, n.id, null, null, false);
        switch (n.type)
        {
          case "array":
          case "ipv4":
          case "ipv6":
          case "url":
          case "mac":
          case "number":
          case "string":
          case "float":
          case "bool":
            cbIdx("."+this._makeIndex(localIndex), n.value);
            break;
          case "group":
          case "group+":
            this.__eachIndex(n, localIndex, cbIdx);
            break;
          case "list":
          case "list+":
            for (var i=0;i<n.value.length;i++)
            {
              this.__eachIndex(n.value[i], this.makeIndex(localIndex, i, n, i, false), cbIdx);
            }
            break;
          default:
            break;
        }
      }
    }
  }

  this.eachIndex = function(cfg, cbIdx)
  {
    for (var a in cfg)
    {
      if (!cfg.hasOwnProperty(a))
        continue;
      var o = cfg[a];
      if (gmtIsObject(o))
      {
        this.__eachIndex(o, o.id, cbIdx);
      }
    }
  }

  this._makeIndex = function(index)
  {
    return "__elVal_"+index.replace(/\./g, "_").replace(/:/g, "_");
  }
  this.makeIndex = function(prefix, id, n, i, eVal)
  {
    var r = prefix+"."+id+(n && n.index ? ":"+ n.value[i][n.index].value : "");
    return eVal ? r.replace(/\./g, "_").replace(/:/g, "_") : r;
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
        var localIndex = this.makeIndex(index,n.id,null,null,false);
        group.append("<label class='group' for='"+localIndex+"'>"+label+"</label>");
        switch (n.type)
        {
          case "array":
            var html = '<select id="'+b+'">';
            for (var i=0;i<n.range.length;i++)
            {
              html += '<option value="'+n.range[i]+'">'+n.range[i]+'</option>';
            }
            html += '</select>';
            val = $(html).appendTo(group).attr("index", localIndex);
            if (n.writable) stat.w++, val.addClass("writable");
            if (n.value) val.val(n.value);
            val.attr("id",localIndex);
            val.attr("cfgType", n.type);
            val.data("osrc", n);
            val.data("stat", stat);
            val.data("cfg", this);
            val.addClass(this._makeIndex(localIndex));
            val.data("setVal", function(__el, __val){
                __el.data("osrc").value = __val;
                __el.val(__val);
              });
            if (n.writable)
              val.on("change", function(e){
                  $(this).removeClass("error")
                  $(this).attr("title","");
                  var loader = $(this).data("cfg");
                  loader.changeConfigItem($(this), 
                    function(__ev){return __ev.val();},
                    function(__ev,__reason){
                      __ev.addClass("error");
                      __ev.attr("title",__reason||"");
                    });
                });
            break;
          case "ipv6":
          case "ipv4":
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
            val.attr("id",localIndex);
            val.attr("cfgType", n.type);
            val.attr("index", localIndex);
            val.data("osrc", n);
            val.data("stat", stat);
            val.data("cfg", this);
            val.addClass(this._makeIndex(localIndex));
            val.data("setVal", function(__el, __val){
                __el.data("osrc").value = __val;
                if (__el.hasClass("writable"))
                  __el.val(__val);
                else
                  __el.text(__val);
              });
            if (n.writable)
            {
              val.ipAddress({v:n.type=="ipv6" ? 6 : 4});
              val.on("change", function(e){
                  $(this).removeClass("error")
                  $(this).attr("title","");
                  var loader = $(this).data("cfg");
                  loader.changeConfigItem($(this), 
                    function(__ev){return __ev.val().replace(/_/g,"");},
                    function(__ev,__reason){
                      __ev.addClass("error");
                      __ev.attr("title",__reason||"");
                    });
                });
            }
            break;
          case "url":
          case "mac":
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
            val.attr("id",localIndex);
            val.attr("cfgType", n.type);
            val.attr("index", localIndex);
            val.data("osrc", n);
            val.data("stat", stat);
            val.data("cfg", this);
            val.addClass(this._makeIndex(localIndex));

            val.data("setVal", function(__el, __val){
                __el.data("osrc").value = __val;
                if (__el.hasClass("writable"))
                  __el.val(__val);
                else
                  __el.text(__val);
              });

            if (n.writable)
            {
              val.on("change", function(e){
                  $(this).removeClass("error")
                  $(this).attr("title","");
                  var loader = $(this).data("cfg");
                  loader.changeConfigItem($(this), 
                    function(__ev){return __ev.val();},
                    function(__ev,__reason){
                      __ev.addClass("error");
                      __ev.attr("title",__reason||"");
                    });
                });
            }
            else
            {
              if ((n.type == "float" || n.type == "number") 
                && n.subType == "strength" 
                && gmtIsArray(n.range) 
                && n.range.length == 2)
              {
                val.attr("title",n.value).empty();
                val.addClass("signal_strength").addClass("bar");
                var signal = val.signal({exponential: false, height: group.height()});
                signal.Update(Math.abs(100*n.value/(n.range[1]-n.range[0])));
                val.data("signal", signal);
                val.data("range", n.range[1]-n.range[0]);

                val.data("setVal", function(__el, __val){
                    __el.data("osrc").value = __val;
                    var signal = __el.data("signal");
                    var range = __el.data("range");
                    signal.Update(Math.abs(100*__val/range));
                    __el.attr("title",__val);
                  });
              }
              else if ((n.type == "float" || n.type == "number") 
                && n.subType == "usage" 
                && gmtIsArray(n.range) 
                && n.range.length == 2)
              {
                val.attr("title",n.value).empty();
                var pie = $('<figure class="chart"></figure>').appendTo(val);
                var txt = $('<div class="text">'+n.value+'</div>').appendTo(val);
                var percent = Math.round(Math.abs(100*n.value/(n.range[1]-n.range[0])));
                pie.css("margin", "5px 0 0 0");
                pie.csspiechart({
                  values: [
                      percent,
                      100-percent,
                  ],
                  size:30
                });
                val.css("display", "flex");
                txt.css("margin-left","10px");
                val.data("pie", pie);
                val.data("txt", txt);
                val.data("range", n.range[1]-n.range[0]);
                val.data("setVal", function(__el, __val){
                    __el.data("osrc").value = __val;
                    var pie = __el.data("pie");
                    var txt = __el.data("txt");
                    var range = __el.data("range");
                    var percent = Math.round(Math.abs(100*__val/range));
                    pie.csspiechart({
                      values: [
                          percent,
                          100-percent,
                      ],
                      size:30
                    });
                    txt.text(__val);
                    __el.attr("title",__val);
                  });
              }
            }
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
            val.attr("id",localIndex);
            val.attr("cfgType", n.type);
            val.attr("index", localIndex);
            val.data("osrc", n);
            val.data("stat", stat);
            val.attr("title", gmtLangBuild([n.value ? "Yes" : "No"],1));
            val.prop('disabled', !n.writable);
            val.addClass(this._makeIndex(localIndex));
            val.data("setVal", function(__el, __val){
                __el.data("osrc").value = __val;
                if (__el.hasClass("writable"))
                  __el.prop('checked', __val ? true : false);
                else
                {
                  __el.html(__val ? "&#10004;" : "&#10008;");
                  __el.css("color",__val ? "green":"#ad5454");
                }
              });
            val.data("setVal")(val, n.value);
  		      if (n.writable)
  		      {
              val.data("cfg", this);
  			      val.on("change", function(e){
  				      $(this).removeClass("error")
  				      $(this).attr("title","");
                var loader = $(this).data("cfg");
  				      loader.changeConfigItem($(this), 
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
            sel.attr("title", gmtLangBuild(["Expand"],1));
            //sel.attr("src", getIconSvg("config/arrow-down.png"));
            group.AlloyFinger({
              "tap":function(e){
                toggleArrow($(this).children("span.arrow"), "&#11205;", "&#11206;");
              }
            });
            sel.attr("sub", b);
            div.addClass(b).hide();
            group.addClass("hasSubsection").css("cursor", "pointer");
            this.load_group(stat, div,n, localIndex);
            break;
          case "group+":
            var sel = $("<span class='arrow char down'></span>").appendTo(group);
            var div = $("<div class='sub-section'></div>").appendTo(section);
            sel.data("subSection",div);
            sel.html("&gt;");
            sel.attr("title", gmtLangBuild(["Detail"],1));
            group.data("cfg", this);
            group.AlloyFinger({
              "tap":function(e){
                var loader = $(this).data("cfg");
                var expand = $(this).children(".arrow");
                var sub = expand.data("subSection");
                if (expand.hasClass("down"))
                {
                  loader.div.find(".section > .input-group").removeClass("show");
                  loader.div.find(".section > .sub-section").removeClass("show");
                  loader.div.find(".section > .input-group").addClass("hidden");
                  loader.div.find(".section > .sub-section").addClass("hidden");
                  sub.addClass("show");
                  sub.removeClass("hidden");
                  sub.show();
                  expand.attr("title", gmtLangBuild(["Return"],1));
                  expand.html("&lt;");
                  expand.removeClass("down");
                  expand.addClass("up");
                  var nextSibling;
                  for (nextSibling = $(this).next();
                    nextSibling.length; 
                    nextSibling = nextSibling.next())
                  { 
                    if (nextSibling.hasClass("input-group"))
                      break;
                  };

                  if (!nextSibling.length)
                    nextSibling = null;
                  $(this).data("oldPos1", nextSibling);
                  $(this).data("oldPos2", $(this).parent());
                  $(this).addClass("show");
                  $(this).removeClass("hidden");
                  $(this).show();
                  $(this).prependTo(loader.div.children(".section"));
                  sub.insertAfter($(this));
                  loader.submitButton.hide();
                  loader.saveButton.hide();
                }
                else
                {
                  loader.submitButton.show();
                  loader.saveButton.show();
                  loader.div.find(".section > .input-group").removeClass("hidden");
                  loader.div.find(".section > .sub-section").removeClass("hidden");
                  loader.div.find(".section > .input-group").addClass("show");
                  loader.div.find(".section > .sub-section").addClass("show");
                  expand.html("&gt;");
                  expand.attr("title", gmtLangBuild(["Detail"],1));
                  expand.removeClass("up")
                  expand.addClass("down")
                  expand.data("subSection").addClass("hidden");
                  expand.data("subSection").hide();
                  var p1 = $(this).data("oldPos1");
                  if (p1)
                  {
                    $(this).insertBefore(p1);
                    sub.insertAfter($(this));
                  }
                  else
                  {
                    var p2 = $(this).data("oldPos2");
                    $(this).appendTo(p2);
                    sub.appendTo(p2);
                  }
                }
              }
            });
            sel.attr("sub", b);
            div.addClass(b).hide();
            group.addClass("hasSubsection").css("cursor", "pointer");
            this.load_group(stat, div, n, localIndex);
            break;
          case "list":
            var sel = $("<span class='arrow expand down'>...</span>").appendTo(group);
            var div = $("<div class='sub-section'/>").appendTo(section);
            sel.data("subSection",div);
            //sel.attr("src", getIconSvg("config/more.png"));
            sel.attr("title", gmtLangBuild(["Expand"],1));
            group.AlloyFinger({
              "tap":function(e){
                toggleArrow($(this).children("span.arrow.expand"),"...","...");
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
             val.addClass(this._makeIndex(localIndex));
             this.load_group(stat, list,n.value[i],this.makeIndex(localIndex,i, n, i, false));
            }
            break;
          case "list+":
            var sel = $(
                 "<div class='list'>"
                   +"<span class='arrow add'>&plus;</span>"
                   +"<span class='arrow expand down'>...</span>"
                +"</div>"
              ).appendTo(group);
            var div = $("<div class='sub-section'/>").appendTo(section);
            var expand = sel.children(".expand");
            expand.data("subSection",div);
            //sel.attr("src", getIconSvg("config/more.png"));
            expand.attr("title", gmtLangBuild(["Expand"],1));
            group.AlloyFinger({
              "tap":function(e){
                if ($(e.target).hasClass("arrow add"))
                  return;
                toggleArrow($(this).find("div > span.arrow.expand"),"...","...");
              }
            });
            var add = sel.children(".arrow.add");
            add.attr("title", gmtLangBuild(["Add"],1));
            var addListItem = {
                section : section,
                group : group,
                expand : expand,
                index : localIndex,
                preindex : index,
                stat: stat,
                n : n,
                div : div,
                addButton : add,
                loader: this,
                addListOne: function(__addListItem, loader, index, stat, div, __n, k){
                  var list = $(
                    '<fieldset class="list sub-section">'
                      +'<legend></legend>'
                      +'<div class="delete">&#9746;</div>'
                    +'</fieldset>').appendTo(div);
           	      if (__n.index)
           	      {
           	        list.children("legend").text(__n.value[k][__n.index].value);
           	      }
                  var del = list.find(".delete");
                  del.attr("title", gmtLangBuild(["Del"],1));
                  del.data("delDiv", div);
                  del.data("i", k);
                  del.data("addListItem", __addListItem);
                  del.AlloyFinger({
                    "tap":function(e){
                      var self = $(this);
                      var delDiv = $(this).data("delDiv");
                      var __addListItem = $(this).data("addListItem");
                      $.MessageBox({
                          buttonDone  : gmtLangBuild(["Confirm"],1),
                          buttonFail  : gmtLangBuild(["Cancel"],1),
                          buttonsOrder: "fail done",  // String
                          message     : gmtLangBuild(["ItemDelMsg"],1)
                      }).done(function(){
                        ____workingBusy.show();
                        var loader = __addListItem.loader;
                        var ___id = loader.___mibid.id++;
                        var kd = {
                            id:___id,
                            value : loader.makeIndex(__addListItem.index, self.data("i"), __addListItem.n, self.data("i"), false)
                          };
                        var y = JSON.stringify(kd);
                        loader.submit($(this), ___id, "data/del.json", y, function(arg){
                            var _nn = arg.n;
                            var ot = arg.stat;
                            for (var v in ot.values)
                            {
                              if (!ot.values.hasOwnProperty(v))
                                continue;
                              if (v.search(arg.index+".") != 0)
                                continue;
                              ot.c--;
                              delete ot.values[v];
                            }

                            _nn.value.splice(self.data("i"), 1);
 
                            arg.div.empty();
                            for (var y=0;y<_nn.value.length;y++)
                            {
                              arg.addListOne(arg, arg.loader, arg.index, arg.stat, arg.div, _nn, y);
                            }
                            if (!_nn.value.length)
                            {
                              if (arg.div.is(":visible"))
                              {
                                arg.group.trigger("tap");
                              }
                              arg.expand.hide();
                              arg.addButton.addClass("single");
                            }
                            //if (arg.loader.action != "data/add.json")
                            //  __load_page(arg.loader.page, null, true);
                          }, __addListItem);
                      }).fail(function(){
                      });              
                    }
                  });
                  loader.load_group(stat, list, __n.value[k], 
                    loader.makeIndex(index, k, __n, k, false));
                },
                add: function(d,item){
                    d.n.value.push(item);
                    d.addListOne(d, d.loader, d.index, d.stat, d.div, d.n, d.n.value.length-1);
                    if (d.n.value.length == 1)
                    {
                      d.expand.show();
                      d.addButton.removeClass("single");
                    }
                  },
              };
            add.data("pos", addListItem)
            add.AlloyFinger({
              "tap":function(e){
                var p = __load_sub_page("TEMPLATE", "template", MAIN_TEMPLATE_display, 
                                $(this).data("pos"), true, "data/add.json");
              }
            });
            sel.children(".expand").attr("sub", b);
            div.addClass(b).hide();
            group.addClass("hasSubsection").css("cursor", "pointer");
            if (!gmtIsArray(n.value))
              break;
            if (!n.value.length)
            {
              if (div.is(":visible"))
              {
                group.trigger("tap");
              }
              expand.hide();
              add.addClass("single");
            }
            for (var i=0;i<n.value.length;i++)
            {
              addListItem.addListOne(addListItem, this, localIndex, stat, div, n, i);
            }
            break;
        }
      }
    }
  }

  this.submit = function(button, ___id, file, data, okCb, arg)
  {
    this.subAction = file;
    nanoAjaxGet(this, file, "GET", "data="+Base64.encode(data), 
      function(response, err){
        //TODO
        var d = this.stat;
        ____workingBusy.hide();
        if (err || !response)
        {
          delete this.___mibid.e[___id];
          console.log("Failed to submit your configurations:"+err);
          $.MessageBox({
              buttonDone  : gmtLangBuild(["Confirm"],1),
              buttonFail  : null,
              message     : gmtLangBuild(["CfgFailed"],1),
          }).done(function(){
          }).fail(function(){
          });              
        }
        else
        {
          var x=response;
          if (!x.hasOwnProperty("id"))
          {
            delete this.___mibid.e[___id];
            return;
          }
          if (x.id != ___id || !this.___mibid.e[x.id])
          {
            return;
          }

          var hasError = x.err && gmtIsArray(x.err) && x.err.length;
          if (hasError)
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
            var errStr = "";
            for (var i=0;i<x.err.length;i++)
            {
              errStr += "<br/>"+x.err[i].id+":"+x.err[i].reason;
            }
            $.MessageBox({
                buttonDone  : gmtLangBuild(["Confirm"],1),
                buttonFail  : null,
                message     : gmtLangBuild(["CfgFailed"],1) + errStr,
            }).done(function(){
            }).fail(function(){
            });              
          }
          delete this.___mibid.e[___id];

          if (this.subAction != "data/del.json")
          {
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
          }
          
          if (!this.alwaysActive && button)
            changeSubmitButtonState(button,0);
          if (!hasError && okCb)
          {
            okCb(arg);
          }
        }
      });
      this.___mibid.e[___id] = 1;
    },
  this.load_config = function(p, div, cfg)
  {
    this.div = div;
    var stat = this.stat;
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
      var button = null;
      var wrap = $("<div/>").appendTo(div);
      var bClass = p.type == "normal" ? "save" : "return";
      var bLabel = p.type == "normal" ? "SaveConfig" : "Return";
      var save = $("<button class='"+bClass+"'>"+gmtLangBuild([bLabel],0)+"</button>").appendTo(wrap);
      save.attr("bClass", bClass)
      save.attr("bLabel", bLabel)
      if (p.type != "subgroup")
        button = $("<button class='commit'>"+gmtLangBuild(["SubmitConfig"],0)+"</button>").appendTo(wrap);
      wrap.css("display","flex").css("justify-content","space-between");
      save.css("margin","0").css("margin-left","20px");
      div.data("stat",stat);
      div.data("submit",button);
      stat.submit = button;
      save.data("stat",stat);
      changeSubmitButtonState(save,1);
      if (this.alwaysActive)
        changeSubmitButtonState(button,1);
      save.data("cfg",this);
      save.data("page",p);
      this.submitButton = button;
      this.saveButton = save;
      save.AlloyFinger({
        "tap":function(e){
          if ($(this).hasClass("save"))
          {
            ____workingBusy.show();
            var d = $(this).data("stat");
            var loader = $(this).data("cfg");
            var ___id = loader.___mibid.id++;
            var k = {
                id:___id,
                value:{"0.9999999":1}
              };
            var y = JSON.stringify(k);
            loader.submit(null, ___id, "data/savemib.json", y, function(arg){
                $.MessageBox({
                    buttonDone  : gmtLangBuild(["Confirm"],1),
                    buttonFail  : null,
                    message     : gmtLangBuild(["SaveOk"],1),
                }).done(function(){
                }).fail(function(){
                });              
              }, null);
          }
          else if ($(this).hasClass("return"))
          {
            var sub = save.data("page");
            __close_sub_page(sub);
          }
        }
    	});
      if (button)
      {
        button.css("margin","0").css("margin-right","20px");
        button.data("stat",stat);
        button.data("cfg",this);
        button.data("page",p);
        button.data("obj",cfg);
        button.AlloyFinger({
          "tap":function(e){
            var page = $(this).data("page");
            ____workingBusy.show();
            var d = $(this).data("stat");
            var loader = $(this).data("cfg");
            var cfg = $(this).data("obj");
            var ___id = loader.___mibid.id++;
            if (!loader.alwaysActive && !d.c) 
            {
              ____workingBusy.hide();
              return;
            }
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
            var k = {
                id:___id,
                value:{}
              };
            for (m in d.values)
            {
              if (d.values.hasOwnProperty(m))
              {
                k.value[m] = d.values[m].v;
              }
            }
            var y = JSON.stringify(k);
            loader.submit($(this), ___id, loader.action, y, function(arg){
                if (arg.loader.action == "data/add.json")
                {
                  $.MessageBox({
                      buttonDone  : gmtLangBuild(["Confirm"],1),
                      buttonFail  : null,
                      message     : gmtLangBuild(["AddOk"],1),
                  }).done(function(){
                  }).fail(function(){
                  });              
                }
                if (!arg.p.data || !arg.p.data.add)
                  return;
                
                if (arg.p.type == "template"
                  && gmtIsObject(arg.cfg.template))
                {
                  arg.p.data.add(arg.p.data, arg.cfg.template);
                }
              }, {p:page,cfg:cfg,loader:loader});
          }
        });
      }
    }
  }
}
