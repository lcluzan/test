Voici une traduction structurée et fidèle du sujet **webserv** de l’école 42, en conservant les termes techniques et la mise en forme originale.

---

# **Webserv**

**C’est maintenant que vous allez enfin comprendre pourquoi les URLs commencent par HTTP.**

---

## **Résumé**

Ce projet consiste à écrire votre propre serveur HTTP. Vous pourrez le tester avec un vrai navigateur. HTTP est l’un des protocoles les plus utilisés sur Internet. Comprendre ses subtilités sera utile, même si vous ne travaillez pas sur un site web.

**Version : 24.0**

---

## **Table des matières**

- I. Introduction 
- II. Règles générales
- III. Instructions concernant l’IA
- IV. Partie obligatoire
	- IV.1 Exigences
	- IV.2 Pour MacOS uniquement
	- IV.3 Fichier de configuration 
- V. Exigences pour le README
- VI. Partie bonus
- VII. Soumission et évaluation par les pairs 14

---

## **Chapitre I : Introduction**

Le **Hypertext Transfer Protocol (HTTP)** est un protocole de communication pour les systèmes d’information hypermédia distribués et collaboratifs. HTTP est la base de la communication de données pour le World Wide Web, où les documents hypertexte incluent des hyperliens vers d’autres ressources accessibles facilement par l’utilisateur, par exemple en cliquant avec une souris ou en touchant l’écran d’un navigateur web.

HTTP a été développé pour supporter la fonctionnalité hypertexte et la croissance du World Wide Web.

La fonction principale d’un serveur web est de stocker, traiter et livrer des pages web aux clients. La communication client-serveur se fait via le protocole HTTP.

Les pages livrées sont le plus souvent des documents HTML, qui peuvent inclure des images, des feuilles de style et des scripts en plus du contenu textuel.

Plusieurs serveurs web peuvent être utilisés pour un site à fort trafic, répartissant le trafic entre plusieurs machines physiques.

Un agent utilisateur, généralement un navigateur web ou un robot d’indexation, initie la communication en demandant une ressource spécifique via HTTP, et le serveur répond avec le contenu de cette ressource ou un message d’erreur s’il ne peut pas le faire. La ressource est généralement un fichier réel sur le stockage du serveur, ou le résultat d’un programme. Mais ce n’est pas toujours le cas et cela peut en réalité être bien d’autres choses.

Bien que sa fonction principale soit de servir du contenu, HTTP permet également aux clients d’envoyer des données. Cette fonctionnalité est utilisée pour soumettre des formulaires web, y compris le téléchargement de fichiers.

---

## **Chapitre II : Règles générales**

- Votre programme ne doit jamais planter (même en cas de manque de mémoire) ou se terminer de manière inattendue. Si cela arrive, votre projet sera considéré comme non fonctionnel et votre note sera 0.
- Vous devez soumettre un **Makefile** qui compile vos fichiers sources. Il ne doit pas effectuer de relinkage inutile.
- Votre **Makefile** doit au moins contenir les règles : `$(NAME)`, `all`, `clean`, `fclean` et `re`.
- Compilez votre code avec `c++` et les flags `-Wall -Wextra -Werror`.
- Votre code doit être conforme à la norme **C++ 98** et doit toujours compiler avec le flag `-std=c++98`.
- Assurez-vous d’utiliser autant de fonctionnalités C++ que possible (par exemple, préférez `<cstring>` à `<string.h>`). Vous pouvez utiliser des fonctions C, mais privilégiez toujours leurs versions C++ si possible.
- Toute bibliothèque externe et les bibliothèques **Boost** sont interdites.

---

## **Chapitre III : Instructions concernant l’IA**

### **Contexte**

Pendant votre parcours d’apprentissage, l’IA peut vous aider dans de nombreuses tâches. Prenez le temps d’explorer les différentes capacités des outils d’IA et comment ils peuvent soutenir votre travail. Cependant, abordez-les toujours avec prudence et évaluez de manière critique les résultats. Que ce soit du code, de la documentation, des idées ou des explications techniques, vous ne pouvez jamais être totalement sûr que votre question était bien formulée ou que le contenu généré est exact. Vos pairs sont une ressource précieuse pour vous aider à éviter les erreurs et les angles morts.

### **Message principal**

- Utilisez l’IA pour réduire les tâches répétitives ou fastidieuses.
- Développez des compétences en **prompting** (coding et non-coding) qui bénéficieront à votre future carrière.
- Apprenez comment fonctionnent les systèmes d’IA pour mieux anticiper et éviter les risques courants, les biais et les problèmes éthiques.
- Continuez à développer vos compétences techniques et transversales en travaillant avec vos pairs.
- N’utilisez que le contenu généré par l’IA que vous comprenez pleinement et dont vous pouvez assumer la responsabilité.

### **Règles pour les apprenants**

