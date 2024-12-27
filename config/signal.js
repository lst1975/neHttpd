(function( $ ) {
 
    $.fn.signal = function(options) {
        var maxVal = 100;
        var settings = $.extend({
            color: "#61c419",
            backgroundColor: "lightgrey",
            numberOfBars: 10,
            exponential: false
        }, options );


        var element = this;
        function createBars(){
            var baseHeight = (settings.height/10 || 10);
            var w = baseHeight*2;
            for(var i =0; i< settings.numberOfBars; i++){
                var height = (baseHeight) + (settings.exponential ?  Math.exp(i*0.5) : (i+baseHeight));
                var bar = $('<div class="bar" style="height:'+height+'px;"></div>').appendTo(element);
                bar.width(w);
            }
        }

        function update(value){
            var bars = $(".bar", element);
            bars.removeClass("active").removeClass("inactive");
            var active =  Math.round((value/100)*settings.numberOfBars);
            for(var i = 0; i < bars.length; i++){
                var bar = $(bars[i]);
                setClasses(bar, i < active);
            }
        }

        function setClasses(bar, isActive){
            if(isActive){
                bar.css("background-color",settings.color);
            }
            else{
                bar.css("background-color",settings.backgroundColor);
            }
        }

        createBars();
        return {
            Update: update
        }
 
    };
 
}( jQuery ));
