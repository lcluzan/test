>[!important] 
>- Une architecture orientée objet en C++98 pour gérer les sockets doit reposer sur des classes bien définies avec des responsabilités claires et des mécanismes robustes de gestion des ressources.
>- Le modèle de composition est recommandé pour sa flexibilité, sa maintenabilité et sa capacité à réduire le couplage fort entre classes.
>- L’utilisation d’interfaces abstraites, de l’injection de dépendances et du patron RAII est essentielle pour garantir la robustesse et la gestion propre des ressources.
>- Les tests unitaires, intégrés dès la conception, sont indispensables pour assurer la qualité et la maintenabilité du code, notamment via des frameworks comme Boost.Test.
>- La gestion des erreurs et la libération explicite des ressources (sockets) doivent être systématiques pour éviter les fuites et les blocages.

---
# 📌 Introduction

Le projet **webServ**, un serveur HTTP, nécessite une architecture orientée objet en C++98 capable de gérer efficacement les sockets TCP, tout en assurant la robustesse, la maintenabilité et la performance. La gestion des sockets implique la création, la configuration, la surveillance et la libération des ressources réseau, ce qui soulève des défis techniques importants en C++98, notamment la gestion des erreurs, la prévention des fuites de ressources et la gestion des événements réseau.

Cette analyse détaillée propose une architecture structurée autour de classes principales bien définies, évalue les modèles d’architecture possibles (héritage, composition, événementiel), recommande des stratégies pour minimiser le couplage fort, et insiste sur les bonnes pratiques de gestion des ressources et de tests unitaires. Elle s’appuie sur les principes de conception orientée objet, les design patterns reconnus, et les meilleures pratiques de programmation réseau en C++98.

#
---
# 🧠 Définition des classes principales et leurs responsabilités

## Classe abstraite `ANetworkTcp`

Cette classe abstraite constitue la base de l’architecture, définissant l’interface commune pour les opérations fondamentales sur les sockets TCP. Elle encapsule les fonctionnalités essentielles telles que l’ouverture, la fermeture, la liaison, l’écoute et l’acceptation des connexions.

- **Méthodes publiques abstraites** :  
    `virtual void openSocket() = 0` : ouvre une socket.  
    `virtual void closeSocket() = 0` : ferme une socket.  
    `virtual void bindSocket() = 0` : attache une adresse à la socket.  
    `virtual void listenSocket() = 0` : met la socket en mode écoute.  
    `virtual void acceptConnection() = 0` : accepte une connexion entrante.
    
- **Attributs protégés** :  
    `int socketDescriptor` : descripteur de socket.  
    `struct sockaddr_in socketAddress` : structure d’adresse socket.
    

Cette classe abstraite garantit que toutes les classes dérivées implémentent ces méthodes, assurant une interface cohérente et facilitant la gestion des sockets dans un cadre orienté objet.

## Classe `ServerSocket`

Cette classe concrète hérite de `ANetworkTcp` et gère spécifiquement les sockets côté serveur. Elle est responsable de la création du serveur, de la gestion des connexions clients entrantes, et de la communication avec ces clients.

- **Méthodes publiques** :  
    `void startServer(int port)` : démarre le serveur sur un port donné.  
    `void handleClientConnection(int clientSocket)` : traite la connexion d’un client.  
    `void sendData(int clientSocket, const char* data)` : envoie des données à un client.  
    `void receiveData(int clientSocket, char* buffer, int bufferSize)` : reçoit des données d’un client.
    
- **Attributs privés** :  
    `int serverSocket` : descripteur de la socket serveur.  
    `std::vector<int> clientSockets` : vecteur des descripteurs de sockets clients connectés.
    

Cette classe centralise la logique serveur, gère les ressources associées aux clients connectés, et assure la communication bidirectionnelle.

## Classe `ClientSocket`

Cette classe concrète hérite également de `ANetworkTcp` et gère les sockets côté client. Elle est responsable de la connexion au serveur, de l’envoi et de la réception des données.

- **Méthodes publiques** :  
    `void connectToServer(const char* serverAddress, int port)` : établit la connexion au serveur.  
    `void sendData(const char* data)` : envoie des données au serveur.  
    `void receiveData(char* buffer, int bufferSize)` : reçoit des données du serveur.
    
- **Attributs privés** :  
    `int clientSocket` : descripteur de la socket client.  
    `struct sockaddr_in serverAddress` : adresse du serveur.
    

Cette classe encapsule la logique client, gère la socket client et assure la communication avec le serveur.

## Classe `EventLoop`

Cette classe gère la surveillance des événements sur les sockets, notamment pour détecter les connexions entrantes, les déconnexions, ou les erreurs réseau.

- **Méthodes publiques** :  
    `void addSocket(int socket)` : ajoute une socket à la boucle d’événements.  
    `void removeSocket(int socket)` : supprime une socket de la boucle d’événements.  
    `void run()` : exécute la boucle d’événements principale.
    
- **Attributs privés** :  
    `std::vector<int> sockets` : vecteur des sockets surveillées.  
    `bool running` : état d’exécution de la boucle.
    