- Prenez le temps d’explorer les outils d’IA et de comprendre leur fonctionnement, afin de les utiliser de manière éthique et de réduire les biais potentiels.
- Réfléchissez à votre problème avant de formuler une requête : cela vous aide à écrire des prompts plus clairs, détaillés et pertinents en utilisant un vocabulaire précis.
- Développez l’habitude de vérifier systématiquement, réviser, questionner et tester tout ce qui est généré par l’IA.
- Recherchez toujours une **relecture par les pairs** : ne vous fiez pas uniquement à votre propre validation.

---

## **Chapitre IV : Partie obligatoire**

|Nom du programme|webserv|
|---|---|
|Fichiers à soumettre|Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp, fichiers de configuration|
|Makefile|Doit contenir les règles : `$(NAME)`, `all`, `clean`, `fclean`, `re`|
|Arguments|[Un fichier de configuration]|
|Fonctions externes autorisées|Toutes les fonctionnalités doivent être implémentées en C++ 98. Fonctions autorisées : `execve`, `pipe`, `strerror`, `gai_strerror`, `errno`, `dup`, `dup2`, `fork`, `socketpair`, `htons`, `htonl`, `ntohs`, `ntohl`, `select`, `poll`, `epoll` (`epoll_create`, `epoll_ctl`, `epoll_wait`), `kqueue` (`kqueue`, `kevent`), `socket`, `accept`, `listen`, `send`, `recv`, `chdir`, `bind`, `connect`, `getaddrinfo`, `freeaddrinfo`, `setsockopt`, `getsockname`, `getprotobyname`, `fcntl`, `close`, `read`, `write`, `waitpid`, `kill`, `signal`, `access`, `stat`, `open`, `opendir`, `readdir`, `closedir`.|
|Libft autorisée|Non applicable|
|Description|Un serveur HTTP en C++ 98|

Vous devez écrire un serveur HTTP en C++ 98.

Votre exécutable doit être lancé comme suit :

`./webserv [fichier de configuration]`

Bien que `poll()` soit mentionné dans le sujet et la feuille d’évaluation, vous pouvez utiliser toute fonction équivalente comme `select()`, `kqueue()`, ou `epoll()`.

---

### **IV.1 Exigences**

- Votre programme doit utiliser un ==fichier de configuration==, fourni en argument sur la ligne de commande ou disponible dans un chemin par défaut.
- Vous ne pouvez pas ==utiliser `execve` pour lancer un autre serveur web.
- Votre serveur doit rester ==**non-bloquant** en permanence== et ==gérer correctement les déconnexions des clients== si nécessaire.
- Il doit être ==non-bloquant et utiliser un seul `poll()` (ou équivalent)== pour toutes les ==opérations d’I/O entre les clients et le serveur== (y compris l’écoute).
- `poll()` (ou équivalent) doit ==surveiller== à la fois la lecture et l’écriture ==simultanément==.
- Vous ne devez ==jamais effectuer une opération de lecture ou d’écriture sans passer par `poll()` (ou équivalent).
- Vérifier la valeur de `errno` pour ajuster le comportement du serveur est strictement interdit après une opération de lecture ou d’écriture.
- Vous n’êtes pas obligé d’utiliser ==`poll()` (ou une fonction équivalente)== pour les ==fichiers disques réguliers== ; les opérations `read()` et `write()` sur ceux-ci ne nécessitent pas de ==notifications de disponibilité==.

>[!important] Les I/O pouvant attendre des données (sockets, pipes/FIFOs, etc.) doivent être non-bloquantes et gérées par un seul `poll()` (ou équivalent). Appeler `read/recv` ou `write/send` sur ces descripteurs sans vérification préalable de disponibilité entraînera une note de 0. Les fichiers disques réguliers sont exemptés.

- Lorsqu’on utilise ==`poll()` ou toute fonction équivalente==, vous pouvez utiliser ==toutes les macros ou fonctions d’aide associées== (par exemple, `FD_SET` pour `select()`).
- Une requête vers votre serveur ne doit ==jamais rester bloquée indéfiniment==.
- Votre serveur doit être ==compatible avec les navigateurs web standards== de votre choix.
- Vous pouvez ==utiliser NGINX pour comparer les en-têtes et les comportements de réponse== (attention aux différences entre les versions HTTP).
- Vos ==codes de statut== de réponse HTTP doivent être ==précis==.
- Votre serveur doit avoir des ==pages d’erreur par défaut== si aucune n’est fournie.
- Vous ne pouvez ==pas utiliser `fork` pour autre chose que le CGI== (comme PHP, Python, etc.).
- Vous devez pouvoir ==servir un site web entièrement statique==.
- Les clients doivent pouvoir ==télécharger des fichiers==.
- Vous devez ==implémenter au moins les méthodes GET, POST et DELETE==.

Testez votre serveur en ==conditions de stress ==pour vous assurer qu’il reste disponible en permanence.

Votre serveur doit pouvoir ==écouter sur plusieurs ports== pour délivrer différents contenus (voir Fichier de configuration).

---

### **IV.2 Pour MacOS uniquement**

Étant donné que macOS gère `write()` différemment des autres systèmes Unix, vous êtes autorisé à utiliser `fcntl()`.

Vous devez utiliser les descripteurs de fichiers en mode non-bloquant pour obtenir un comportement similaire à celui des autres systèmes Unix.

