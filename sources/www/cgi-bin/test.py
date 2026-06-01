#!/usr/bin/env python3
import os
import sys

# 1. Crucial CGI Header: tells the server and browser what content type follows
print("Content-Type: text/html\r\n\r\n")

# 2. Print HTML structures to display variables clearly during defense
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head><title>Webserv 42 - Python CGI Test</title></head>")
print("<body style='font-family: sans-serif; padding: 20px; background-color: #f4f4f9; color: #333;'>")
print("    <h1 style='color: #2b5b84;'>Webserv 42 - CGI Verification Page 🐍</h1>")
print("    <p>If you see this page, your <code>execve()</code> and interpreter mappings are fully functional!</p>")
print("    <hr>")

# Display standard metadata
print("    <h3>1. Request Basics</h3>")
print("    <ul>")
print(f"        <li><strong>Method:</strong> {os.environ.get('REQUEST_METHOD', 'N/A')}</li>")
print(f"        <li><strong>Protocol:</strong> {os.environ.get('SERVER_PROTOCOL', 'N/A')}</li>")
print("    </ul>")

# Display GET arguments (Query string validation)
print("    <h3>2. GET Variables (Query String Bonus)</h3>")
print(f"    <p><strong>Raw QUERY_STRING:</strong> <span style='color: green;'>{os.environ.get('QUERY_STRING', 'None / Empty')}</span></p>")

# Display Cookies (Header forwarding validation)
print("    <h3>3. Session / Cookie Bonus</h3>")
print(f"    <p><strong>Raw HTTP_COOKIE:</strong> <span style='color: purple;'>{os.environ.get('HTTP_COOKIE', 'No cookies sent')}</span></p>")

# Display incoming POST stream payloads
print("    <h3>4. POST Body Content</h3>")
method = os.environ.get('REQUEST_METHOD', '')
if method == "POST":
    try:
        # Safely read content_length bytes from standard input pipe
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length > 0:
            body = sys.stdin.read(content_length)
            print(f"    <pre style='background: #e8e8e8; padding: 10px; border-radius: 5px;'>{body}</pre>")
        else:
            print("    <p style='color: gray;'>POST method detected, but body length is 0.</p>")
    except Exception as e:
        print(f"    <p style='color: red;'>Error reading standard input stream: {e}</p>")
else:
    print("    <p style='color: gray;'>Not a POST request (Standard input stream is empty).</p>")

print("</body>")
print("</html>")
