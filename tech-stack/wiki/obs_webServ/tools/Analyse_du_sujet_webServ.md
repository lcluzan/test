>[!important] _cette analyse est faite a apartire de la verssion 23.1:_ [**en.sujetWebServ**](en.subject.webserveur.pdf)

#### 1. Resumer du projet

**Objectif global** : Écrire un serveur HTTP en C++ 98, compatible avec les navigateurs standards, capable de gérer des requêtes GET, POST, DELETE, et de servir des pages statiques ou dynamiques (via CGI).

>[!info] **Mots-clés**
>
> HTTP, CGI, configuration file, non-blocking, poll()/select()/epoll(), sockets, RFC, NGINX, stress test, virtual host, error pages.
---
#### 2. Résumé et points clés

Résumé des attentes:

| Point                      | Détails                                                                                                                                   |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| **Objectif global**        | Implémenter un serveur HTTP en C++ 98, capable de gérer plusieurs ports, des requêtes HTTP, et des CGIs.                                  |
| **Livrables**              | Code source (Makefile, *.cpp, *.hpp), fichier de configuration, pages d’erreur par défaut, tests de stress.                               |
| **Contraintes techniques** | Non-blocking I/O, utilisation de poll()/select()/epoll(), gestion des erreurs, compatibilité avec les navigateurs, respect des RFCs HTTP. |
| **Critères d’évaluation**  | Robustesse (pas de crash), gestion des erreurs, performance, respect des normes C++ 98, compatibilité avec NGINX.                         |
| **Bonus**                  | Cookies, gestion de session, support de plusieurs types de CGI.                                                                           |

---
#### 3. Notions à maîtriser

Concepts/Technologies

| Priorité                       | Concept/Technologie                                                                                       | Ressources                                                                                                                           |
| ------------------------------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| **À connaître absolument**     | Sockets, I/O non-bloquant, poll()/select()/epoll(), protocole HTTP, gestion des processus (fork pour CGI) | [RFC HTTP/1.1](https://tools.ietf.org/html/rfc2616), [Tutoriel sockets en C++](https://www.geeksforgeeks.org/socket-programming-cc/) |
| **Important**                  | Parsing de requêtes HTTP, gestion des erreurs, configuration file, CGI (environnement, communication)     | [Documentation NGINX](https://nginx.org/en/docs/), [CGI Spec](https://tools.ietf.org/html/rfc3875)                                   |
| **Utile pour aller plus loin** | Stress testing, virtual hosts, cookies, gestion de session                                                | [Load Testing Tools](https://loadimpact.com/), [MDN HTTP Cookies](https://developer.mozilla.org/en-US/docs/Web/HTTP/Cookies)         |

---
#### 4. Exemples concrets

##### Exemple 1 : Création d’un socket non-bloquant en C++

~~~ cpp
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h> 

int create_non_blocking_socket() 
{     
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
	    return -1;
    fcntl(sockfd, F_SETFL, O_NONBLOCK); // Mode non-bloquant
    return sockfd;
}
~~~

>[!info] **Ressource** : [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

##### Exemple 2 : Parsing d’une requête HTTP

~~~ cpp
std::string parse_http_request(const std::string& request) 
{
    size_t method_end = request.find(' ');
    if (method_end == std::string::npos)
	    return "ERROR";
	std::string method = request.substr(0, method_end);
	// ... (extraire path, version HTTP, headers)
	return method; 
}
~~~

>[!info] **Ressource** : [Mozilla HTTP Messages](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages)

---
#### 5. Anticipation des erreurs

Erreurs courantes et solutions

| Erreur                                                             | Solution/Bonne pratique                                                            |
| ------------------------------------------------------------------ | ---------------------------------------------------------------------------------- |
| Mauvaise gestion des ports (ex : écoute sur un port déjà utilisé)  | Vérifier la disponibilité du port avec `bind()` et gérer les erreurs.              |
| Fuites mémoire (ex : oubli de `close()` sur les sockets)           | Utiliser RAII (smart pointers en C++11+, mais ici C++98 : wrappers personnalisés). |
| Blocage du serveur (I/O bloquant)                                  | Toujours utiliser `poll()`/`select()` et vérifier les flags de readiness.          |
| Mauvaise gestion des requêtes HTTP (ex : parsing incorrect)        | Tester avec `telnet` et comparer avec NGINX.                                       |
| Erreurs de configuration (ex : chemin incorrect pour les fichiers) | Valider le fichier de config au démarrage et fournir des messages d’erreur clairs. |

>[!important] _ lien pour retourner a indexObs:_ [**index**](indexObs_WebServe.md)
