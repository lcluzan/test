>[!important] _note _

# index de la page 
## fonction utile socket TCP/IP

>[!info] convertir les valeur entre l'ordre des octets de l'hôte et du réseaux 
>~~~cpp
>#include <arpa/inet.h>
>
>uint32_t htonl(uint32_t hostlong);
>uint16_t htons(uint16_t hostshort);
>uint32_t ntohl(uint32_t netlong);
>uint16_t ntohs(uint16_t netshort);
>~~~
>_Description de la fonction _

>[!info] Multiplexages I/O synchrone 
>~~~cpp
int epoll_create(int nb);
>~~~
>_Description de fonction_
>~~~cpp
>int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
>~~~
>_Description de fonction_
>~~~cpp
>int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
>~~~
>_ Description de fonction _

---

>[!info] Creation de socket avec `socket()`
>~~~ cpp 
>#include <sys/socket.h>
>int socket(int domain, int type, int protocol);
>~~~
>_ Description de la fonction _

>[!info] acepter la connection sur un socket `accept()`
>~~~cpp 
>int accept(int sockfd, struct sockaddr *restrict add, socklen_t *restrict addrlen);
>~~~
> _Description de la fonction _

>[!info] écouter la connection sur un socket `listen()`
>~~~ cpp
>int listen(int sockfd, int backlog);
>~~~
>_ Description de la fonction _

>[!info] envoyée un message dans le socket `send()`
>~~~cpp
>ssize_t send(int sockfd, const void *buff, size_t len, int flags);
>~~~
>_Description de la fonction _

>[!info] recevoir un message de la socket `recv()`
>~~~cpp 
>ssize_t recv(int sockfd, void *buf, size_t len, int flag);
>~~~
>_Description de la fonction _

>[!info] identifier un socket `bind()`
>~~~ cpp
>int bind(int sockfd, const struct sockaddr *address, socklen_t address_len);
>
>struct sockaddr_in 
>{ 
 >   __uint8_t         sin_len; 
>    sa_family_t       sin_family; 
>    in_port_t         sin_port; 
>    struct in_addr    sin_addr; 
>    char              sin_zero[8]; 
>};
>~~~
>_Description de la fonction _

>[!info] initialiser la connection a un socket `connect()`
>~~~cpp
>int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
>~~~
>_Description de la fonction_

>[!info] manipulation d'address IP v.4 `inet_addr()`
>~~~cpp 
>#include <arpa/inet.h>
>in_addr_t inet_addr(const char*cp);
>~~~
>_Description de la fonction_

>[!info] configurer le socket `setsockopt()`
>~~~cpp 
>int setsockopt(int sockfd, int level, int option_name, const void *option_value, socklen_t option_len);
>~~~
>_Description de fonction _

>[!info] manipuler le descripteur de fichier `fcntl()`
>~~~cpp 
>int fcntl(int fd, int cmd, ...);
>~~~
>_Description de la fonction_

---
#
>[!important] _ lien pour retourner a indexObs:_ [**index**](indexObs_WebServe.md)
