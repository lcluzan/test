/*
#include <sys/stat.h>   // Pour stat()
#include <unistd.h>     // Pour access()
#include <cerrno>       // Pour errno
#include <cstring>      // Pour strerror

t_httpResponse HttpHandler::serveStaticFile(const std::string& path) {
    // 🔹 1. Afficher le chemin reçu
    std::cout << COLOR_YELLOW << "[DEBUG] Attempting to open file: '" << path << "'" << COLOR_RESET << std::endl;

    // 🔹 2. Vérifier si le chemin est absolu ou relatif
    if (path[0] == '/') {
        std::cout << "[DEBUG] Path is absolute." << std::endl;
    } else {
        std::cout << "[DEBUG] Path is relative." << std::endl;
    }

    // 🔹 3. Vérifier si le fichier existe avec access()
    if (access(path.c_str(), F_OK) == -1) {
        std::cout << COLOR_RED << "[DEBUG] File does NOT exist: " << strerror(errno) << COLOR_RESET << std::endl;
        return HandlerErrorHttp(404);
    } else {
        std::cout << "[DEBUG] File exists." << std::endl;
    }

    // 🔹 4. Vérifier les permissions en lecture
    if (access(path.c_str(), R_OK) == -1) {
        std::cout << COLOR_RED << "[DEBUG] No read permission: " << strerror(errno) << COLOR_RESET << std::endl;
        return HandlerErrorHttp(403);
    } else {
        std::cout << "[DEBUG] Read permission OK." << std::endl;
    }

    // 🔹 5. Vérifier si c'est un fichier régulier (pas un dossier)
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) == -1) {
        std::cout << COLOR_RED << "[DEBUG] stat() failed: " << strerror(errno) << COLOR_RESET << std::endl;
        return HandlerErrorHttp(404);
    }
    if (!S_ISREG(path_stat.st_mode)) {
        std::cout << COLOR_RED << "[DEBUG] Path is NOT a regular file (maybe a directory)." << COLOR_RESET << std::endl;
        return HandlerErrorHttp(403);
    } else {
        std::cout << "[DEBUG] Path is a regular file." << std::endl;
    }

    // 🔹 6. Afficher le répertoire de travail actuel
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "[DEBUG] Current working directory: " << cwd << std::endl;
    } else {
        std::cout << COLOR_RED << "[DEBUG] getcwd() failed: " << strerror(errno) << COLOR_RESET << std::endl;
    }

    // 🔹 7. Essayer d'ouvrir le fichier
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cout << COLOR_RED << "[DEBUG] Failed to open file with ifstream: " << strerror(errno) << COLOR_RESET << std::endl;
        return HandlerErrorHttp(404);
    } else {
        std::cout << "[DEBUG] File opened successfully." << std::endl;
    }

    // 🔹 8. Lire le contenu
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::cout << "[DEBUG] File size: " << content.size() << " bytes." << std::endl;

    // 🔹 9. Préparer la réponse
    t_httpResponse response;
    response.status = 200;
    response.headers["Content-Type"] = getMimeType(path);
    response.body = content;

    return response;
}

