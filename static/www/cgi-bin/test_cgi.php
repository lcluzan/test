<?php
// Print headers so the C++ server knows what type of data is coming back
echo "Content-Type: text/plain\r\n\r\n";

if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    echo "--- GET REQUEST SUCCESSFUL ---\n";
    echo "Query String Received: " . $_SERVER['QUERY_STRING'] . "\n";

    // PHP automatically parses the QUERY_STRING into the $_GET array
    if (isset($_GET['name'])) {
        echo "Hello, " . $_GET['name'] . "!\n";
    }
}
elseif ($_SERVER['REQUEST_METHOD'] === 'POST') {
    echo "--- POST REQUEST SUCCESSFUL ---\n";

    // php://input grabs the raw body sent to STDIN via your cgi_write_fd
    $body = file_get_contents('php://input');

    echo "Body length received: " . strlen($body) . " bytes\n";
    echo "Body content:\n" . $body . "\n";
}
?>