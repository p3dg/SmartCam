<?php
require 'Predis/Autoloader.php';
Predis\Autoloader::register();
$redis = new Predis\Client();

if(isset($_POST['pan']) && isset($_POST['tilt']))
  {
    $redis = new Predis\Client();
    $pan = intval($_POST['pan']);
    $tilt = intval($_POST['tilt']);
    $redis->set('visca:camera:pan', "$pan");
    $redis->set('visca:camera:tilt', "$tilt");
    $redis->lpush('visca:command', 'm');
    $redis->expire('visca:command', '5');

    /*$command = '/var/www/visca_handler';
    $usb_device = '/dev/ttyUSB0';
    $pan = intval($_POST['pan']);
    $tilt = intval($_POST['tilt']);

    //this is definitely a security hole... oops
    shell_exec("$command $usb_device a $pan $tilt"); */
  }

if(isset($_POST['zoom']))
  {
    $zoom = intval($_POST['zoom']);
    $redis->set('visca:camera:zoom', "$zoom");
    $redis->lpush('visca:command', 'z');
    $redis->expire('visca:command', '5');
  }

if(isset($_POST['misc']))
  {
    $command = $_POST['misc'];
    if($command == "reset")
      {
        $redis->lpush('visca:command', 'r');
        $redis->expire('visca:command', '5');
      }
    else if($command == "home")
      {
        $redis->lpush('visca:command', 'h');
        $redis->expire('visca:command', '5');
      }
  }

if(isset($_POST['controller']))
  {
    if($_POST['controller'] == "automatic")
      {
        $redis->set('vision:movement:controller', 'automatic');
      }
    else
      {
        $redis->set('vision:movement:controller', 'manual');
      }
  }

if(isset($_POST['position']))
  {
    $redis->set('vision:movement:position', $_POST['position']);
  }

?>
