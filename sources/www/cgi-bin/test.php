<?php

echo "<!DOCTYPE html>\n";
echo "<html lang='fr'>\n";
echo "<head><title>Test CGI Webserver 42</title></head>\n";
echo "<body style='font-family: sans-serif; padding: 2rem;'>\n";
echo "    <h1 style='color: #4CAF50;'> Succès : Hello depuis le CGI PHP !</h1>\n";
echo "    <p>Si tu vois cette page, c'est que <strong>fork()</strong>,<strong>pipes</strong> et<strong>execve()</strong> fonctionnent !</p>\n";

echo "    <hr>\n";
echo "    <h3>Variables d'environnement reçues :</h3>\n";
echo "    <ul>\n";
echo "        <li><strong>REQUEST_METHOD</strong> : " . (isset($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : 'Non définie') . "</li>\n";
echo "        <li><strong>PATH_INFO</strong> : " . (isset($_SERVER['PATH_INFO']) ? $_SERVER['PATH_INFO'] : 'Non définie') . "</li>\n";
echo "        <li><strong>SERVER_PROTOCOL</strong> : " . (isset($_SERVER['SERVER_PROTOCOL']) ? $_SERVER['SERVER_PROTOCOL'] : 'Non définie') . "</li>\n";
echo "    </ul>\n";
echo "</body>\n";
echo "</html>\n";
?>