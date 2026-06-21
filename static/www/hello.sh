#!/bin/bash
echo -ne "Content-Type: text/html\r\n\r\n"
echo "<html><body>"
echo "<h1>Hello from a Bash CGI script! 🐚</h1>"
echo "<p>Your user agent is: $HTTP_USER_AGENT</p>"
echo "</body></html>"
