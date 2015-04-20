<!DOCTYPE html> 
<html> 
  <head> 
  <title>Automated Presenter Tracking System</title> 
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <link rel="stylesheet" href="css/jquery.mobile-1.1.0.min.css" />
  <script src="js/jquery-1.7.1.min.js"></script>
  <script src="js/jquery.mobile-1.1.0.min.js"></script>
  <script type="text/javascript">
  var reloadImages = 0;
    var slider_timeout = 0;
    var slider_zoom_timeout = 0;
function reloadImg(id) {
  var obj = document.getElementById(id);
  var src = obj.src;
  var pos = src.indexOf('?');
  if (pos >= 0) {
    src = src.substr(0, pos);
  }
  var date = new Date();
  obj.src = src + '?v=' + date.getTime();
  return true;
}

function init_slider_start_stop(id) {
    $(id).live('mousedown', function(){$(id).trigger('start');});
    $(id).live('mouseup', function(){$(id).trigger('stop');});
    $(id).live('touchstart', function(){$(id).trigger('start');});
    $(id).live('touchend', function(){$(id).trigger('stop');});

}

$(document).delegate("#page", "pageinit", function() {
    //get start/stop events
    init_slider_start_stop('#slider-tilt');
    init_slider_start_stop('#slider-pan');
    init_slider_start_stop('#slider-zoom');
    
    $('#cameraPoll').bind( "change", function(event, ui) {
      if($('#cameraPoll').val() == 'off') {
        clearInterval(reloadImages);
      } else {
        reloadImages = setInterval(function() {
          reloadImg("input");
          //reloadImg("skinfilter");
          //reloadImg("painting")
        }, 250);
      }
    });

    $('#autoTrack').bind( "change", function(event, ui) {
      if($('#autoTrack').val() == 'off') {
        $.post("camera_command.php", { controller: "manual" } );
      } else {
        $.post("camera_command.php", { controller: "automatic" } );
      }
    
    });
    
  $('#fullMovement').bind( "change", function(event, ui) {
    if($('#fullMovement').val() != 'on') {
      $( "#slider-tilt" ).attr("min", -24);
      $( "#slider-tilt" ).attr("max", 24);
      $( "#slider-pan" ).attr("min", -65);
      $( "#slider-pan" ).attr("max", 65);
    } else {
      $( "#slider-tilt" ).attr("min", -30);
      $( "#slider-tilt" ).attr("max", 90);
      $( "#slider-pan" ).attr("min", -170);
      $( "#slider-pan" ).attr("max", 170);            
    }
    $( "#slider-pan" ).slider('refresh');
    $( "#slider-tilt" ).slider('refresh');
    });

//$('#slider').siblings('.ui-slider').bind('tap', function(event, ui){ makeAjaxChange($(this).siblings('input')); });
    //$('#slider').siblings('.ui-slider a').bind('taphold', function(event,
    //ui){ makeAjaxChange($(this).parent().siblings('input'));
    //$('#slider-pan').next().children('a').bind('vmouseup', send_new_cordinates);
    $( "#slider-zoom" ).bind( "change", function() {
      //if(event.type == 'vmousedown')
      //window.clearTimeout(slider_zoom_timeout);
      //slider_zoom_timeout = window.setTimeout(send_new_zoom, 200);
    });

    $( "#slider-pan" ).bind( "change", function() {
      //if(event.type == 'vmousedown')
      //window.clearTimeout(slider_timeout);
      //slider_timeout = window.setTimeout(send_new_cordinates, 200);
      //send_new_cordinates();
    });
    $( "#slider-tilt" ).bind( "change", function() {
     // if(event.type == 'vmousedown')
      //window.clearTimeout(slider_timeout);
      //slider_timeout = window.setTimeout(send_new_cordinates, 200);
      //send_new_cordinates();
    });


    $("#btn_home").click(function(){
      $.post("camera_command.php", { misc: "home" } );
    });

    $("#btn_reset").click(function(){
      $.post("camera_command.php", { misc: "reset" } );
    });

    $("#btn_move").click(function(){
      send_new_zoom();
      send_new_cordinates();
    });

    $("#btn_refresh_data").click(function(){
      refresh_data();
    });

    $('#roomPosition').change(function() {
      $.post("camera_command.php", {position: $('#roomPosition').val()});
    });

    refresh_data();

});

