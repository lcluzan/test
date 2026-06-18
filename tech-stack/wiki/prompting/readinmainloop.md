This is a great question. The short answer is: **No, this behavior is not normal, and it strictly violates the project's non-blocking requirements.** If your server behaves this way during an evaluation, it will be considered blocking, which is a major project violation.

Here is a breakdown of why this is happening, why it violates the subject, and how you need to architect your server to fix it.

### **Why this violates the rules**

The core objective of using `poll()` (or `select`/`epoll`/`kqueue`) is to multiplex I/O. This means your single-threaded server should be able to juggle multiple clients concurrently without ever stopping to wait for one of them to finish.

In your current scenario, Client 1 is holding the server hostage. Because Client 1 hasn't finished its request (it hasn't sent the final `\r\n\r\n`), your server is refusing to process the events triggered by Client 2.

The subject explicitly states:

* "Your server must remain non-blocking at all times..."


* "It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O operations between the clients and the server..."



If Client 2 has to wait for Client 1 to finish typing, your server is acting synchronously (sequentially), not asynchronously.

### **The Likely Culprit (Why your code is doing this)**

This is a very common hurdle in the `webserv` project. It almost always stems from one of two architectural mistakes:

1. **The "While" Loop Trap:** When `poll()` tells you Client 1 is ready to be read, you might be entering a `while` loop that continuously calls `recv()` until the entire HTTP request is received. Because you are stuck inside this `while` loop, your code never returns to the main `poll()` loop to check on Client 2.
2.
**Missing `O_NONBLOCK`:** You might not have properly set the client socket to non-blocking using `fcntl()`. If a socket is blocking, calling `recv()` when there is no more data currently in the buffer will freeze the entire program until the client sends more data.



### **How to Fix It: The State Machine Approach**

To build a truly non-blocking server, you need to treat each `poll()` event as a single "tick" and maintain the state of each client in the background.

Here is the logic flow you should aim for:

1. **Accept the connection:** Set the new client socket to non-blocking using `fcntl()`. Add the new file descriptor to your `poll()` monitoring list.
2. **`poll()` detects data:** `poll()` tells you Client 1 has data ready to read.
3. **Read ONCE:** Call `recv()` or `read()` on Client 1 **only once** (or until it returns `EAGAIN`/`EWOULDBLOCK`).
4. **Buffer the data:** Append the received bytes to a buffer specific to that client (e.g., store it in a `std::map<int, std::string> client_buffers;` where the key is the `fd`).
5. **Check for completion:** Check if Client 1's buffer contains a complete HTTP request (e.g., it contains `\r\n\r\n` and the body matches the `Content-Length`).
* *If NO:* **Do nothing else for this client.** Immediately break out and go back to the main `poll()` loop!
* *If YES:* Process the request, generate the HTTP response, and flag this client as ready to *write*.


6. **Back to `poll()`:** Because you returned to the main `poll()` loop immediately, `poll()` will now see that Client 2 also has data ready to read, and you can repeat the process for Client 2.

By storing incomplete requests in a buffer and immediately returning to `poll()`, your server will happily bounce between Client 1 and Client 2, reading a few bytes from whoever is ready, without ever freezing.

---

How are you currently structuring your `recv()` calls when a socket is ready—are you using a loop to try and read the whole request at once, or are you reading chunk by chunk and storing it?



=======================================================================


The error message actually *is* coming from your code, and you have just hit the exact architectural issue I mentioned previously!

Here is exactly what is happening, why it causes that specific error, and how to fix it to comply with the subject.

### **Where the error is coming from**

Look at your `SocketHandler.cpp` file, specifically the `readFromSocket` function:

```cpp
ssize_t SocketHandler::readFromSocket(int fd, char* buffer, size_t size)
{
    ssize_t bytes_read = read(fd, buffer, size);

    if (bytes_read == -1)
        throw std::runtime_error("read() " + std::string(strerror(errno)));

    return bytes_read;
}

```

The string `"Resource temporarily unavailable"` is the standard system message for the `EAGAIN` or `EWOULDBLOCK` error codes.

