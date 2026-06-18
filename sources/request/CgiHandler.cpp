/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 15:32:15 by lcluzan           #+#    #+#             */
/*   Updated: 2026/06/17 21:52:24 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>
#include <webserver.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <sstream>

std::vector<std::string> HttpHandler::buildCgiEnv(const t_httpRequest& request, const std::string& filename, const std::string& script_name, const std::string& query_string, const ServerConfig& config) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.method);
    env.push_back("SERVER_PROTOCOL=" + request.version);
    env.push_back("PATH_INFO=" + script_name);
    env.push_back("SCRIPT_NAME=" + script_name);
    env.push_back("SCRIPT_FILENAME=" + filename);
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_SOFTWARE=Webserv/1.0");
    env.push_back("QUERY_STRING=" + query_string); // Add QUERY_STRING for GET request

   // 1. Set safe defaults based on the Server configuration
    std::string server_name = "127.0.0.1";

   std::stringstream ss_port;
    ss_port << config.getPort(); // Convert the config port (int) to string
    std::string server_port = ss_port.str();

    // 2. Try to extract the exact requested host/port from the HTTP Headers
    std::map<std::string, std::string>::const_iterator host_it = request.headers.find("Host");
    if (host_it == request.headers.end()) {
        host_it = request.headers.find("host"); // Fallback for lowercase
    }

    if (host_it != request.headers.end()) {
        std::string host_val = host_it->second;
        size_t colon_pos = host_val.find(':');

        if (colon_pos != std::string::npos) {
            // Found a port in the Host header (e.g., "localhost:8080")
            server_name = host_val.substr(0, colon_pos);
            server_port = host_val.substr(colon_pos + 1);
        } else {
            // No port in Host header (e.g., "localhost"), keep the config port
            server_name = host_val;
        }
    }
    env.push_back("SERVER_NAME=" + server_name);
    env.push_back("SERVER_PORT=" + server_port);

    env.push_back("REMOTE_ADDR=127.0.0.1"); // Ideally, get this from ClientInfo later!

    if (request.method == "POST") {
        std::stringstream ss;
        ss << request.body.size();
        env.push_back("CONTENT_LENGTH=" + ss.str());

        std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
        if (it != request.headers.end()) {
            env.push_back("CONTENT_TYPE=" + it->second);
        }
    }
    //Give all the headers to Cgi
    for (std::map<std::string, std::string>::const_iterator it = request.headers.begin(); it !=request.headers.end(); ++it) {
        std::string header_name = "HTTP_" + it->first;
        for (size_t i = 0; i < header_name.length(); ++i) {
            if (header_name[i] == '-') header_name[i] = '_';
            header_name[i] = std::toupper(header_name[i]);
        }
        env.push_back(header_name + "=" + it->second);
    }
    return env;
}

std::vector<char*> HttpHandler::stringsToCharPtrs(const std::vector<std::string>& strings) {
    std::vector<char*> ptrs;
    for (size_t i = 0; i < strings.size(); ++i) {
        ptrs.push_back(const_cast<char*>(strings[i].c_str()));
    }
    ptrs.push_back(NULL);
    return ptrs;
}

void HttpHandler::handleCgiChild(const std::string& path, const t_httpRequest& request, int pipe_in[2], int pipe_out[2], const ServerConfig& config, const std::string& interpreter, const LocationConfig& location) {
    close(pipe_in[1]); // Ferme l'extrémité d'écriture du parent
    close(pipe_out[0]); // Ferme l'extrémité de lecture du parent
    dup2(pipe_in[0], STDIN_FILENO);   // Le CGI lit depuis STDIN (lié à pipe_in[0])
    dup2(pipe_out[1], STDOUT_FILENO); // Le CGI écrit dans STDOUT (lié à pipe_out[1])
    close(pipe_in[0]);
    close(pipe_out[1]);

    // Parse the path and query string
    std::string actual_path = path;
    std::string query_string = "";
    size_t question_mark_pos = path.find('?');

    if(question_mark_pos != std::string::npos){
        actual_path = path.substr(0, question_mark_pos);
        query_string = path.substr(question_mark_pos + 1);
    }

    std::string root = location.getRoot();
    std::string full_path = root + actual_path;

    // Isolate the directory and filename
    size_t  last_slash = full_path.find_last_of('/');
    std::string dir_path = (last_slash != std::string::npos) ? full_path.substr(0, last_slash) : ".";
    std::string filename = (last_slash != std::string::npos) ? full_path.substr(last_slash + 1) : full_path;

    // 1. Change directory context
    if (!dir_path.empty()) {
        if (chdir(dir_path.c_str()) == -1) {
            std::cout << "Status: 500 Internal Server Error\r\n\r\n";
            std::cout << "<h1>CGI Error: chdir failed</h1>\n";
            std::cout << "Tried to go to: " << dir_path << "<br>\n";
            std::cout << "Error: " << strerror(errno) << "\n";
            exit(1);
        }
    }

    // 2. Build Environment using JUST the filename
    std::vector<std::string> env_strings = buildCgiEnv(request, filename, actual_path, query_string, config);
    std::vector<char*> envp = stringsToCharPtrs(env_strings);

    // 3. Build Execution Args using the config interpreter and just the filename
    std::vector<std::string> arg_strings;
    arg_strings.push_back(interpreter);
    arg_strings.push_back(filename);
    std::vector<char*> args = stringsToCharPtrs(arg_strings);

    // Execute
    if (!arg_strings.empty()) {
        execve(arg_strings[0].c_str(), &args[0], &envp[0]);
    }
    // If execve fails, exit safely
    std::cout << "Status: 500 Internal Server Error\r\n\r\n"; // print de debug a voir ce quon en fait
    std::cout << "<h1>CGI Error: execve failed</h1>\n";
    std::cout << "Interpreter: " << interpreter << "<br>\n";
    std::cout << "Script: " << filename << "<br>\n";
    std::cout << "Error code: " << strerror(errno) << "<br>\n";
    exit(1);
}


t_httpResponse HttpHandler::executeCgi(const std::string& path, const t_httpRequest& request, const ServerConfig& config, const std::string& interpreter, const LocationConfig& location){
    t_httpResponse response;
    int pipe_in[2];
    int pipe_out[2];

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        response.status = 500;
        response.body = "<html><body>500 Internal Server Error: pipe failed</body></html>";
        return response;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        response.status = 500;
        response.body = "<html><body>500 Internal Server Error: fork failed</body></html>";
        return response;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        handleCgiChild(path, request, pipe_in, pipe_out, config, interpreter, location);
    } else {
        // --- PARENT PROCESS ---
        close(pipe_in[0]);
        close(pipe_out[1]);

        // Make pipes non-blocking
        fcntl(pipe_in[1], F_SETFL, O_NONBLOCK);
        fcntl(pipe_out[0], F_SETFL, O_NONBLOCK);

        response.status = 0; // Pending CGI
        response.is_cgi = true;
        response.cgi_read_fd = pipe_out[0];
        response.cgi_write_fd = pipe_in[1];
        response.cgi_pid = pid;
        response.body = request.body; // To be written to the CGI later
    }

    return response;
}