function refresh_data()
{
    $.get("poll_info.php", function(data)
          {
            $('#slider-pan').val(data['pan']);
            $('#slider-tilt').val(data['tilt']);
            $('#slider-zoom').val(data['zoom']);
            if(data['controller'] == 'manual')
              $('#autoTrack').val('off');
            else
              $('#autoTrack').val('on');

            $('#slider-pan').slider('refresh');
            $('#slider-tilt').slider('refresh');
            $('#slider-zoom').slider('refresh');
            $('#autoTrack').slider('refresh');

            var html = '';
            var len = data["states"].length;
            for (var i = 0; i< len; i++) {
              if(data["states"][i] == data["position"])
                html += '<option selected=\"selected\" value="' + data["states"][i] + '">' +  data["states"][i] + '</option>';
              else
                html += '<option value="' + data["states"][i] + '">' +  data["states"][i] + '</option>';
            }
            $('#roomPosition').html(html);
            $('#roomPosition').selectmenu('refresh', true);
            $('#roomName').html("<strong>Room:</strong> " + data["roomname"]);
          }, "json");
}
function send_new_zoom()
{
    $.post("camera_command.php", { zoom: $("#slider-zoom").val() } );
}

function send_new_cordinates()
{
    console.log($("#slider-pan").val() + " " + $("#slider-tilt").val() );
  $.post("camera_command.php", { pan: $("#slider-pan").val(), tilt: $("#slider-tilt").val() } );
}


  </script>
  </head> 
  <body> 
    
    <div id="page" data-role="page" data-theme="c">
      
      <div data-role="header">
	<h1>Automated Presenter Tracking System</h1>
      </div><!-- /header -->
      
      <div data-role="content">	
	<div class="ui-grid-a">
	  <div class="ui-block-a">
            <img id="input" src="cgi/status_image.cgi" style="width:100%;height:auto;" />
          </div>
      </div>
      <div class="ui-grid-a">
	<div class="ui-block-a">
          <label for="cameraPoll">Enable Camera Polling</label>
          <select name="slider" id="cameraPoll" data-role="slider">
	    <option value="off">Off</option>
	    <option value="on">On</option>
          </select> 

          <label for="autoTrack">Enable Automated Tracking</label>
          <select name="slider" id="autoTrack" data-role="slider">
	    <option value="off">Off</option>
	    <option value="on">On</option>
          </select> 

          <br/>
          <br/>
          <br/>
          <span id="roomName"></span><br/>
          <div data-role="fieldcontain">
            <label for="roomPosition" class="select">Position:</label>
	    <select name="roomPosition" id="roomPosition" data-native-menu="false">  
	    </select>
          </div>

          </div>

	<div class="ui-block-b">
	  <label for="slider-pan">Camera Pan:</label>
          <input type="range" name="slider" id="slider-pan" value="0" min="-65" max="65" />

	  <label for="slider-tilt">Camera Tilt:</label>
          <input type="range" name="slider" id="slider-tilt" value="0" min="-24" max="24" />

	  <label for="slider-zoom">Camera Zoom:</label>
          <input type="range" name="slider" id="slider-zoom" value="0" min="0" max="16384" data-highlight="true"/>

          <label for="fullMovement">Enable Full Camera Movement</label>
          <select name="slider" id="fullMovement" data-role="slider">
	    <option value="off">Off</option>
	    <option value="on">On</option>
          </select> 

          <a href="#" id="btn_move" data-role="button"
          data-icon="check" data-inline="true">Move Camera</a>

          <a href="#" id="btn_refresh_data" data-role="button"
          data-icon="refresh" data-inline="true">Refresh Data</a>

        </div>

	<!--<div class="ui-block-c">
        </div>-->
      </div><!-- /grid-b -->

      </div><!-- /content -->

      <div data-role="footer" class="ui-bar">
          <a href="#" id="btn_home" data-role="button" data-icon="home" data-inline="true">Go To Home</a>
          <a href="#" id="btn_reset" data-role="button" data-icon="refresh" data-inline="true">Reset</a>
	<h4>ECE Team 12 2012</h4>

      </div><!-- /header -->

    </div><!-- /page -->
    
  </body>
</html>