Cette classe est essentielle pour un serveur HTTP performant, car elle permet de gérer de manière asynchrone les événements réseau, évitant ainsi les appels bloquants et améliorant la scalabilité.

---

# 
---
# 🧮 Tableau synthétique des classes principales

|Classe|Rôle|Méthodes principales|Attributs principaux|
|---|---|---|---|
|`ANetworkTcp`|Interface abstraite base pour sockets TCP|`openSocket()`, `closeSocket()`, `bindSocket()`, `listenSocket()`, `acceptConnection()`|`socketDescriptor`, `socketAddress`|
|`ServerSocket`|Gestion des sockets côté serveur|`startServer()`, `handleClientConnection()`, `sendData()`, `receiveData()`|`serverSocket`, `clientSockets`|
|`ClientSocket`|Gestion des sockets côté client|`connectToServer()`, `sendData()`, `receiveData()`|`clientSocket`, `serverAddress`|
|`EventLoop`|Surveillance des événements sur sockets|`addSocket()`, `removeSocket()`, `run()`|`sockets`, `running`|
#
---

# 📊 Analyse comparative des modèles d’architecture

### Héritage

- **Avantages** :
    
    - Facilité de réutilisation du code via des classes dérivées.
    - Hiérarchie claire facilitant la compréhension.
    - Gestion centralisée des erreurs dans la classe mère.
- **Inconvénients** :
    
    - Couplage fort entre classes mère et filles.
    - Rigidité : modifications dans la classe mère impactent toutes les classes filles.
    - Risque de problèmes de performance liés à la complexité croissante.

### Composition

- **Avantages** :
    
    - Réduction du couplage fort, chaque classe est responsable d’une seule tâche.
    - Flexibilité accrue pour évolutions futures.
    - Facilité de maintenance et d’extensibilité.
- **Inconvénients** :
    
    - Nécessite plus de code pour déléguer les tâches entre objets.
    - Complexité accrue pour les débutants.

### Modèle événementiel (basé sur callbacks)

- **Avantages** :
    
    - Gestion efficace des événements réseau, réduction du couplage.
    - Facilité de gestion des erreurs via des callbacks dédiés.
    - Adapté aux architectures asynchrones.
- **Inconvénients** :
    
    - Complexité d’implémentation plus élevée.
    - Nécessite des mécanismes supplémentaires pour gérer les événements.
#
---

# ⚙️ Tableau comparatif des modèles d’architecture

|Modèle|Avantages|Inconvénients|
|---|---|---|
|Héritage|Réutilisation facile, clarté, gestion erreurs|Couplage fort, rigidité, risques de performance|
|Composition|Faible couplage, flexibilité, maintenabilité|Plus de code, complexité accrue|
|Événementiel|Gestion efficace des événements, faible couplage|Complexité d’implémentation, mécanismes supplémentaires|
#
---

# 📋 Recommandation finale

Le modèle de **composition** est recommandé pour le projet webServ car il offre la meilleure flexibilité, facilite la maintenance et réduit le couplage fort entre classes. Il permet d’évoluer facilement et de gérer les dépendances via des interfaces et l’injection de dépendances. La gestion des événements réseau sera encapsulée dans la classe `EventLoop`, qui surveille les sockets et gère les connexions entrantes de manière asynchrone, améliorant ainsi la performance et la scalabilité.
#
---

# ⛓️ Gestion des dépendances et des ressources

### Réduction du couplage fort

- **Interfaces abstraites** : définir des interfaces pour les classes de gestion des sockets permet de découpler les implémentations concrètes et de réduire les dépendances directes. Cela facilite la maintenance et l’évolution du code.
    
- **Injection de dépendances** : ce patron de conception permet d’injecter les dépendances nécessaires à une classe plutôt que de les créer directement, réduisant ainsi le couplage fort et améliorant la modularité.
    
- **RAII (Resource Acquisition Is Initialization)** : ce patron garantit que les ressources (sockets) sont initialisées lors de la création d’un objet et libérées automatiquement à la destruction, évitant ainsi les fuites de ressources.
    

### Libération des ressources

- **Fermeture explicite des sockets** : il est crucial d’appeler les fonctions `shutdown()` puis `closesocket()` (ou `close()` sous Unix) pour libérer les ressources associées aux sockets, notamment en cas d’erreur ou d’exception. Cela évite les fuites de descripteurs et les problèmes de performance.
    
- **Gestion des erreurs** : vérifier systématiquement les valeurs de retour des appels système (ex : `socket()`, `bind()`, `listen()`, `accept()`, `recv()`, `send()`) pour détecter et gérer les erreurs. Cela permet d’éviter des plantages et de libérer les ressources en cas d’échec.
    
#
---

# 🗒️ Tableau synthétique des stratégies de gestion des ressources

