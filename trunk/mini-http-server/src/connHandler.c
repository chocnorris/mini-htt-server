
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "dataHandler.c"

#define BUFFLEN 1024


int inicializarServidor(char *ip, int p){
	int sockfd,new_fd;
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
	struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
	socklen_t addr_size;
	int numbytes,size;
	char buffer [BUFFLEN];

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(p);
	if( (strcmp(ip,"0.0.0.0")==0) || (strcmp(ip,"")==0))
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		if (inet_pton(AF_INET, ip, &(my_addr.sin_addr))==-1){
			perror("inet_pton");
			exit (EXIT_FAILURE);
		}
	bzero(&(my_addr.sin_zero), 8);
	/* se crea el socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	pid_t pid_padre,pid_hijo;
	pid_padre=getpid();
	listen(sockfd,10);
	addr_size= sizeof their_addr;
	while (1){
		new_fd=accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		pid_hijo=fork();
		if(pid_hijo!=0){
			close(new_fd);
			continue;
		}
		close(sockfd);
		bzero(buffer,BUFFLEN);
		if ((numbytes=recv(new_fd, buffer, BUFFLEN, 0)) == -1) {
			perror("recv");
			exit(1);
		}

		response r;
		procesarPedido(buffer,&r);
		enviarHeader(r.codigo,new_fd, r.path);
		if (r.codigo!=HTTP_FNOTFND)
			enviarArchivo(r.path,new_fd);
		else
			enviarArchivo("404.html",new_fd);
		close(new_fd);
		return 0;
		}
	close(sockfd);
	return 0;
}
