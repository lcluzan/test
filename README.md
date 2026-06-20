*This project has been created as part of the 42 curriculum by bchallat, tjacquel and lcluzan.*

# Webserv

## Description
Webserv is a custom, non-blocking HTTP server from scratch in C++98. At its core, whenever a browser needs a file hosted on a web server, it requests the file vie HTTP. This software accepts the request via TCP sockets, finds or generates the requested document, and send it back to the client.

**Key Features:**
* Accepts connections from web browsers and CGI tools like `curl` or `telnet`.
* Fully understands and processes core HTTP methods (`GET`, `POST` and `DELETE`).
* Delivers accurate HTTP response status code (e.g., `200 OK`, `300 Multiple Choices`, `400 Bad Request`, `404 Not Found`, `500 Internal Server Error`).
* Serves static content including HTML, CSS and images (handling various media types).
* Manages complex behaviors via a custom `config.conf` file, supporting multiple virtual hosts (server delineated by IP or port).
* Implements CGI (Common Gateway Interface) to execute external scripts (like PHP or Python) by passing environments variables and managing `execve` forks.

---

## Instructions

* `make` : Compiles the `webserv` executable.
* `make clean` : Remove object files.
* `make fclean` : Removes object files and the executable.
* `make re` : Completely recompiles the project.

### Execution
Run the server with an explicit configuration file.
```bash
./webserv [path/to/config.conf]
```

### Testing commands

#### Testing the Server
Once the server is running, you can test it using various command-line tools:

- **Curl:**
	`curl -v http://localhost:8080/`
- **Telnet:**
	`telnet localhost 8080` (useful for manually typing HTTP request)

#### Stress Testing Commands
To ensure the server doesn't crash under high load, you can use Siege:

- **Run a basic stress test:**
	`siege -b -c50 -t10s http://127.0.0.1:8080/index.html`

#### Memory Leak Testing

- `valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./webserv`

## Ressources

### 📚 In-detph Documentation (Wiki)
We have compiled extensive documentation, research, and architectural diagrams during the development of this project.
👉 **[Click here to access the Project Wiki](./tech-stack/wiki)**

In the Wiki, you will find deep dives into:
* TCP Socket Programming (socket, bind, listen, accept, epoll)
* HTTP Protocol handling and parsing
* Common Gateway Interface (CGI) architecture
* Diagrams of our server's architecture
