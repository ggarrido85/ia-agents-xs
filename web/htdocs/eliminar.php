<?php

$config = parse_ini_file('../config/config.ini');

$dirFile = $config['clasificacionFile'];
//die($dirFile );
$dh = @opendir($dirFile);
if (!$dh)
	die('No se pudo abrir el directorio');
while ($file = readdir($dh)) {
	if ($file == '.' || $file == '..')
		continue;
	$parts = explode('.', $file);
	$extencion = array_pop($parts);
	
	if ( strpos($extencion,'xml' )!== false ) {
		unlink($dirFile . $file);
	}
}
closedir($dh);
