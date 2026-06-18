>[!important] _note _
# 📌 Introduction 

Les codes de statut de réponse HTTP indiquent si une requête [HTTP](https://developer.mozilla.org/fr/docs/Web/HTTP) a été exécutée avec succès ou non. Les réponses sont regroupées en cinq classes :

>[!source] _Source de l'intro:_ [Developer.mozialla.org](https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Status).
# 
---
# 🧠 Code des réponse HTTP 

>[!info] les codes **essentiels** pour le projet  :
>
>- **200 OK** : Réussite pour les fichiers statiques et les CGI.
>- **400 Bad Request** : Requête malformée.
>- **403 Forbidden** : Accès refusé (ex : tentative d’accès à `/etc/passwd`).
>- **404 Not Found** : Fichier ou script CGI introuvable.
>- **405 Method Not Allowed** : Méthode HTTP non autorisée (ex : `PUT` non implémenté).
>- **500 Internal Server Error** : Erreur interne (ex : échec de lecture d’un fichier ou exécution d’un CGI).
>- **501 Not Implemented** : Méthode HTTP non implémentée (ex : `DELETE`).
## Code de réussites [2xx - 299]

| Code | Message    | Description                                                   | Exemple d'utilisation dans webServ                                     |
| ---- | ---------- | ------------------------------------------------------------- | ---------------------------------------------------------------------- |
| 200  | OK         | La requête a réussi.                                          | Réponse à une requête `GET` pour un fichier statique ou un script CGI. |
| 201  | Created    | La requête a abouti à la création d’une ressource.            | Peu probable dans webServ (sauf si tu implémentes `PUT`).              |
| 204  | No Content | La requête a réussi, mais il n’y a pas de contenu à renvoyer. | Réponse à une requête `HEAD` (si tu l’implémentes).                    |
## Code de Redirection [3xx-399]

|Code|Message|Description|Exemple d'utilisation dans webServ|
|---|---|---|---|
|301|Moved Permanently|La ressource a été déplacée définitivement.|Redirection d’une ancienne URL vers une nouvelle.|
|302|Found|La ressource a été déplacée temporairement.|Redirection temporaire (ex : maintenance).|
|304|Not Modified|La ressource n’a pas été modifiée depuis la dernière requête (cache).|Si tu implémentes la gestion du cache avec `If-Modified-Since`.|
## Code d'Erreur Client [4xx-499]

| Code | Message            | Description                                                        | Exemple d'utilisation dans webServ                                              |
| ---- | ------------------ | ------------------------------------------------------------------ | ------------------------------------------------------------------------------- |
| 400  | Bad Request        | La requête est malformée (syntaxe invalide).                       | Si la requête HTTP ne respecte pas le format (ex : ligne de requête manquante). |
| 403  | Forbidden          | Le serveur refuse d’autoriser la requête.                          | Accès refusé à un fichier ou répertoire protégé.                                |
| 404  | Not Found          | La ressource demandée n’existe pas.                                | Fichier ou script CGI introuvable.                                              |
| 405  | Method Not Allowed | La méthode HTTP utilisée n’est pas autorisée pour cette ressource. | Si un client envoie une requête `DELETE` alors que seul `GET` est autorisé.     |
| 408  | Request Timeout    | Le serveur a mis trop de temps à recevoir la requête.              | Si le client ne envoie pas la requête complète dans un délai raisonnable.       |
| 411  | Length Required    | La requête nécessite un header `Content-Length` qui est manquant.  | Pour les requêtes `POST` sans `Content-Length`.                                 |
| 413  | Payload Too Large  | La requête est trop grande pour être traitée par le serveur.       | Si le corps de la requête dépasse une limite définie.                           |
| 414  | URI Too Long       | L’URI demandée est trop longue.                                    | Si le chemin de la requête est anormalement long.                               |
## Code d'Erreur Serveur [5xx-599]

| Code | Message                    | Description                                                             | Exemple d'utilisation dans webServ                                            |
| ---- | -------------------------- | ----------------------------------------------------------------------- | ----------------------------------------------------------------------------- |
| 500  | Internal Server Error      | Le serveur a rencontré une erreur interne.                              | Erreur lors de l’exécution d’un script CGI ou lecture d’un fichier.           |
| 501  | Not Implemented            | La méthode HTTP n’est pas implémentée par le serveur.                   | Si le serveur reçoit une requête `PATCH` (non implémentée).                   |
| 502  | Bad Gateway                | Le serveur a reçu une réponse invalide d’un autre serveur (ex : proxy). | Peu probable dans webServ (sauf si tu implémentes un proxy).                  |
| 503  | Service Unavailable        | Le serveur est temporairement indisponible (ex : surcharge).            | Si le serveur ne peut pas traiter la requête (ex : trop de connexions).       |
| 504  | Gateway Timeout            | Le serveur n’a pas reçu de réponse à temps d’un autre serveur.          | Peu probable dans webServ.                                                    |
| 505  | HTTP Version Not Supported | La version HTTP utilisée n’est pas supportée.                           | Si le client envoie `HTTP/2.0` et que ton serveur ne supporte que `HTTP/1.1`. |
#
---
>[!important] _ lien pour retourner a index:_ [**00-index-wiki**](00-index-wiki.md)