#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 2000
#define BUFFLEN 1024
int es_exit(char *msg);

int main(int argc, char **argv)
{
	int sockfd,new_fd;
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
	struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
	socklen_t addr_size;
	int numbytes,size;
	char buffer [BUFFLEN];

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERVER_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
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

	while (1){
		/* Se le da un nombre al socket */
		listen(sockfd,10);
		addr_size= sizeof their_addr;
		fork();
		new_fd=accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		while (1){
			bzero(buffer,BUFFLEN);
			if ((numbytes=recv(new_fd, buffer, BUFFLEN, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			if (es_exit(buffer)==0)
				break;
			size=strlen(buffer);
			if ((numbytes=send(new_fd, buffer, size, 0)) == -1) {
				perror("send");
				exit(1);
			}
			printf("Se envió un mensaje\n");
		}
		/* devolvemos recursos al sistema */
		close(new_fd);
	}
	close(sockfd);
	return 0;
}

int es_exit(char *msg){
	return strncmp(msg, "exit", 4);
}

