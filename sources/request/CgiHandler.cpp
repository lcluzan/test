//#include "../../include/cgi/CgiHandler.hpp"
#include <request/HttpHandler.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <sstream>

std::vector<std::string> HttpHandler::buildCgiEnv(const t_httpRequest& request, const std::string& script_path) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.method);
    env.push_back("SERVER_PROTOCOL=" + request.version);
    env.push_back("PATH_INFO=" + script_path);
    env.push_back("SCRIPT_FILENAME=" + script_path);
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_SOFTWARE=Webserv/1.0");

    if (request.method == "POST") {
        std::stringstream ss;
        ss << request.body.size();
        env.push_back("CONTENT_LENGTH=" + ss.str());

        std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
        if (it != request.headers.end()) {
            env.push_back("CONTENT_TYPE=" + it->second);
        }
    }

    return env;
}


t_httpResponse HttpHandler::executeCgi(const std::string& path, const t_httpRequest& request) {
    t_httpResponse response;
    int pipe_in[2];  // Parent écrit dans pipe_in[1], enfant lit depuis pipe_in[0]
    int pipe_out[2]; // Parent lit depuis pipe_out[0], enfant écrit dans pipe_out[1]

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
        // --- ENFANT (CGI) ---
        close(pipe_in[1]); // Ferme l'extrémité d'écriture du parent
        close(pipe_out[0]); // Ferme l'extrémité de lecture du parent

        dup2(pipe_in[0], STDIN_FILENO);   // Le CGI lit depuis STDIN (lié à pipe_in[0])
        dup2(pipe_out[1], STDOUT_FILENO); // Le CGI écrit dans STDOUT (lié à pipe_out[1])

        close(pipe_in[0]);
        close(pipe_out[1]);

        std::string full_path = "./sources/www" + path;
        std::vector<std::string> env_strings = buildCgiEnv(request, full_path);

        // Alloue envp
        char **envp = new char*[env_strings.size() + 1];
        for (size_t i = 0; i < env_strings.size(); ++i) {
            envp[i] = new char[env_strings[i].size() + 1];
            std::strcpy(envp[i], env_strings[i].c_str());
        }
        envp[env_strings.size()] = NULL;

        // Alloue args
        char *script_path = strdup(full_path.c_str());
        char *args[] = { (char*)"/usr/bin/php-cgi", script_path, NULL };

        execve(args[0], args, envp);

        // Si execve échoue
        for (size_t i = 0; i < env_strings.size(); ++i) delete[] envp[i];
        delete[] envp;
        free(script_path);
        exit(1);
    } else {
        // --- PARENT (Webserv) ---
        close(pipe_in[0]); // Ferme l'extrémité de lecture de l'enfant
        close(pipe_out[1]); // Ferme l'extrémité d'écriture de l'enfant

        // Envoie le corps de la requête POST au CGI
        if (!request.body.empty()) {
            if (write(pipe_in[1], request.body.c_str(), request.body.size()) == -1) {
                close(pipe_in[1]);
                close(pipe_out[0]);
                waitpid(pid, NULL, 0);
                response.status = 500;
                response.body = "<html><body>500 Internal Server Error: write failed</body></html>";
                return response;
            }
        }
        close(pipe_in[1]); // Plus besoin d'écrire

        // Lis la sortie du CGI
        char buffer[4096];
        std::string cgi_output;
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            cgi_output += buffer;
        }
        close(pipe_out[0]);

        waitpid(pid, NULL, 0); // Attend la fin du CGI

        // Traite la réponse
        size_t header_end = cgi_output.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            response.status = 200;
            response.body = cgi_output.substr(header_end + 4);
        } else {
            // Si pas de headers, on considère que tout est le corps
            response.status = 200;
            response.body = cgi_output;
        }
        return response;
    }
}

