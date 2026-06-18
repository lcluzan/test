## 🗂️ Structure du répertoire `wiki/` 

~~~ bash
wiki/
├── 00-index.md        # Page d'accueil du wiki (liens vers toutes les sections)
├── 01-concepts/                 # Notions théoriques fondamentales
│   ├── 01-protocole-http.md     # Explication du protocole HTTP/1.1
│   ├── 02-sockets.md            # Gestion des sockets en C++
│   ├── 03-common-gateway-interface.md # Rôle du CGI et variables d'environnement
│   └── 04-multithreading.md # Gestion des clients multiples (select/poll/epoll)
│
├── 02-architecture/             # Choix architecturaux
│   ├── 01-global-design.md      # Schéma d'ensemble du serveur
│   ├── 02-routing.md            # Mécanisme de routage des requêtes
│   ├── 03-gestion-erreurs.md    # Stratégie de gestion des erreurs HTTP
│   └── 04-performances.md       # Optimisations (ex : pool de threads, cache)
│
├── 03-implementation/           # Détails techniques et code
│   ├── 01-parsing-http.md       # Parsing des requêtes HTTP
│   ├── 02-static-files.md       # Servir des fichiers statiques
│   ├── 03-cgi-execution.md      # Exécution de scripts CGI
│   ├── 04-response-building.md  # Construction des réponses HTTP
│   └── 05-exemples-code/        # Dossier pour les extraits de code commentés
│       ├── client_socket.hpp    # Exemple de classe pour gérer un client
│       └── http_parser.cpp      # Implémentation du parseur
│
├── 04-references/               # Ressources externes
│   ├── 01-rfc-http.md           # Liens vers les RFCs HTTP/1.1
│   ├── 02-cpp-docs.md           # Documentation C++ utilisée
│   └── 03-outils.md              # Outils utilisés (Valgrind, curl, etc.)
│
└── 05-changelog.md              # Historique des modifications du wiki
~~~

# 
---
#  📝 Template pour chaque fichier Markdown
## [Titre]

## 📌 Introduction
- Objectif
- Public cible
- Prérequis

## 🧠 Concepts clés
- Définitions
- Exemples

## ⚙️ Choix techniques
- Alternatives envisagées
- Justification du choix retenu
- Avantages/Inconvénients

## 💻 Exemples de code

```cpp
// Code commenté
```

#  
---