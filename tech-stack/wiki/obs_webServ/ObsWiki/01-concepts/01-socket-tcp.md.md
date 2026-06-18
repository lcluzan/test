>[!important] _note _
#  📌 Introduction

Ce concept de communication est fondamental pour Internet, car les sockets permettent l'échange de données entre les systèmes. 
# 
---
# 🧠 Programmation de socket

Un socket est un point de connexion de communication (noeud final) que vous pouvez nommer et utiliser dans un réseau. La programmation de socket indique comment utiliser les API socket pour établir des liaisons de communication entre des processus distants et locaux.

Les processus qui utilisent un socket peuvent résider sur le même système ou sur des systèmes différents sur des réseaux différents. Les sockets sont utiles pour les applications autonomes et réseau. Les sockets vous permettent d'échanger des informations entre des processus sur la même machine ou sur un réseau, de distribuer le travail sur la machine la plus efficace et d'accéder facilement aux données centralisées. Les interfaces de programme d'application (API) de socket sont la norme réseau pour TCP/IP. Un large éventail de systèmes d'exploitation prennent en charge les API de socket. IBM i Les sockets prennent en charge plusieurs protocoles de transport et de réseau. Les fonctions système de socket et les fonctions réseau de socket sont autorisant les unités d'exécution multiples.

Les programmeurs qui utilisent Integrated Language Environment® (ILE) C peuvent se référer à cette collection de thèmes pour développer des applications socket. Vous pouvez également coder l'API des sockets à partir d'autres langages ILE, tels que RPG.

>[!source] _Source documentation IBM:_ [ibm.com/docs/fr](https://www.ibm.com/docs/fr/i/7.5.0?topic=communications-socket-programming).
# 
---
# 💻 Création du socketTCP/IP du server:
## Creation de la socket:

>[!info] Creation du socket TCP
>~~~ cpp 
>#include <sys/socket.h>
>
>int sockfd = socket(AF_INET, SOCK_STREAM, 0);
>if ( sockfd == -1 )
>{
>	std::cerr << "error: creation socket echouer" << std::endl;
 >}
>~~~
>_ on stoke le file descriptor dans la variable_ `int sockfd ` 

>[!info] configurer l'adress du serveur
>~~~cpp 
>#define PORT 8080
>
struct sockaddr_in 
{ 
  __uint8_t         sin_len; 
   sa_family_t       sin_family; 
   in_port_t         sin_port; 
   struct in_addr    sin_addr; 
   char              sin_zero[8]; 
};
>
>struct sockaddr_in server_addr;
>server_addr.sin_family = AF_INET; 
>server_addr.sin_addr.s_addr = INADDR_ANY; 
>server_addr.sin_port = htons(PORT);
>~~~
>_structure sockaddr_in_
>- `sin_family` = `domain`
>- `sin_port` = a port number
>- `sin_addr` = address for the socket (for example `inet_addr("127.0.0.1")` or const like `INADDR_ANY`) [info](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/ns-winsock2-in_addr)

>[!info] Associer le socket à l'adresse et au port
>~~~cpp
>int bind_return = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
>if ( bind_return == -1 )
>{
>	std::cerr << "error: identification du socket" << std::endl;
>}
>~~~
>_blind associ le file descriptor au port 8080_

>[!info] Mettre le socket en mode écoute
>~~~cpp 
>#define MAX_CLIENTS 10
>
>int listten_return = listen(sockfd, MAX_CLIENTS);
>if ( listten_return == -1)
>{
>	std::cerr << "error: l'ecoute a echouer" << std::endl;
>}
>~~~
>_listen écoute les file descriptor dans la limite du maximum de cliens definie _

>[!important] Après l'execution de la fonction socket toute les erreur doivent fermer les file descriptor 
>~~~cpp 
	>close(sockfd);
>~~~
## Gestion des requêtes:

### 1. Initialiser la structure pollfd pour surveiller les sockets

>[!info] librairie <poll.h>
>~~~ cpp
>#include <poll.h>
>
>int poll(struct pollfd *fds, nfds_t nfds, int timeout);
>
>Strut pollfd {
>	int fd;
>	short events;
>	short recents;
>};
>
>~~~
>_Description de fonction _

>[!info] initialiser la structure `pollfd`
>~~~cpp 
>struct pollfd fds[MAX_CLIENTS + 1];
>Fds[0].fd = sock fd;
>fds[0].events = POLLIN;
>
>/******* init other slots a -1 *******/
>
>For (int i = 1; i <= MAX_CLIENTS + 1; i++)
>{
>	Fds[i].fd = -1;
>}
>~~~

### 2.  attendre une activité sur les sockets ( Timeout infini )
>[!info] Attendre une activité sur les socket
>~~~ cpp
>int ret = poll(fds, MAX_CLIENTS + 1, -1);
>if (ret = -1)
>{
>	std::cerr << “ error: poll failed“ << std::endl;
>}
>~~~
>_ bien arrêter la boucle en cas d’erreur _

### 3. Vérifier si une nouvelle connection arrive 

>[!info] accepter la nouvelle connexion 
>~~~ cpp 
>if (fds[0].revents && POLLIN)
>{
>	struct sockaddr_in clients_addr;
>	socklen_t client_addr_len =sizeof(client_addr);
>	int client_fd = accept(server_fd (struct sockaddr *)&client_addr, &client_addr_len);
>	if (client_fd == -1)
>	{
>		std::cerr << “ error: failed new connection“ << std::endl;
>	}
>	else 
>		std::cout << “Nouvelle conection depuis“ << inet_ntoa(client_addr.sin_addr) << std::
>~~~

>[!info] Trouver un slot libre dans fds
>~~~ cpp 
>int index = 1;
>while (index <= MAX_CLIENTS && fds[i].fd != -1)
>	Index++;
>if (index == MAX_CLIENTS + 1)
>{
>	Std::cerr << “Trop de clients, conection refusée.“ << std::endl;
>	close(client_fd);
>}
>else if (fds[i].fd == -1)
>{
>	fds[i].fd = client_fd;
>	fds[i].events = POLLIN;
>} 								
>~~~
>_DESCRIPTION_

### 4. Traiter les requêtes des clients existants 
>[!info] parcourir les requête clients 
>~~~ cpp
>for (int i=1; i<= MAX_CLIENTS; i++)
>{
>	char buffer[BUFFER_SIZE];
>	memeset(buffer, 0, BUFFER_SIZE);
>	ssize_t bytes_read = read(fds[i].fd, buffer, BUFFER_SIZE - 1);
>	if (bytes_read <= 0)
>	{
>		std::out << “clients déconecté.“ << std::endl;
>		close(fds[i].fd);
>		fds[i]fd = -1; 
>	}
>	else
>	{
>		//Afficher la requête brute (pour debogage)
>		std:coût << “Requête reçue (“ << bytes_read << “octets) :\n “ << buffer << std::endl;
>	}
>}
>~~~ 

#
---
>[!important] _ lien pour retourner a index:_ [**00-index-wiki**](00-index-wiki.md)