When a socket is set to non-blocking, and you try to `read()` from it but the client hasn't sent any more data yet, `read()` does not wait (block). Instead, it immediately returns `-1` and sets `errno` to `EAGAIN`. Your code sees the `-1`, calls `strerror(EAGAIN)`, throws a `std::runtime_error`, which is caught in your `main()` function, prints the error, and forcefully exits the server.

### **Why your code triggers it (The `while (true)` trap)**

The root of the problem is in `webserver.cpp` inside your `readFullHttpRequest` function:

```cpp
std::string readFullHttpRequest(int fd, EventLoop& loop) {
    // ...
    while (true) {
        ssize_t bytes_read = loop.readFromClient(fd, buffer, sizeof(buffer));
        // ...

```

When you type `GET /` in telnet (which is 6 bytes), `poll()` correctly detects `POLLIN` and wakes up.

1. The first iteration of your `while (true)` loop reads those 6 bytes successfully.
2. Because the request doesn't have `\r\n\r\n` yet, the loop continues to the second iteration.
3. It calls `read()` again. But telnet hasn't sent anything else yet.
4. Because the socket is now non-blocking, `read()` returns `-1` (`EAGAIN`), throwing the exception and crashing your server.

**Critical Subject Rule Violation:**
The subject explicitly states: "You must never do a read or a write operation without going through poll() (or equivalent)."

By using a `while(true)` loop to call `read()` multiple times in a row, you are calling `read()` without `poll()` telling you it's safe to do so. This will result in a grade of 0 during evaluation.

### **How to fix this (The State Machine)**

You need to completely remove the `while (true)` loop from `readFullHttpRequest`. You must only read **once** every time `poll()` tells you to.

To achieve this, your server needs a "memory" for each client.

**Step 1: Update `ClientInfo.hpp` / `ClientInfo.cpp**`
Give your `ClientInfo` class a way to store partial requests.

```cpp
class ClientInfo {
  // ...
  public:
    void appendRequestData(const std::string& data) { _requestBuffer += data; }
    const std::string& getRequestBuffer() const { return _requestBuffer; }
    void clearRequestBuffer() { _requestBuffer.clear(); }
  private:
    std::string _requestBuffer;
};

```

**Step 2: Rewrite the reading logic in `webserver.cpp**`
When `poll()` triggers for a client socket, read *once*. Append the data to that client's buffer, check if the request is complete, and if not, just return to `poll()`.

```cpp
// Inside handler_conection:
ClientInfo* client = _client_manager.getClient(fd);
char buffer[BUFFER_SIZE];
ssize_t bytes_read = loop.readFromClient(fd, buffer, sizeof(buffer));

if (bytes_read > 0) {
    client->appendRequestData(std::string(buffer, bytes_read));

    // Check if the request is complete (has \r\n\r\n and full Content-Length body)
    if (isRequestComplete(client->getRequestBuffer())) {

        // Request is completely received! Now process it:
        t_httpRequest request = HttpHandler::setHttpRequest(client->getRequestBuffer());
        // ... generate response ...

        client->clearRequestBuffer(); // Ready for the next request
    }
    // If NOT complete, do nothing else! Let the loop finish and go back to poll()
}

```

**Step 3: Remove the exception on `read**`
Since you are now only reading when `poll()` says `POLLIN`, you technically shouldn't get an `EAGAIN` error. However, it's safer to catch it without crashing. In `SocketHandler::readFromSocket`, if `bytes_read == -1`, don't throw an exception that kills the server. Just return `-1`, let `readFromClient` see it, and handle it gracefully (e.g., disconnecting the client if it's a fatal error, but not crashing the whole program).

---

By implementing a buffer inside `ClientInfo`, you allow `poll()` to freely switch between telnet Client 1 typing slowly, and Firefox Client 2 sending a huge file all at once.

Are you familiar with how to parse the `\r\n\r\n` and the `Content-Length` simultaneously to determine if a request buffer is truly complete before passing it to `HttpHandler`?