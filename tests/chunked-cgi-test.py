import socket

# 1. The Request Headers pointing to the CGI script
headers = (
    b"POST /cgi-bin/echo.php HTTP/1.1\r\n"
    b"Host: 127.0.0.1:8080\r\n" # set up different host:port if necessary
    b"Transfer-Encoding: chunked\r\n"
    b"Content-Type: text/plain\r\n"
    b"\r\n"
)

# 2. The Chunks
chunk1 = b"Hello from the "
chunk2 = b"first chunk! "
chunk3 = b"And hello from the last chunk!"

def send_chunk(sock, data):
    size_hex = hex(len(data))[2:].encode('utf-8')
    sock.sendall(size_hex + b"\r\n" + data + b"\r\n")

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 8080)) # set up different host:port if necessary

    # Send headers
    s.sendall(headers)

    # Send chunks slowly to simulate a stream
    send_chunk(s, chunk1)
    send_chunk(s, chunk2)
    send_chunk(s, chunk3)

    # Send the final 0 chunk (EOF trigger)
    s.sendall(b"0\r\n\r\n")

    # Read the server's response
    response = s.recv(4096).decode('utf-8')
    print("=== SERVER RESPONSE ===")
    print(response)

except Exception as e:
    print(f"Error: {e}")
finally:
    s.close()