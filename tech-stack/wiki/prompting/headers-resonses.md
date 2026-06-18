It is completely understandable to want to strip this down to an MVP. Reading through RFCs 9110 and 9112 can feel like drowning in "MAYs" and "MUSTs" that don't seem strictly necessary for a basic, functioning server. You have the right mindset: you shouldn't try to rebuild NGINX, you just need to do what is required by the subject and what makes a standard browser like Firefox happy.

Based on the project requirements and how modern browsers behave, your MVP is actually very close. Here is the reality check on what you truly need versus what is just "nice to have."

### 1. The Absolute MVP Headers (Required for Browsers & Subject)

You already have the two most important ones, but there is a third you are missing based on the subject's configuration requirements.

* **`Content-Length`:** (You have this). Crucial. Without this, the browser won't know when the response is finished unless you violently close the socket.
* **`Content-Type`:** (You have this). Essential for the browser. Without it, Firefox will likely default to treating everything as `application/octet-stream` and will prompt the user to download your HTML files instead of rendering them.


*
**`Location`:** **You need this.** The subject specifically demands that you implement "HTTP redirection" in your configuration file. Whenever your server returns a 3xx status code (like 301 Moved Permanently or 302 Found), the browser absolutely requires the `Location` header to know where to redirect the user.



### 2. The "Keep Your Server Alive" Header

* **`Connection` (`keep-alive` or `close`):** While strictly optional in some older RFCs, HTTP/1.1 defaults to persistent connections. Because the subject dictates your server must remain non-blocking at all times and handle client disconnections cleanly , explicitly telling the browser `Connection: close` (if you want to drop them) or `Connection: keep-alive` (if you are keeping the socket open in your `poll()` loop ) will save you from massive headaches with hanging requests and zombie sockets.



### 3. Addressing Your Specific RFC Findings

* **`Allow`:** You nailed it. If you throw a `405 Method Not Allowed`, you really should return `Allow: GET, POST` (or whatever is permitted on that route). Evaluators testing your code via `curl` or Postman will look for this.
* **`Date`:** While Firefox won't crash if you omit it, you are correct that RFC 9110 states it's a MUST for servers with a clock. Because the subject explicitly allows peers to use NGINX to compare header behaviors, strict evaluators *will* check for the `Date` header. In C++98, it's just a quick combination of `gettimeofday()` and `strftime()`. For the 10 lines of code it takes, it's worth implementing to avoid losing points.



### 4. The CGI Caveat

Remember that your server isn't the only thing generating headers. When you execute a CGI script (like PHP or Python), the script itself usually generates headers (at minimum, a `Content-Type` and sometimes a `Status`).

Your `HttpHandler` will need to read the raw output from the CGI pipe, parse out the headers the CGI generated (looking for the `\r\n\r\n` boundary), and properly merge them with your server's base headers before sending the final string back to the client.

---

To summarize, for your MVP, your header checklist should be:

1. `Content-Length`
2. `Content-Type`
3. `Date`
4. `Connection`
5. `Location` (only for 3xx redirects)
6. `Allow` (only for 405 errors)

Have you already mapped out a strategy for how your `executeCgi` function will parse the output from the CGI script to separate its headers from the actual body?