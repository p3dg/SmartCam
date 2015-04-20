<?php
require 'Predis/Autoloader.php';
Predis\Autoloader::register();
$redis = new Predis\Client();

$redis->lpush('visca:command', 'g');
$redis->expire('visca:command', '5');
usleep(50000);

$params = array();
$params["pan"] = $redis->get('visca:camera:pan');
$params["tilt"] = $redis->get('visca:camera:tilt');
$params["zoom"] = $redis->get('visca:camera:zoom');
$params["controller"] = $redis->get('vision:movement:controller');
$params["position"] = $redis->get('vision:movement:position');
$params["roomname"] = $redis->get('vision:movement:roomname');
$params["states"] = $redis->lrange('vision:movement:states', 0, -1);

echo json_encode($params);
