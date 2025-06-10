<?php

$config = parse_ini_file('../config/config.ini');

$dirFile = $config['clasificacionFile'];

$dh = @opendir($dirFile);
if (!$dh)
    die('No se pudo abrir el directorio');

$json = array();
$iteracion = 0;
$clasificaciones = array();
while ($file = readdir($dh)) {
    if ($file == '.' || $file == '..')
        continue;
    $parts = explode('.', $file);
    $extencion = array_pop($parts);

    if (strpos($extencion, 'json') !== false) {
        $data = json_decode(file_get_contents($dirFile . $file), true);
        if (is_array($data)) {
            $result = $data['result'];
            $iteracion += $data['iteracion'];
            foreach ($result as $r) {
                if ($r['denominacion'] === 'NPC') {
                    continue;
                }
                if (!isset($clasificaciones [$r['denominacion']]))
                    $clasificaciones [$r['denominacion']] = $r;
                else {
                    foreach ($r['clasificados'] as $c) {
                        $clasificaciones [$r['denominacion']]['clasificados'][] = $c;
                    }
                }
            }
        }
    }
}

foreach ($clasificaciones as $cla) {
    $json[] = $cla;
}


closedir($dh);

echo json_encode(array('iteracion' => $iteracion, 'result' => $json));
