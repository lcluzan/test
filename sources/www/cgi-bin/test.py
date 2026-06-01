#!/usr/bin/env python3
import os

print("Content-Type: text/html\r\n\r\n")
print("<!DOCTYPE html>")
print("<html><body>")
print("<h2>Python CGI Environment</h2>")
print("<ul>")
print(f"<li>QUERY_STRING: {os.environ.get('QUERY_STRING', 'None')}</li>")
print(f"<li>HTTP_COOKIE: {os.environ.get('HTTP_COOKIE', 'No cookies sent')}</li>")
print("</ul>")
print("</body></html>")
