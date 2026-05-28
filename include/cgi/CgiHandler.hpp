#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <vector>
#include "../request/utils_HandlerHttp.hpp"

class CgiHandler {
    public:
        // La méthode principale que HttpHandler va appeler
        static t_httpResponse executeCgi(const std::string& path, const t_httpRequest& request);

    private:
        // Méthode utilitaire cachée pour l'utilisateur de la classe
        static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& script_path);
        
        // On rend les constructeurs privés si on ne veut utiliser que des méthodes statiques
        CgiHandler();
        ~CgiHandler();
};

#endif