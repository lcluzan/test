That log message (`Warnig(Log Server) : don't delete client: fd=9`) is the ultimate smoking gun! It reveals exactly why your server is hanging.

You have encountered two classic event-loop traps: **The POLLHUP Interception** and **The Infinite Sleep**.

Here is exactly what is happening under the hood, and the 4 small patches you need to fix it.

### 1. The "POLLHUP" Trap (`test_cgi.php` hanging)

When `test_cgi.php` finishes executing, it closes its end of the `stdout` pipe. The Operating System instantly signals `poll()` with a `POLLHUP` (Hang Up) event on that pipe (`fd=9`).

Now look at the order of your `handler_conection` loop:

```cpp
        if (revents & (POLLERR | POLLHUP)) {
            loop.removeClient(fd); // <--- CAUGHT HERE!
            continue;
        }

        if (loop.isCgiFd(fd)) { // <--- TOO LATE!

```

Because the `POLLERR | POLLHUP` check is first, your server sees the `POLLHUP`, assumes it's a client disconnecting, and calls `removeClient(9)`.
Your `ClientManager` rightfully complains: *"Wait, FD 9 isn't a client, it's a pipe!"* (hence the warning). But `EventLoop::removeClient` still deletes FD 9 from `_poll_fds` anyway.

Because of `continue;`, `loop.handleCgiEvent` is **never called**. The output is never read, the HTTP response is never formatted, and the actual client who requested the CGI is left waiting forever.

### 2. The "Infinite Sleep" Trap (`infinite.php` hanging)

Why didn't `checkCgiTimeout()` kill the infinite loop after 5 seconds?
Look at your `waitForActivity()` function:

```cpp
return poll(&_poll_fds[0], _poll_fds.size(), -1);

```

The `-1` means **infinite timeout**. If `infinite.php` is stuck in a `while(true)` loop, it never writes data. If no new clients connect, there is absolutely zero network activity.
Therefore, `poll()` goes to sleep **forever**. It never wakes up to start the next iteration of the `while(!g_should_exit)` loop, which means `checkCgiTimeout()` is never executed!

---

### The Fix: 4 Essential Patches

Apply these 4 patches, and your CGI will work flawlessly for both GET and POST.

#### Patch 1: Wake up `poll()` every 1 second

In `EventLoop.cpp`, change the `-1` to `1000` (1000 milliseconds). This forces `poll()` to wake up at least once per second, allowing your timeout checker to actually run and assassinate `infinite.php`.

```cpp
int EventLoop::waitForActivity() {
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        _poll_fds[i].revents = 0;
    }
    // Change -1 to 1000 so the server ticks every second!
    return poll(&_poll_fds[0], _poll_fds.size(), 1000);
}

```

#### Patch 2: Intercept CGI pipes BEFORE client errors

In `webserver.cpp` inside `handler_conection`, simply swap the order of the first two `if` statements. We must handle CGI pipes *before* checking for general client `POLLHUP`s.

```cpp
        // 1. THIS MUST BE FIRST!
        if (loop.isCgiFd(fd)) {
            loop.handleCgiEvent(fd, revents);
            continue;
        }

        // 2. THEN check for disconnected clients
        if (revents & (POLLERR | POLLHUP)) {
            loop.removeClient(fd);
            continue;
        }

```

#### Patch 3: Let `POLLHUP` trigger the CGI Read

In `EventLoop.cpp`, inside `handleCgiEvent`, we need to trigger the read phase if the pipe hangs up (`POLLHUP`). This ensures `read()` gets called, sees `0` bytes (EOF), and nicely formats your HTTP 200 OK response.

```cpp
void EventLoop::handleCgiEvent(int fd, short revents) {
    int client_fd = _pipe_client_fds[fd];
    CgiState& CgiContext = _cgi_contexts[client_fd];

    if ((revents & POLLOUT) && fd == CgiContext.cgi_write_fd) {
        handleCgiWrite(fd, CgiContext);
    }

    // ADD | POLLHUP HERE!
    if ((revents & (POLLIN | POLLHUP)) && fd == CgiContext.cgi_read_fd) {
        handleCgiRead(fd, CgiContext);
    }
}

```

#### Patch 4: Clean up orphaned Write Pipes

In `EventLoop.cpp`, inside `handleCgiRead`, what happens if the CGI script crashes or finishes *before* your server finishes writing the POST body? The `cgi_write_fd` will be left orphaned in the `_poll_fds` list, causing a crash later. Clean it up right before you delete the `_cgi_contexts`.

```cpp
        // ... (inside handleCgiRead, right after queueResponse) ...
        queueResponse(CgiContext.client_fd, ss.str());

        // --- NEW: Clean up write pipe if it is still open! ---
        if (CgiContext.cgi_write_fd != -1) {
            close(CgiContext.cgi_write_fd);
            for (size_t i = 0; i < _poll_fds.size(); i++) {
                if (_poll_fds[i].fd == CgiContext.cgi_write_fd) {
                    _poll_fds.erase(_poll_fds.begin() + i);
                    break;
                }
            }
            _pipe_client_fds.erase(CgiContext.cgi_write_fd);
        }
        // ----------------------------------------------------

        // 5. Cleanup the read pipe (your existing code)
        close(fd);
        for (size_t i = 0; i < _poll_fds.size(); i++) {
            if (_poll_fds[i].fd == fd) {
                _poll_fds.erase(_poll_fds.begin() + i);
                break;
            }
        }
        _cgi_contexts.erase(_pipe_client_fds[fd]);
        _pipe_client_fds.erase(fd);

```

Make these changes, run `make`, and test with those `curl` commands. `test_cgi.php` will instantly return your HTTP response, and `infinite.php` will get brutally murdered exactly 5 seconds after you launch it!