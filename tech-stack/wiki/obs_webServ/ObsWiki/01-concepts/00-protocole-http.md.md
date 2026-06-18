>[!important] _note _
# 📌 Introduction 

Le protocole HTTP a été initialement spécifié au début des années 1990. Conçu pour être extensible, il a bénéficié de nombreux ajouts au fil des ans, ce qui a conduit à la dispersion de sa spécification dans de nombreux documents (au milieu d'extensions expérimentales abandonnées). Cette page répertorie les ressources pertinentes concernant HTTP. 

>[!source] _Source de l'intro:_ [Developer.mozialla.org](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Resources_and_specifications).
# 
---
# 🧠 Structure d’une requête HTTP

>[!info] Format suivie d’une requête HTTP
>~~~
>[Ligne de requête]
>[Headers]
>[Corps] (Optionnel, séparé par une ligne vide)
>~~~ 

_Exemple d’une requête HPPP:_
~~~ http
GET /index.html HTTP/1.1
Host: localhost:8080
User-Agent: curl/7.68.0
Accept: */*

[Ligne vide]
~~~
## Ligne de requête 

1. La premiere ligne contient la **Méthode** ( GET, POST, DELETE ).
2. La méthode est suivie du **Chemin** ( index.html ).
3. Et enfin elle ce termine avec la **Version** du protocole ( HTTP/1.1 ).

>[!info] voici un exemple`<Méthode><Chemin><Version HTTP>` 
>~~~ http
>GET /index.html http/1.1
>~~~

### Méthode HTTP supportées:

- GET : Récupérer une ressource (fichier statique ou CGI).
- POST : Envoyer des données (ex : formulaires).
- DELETE : Supprimer une ressource.
## Les ligne du header

| Header         | Description                                                                      |
| -------------- | -------------------------------------------------------------------------------- |
| Host           | Nom de domaine et port du serveur (ex : localhost:8080).                         |
| User-Agent     | Navigateur ou outil utilisé par le client (ex : curl/7.68.0).                    |
| Content-Type   | Type de contenu pour les requêtes POST (ex : application/x-www-form-urlencoded). |
| Content-Length | Taille du corps de la requête (pour POST)                                        |
| Connection     | Indique si la connexion doit être fermée après la réponse (ex : keep-alive).     |
#  
---
#  🔎 Ressources pour approfondir

- **[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)** : Explications sur les sockets et HTTP.
- **[HTTP Request Parsing in C (GitHub)](https://github.com/)** : Chercher des projets comme `tinyhttpd`.
- **[MDN Web Docs : Anatomy of an HTTP Request](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages)** : Explication visuelle des requêtes HTTP.

# 
---
>[!important] _ lien pour retourner a index:_ [**00-index-wiki**](00-index-wiki.md)