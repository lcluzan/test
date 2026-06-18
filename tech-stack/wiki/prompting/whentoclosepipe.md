No, you should **not** close them right before `loop.sendResponse()`. In fact, looking at your `handler_conection` snippet, there is a larger architectural missing piece: **you aren't actually reading from or writing to the CGI pipes anywhere in this loop!**

Because you set your pipes to `O_NONBLOCK` in `CgiHandler.cpp`, your server is designed to handle CGI asynchronously. If you immediately call `response.toString()` and `loop.sendResponse()`, you are sending an empty or incomplete response to the client because the CGI process (PHP) hasn't had the time to run and write its output yet.

Here is how you need to handle the CGI file descriptors:

### 1. Don't send the response immediately for CGI

When `response.is_cgi` is true, the response isn't ready yet. Instead of sending it right away, you need to hand those file descriptors over to your `EventLoop` (likely `epoll`, `poll`, or `select`) so the server can write the request body to the CGI and read the CGI's output in the background.

```cpp
      t_httpResponse response = HttpHandler::setHttpResponse(request, config);

      if (response.is_cgi) {
          // --- ASYNC CGI HANDLING ---
          // 1. Do NOT call sendResponse yet.
          // 2. Register response.cgi_read_fd and response.cgi_write_fd into your EventLoop.
          // 3. Keep a mapping in your server that ties these CGI FDs back to the client fd.

          loop.addCgiFds(response.cgi_read_fd, response.cgi_write_fd, fd);
          // (You will need to implement this logic in your EventLoop)
      } else {
          // --- NORMAL STATIC FILE ROUTE ---
          std::string raw_response = response.toString();
          loop.sendResponse(fd, raw_response);

          if (response.status == 400) {
            loop.removeClient(fd);
          }
      }

```

### 2. When do you actually close the FDs?

You close the file descriptors inside your event loop **only after the CGI finishes its I/O**:

1. **Write FD (`cgi_write_fd`)**: Once you have successfully written the entire `request.body` into `cgi_write_fd` (which might take multiple `write()` calls since it's non-blocking), you must call `close(cgi_write_fd)`. This sends an EOF to the PHP script so it knows it can stop waiting for input and start processing.
2. **Read FD (`cgi_read_fd`)**: You read from this FD whenever your event loop says it's ready. Once `read()` returns `0` (EOF), it means the PHP script has finished sending its HTML output. **This is the exact moment you call `close(cgi_read_fd)**`.
3. **Send to Client**: Now that you have the full output from the CGI, you wrap it in the final HTTP response headers, send it to the client via `loop.sendResponse(client_fd, final_response)`, and optionally `waitpid()` on `response.cgi_pid` to prevent zombie processes.

### Summary

If you close the FDs before or right after `sendResponse` in your current code, you will close the pipes before PHP even gets a chance to read the POST data or write the HTML, causing the PHP execution to crash and leak memory/descriptors. You must read/write all the data first, and only close the FDs when the data stream is completely finished.