|Stratégie|Description|Avantages|Inconvénients|
|---|---|---|---|
|Interfaces abstraites|Définir des contrats sans implémentation pour découpler les classes|Réduction du couplage, facilité de maintenance|Plus de code, complexité accrue|
|Injection de dépendances|Injection des dépendances via un framework pour réduire le couplage|Modularité, facilité de test|Complexité d’implémentation|
|RAII|Initialisation et libération automatique des ressources|Évitement des fuites, robustesse|Nécessite une bonne conception des classes|
|Fermeture explicite|Appels à `shutdown()` puis `closesocket()` pour libérer les ressources|Libération propre des ressources|Code supplémentaire pour gérer les erreurs|
|Gestion des erreurs|Vérification systématique des retours des appels système|Robustesse, détection rapide des erreurs|Code supplémentaire pour vérifier les erreurs|

#
---

# 🪤 Pièges courants et bonnes pratiques

### Gestion des fuites de descripteurs

- **Problématique** : les sockets non fermés provoquent des fuites de ressources et des problèmes de performance.
- **Solution** : utiliser des destructeurs RAII pour libérer automatiquement les ressources.
- **Bonnes pratiques** : systématiquement vérifier la fermeture des sockets, utiliser des classes dédiées pour encapsuler la gestion des sockets.

### Problèmes de blocage

- **Problématique** : les appels bloquants (`accept()`, `recv()`, `send()`) peuvent bloquer l’exécution du serveur.
- **Solution** : utiliser des sockets non bloquants et des mécanismes de multiplexage (`select()`, `poll()`, `epoll()`) pour gérer plusieurs connexions simultanément.
- **Bonnes pratiques** : éviter les appels bloquants, gérer les événements réseau de manière asynchrone.

### Gestion des erreurs

- **Problématique** : les erreurs non gérées provoquent des plantages.
- **Solution** : vérifier systématiquement les valeurs de retour des appels système et gérer les erreurs via des exceptions ou des codes d’erreur.
- **Bonnes pratiques** : utiliser des mécanismes de gestion des erreurs robustes, intégrer la gestion des erreurs dès la conception.

### Utilisation de RAII

- **Problématique** : mauvaise gestion des ressources.
- **Solution** : encapsuler les ressources dans des objets RAII qui libèrent automatiquement les ressources.
- **Bonnes pratiques** : appliquer RAII pour les sockets, les fichiers, la mémoire, etc.
#
---

# 🗂️ Stratégie de test

### Tests unitaires

- **Objectif** : valider le bon fonctionnement des unités individuelles du code.
- **Méthodes** : utiliser des frameworks comme Boost.Test pour créer des tests unitaires automatisés.
- **Bonnes pratiques** : isoler les tests avec des mocks pour éviter les effets de bord, tester chaque fonctionnalité indépendamment.

### Tests d’intégration

- **Objectif** : vérifier les interactions entre les classes et la cohérence globale.
- **Méthodes** : utiliser des frameworks de mock pour simuler les interactions entre classes.
- **Bonnes pratiques** : tester les scénarios d’intégration pour détecter les erreurs d’interaction.

### Scénarios de stress

- **Objectif** : évaluer la robustesse du serveur sous charge.
- **Méthodes** : simuler des connexions multiples simultanées avec des outils de test de charge.
- **Bonnes pratiques** : mesurer la performance et la stabilité sous charge élevée.
#
---
# 🔭Tableau synthétique des stratégies de test

|Stratégie|Description|Avantages|Inconvénients|
|---|---|---|---|
|Tests unitaires|Vérification des fonctions individuelles avec mocks|Détection rapide des erreurs, robustesse|Nécessite écriture de tests supplémentaires|
|Tests d’intégration|Vérification des interactions entre classes|Détection des erreurs d’intégration|Complexité accrue, nécessite frameworks mock|
|Scénarios de stress|Simulation de charge élevée pour tester la robustesse|Évaluation de la performance et stabilité|Nécessite outils spécifiques|
#
---

# 📍Conclusion

L’architecture orientée objet en C++98 pour le projet webServ doit reposer sur une conception claire des classes principales (`ANetworkTcp`, `ServerSocket`, `ClientSocket`, `EventLoop`) avec des responsabilités bien définies. Le modèle de composition est recommandé pour sa flexibilité, sa maintenabilité et sa capacité à réduire le couplage fort, notamment via l’utilisation d’interfaces abstraites, de l’injection de dépendances et du patron RAII.

La gestion rigoureuse des ressources, notamment la fermeture explicite des sockets et la gestion des erreurs, est essentielle pour garantir la robustesse et la performance du serveur HTTP. Enfin, une stratégie de test complète, intégrant tests unitaires, tests d’intégration et scénarios de stress, est indispensable pour valider l’architecture et assurer la qualité du code.

Cette approche équilibre propreté du code, extensibilité et performance, tout en anticipant les défis spécifiques à C++98, notamment la gestion des erreurs, la prévention des fuites de ressources et la gestion des événements réseau.
#
---

Cette analyse fournit une base solide pour implémenter la partie gestion des sockets du projet webServ en C++98, en respectant les meilleures pratiques de conception orientée objet et de programmation réseau.

>[!important] _ lien pour retourner a index:_ [**00-index-wiki**](00-index-wiki.md)