Cependant, vous n’êtes autorisé à utiliser `fcntl()` qu’avec les flags suivants :

- `F_SETFL`
- `O_NONBLOCK`
- `FD_CLOEXEC`

Tout autre flag est interdit.

---

### **IV.3 Fichier de configuration**

Vous pouvez vous inspirer de la section `server` du fichier de configuration de NGINX.

Dans le fichier de configuration, vous devez pouvoir :

- Définir toutes les paires interface:port sur lesquelles votre serveur écoutera (définir plusieurs sites web servis par votre programme).
- Configurer des pages d’erreur par défaut.
- Définir la taille maximale autorisée pour le corps des requêtes client.
- Spécifier des règles ou configurations pour une URL/route (pas de regex requis ici), pour un site web, parmi les suivantes :
    - Liste des méthodes HTTP acceptées pour la route.
    - Redirection HTTP.
    - Répertoire où le fichier demandé doit être situé (par exemple, si l’URL `/kapouet` est racine de `/tmp/www`, l’URL `/kapouet/pouic/toto/pouet` cherchera `/tmp/www/pouic/toto/pouet`).
    - Activation ou désactivation de la liste des répertoires.
    - Fichier par défaut à servir lorsque la ressource demandée est un répertoire.
    - Autoriser le téléchargement de fichiers depuis les clients vers le serveur, et spécifier l’emplacement de stockage.
- Exécution de CGI, basée sur l’extension de fichier (par exemple `.php`). Voici quelques remarques spécifiques concernant les CGIs :
    - Vous vous demandez ce qu’est un CGI ?
    - Examinez attentivement les variables d’environnement impliquées dans la communication serveur web-CGI. La requête complète et les arguments fournis par le client doivent être disponibles pour le CGI.
    - N’oubliez pas que, pour les requêtes chunked, votre serveur doit les dé-chunker, le CGI s’attendra à un EOF comme fin du corps.
    - La même chose s’applique à la sortie du CGI. Si aucune `content_length` n’est retournée par le CGI, EOF marquera la fin des données retournées.
    - Le CGI doit être exécuté dans le bon répertoire pour l’accès aux fichiers via des chemins relatifs.
    - Votre serveur doit supporter au moins un CGI (php-CGI, Python, etc.).

Vous devez fournir des fichiers de configuration et des fichiers par défaut pour tester et démontrer que chaque fonctionnalité fonctionne pendant l’évaluation.

Vous pouvez avoir d’autres règles ou informations de configuration dans votre fichier (par exemple, un nom de serveur pour un site web si vous prévoyez d’implémenter des hôtes virtuels).

---

## **Chapitre V : Exigences pour le README**

Un fichier **README.md** doit être fourni à la racine de votre dépôt Git. Son objectif est de permettre à toute personne non familière avec le projet (pairs, staff, recruteurs, etc.) de comprendre rapidement de quoi il s’agit, comment l’exécuter et où trouver plus d’informations.

Le **README.md** doit inclure au moins :

- La toute première ligne doit être en italique et indiquer : _This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]]._
- Une section **"Description"** qui présente clairement le projet, son objectif et un bref aperçu.
- Une section **"Instructions"** contenant toutes les informations pertinentes sur la compilation, l’installation et/ou l’exécution.
- Une section **"Ressources"** listant les références classiques liées au sujet (documentation, articles, tutoriels, etc.), ainsi qu’une description de l’utilisation de l’IA — en précisant pour quelles tâches et quelles parties du projet.

D’autres sections peuvent être requises selon le projet (par exemple, exemples d’utilisation, liste des fonctionnalités, choix techniques, etc.).

Votre **README** doit être écrit en anglais.

---

## **Chapitre VI : Partie bonus**

Voici quelques fonctionnalités supplémentaires que vous pouvez implémenter :

- Support des cookies et gestion des sessions (fournir des exemples simples).
- Gestion de plusieurs types de CGI.

La partie bonus ne sera évaluée que si la partie obligatoire est entièrement complétée sans problème. Si vous ne remplissez pas toutes les exigences obligatoires, votre partie bonus ne sera pas évaluée.

---

## **Chapitre VII : Soumission et évaluation par les pairs**

Soumettez votre travail dans votre dépôt **Git** comme d’habitude. Seul le contenu de votre dépôt sera évalué pendant la défense. Assurez-vous de vérifier les noms de vos fichiers pour vous assurer qu’ils sont corrects.

Pendant l’évaluation, une **modification mineure du projet** peut parfois être demandée. Cela pourrait impliquer un changement de comportement mineur, quelques lignes de code à écrire ou réécrire, ou une fonctionnalité facile à ajouter.

Bien que cette étape ne soit pas applicable à tous les projets, vous devez être prêt si elle est mentionnée dans les directives d’évaluation.

Cette étape vise à vérifier votre compréhension réelle d’une partie spécifique du projet. La modification peut être effectuée dans n’importe quel environnement de développement de votre choix (par exemple, votre configuration habituelle), et elle doit être réalisable en quelques minutes — sauf si un délai spécifique est défini dans le cadre de l’évaluation.