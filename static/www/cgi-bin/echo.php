<?php
// Read the raw unchunked body piped from the C++ server
$body = file_get_contents("php://input");

// Send it back to the client!
echo "SUCCESS! The CGI script received the following text:\n";
echo "---------------------------------------------------\n";
echo $body;
echo "\n---------------------------------------------------\n";
?>