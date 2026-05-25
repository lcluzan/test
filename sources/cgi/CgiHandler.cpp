// #include "../../include/cgi/CgiHandler.hpp"
// #include "../../include/network/EventLoop.hpp"
// #include "../../sources/network/SocketHandler.cpp"
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <cstring>
// #include <cstdlib>
// #include <sstream>
#include <cgi/CgiHandler.hpp>

std::vector<std::string> CgiHandler::buildCgiEnv(const t_httpRequest& request, const std::string& script_path) {
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

CgiContext* CgiHandler::startCgi(const std::string& path, const t_httpRequest& request, int client_fd) {
    int pipe_in[2];
    int pipe_out[2];

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
        return NULL;

    pid_t pid = fork();

    if (pid == 0) {
        // --- ENFANT ---
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[1]); close(pipe_in[0]);
        close(pipe_out[0]); close(pipe_out[1]);

        std::string full_path = "./sources/www" + path;
        std::vector<std::string> env_strings = buildCgiEnv(request, full_path);

        char **envp = new char*[env_strings.size() + 1];
        for (size_t i = 0; i < env_strings.size(); ++i) {
            envp[i] = new char[env_strings[i].size() + 1];
            std::strcpy(envp[i], env_strings[i].c_str());
        }
        envp[env_strings.size()] = NULL;

        char *args[] = { (char*)"/usr/bin/php-cgi", (char*)full_path.c_str(), NULL };

        execve(args[0], args, envp);

        // Si execve échoue
        for (size_t i = 0; i < env_strings.size(); ++i) delete[] envp[i];
        delete[] envp;
        exit(1);
    }
    else if (pid > 0) {
        // --- PARENT ---
        close(pipe_in[0]);  // Le parent n'a pas besoin de lire l'entrée
        close(pipe_out[1]); // Le parent n'a pas besoin d'écrire la sortie

        // Rendre les descripteurs restants NON-BLOQUANTS
        setNonBlocking(pipe_in[1]);
        setNonBlocking(pipe_out[0]);

        // On remplit le contexte pour EventLoop
        CgiContext* ctx = new CgiContext();
        ctx->client_fd = client_fd;
        ctx->pipe_in = pipe_in[1];
        ctx->pipe_out = pipe_out[0];
        ctx->pid = pid;
        ctx->request_body = request.body; // Très utile pour les requêtes POST
        ctx->bytes_written = 0;

        return ctx;
    }
    return NULL; // En cas d'erreur du fork
}
