/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 **************************************************************************************
 *                              
 *                    https://github.com/lst1975/TDMA-ng-Wifi
 *                              
 **************************************************************************************
 */
/*************************************************************************************
 *                               ngRTOS Kernel V2.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 *************************************************************************************
 *                              https://github.com/lst1975/ngRTOS
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
 */
/* AlloyFinger v0.1.15
 * By dntzhang
 * Github: https://github.com/AlloyTeam/AlloyFinger
 */
; (function () {
  function getLen(v) {
    return Math.sqrt(v.x * v.x + v.y * v.y);
  }

  function dot(v1, v2) {
    return v1.x * v2.x + v1.y * v2.y;
  }

  function getAngle(v1, v2) {
    var mr = getLen(v1) * getLen(v2);
    if (mr === 0) return 0;
    var r = dot(v1, v2) / mr;
    if (r > 1) r = 1;
    return Math.acos(r);
  }

  function cross(v1, v2) {
    return v1.x * v2.y - v2.x * v1.y;
  }
  function getEvtX(evt) {
    return evt.pageX;
  }
  function getEvtY(evt) {
    return evt.pageY;
  }

  function getRotateAngle(v1, v2) {
    var angle = getAngle(v1, v2);
    if (cross(v1, v2) > 0) {
      angle *= -1;
    }

    return angle * 180 / Math.PI;
  }

  var HandlerAdmin = function(el) {
    this.handlers = [];
    this.el = el;
  };

  HandlerAdmin.prototype.add = function(handler) {
    this.handlers.push(handler);
  }

  HandlerAdmin.prototype.del = function(handler) {
    if(!handler) this.handlers = [];

    for(var i=this.handlers.length; i>=0; i--) {
      if(this.handlers[i] === handler) {
        this.handlers.splice(i, 1);
      }
    }
  }

  HandlerAdmin.prototype.dispatch = function(one, evt, el) {
    for(var i=0,len=this.handlers.length; i<len; i++) {
      var handler = this.handlers[i];
      if(typeof handler === 'function') 
      {
        handler.apply(this.el, [evt, el]);
        if (one) break;
      }
    }
  }

  function wrapFunc(el, handler) {
    var handlerAdmin = new HandlerAdmin(el);
    handlerAdmin.add(handler);

    return handlerAdmin;
  }

  var AlloyFinger = function (el, option) {

    this.element = typeof el == 'string' ? document.querySelector(el) : el;

    this.start = this.start.bind(this);
    this.move = this.move.bind(this);
    this.end = this.end.bind(this);
    this.cancel = this.cancel.bind(this);
    this.finish = this.finish.bind(this);
    this.isMouseout = this.isMouseout.bind(this);
    this.isTouchend = option.isTouchend || function(evt){return true;};
    this.stopPropagation = option.stopPropagation;
    this.arg = option.arg;
    this.movestart = false;
    this.checkSwipe = false;
    if (sysConfig.isTouchDevice)
    {
    	this.element.addEventListener("touchstart", this.start, false);
    	this.element.addEventListener("touchmove", this.move, false);
    	this.element.addEventListener("touchend", this.end, false);
    	this.element.addEventListener("touchcancel", this.cancel, false);
    }
    else
    {
    	this.element.addEventListener("mousedown", this.start, false);
    	this.element.addEventListener("mousemove", this.move, false);
    	this.element.addEventListener("mouseup", this.finish, false);
   	  this.element.addEventListener("mouseout", this.isMouseout, false);
    }
    
    this.preV = { x: null, y: null };
    this.pinchStartLen = null;
    this.zoom = 1;
    this.isDoubleTap = false;

    this.reinitEvents = function(option)
    {
      var noop = function () { };

      this.option = option;
      this.rotate = wrapFunc(this.element, option.rotate || noop);
      this.touchStart = wrapFunc(this.element, option.touchStart || noop);
      this.multipointStart = wrapFunc(this.element, option.multipointStart || noop);
      this.multipointEnd = wrapFunc(this.element, option.multipointEnd || noop);
      this.pinch = wrapFunc(this.element, option.pinch || noop);
      this.swipe = wrapFunc(this.element, option.swipe || noop);
      this.tap = wrapFunc(this.element, option.tap || noop);
      this.doubleTap = wrapFunc(this.element, option.doubleTap || noop);
      this.longTap = wrapFunc(this.element, option.longTap || noop);
      this.singleTap = wrapFunc(this.element, option.singleTap || noop);
      this.pressMove = wrapFunc(this.element, option.pressMove || noop);
      this.twoFingerPressMove = wrapFunc(this.element, option.twoFingerPressMove || noop);
      this.touchMove = wrapFunc(this.element, option.touchMove || noop);
      this.touchEnd = wrapFunc(this.element, option.touchEnd || noop);
      this.touchCancel = wrapFunc(this.element, option.touchCancel || noop);
    
      if (typeof option.touchMove === "function")
      {
        this.checkSwipe = true;
      }
    }
    this.reinitEvents(option);

    this._cancelAllHandler = this.cancelAll.bind(this);
    window.addEventListener('scroll', this._cancelAllHandler);

    this.delta = null;
    this.last = null;
    this.now = null;
    this.tapTimeout = null;
    this.singleTapTimeout = null;
    this.longTapTimeout = null;
    this.swipeTimeout = null;
    this.x1 = this.x2 = this.y1 = this.y2 = null;
    this.preTapPosition = { x: null, y: null };
  };

  AlloyFinger.prototype = {
    start: function (evt) {
      if (sysConfig.isTouchDevice && !evt.touches)
  		  return;

      if (evt.touches) 
  		  this.touchOk = true;
      if (evt.which == 3) 
  		  return;
  	  this.stamp = gmtTime();
      this.movestart = true;
      var has = evt.touches;
      this.now = Date.now();
      this.x1 = has?evt.touches[0].pageX:getEvtX(evt);
      this.y1 = has?evt.touches[0].pageY:getEvtY(evt);
      this.delta = this.now - (this.last || this.now);
      this.touchStart.dispatch(this.stopPropagation, evt, this.element);
      if (this.preTapPosition.x !== null) {
        this.isDoubleTap = (this.delta > 0 && this.delta <= 250 
              && Math.abs(this.preTapPosition.x - this.x1) < 30 
              && Math.abs(this.preTapPosition.y - this.y1) < 30);
        if (this.isDoubleTap) clearTimeout(this.singleTapTimeout);
      }
      this.preTapPosition.x = this.x1;
      this.preTapPosition.y = this.y1;
      this.last = this.now;
      var preV = this.preV;
      var len = has ? evt.touches.length : 0;
      if (len > 1) {
        this._cancelLongTap();
        this._cancelSingleTap();
        var v = { x: evt.touches[1].pageX - this.x1, y: evt.touches[1].pageY - this.y1 };
        preV.x = v.x;
        preV.y = v.y;
        this.pinchStartLen = getLen(preV);
        this.multipointStart.dispatch(this.stopPropagation, evt, this.element);
      }
      this._preventTap = false;
      this.longTapTimeout = setTimeout(function () {
        this.longTap.dispatch(this.stopPropagation, evt, this.element);
        this._preventTap = true;
      }.bind(this), 750);
    },
    move: function (evt) {
	    if (!this.movestart)
		    return;
      if (this.touchOk && !evt.touches) 
        return;
      var has = evt.touches;
      var preV = this.preV,
        len = has?evt.touches.length:0,
        currentX = has?evt.touches[0].pageX:getEvtX(evt), 
        currentY = has?evt.touches[0].pageY:getEvtY(evt);
      this.isDoubleTap = false;
      if (len > 1) {
        var sCurrentX = evt.touches[1].pageX,
          sCurrentY = evt.touches[1].pageY
        var v = { x: evt.touches[1].pageX - currentX, y: evt.touches[1].pageY - currentY };

        if (preV.x !== null) {
          if (this.pinchStartLen > 0) {
            evt.zoom = getLen(v) / this.pinchStartLen;
            this.pinch.dispatch(this.stopPropagation, evt, this.element);
          }

          evt.angle = getRotateAngle(v, preV);
          this.rotate.dispatch(this.stopPropagation, evt, this.element);
        }
        preV.x = v.x;
        preV.y = v.y;

        if (this.x2 !== null && this.sx2 !== null) {
          evt.deltaX = (currentX - this.x2 + sCurrentX - this.sx2) / 2;
          evt.deltaY = (currentY - this.y2 + sCurrentY - this.sy2) / 2;
        } else {
          evt.deltaX = 0;
          evt.deltaY = 0;
        }
        this.twoFingerPressMove.dispatch(this.stopPropagation, evt, this.element);

        this.sx2 = sCurrentX;
        this.sy2 = sCurrentY;
      } else {
        if (this.x2 !== null) {
          evt.deltaX = currentX - this.x2;
          evt.deltaY = currentY - this.y2;

          //move事件中添加对当前触摸点到初始触摸点的判断，
          //如果曾经大于过某个距离(比如10),就认为是移动到某个地方又移回来，应该不再触发tap事件才对。
          var movedX = Math.abs(this.x1 - this.x2),
            movedY = Math.abs(this.y1 - this.y2);

          if(movedX > 10 || movedY > 10){
            this._preventTap = true;
          }

        } else {
          evt.deltaX = 0;
          evt.deltaY = 0;
        }
        
        
        this.pressMove.dispatch(this.stopPropagation, evt, this.element);
      }

      this.touchMove.dispatch(this.stopPropagation, evt, this.element);

      this._cancelLongTap();
      this.x2 = currentX;
      this.y2 = currentY;
      
      if (len > 1) {
        evt.preventDefault();
      }
    },
    end: function (evt) {
      if (this.touchOk && !evt.changedTouches) return;
	      this.movestart = false;
      this._cancelLongTap();
      var self = this;
      if (!this.touchOk || evt.touches.length < 2) 
      {
        if (this.multipointEnd)
        {
          this.multipointEnd.dispatch(this.stopPropagation, evt, this.element);
          this.sx2 = this.sy2 = null;
        }
      }

      //swipe
      var stamp = gmtTime();
      if ((!this.checkSwipe || stamp - this.stamp < 600) 
      	  && ((this.x2 && Math.abs(this.x1 - this.x2) > 30) 
      			|| (this.y2 && Math.abs(this.y1 - this.y2) > 30))) 
      {
        evt.direction = this._swipeDirection(this.x1, this.x2, this.y1, this.y2);
        this.swipeTimeout = setTimeout(function () {
          self.swipe.dispatch(self.stopPropagation, evt, self.element);
        }, 5)
      } 
	  else 
	  {
        this.tapTimeout = setTimeout(function () {
          if(!self._preventTap){
            self.tap.dispatch(self.stopPropagation, evt, self.element);
          }
          // trigger double tap immediately
          if (self.isDoubleTap) {
            self.doubleTap.dispatch(self.stopPropagation, evt, self.element);
            self.isDoubleTap = false;
          }
        }, 5)

        if (!self.isDoubleTap) {
          self.singleTapTimeout = setTimeout(function () {
            self.singleTap.dispatch(self.stopPropagation, evt, self.element);
          }, 250);
        }
      }

      if (this.touchEnd)
      {
        this.touchEnd.dispatch(this.stopPropagation, evt, this.element);
      }
      
      this.preV.x = 0;
      this.preV.y = 0;
      this.zoom = 1;
      this.pinchStartLen = null;
      this.x1 = this.x2 = this.y1 = this.y2 = null;
    },
    cancelAll: function () {
      this._preventTap = true
      clearTimeout(this.singleTapTimeout);
      clearTimeout(this.tapTimeout);
      clearTimeout(this.longTapTimeout);
      clearTimeout(this.swipeTimeout);
    },
    cancel: function (evt) {
      this.movestart = false;
  	  this.touchOk = false;
      this.cancelAll();
      this.touchCancel.dispatch(this.stopPropagation, evt, this.element);
    },
    finish: function (evt) {
        if (!this.movestart)
          return;
        var self = this;
        this.end(evt);
        this.cancelTimeout = setTimeout(function () {
          self.cancel(evt);
        }, 10);
    },
    isMouseout: function (evt) {
        if (!this.movestart)
          return;
        if (this.isTouchend(evt, this.arg))
          this.finish(evt);
    },
    _cancelLongTap: function () {
      clearTimeout(this.longTapTimeout);
    },
    _cancelSingleTap: function () {
      clearTimeout(this.singleTapTimeout);
    },
    _swipeDirection: function (x1, x2, y1, y2) {
      return Math.abs(x1 - x2) >= Math.abs(y1 - y2) ? 
			(x1 - x2 > 0 ? 'Left' : 'Right') 
			: 
			(y1 - y2 > 0 ? 'Up' : 'Down')
    },

    on: function(evt, handler) {
      if(this[evt]) {
        this[evt].add(handler);
      }
    },

    off: function(evt, handler) {
      if(this[evt]) {
        this[evt].del(handler);
      }
    },

    destroy: function() {
      if(this.singleTapTimeout) clearTimeout(this.singleTapTimeout);
      if(this.tapTimeout) clearTimeout(this.tapTimeout);
      if(this.longTapTimeout) clearTimeout(this.longTapTimeout);
      if(this.swipeTimeout) clearTimeout(this.swipeTimeout);
      if(this.cancelTimeout) clearTimeout(this.cancelTimeout);

      if (sysConfig.isTouchDevice)
      {
        this.element.removeEventListener("touchstart", this.start);
    	  this.element.removeEventListener("touchmove", this.move);
    	  this.element.removeEventListener("touchend", this.end);
    	  this.element.removeEventListener("touchcancel", this.cancel);
      }
      else
      {
    	  this.element.removeEventListener("mousedown", this.start);
    	  this.element.removeEventListener("mousemove", this.move);
    	  this.element.removeEventListener("mouseup", this.finish);
    	  this.element.removeEventListener("mouseout", this.finish);
      }
      
      this.rotate.del();
      this.touchStart.del();
      this.multipointStart.del();
      this.multipointEnd.del();
      this.pinch.del();
      this.swipe.del();
      this.tap.del();
      this.doubleTap.del();
      this.longTap.del();
      this.singleTap.del();
      this.pressMove.del();
      this.twoFingerPressMove.del()
      this.touchMove.del();
      this.touchEnd.del();
      this.touchCancel.del();

      this.preV
            = this.pinchStartLen
            = this.zoom
            = this.isDoubleTap
            = this.delta
            = this.last
            = this.now
            = this.tapTimeout
            = this.singleTapTimeout
            = this.longTapTimeout
            = this.swipeTimeout
            = this.x1
            = this.x2
            = this.y1
            = this.y2
            = this.preTapPosition
            = this.rotate
            = this.touchStart
            = this.multipointStart
            = this.multipointEnd
            = this.pinch
            = this.swipe
            = this.tap
            = this.doubleTap
            = this.longTap
            = this.singleTap
            = this.pressMove
            = this.touchMove
            = this.touchEnd
            = this.touchCancel
            = this.twoFingerPressMove
            = null;

      window.removeEventListener('scroll', this._cancelAllHandler);
      return null;
    }
  };

  if (typeof module !== 'undefined' && typeof exports === 'object') {
    module.exports = AlloyFinger;
  } else {
    window.AlloyFinger = AlloyFinger;
  }
})();

(function(factory) {
    if (typeof define === 'function' && define.amd) {
        define(['jquery', 'alloy_finger'], factory);
    } else if (typeof exports === 'object') {
        factory(require('jquery'), require('alloy_finger'));
    } else {
        factory(jQuery, AlloyFinger);
    }
}(function($, AlloyFinger) {
    function createAlloyFinger(el, options) {
        var $el = $(el);
        var af = $el.data("AlloyFinger");
        if(!af) {
            $el.data("AlloyFinger", new AlloyFinger($el[0], options));
        }
        else
        {
            af.reinitEvents($.extend(af.option,options));
        }
    }
    function destroyAlloyFinger(el) {
        var $el = $(el);
        var af = $el.data("AlloyFinger");
        if(af) {
            af.destroy();
        }
    }

    $.fn.AlloyFinger = function(options) {
        if (!options)
        {
          return this.each(function() {
              destroyAlloyFinger(this);
          });
        }
        else
        {
          return this.each(function() {
              createAlloyFinger(this, options);
          });
        }
    };
}));
