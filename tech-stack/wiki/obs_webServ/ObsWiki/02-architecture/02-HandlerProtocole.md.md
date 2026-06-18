> Projet 42 — Webserv | Standard C++98 | Non-blocking I/O | `poll()` unique

---

## Table des Matières

1. [Vue d'ensemble du flux HTTP](#1-vue-densemble-du-flux-http)
2. [Classes principales](#2-classes-principales)
3. [Modèles d'architecture](#3-modèles-darchitecture)
4. [Attributs & Méthodes détaillés](#4-attributs--méthodes-détaillés)
5. [Gestion des dépendances](#5-gestion-des-dépendances)
6. [Gestion des erreurs HTTP](#6-gestion-des-erreurs-http)
7. [Pièges à éviter](#7-pièges-à-éviter)
8. [Stratégie de tests](#8-stratégie-de-tests)

---

## 1. Vue d'ensemble du flux HTTP

```
[Réseau] ──► poll() ──► Server
                            │
                    ┌───────▼────────┐
                    │    Client      │  ← state machine par connexion
                    │  (fd, state)   │
                    └───────┬────────┘
                            │
              ┌─────────────▼──────────────┐
              │     HttpRequestParser      │  ← parsing incrémental
              └─────────────┬──────────────┘
                            │ HttpRequest
              ┌─────────────▼──────────────┐
              │          Router            │  ← résolution LocationConfig
              └─────────────┬──────────────┘
                            │
             ┌──────────────┼──────────────┐
             ▼              ▼              ▼
    StaticFileHandler   CGIHandler    RedirectHandler
             │              │              │
             └──────────────┼──────────────┘
                            │ HttpResponse
                    ┌───────▼────────┐
                    │  Client (send) │
                    └───────┬────────┘
                            │
                        [Réseau]
```

**Principe cardinal** : la boucle `poll()` est le **seul** point de blocage. Tout le reste est non-bloquant et piloté par des états.

---

## 2. Classes principales

| Classe | Rôle | Méthodes clés |
|--------|------|---------------|
| `Server` | Boucle événementielle, gestion des sockets d'écoute, dispatch des événements | `init()`, `run()`, `_acceptClient()`, `_dispatch()`, `_checkTimeouts()` |
| `Client` | Machine d'états par connexion — buffers, parsing, réponse | `readData()`, `writeData()`, `getState()`, `reset()` |
| `Config` | Parse le fichier de configuration | `parse(path)`, `getServers()` |
| `ServerConfig` | Bloc `server {}` — port, host, error pages | `matchLocation(uri)` |
| `LocationConfig` | Bloc `location {}` — root, méthodes, CGI, upload | *(struct de données)* |
| `HttpRequestParser` | Parse les octets bruts en `HttpRequest` (machine d'états) | `feed(data)`, `isComplete()`, `hasError()`, `getRequest()`, `reset()` |
| `HttpRequest` | Requête HTTP parsée (POD enrichi) | `getHeader(name)`, `getContentLength()` |
| `HttpResponse` | Réponse HTTP sérialisable | `setStatus()`, `setHeader()`, `setBody()`, `serialize()` |
| `Router` | Résout la `LocationConfig` et instancie le bon handler | `resolve(request, location)` |
| `IRequestHandler` | Interface Strategy pour les handlers | `handle(request, response)`, `isAsync()` |
| `StaticFileHandler` | Sert des fichiers statiques, gère GET/DELETE/upload | `handle()` |
| `CGIHandler` | Fork/exec du script CGI, gestion async des pipes | `startProcess()`, `readOutput()`, `finalize()`, `cleanup()` |
| `RedirectHandler` | Retourne une réponse de redirection (301/302) | `handle()` |
| `MimeTypes` | Map extension → Content-Type | `getType(ext)` |

---

## 3. Modèles d'architecture

### 3.1 Reactor Pattern (boucle événementielle)

```
┌──────────────── Server::run() ────────────────┐
│                                               │
│   while (_running) {                          │
│     poll(_pollFds, _nfds, TIMEOUT_MS)         │
│                                               │
│     for each fd with event:                   │
│       if fd == listenFd  → acceptClient()     │
│       if POLLIN          → handleRead(fd)     │
│       if POLLOUT         → handleWrite(fd)    │
│       if POLLERR/POLLHUP → removeClient(fd)   │
│   }                                           │
└───────────────────────────────────────────────┘
```

**Avantages** : un seul `poll()` pour tout (sujets obligatoires), adapté au C++98, scalable sur un seul thread.  
**Inconvénients** : complexité de la gestion des fds CGI dans le même `poll()`.

---

### 3.2 State Machine Pattern (Client)

Chaque connexion est une machine d'états. Cela évite tout couplage entre le parsing et l'envoi.

```
READING_REQUEST
      │ (parser.isComplete())
      ▼
  PROCESSING ──────────────────┐
      │ (handler sync)          │ (handler async = CGI)
      │                         ▼
      │                   CGI_WRITING (stdin)
      │                         │
      │                   CGI_READING (stdout)
      │                         │
      ▼◄────────────────────────┘
WRITING_RESPONSE
      │ (tout envoyé)
      ▼
    DONE / KEEP_ALIVE
```

**Avantages** : traçabilité parfaite, pas de callback hell, reprise simple dans la boucle poll.  
**Inconvénients** : prolifération des états si mal encapsulé — utiliser un `enum ClientState` explicite.

---

### 3.3 Strategy Pattern (Handlers)

```cpp
// Interface C++98 (pas de concept ni template)
class IRequestHandler {
public:
    virtual ~IRequestHandler() {}
    virtual void handle(const HttpRequest& req, HttpResponse& res) = 0;
    virtual bool isAsync() const { return false; }
};

// Implémentations concrètes
class StaticFileHandler : public IRequestHandler { ... };
class CGIHandler        : public IRequestHandler { ... };
class RedirectHandler   : public IRequestHandler { ... };
```

**Avantages** : ajout d'un nouveau handler sans toucher au `Router` ni au `Client`, testable indépendamment.  
**Inconvénients** : nécessite une gestion manuelle du cycle de vie du handler (pas de `std::unique_ptr` en C++98) → le `Client` est propriétaire du handler et le détruit dans son destructeur.

---

### 3.4 Recommandation

| Couche | Patron recommandé | Justification |
|--------|-------------------|---------------|
| Boucle I/O | **Reactor** | Impose le `poll()` unique du sujet |
| Connexion | **State Machine** | Gestion claire des étapes parsing → traitement → envoi |
| Handlers | **Strategy** | Open/Closed : extensible sans modification |
| Config | **Value Object** | `ServerConfig`/`LocationConfig` comme structs immuables après parsing |
| CGI async | **Async Subprocess** (pipes + poll) | Seule façon de rester non-bloquant avec fork/exec |

---

## 4. Attributs & Méthodes détaillés

### 4.1 `Server`

```cpp
class Server {
public:
    Server();
    ~Server();

    void loadConfig(const std::string& configPath);
    void init();   // bind/listen sur tous les ServerConfig
    void run();    // boucle poll() principale
    void stop();

private:
    void _acceptClient(int listenFd, const ServerConfig& conf);
    void _handleRead(int fd);
    void _handleWrite(int fd);
    void _handleCGIRead(int fd);
    void _handleCGIWrite(int fd);
    void _removeClient(int fd);
    void _checkTimeouts();
    void _rebuildPollFds();  // reconstruit le tableau après ajout/suppression

    Config                   _config;
    std::map<int, ServerConfig*> _listenFdToConf;  // fd écoute → config
    std::map<int, Client*>   _clients;             // fd client → Client
    std::map<int, int>       _cgiFdToClientFd;     // fd CGI → fd client
    std::vector<struct pollfd> _pollFds;
    bool                     _running;

    static const int TIMEOUT_MS     = 5000;
    static const int CLIENT_TIMEOUT = 30;  // secondes
};
```

---

### 4.2 `Client`

```cpp
enum ClientState {
    STATE_READING,       // lecture de la requête
    STATE_PROCESSING,    // traitement sync (StaticFile, Redirect)
    STATE_CGI_WRITING,   // écriture vers stdin CGI
    STATE_CGI_READING,   // lecture de stdout CGI
    STATE_WRITING,       // envoi de la réponse
    STATE_DONE           // connexion à fermer
};

class Client {
public:
    Client(int fd, const ServerConfig& conf);
    ~Client();  // libère _handler si alloué

    void         readData();   // appelé sur POLLIN
    void         writeData();  // appelé sur POLLOUT
    ClientState  getState() const;
    int          getFd() const;
    time_t       getLastActivity() const;

    // Pour les CGI : fds à surveiller dans poll()
    int          getCGIWriteFd() const;  // stdin du CGI (POLLOUT)
    int          getCGIReadFd()  const;  // stdout du CGI (POLLIN)
    void         onCGIWriteReady();
    void         onCGIReadReady();

private:
    void _processRequest();   // instancie le bon handler via Router
    void _buildErrorResponse(int code);

    int              _fd;
    ClientState      _state;
    std::string      _readBuf;
    std::string      _writeBuf;
    size_t           _writeOffset;

    HttpRequestParser _parser;
    HttpRequest*      _request;   // NULL jusqu'à parsing complet
    HttpResponse*     _response;  // NULL jusqu'à traitement
    IRequestHandler*  _handler;   // NULL si sync, CGIHandler* si async

    const ServerConfig& _serverConf;
    time_t              _lastActivity;
};
```

---

### 4.3 `HttpRequestParser`

```cpp
enum ParseState {
    PS_REQUEST_LINE,
    PS_HEADERS,
    PS_BODY,
    PS_CHUNKED_SIZE,
    PS_CHUNKED_DATA,
    PS_COMPLETE,
    PS_ERROR
};

class HttpRequestParser {
public:
    HttpRequestParser();
    ~HttpRequestParser();

    // Alimentation incrémentale (données partielles bienvenues)
    void feed(const char* data, size_t len);

    bool         isComplete() const;
    bool         hasError()   const;
    int          getErrorCode() const;  // 400, 413, 505…
    HttpRequest* releaseRequest();      // transfert de propriété
    void         reset();

private:
    bool _parseRequestLine(const std::string& line);
    bool _parseHeaderLine(const std::string& line);
    bool _validateHeaders();
    bool _processBody();
    bool _processChunkedLine(const std::string& line);
    bool _isValidMethod(const std::string& m) const;
    bool _isValidUri(const std::string& uri) const;

    ParseState   _state;
    std::string  _buf;
    HttpRequest* _req;
    size_t       _bodyExpected;
    size_t       _chunkSizeExpected;
    int          _errorCode;

    static const size_t MAX_HEADER_SIZE = 8192;
    static const size_t MAX_REQUEST_LINE = 4096;
};
```

---

### 4.4 `HttpRequest`

```cpp
struct HttpRequest {
    std::string method;
    std::string uri;
    std::string path;        // URI sans query string
    std::string queryString;
    std::string httpVersion; // "HTTP/1.0" ou "HTTP/1.1"
    std::map<std::string, std::string> headers;  // lowercase keys
    std::string body;
    bool        isChunked;
    bool        keepAlive;

    // Helpers
    std::string getHeader(const std::string& key) const;
    size_t      getContentLength() const;
    bool        hasHeader(const std::string& key) const;
};
```

> **Note** : stocker les clés d'en-tête en minuscules dès le parsing — HTTP est case-insensitive (RFC 7230).

---

### 4.5 `HttpResponse`

```cpp
class HttpResponse {
public:
    HttpResponse();

    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body, const std::string& contentType = "text/html");
    void appendBody(const std::string& chunk);  // pour CGI streaming

    std::string serialize() const;  // génère les octets à envoyer

    // Utilitaires statiques
    static std::string defaultErrorPage(int code);
    static std::string reasonPhrase(int code);

private:
    int         _statusCode;
    std::string _reason;
    std::map<std::string, std::string> _headers;
    std::string _body;

    static const std::string HTTP_VERSION;  // "HTTP/1.1"
};
```

---

### 4.6 `Router`

```cpp
class Router {
public:
    explicit Router(const ServerConfig& conf);

    // Retourne un handler alloué sur le tas — le Client en est propriétaire
    IRequestHandler* resolve(const HttpRequest& req,
                             const LocationConfig*& outLocation) const;

private:
    const LocationConfig* _matchLocation(const std::string& uri) const;
    bool _methodAllowed(const std::string& method,
                        const LocationConfig& loc) const;
    bool _isCGI(const std::string& path,
                const LocationConfig& loc,
                std::string& outBinary) const;

    const ServerConfig& _conf;
};
```

> La méthode `resolve()` est le seul endroit qui décide quel handler créer. En ajoutant un type de handler, seul `resolve()` change.

---

### 4.7 `CGIHandler`

```cpp
class CGIHandler : public IRequestHandler {
public:
    CGIHandler(const LocationConfig& loc,
               const std::string& scriptPath,
               const std::string& cgiBinary);
    virtual ~CGIHandler();

    // Interface IRequestHandler (synchrone — non utilisée directement)
    virtual void handle(const HttpRequest& req, HttpResponse& res);
    virtual bool isAsync() const { return true; }

    // Interface async — appelée par Client sur événements poll()
    void startProcess(const HttpRequest& req);

    int  getInputFd()  const;  // POLLOUT → écrire stdin
    int  getOutputFd() const;  // POLLIN  → lire stdout

    void writeInput();         // envoie le body en chunks
    void readOutput();         // accumule la sortie CGI
    bool isInputDone()  const;
    bool isOutputDone() const;

    void finalize(HttpResponse& res);  // parse headers CGI + construit réponse
    void cleanup();                    // ferme les pipes, waitpid

private:
    void _buildEnv(const HttpRequest& req, std::vector<std::string>& env) const;
    bool _isTimedOut() const;
    void _parseCGIOutput(HttpResponse& res);

    const LocationConfig& _loc;
    std::string           _scriptPath;
    std::string           _cgiBinary;

    pid_t       _pid;
    int         _pipeIn[2];   // [0] lu par CGI, [1] écrit par server
    int         _pipeOut[2];  // [0] lu par server, [1] écrit par CGI

    std::string _inputBuf;    // body à envoyer au CGI
    size_t      _inputOffset;
    std::string _outputBuf;   // sortie accumulée du CGI

    bool        _inputDone;
    bool        _outputDone;
    time_t      _startTime;

    static const int CGI_TIMEOUT_SEC = 10;
};
```

---

### 4.8 Config (structs de données)

```cpp
struct LocationConfig {
    std::string              path;               // "/api"
    std::string              root;               // "/var/www"
    std::string              index;              // "index.html"
    std::vector<std::string> allowedMethods;     // ["GET", "POST"]
    bool                     autoindex;
    std::string              uploadPath;         // "" si désactivé
    std::map<std::string, std::string> cgiExtensions; // ".php" → "/usr/bin/php-cgi"
    std::string              redirect;           // "" si aucune
    int                      redirectCode;       // 301 ou 302
    size_t                   clientMaxBodySize;  // bytes, 0 = hérité du serveur
};

struct ServerConfig {
    std::string              host;
    int                      port;
    std::string              serverName;
    std::map<int, std::string> errorPages;       // 404 → "/errors/404.html"
    size_t                   clientMaxBodySize;
    std::vector<LocationConfig> locations;

    const LocationConfig* matchLocation(const std::string& uri) const;
    // Longest prefix match — ne nécessite pas de regex
};

class Config {
public:
    bool parse(const std::string& filePath);
    const std::vector<ServerConfig>& getServers() const;
    bool hasError() const;
    const std::string& getError() const;

private:
    std::vector<ServerConfig> _servers;
    std::string               _error;
};
```

---

## 5. Gestion des dépendances

### 5.1 Hiérarchie de propriété (ownership)

```
Server
  └── Client*  (map fd → Client, Server détruit)
        ├── HttpRequestParser  (membre valeur)
        ├── HttpRequest*       (Client alloue, libère dans destructor)
        ├── HttpResponse*      (Client alloue, libère dans destructor)
        └── IRequestHandler*   (Router alloue, Client libère dans destructor)
              └── CGIHandler   (possède les pipes et le PID)
```

> **Règle d'or C++98** : l'allocateur est le destructeur. Si `A` crée `B*`, `A` le détruit.

### 5.2 Éviter le couplage fort

```
INTERDIT :                           CONSEILLÉ :
Client → CGIHandler (méthodes)       Client → IRequestHandler (interface)
Router → StaticFileHandler           Router → IRequestHandler* (retourné)
CGIHandler → HttpResponse (direct)   CGIHandler::finalize(HttpResponse&)
```

Le `Router` est le seul à connaître les classes concrètes de handlers — c'est le seul couplage fort intentionnel.

### 5.3 Gestion des fds CGI dans `poll()`

```
_pollFds = [
  { listenFd1, POLLIN },   // sockets d'écoute
  { listenFd2, POLLIN },
  { clientFd1, POLLIN | POLLOUT },  // connexion client
  { cgiInFd,   POLLOUT },           // stdin du CGI (écriture)
  { cgiOutFd,  POLLIN  },           // stdout du CGI (lecture)
  ...
]
```

La map `_cgiFdToClientFd` permet au `Server` de retrouver le `Client` associé depuis un événement sur un fd CGI.

---

## 6. Gestion des erreurs HTTP

### 6.1 Codes d'erreur par couche

| Couche | Codes | Déclencheur |
|--------|-------|-------------|
| Parser | 400 Bad Request | Ligne de requête malformée |
| Parser | 413 Content Too Large | Body > `clientMaxBodySize` |
| Parser | 505 HTTP Version Not Supported | Version non reconnue |
| Router | 405 Method Not Allowed | Méthode absente de `allowedMethods` |
| Router | 404 Not Found | Aucune `LocationConfig` correspondante |
| StaticFile | 403 Forbidden | Permissions fichier |
| StaticFile | 404 Not Found | Fichier inexistant |
| CGI | 500 Internal Server Error | fork/exec échoue |
| CGI | 504 Gateway Timeout | CGI dépasse `CGI_TIMEOUT_SEC` |
| Server | 500 Internal Server Error | Erreur inattendue |

### 6.2 Architecture de la gestion d'erreur

```cpp
// Dans Client::_buildErrorResponse()
void Client::_buildErrorResponse(int code) {
    _response = new HttpResponse();
    _response->setStatus(code);

    // 1. Chercher une page d'erreur personnalisée dans la config
    std::map<int, std::string>::const_iterator it =
        _serverConf.errorPages.find(code);

    if (it != _serverConf.errorPages.end()) {
        std::string fullPath = /* root */ + it->second;
        // Lire le fichier (disk file → pas besoin de poll())
        std::string content = readFileContent(fullPath);
        if (!content.empty()) {
            _response->setBody(content, "text/html");
            _state = STATE_WRITING;
            return;
        }
    }

    // 2. Page d'erreur par défaut générée en mémoire
    _response->setBody(HttpResponse::defaultErrorPage(code), "text/html");
    _state = STATE_WRITING;
}
```

### 6.3 Timeouts

```
Timeout client   : LAST_ACTIVITY + 30s  → fermer connexion (408 Request Timeout)
Timeout CGI      : CGI_START + 10s      → kill(pid, SIGKILL), répondre 504
Timeout requête  : lecture partielle > 10s → fermer connexion
```

---

## 7. Pièges à éviter

### 7.1 I/O bloquante

```cpp
// ❌ INTERDIT — grade 0 immédiat
ssize_t n = read(clientFd, buf, sizeof(buf));

// ✅ Correct — toujours passer par poll() d'abord
// Dans Server::run() : si POLLIN sur clientFd → appeler client->readData()
void Client::readData() {
    char buf[4096];
    ssize_t n = recv(_fd, buf, sizeof(buf), 0);
    if (n <= 0) { _state = STATE_DONE; return; }
    _parser.feed(buf, n);
    _lastActivity = time(NULL);
    if (_parser.isComplete()) _processRequest();
    else if (_parser.hasError()) _buildErrorResponse(_parser.getErrorCode());
}
```

### 7.2 `errno` après read/write

```cpp
// ❌ INTERDIT par le sujet
ssize_t n = read(fd, buf, size);
if (errno == EAGAIN) { /* ... */ }

// ✅ Se fier uniquement à la valeur de retour
ssize_t n = recv(fd, buf, sizeof(buf), 0);
if (n < 0)  { /* erreur → fermer */ }
if (n == 0) { /* connexion fermée par le client */ }
```

### 7.3 Processus CGI zombies

```cpp
// ❌ oublier waitpid → accumulation de zombies
void CGIHandler::cleanup() {
    if (_pipeIn[1]  != -1) { close(_pipeIn[1]);  _pipeIn[1]  = -1; }
    if (_pipeOut[0] != -1) { close(_pipeOut[0]); _pipeOut[0] = -1; }
    if (_pid > 0) {
        int status;
        waitpid(_pid, &status, WNOHANG);  // non-bloquant
        // Si pas encore terminé après timeout, SIGKILL puis waitpid(WNOHANG)
        _pid = -1;
    }
}
```

### 7.4 Fuites mémoire (pas de RAII C++11)

```cpp
// Pattern RAII C++98 manuel dans Client::~Client()
Client::~Client() {
    delete _request;  _request  = NULL;
    delete _response; _response = NULL;
    delete _handler;  _handler  = NULL;  // appelle ~CGIHandler → cleanup()
    if (_fd != -1) { close(_fd); _fd = -1; }
}
```

### 7.5 Reconstruction du tableau `pollfd`

```cpp
// ❌ Modifier _pollFds en cours d'itération → comportement indéfini
// ✅ Collecter les fds à supprimer, rebuild après la boucle
std::vector<int> toRemove;
for (/* événements poll */) {
    if (/* client terminé */) toRemove.push_back(fd);
}
for (size_t i = 0; i < toRemove.size(); ++i)
    _removeClient(toRemove[i]);
_rebuildPollFds();  // reconstruction du vecteur proprement
```

### 7.6 Longest prefix match pour les locations

```cpp
// ❌ Premier match → peut matcher "/api" avant "/api/upload"
// ✅ Parcourir toutes les locations, garder le plus long préfixe qui correspond
const LocationConfig* ServerConfig::matchLocation(const std::string& uri) const {
    const LocationConfig* best = NULL;
    size_t bestLen = 0;
    for (size_t i = 0; i < locations.size(); ++i) {
        const std::string& locPath = locations[i].path;
        if (uri.substr(0, locPath.size()) == locPath && locPath.size() > bestLen) {
            best    = &locations[i];
            bestLen = locPath.size();
        }
    }
    return best;
}
```

### 7.7 Chunked transfer encoding

```
// Client → Server : le parser doit décoder
POST /upload HTTP/1.1
Transfer-Encoding: chunked

5\r\n
Hello\r\n
0\r\n
\r\n

// Après décodage dans HttpRequestParser : body = "Hello"
// Le CGI reçoit le body décodé + EOF final (fermeture de pipe)
```

---

## 8. Stratégie de tests

### 8.1 Tests unitaires — sans framework externe

```cpp
// Fichier test_parser.cpp (compilé séparément)
// Utiliser assert() ou un mini-framework maison (TestSuite + ASSERT_EQ)

void testCompleteGET() {
    HttpRequestParser p;
    const char* raw = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    p.feed(raw, strlen(raw));
    assert(p.isComplete());
    assert(!p.hasError());
    HttpRequest* req = p.releaseRequest();
    assert(req->method  == "GET");
    assert(req->path    == "/index.html");
    assert(req->getHeader("host") == "localhost");
    delete req;
    std::cout << "[PASS] testCompleteGET\n";
}

void testChunkedBody() { /* ... */ }
void testOversizedBody() { /* ... */ }
void testMalformedRequestLine() { /* ... */ }
void testIncrementalFeed() {  // simule données fragmentées
    HttpRequestParser p;
    const char* part1 = "GET /index.htm";
    const char* part2 = "l HTTP/1.1\r\nHost: loc";
    const char* part3 = "alhost\r\n\r\n";
    p.feed(part1, strlen(part1)); assert(!p.isComplete());
    p.feed(part2, strlen(part2)); assert(!p.isComplete());
    p.feed(part3, strlen(part3)); assert(p.isComplete());
    /* ... */
}
```

### 8.2 Tests d'intégration — `curl` / `telnet`

```bash
# Test basique GET
curl -v http://localhost:8080/

# Test upload POST
curl -v -X POST -F "file=@/tmp/test.txt" http://localhost:8080/upload

# Test DELETE
curl -v -X DELETE http://localhost:8080/files/test.txt

# Test CGI Python
curl -v http://localhost:8080/cgi-bin/hello.py?name=42

# Test corps trop grand (413)
curl -v -X POST --data-binary @/tmp/bigfile.bin http://localhost:8080/upload

# Test méthode non autorisée (405)
curl -v -X PUT http://localhost:8080/

# Test requête malformée (400) via telnet
echo -e "GET ??? BAD\r\n\r\n" | nc localhost 8080

# Test timeout CGI (504)
curl -v http://localhost:8080/cgi-bin/slow.py

# Test HTTP/1.0 vs HTTP/1.1 Keep-Alive
curl -v --http1.0 http://localhost:8080/
```

### 8.3 Comparaison avec NGINX

```bash
# Lancer NGINX sur le port 8081, webserv sur 8080
# Comparer les headers de réponse :
diff <(curl -sI http://localhost:8080/404) \
     <(curl -sI http://localhost:8081/404)
```

### 8.4 Stress testing

```bash
# Avec siege (non-bloquant obligatoire !)
siege -c 100 -t 30S http://localhost:8080/

# Avec wrk
wrk -t4 -c100 -d30s http://localhost:8080/

# Avec ab (Apache Bench)
ab -n 10000 -c 100 http://localhost:8080/
```

### 8.5 Script de test Python (recommandé par le sujet)

```python
#!/usr/bin/env python3
# test_webserv.py

import socket
import time

def send_raw(host, port, data, timeout=2):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(timeout)
    s.connect((host, port))
    s.sendall(data.encode())
    try:
        resp = b""
        while True:
            chunk = s.recv(4096)
            if not chunk: break
            resp += chunk
    except socket.timeout:
        pass
    s.close()
    return resp.decode(errors='replace')

def test_404():
    resp = send_raw("localhost", 8080, "GET /nonexistent HTTP/1.1\r\nHost: localhost\r\n\r\n")
    assert "404" in resp, f"Expected 404, got: {resp[:100]}"
    print("[PASS] test_404")

def test_slow_client():
    """Client qui envoie la requête byte par byte"""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", 8080))
    req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    for c in req:
        s.send(c.encode())
        time.sleep(0.01)
    resp = s.recv(4096).decode()
    assert "200" in resp
    s.close()
    print("[PASS] test_slow_client")

if __name__ == "__main__":
    test_404()
    test_slow_client()
    # ajouter tous les cas...
```

### 8.6 Checklist de validation

- [ ] Le serveur tient 30 secondes sous `siege -c 100`
- [ ] Pas de fuite mémoire (`valgrind --leak-check=full`)
- [ ] Pas de fd ouvert après fermeture client (`lsof -p <pid>`)
- [ ] CGI Python et PHP fonctionnent
- [ ] Upload de fichier > 1 MB → 413 si configuré
- [ ] Directory listing activé/désactivé selon config
- [ ] Redirection 301 fonctionne
- [ ] Méthodes GET, POST, DELETE fonctionnent
- [ ] Pages d'erreur personnalisées servies
- [ ] Requête incomplète → timeout propre (pas de hang)
- [ ] Comportement identique à NGINX sur les codes de statut
- [ ] Compilation sans warning avec `-Wall -Wextra -Werror -std=c++98`

---

*Document généré pour le projet Webserv 42 — v24.0 — C++98*
