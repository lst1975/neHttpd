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
/*!
 * CSSPieChart jQuery plugin
 * Copyright 2011, Miguel L Gonzalez
 * Licensed under the GPL Version 3 license.
 * http://mentadreams.com
 *
 * csspiechart jQuery function, takes object options as argument with properties:
 * values: absolute values to be represented in the pieChart
 * size: size of the pie chart (width and height)
 * colors: array of valid CSS colors, i.e. #ff0, rgb(255,40,40). For now it's not recommended to use non-opaque colours
 * TODO: modify the CSS for width and height
 * TODO: accept an option for explicit percentage values
 * 
 */
(function($){
    $.fn.csspiechart = function(options) {

        //Default values
        var defaults = {
            values: [10,20,30],
            size: 200,
            colors: ['orange','green','yellow', 'red','#0ff', 'blue','purple'], //the rainbow colors as default
        };

        //Merge the default options witht the values in the params
        var options = $.extend(defaults, options);

        //Do for all the elements
        return this.each(function() {
            obj = $(this);
            var body = obj.html();
            sum = 0;
            total = 0;
            //Get the sum of values
            for(val in options.values) {
                //fix NaN values
                if (isNaN(options.values[val]))
                    options.values[val] = 0;
                total+=options.values[val];
            }
            angles = new Array();
            //Clean the content
            obj.html('');
            for(val in options.values) {
                angle = parseInt(36000*(options.values[val]/total))/100;
                angles.push(angle);
				var index = val;
				if (options.values.length == 2)
				{
				  if (Number(val)==0)
				  {
					  if (options.values[0] > 80)
						index = 3;
					  else if (options.values[0] > 66)
						index = 6;
					  else if (options.values[0] > 33)
						index = 0;
					  else
						index = 2;
				  }
				  else
					index = 1;
				}
                //If the angle of this slice is > 180 degrees we need 2 divs for displaying that
                if (angle > 180){
                    obj.append('<div style="-moz-transform:rotate('+sum+'deg);-webkit-transform:rotate('+sum+'deg)" class="front"><div style="background:'+options.colors[index]+';-moz-transform:rotate('+(180)+'deg);-webkit-transform:rotate('+(180)+'deg)" class="pie"></div></div>');
                    obj.append('<div style="-moz-transform:rotate('+(sum+179)+'deg);-webkit-transform:rotate('+(sum+179)+'deg)" class="front"><div style="background:'+options.colors[index]+';-moz-transform:rotate('+(angle-179)+'deg);-webkit-transform:rotate('+(angle-179)+'deg)" class="pie"></div></div>');
                }else{
                    obj.append('<div style="-moz-transform:rotate('+sum+'deg);-webkit-transform:rotate('+sum+'deg)" class="front"><div style="background:'+options.colors[index]+';-moz-transform:rotate('+angle+'deg);-webkit-transform:rotate('+angle+'deg)" class="pie"></div></div>');
                }
                sum+=angle;
            }
            //The back of the Pie, for the border and shadow
            obj.append('<div class="holder back"></div>');
        });
    };
})(jQuery);

