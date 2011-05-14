
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
	if(strcmp(ip,"0.0.0.0")==0)
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		if (inet_pton(AF_INET, ip, &(my_addr.sin_addr))==-1){
			perror("inet_pton");
			exit (1);
		}
	bzero(&(my_addr.sin_zero), 8);
	/* se crea el socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	printf("Creando socket ....\n");
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
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
		printf("Se aceptó una conexión\n");
		bzero(buffer,BUFFLEN);
		if ((numbytes=recv(new_fd, buffer, BUFFLEN, 0)) == -1) {
			perror("recv");
			exit(1);
		}

		response r;
		procesarPedido(buffer,&r);
		enviarHeader(r.codigo,new_fd);
		if (r.codigo!=HTTP_FNOTFND)
			enviarHTML(r.path,new_fd);
		else
			enviarHTML("404.html",new_fd);

		printf("Se envió un mensaje\n");
		close(new_fd);
		return 0;
		}
	close(sockfd);
	return 0;
}