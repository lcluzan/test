You are asking excellent architectural questions. Let's clear up these confusions, because understanding the *difference* between these concepts is exactly what you will be tested on during your defense.

Here is the breakdown of why you need these variables, the difference between them, and a full mapping of the subject requirements to your classes.

---

### 1. `error_page` vs `return_route` (Redirection)

They might seem similar because they both deal with HTTP codes, but they do completely different jobs in a web server!

* **`error_page` (Internal Error Handling):** If a user asks for `/doesnotexist.html`, your server internally generates a `404` error. Instead of sending a blank 404 page, the `error_page` directive tells your server: *"Ah, for 404s, quietly open the file `/errors/404.html` and send its content to the user instead."* The URL in the user's browser **does not change**.
* **`return_route` (HTTP Redirection):** This fulfills the subject requirement: *"HTTP redirection."* If you move your website to a new domain, you use a `301` or `302` redirect. If the user asks for `/old-site`, your server replies: *"I don't have that file, but here is a 301 code and a new URL: `/new-site`."* The user's browser receives this, **changes the URL in the search bar**, and makes a brand new request to `/new-site`.

### 2. `upload_enable` & `upload_loc`

The subject explicitly states: *"Uploading files from the clients to the server is authorized, and storage location is provided."*

If a client sends an HTTP `POST` request containing a file (like an image), your server needs to know:

1. Am I allowed to save files here? (`upload_enable`)
2. Where on the hard drive do I save it? (`upload_loc`)

You *must* have variables for these to pass the evaluation.

### 3. `host` (IP) vs `server_name`

These are definitely **not** duplicates. They handle two completely different layers of networking.

* **`host` (or `ip`):** This is for the **Transport Layer (TCP)**. When you call `bind()` in C++, you bind the socket to a specific IP address (like `127.0.0.1` or `0.0.0.0`). It tells the operating system: *"Listen for traffic on this physical network interface."*
* **`server_name`:** This is for the **Application Layer (HTTP)**. Imagine you have one server listening on `127.0.0.1:80`. The user configures two server blocks: one for `server_name cat.com;` and one for `server_name dog.com;`. When a request comes in to `127.0.0.1:80`, your C++ code will look at the HTTP `Host:` header. If the header says `Host: cat.com`, you route the request to the first config. If it says `dog.com`, you route it to the second.

### 4. What about `cgi_pass`? Do you need it?

The subject requires: *"Execution of CGI, based on file extension (for example .php)"*.

Some students "hardcode" the CGI path in their C++ code (e.g., `if (extension == ".php") execve("/usr/bin/php-cgi")`). **This is considered bad practice.** The point of a configuration file is to avoid hardcoding!

To do it the "NGINX way", you create a location block for the extension, and use a directive to specify the executable:

```nginx
location .php {
    cgi_pass /usr/bin/php-cgi;
}

```

If you don't add a `cgi_pass` (or similar) variable to your `LocationConfig`, your server won't know *where* the PHP or Python executable lives on the computer evaluating your project!

---

### The Ultimate Subject-to-Class Mapping

Here is exactly how the 42 subject requirements map to the variables you have (or need to have) in your classes.

#### **ServerConfig Class (Global Rules)**

| Subject Requirement | Your Variable | Notes |
| --- | --- | --- |
| <br>*Define all the interface:port pairs*

 | `ip` / `port` | Handled via the `listen` directive. |
| <br>*Set up default error pages.*

 | `std::map<int, std::string> error_page;` | You have this in `LocationConfig`, but NGINX also allows it globally in `ServerConfig`. |
| <br>*Set the maximum allowed size...*

 | `client_max_body_size` | You correctly implemented this with K/M/G. |
| <br>*server name for a website*

 | `server_name` | Matches the `Host` HTTP header. |

#### **LocationConfig Class (Route Rules)**

| Subject Requirement | Your Variable | Notes |
| --- | --- | --- |
| <br>*List of accepted HTTP methods*

 | `std::set<std::string> methods;` | (GET, POST, DELETE). |
| <br>*HTTP redirection.*

 | `std::map<int, std::string> return_route;` | For 301/302 redirects. |
| <br>*Directory where the requested file...*

 | `std::string root;` | How `/kapouet` maps to `/tmp/www`.

 |
| <br>*Enabling or disabling directory listing.*

 | `bool autoindex;` | Shows a folder UI if no index file exists. |
| <br>*Default file to serve*

 | `std::string index;` | Usually `index.html`. |
| <br>*Uploading files... is authorized*

 | `bool upload_enable;` |  |
| <br>*...and storage location is provided.*

 | `std::string upload_loc;` |  |
| <br>*Execution of CGI, based on file extension*

 | **`std::string cgi_pass;`** | **(You need to add this!)** Points to the CGI executable. |

If your classes contain these variables, and your semantic analyzer populates them correctly without crashing, **you have 100% completed the configuration parsing requirements for this project.**