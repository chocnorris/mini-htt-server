
/********************************************************************************************
 *
 * 	-connHandler.c-
 * 	Descripción
 * 		Implementa el control de la conexión:
 *	 	Poner el proceso a la escucha y aceptar la conexión con los clientes,
 * 	 	creando un hijo por cada conexión aceptada.
 * 		Cada hijo recibe una solicitud y usando dataHandler.c envía la respuesta que corresponde.
 *
 ********************************************************************************************/

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
#define MAX_LISTENBUF 10

/******************************************************************************************************************
	-correrServidor-
    Descripción
        Poner servidor a la espera de conexiones y manejar la interacción básica para la respuesta de solicitudes.
    Parametros
        @ip	IP en la cual escuchar.
        @p	PUERTO en el cual escuchar.
    Retorno
		EXIT_SUCCESS si termina con éxito.
		EXIT_FAILURE en caso contrario.
 ******************************************************************************************************************/
int correrServidor(char *ip, int p){
	int sockfd,new_fd;
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
	struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
	socklen_t addr_size;
	int numbytes,size;
	char buffer [BUFFLEN];

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(p);
	/* Establecimiento de IP */
	if( (strcmp(ip,"0.0.0.0")==0) || (strcmp(ip,"")==0))
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		if (inet_pton(AF_INET, ip, &(my_addr.sin_addr))==-1){
			perror("inet_pton");
			printf("nro error= %d.\n", errno);
			exit (EXIT_FAILURE);
		}
	char *anyaddr=(char*)malloc(sizeof(char)*INET_ADDRSTRLEN);
	inet_ntop(AF_INET,&(my_addr.sin_addr),anyaddr,sizeof my_addr);
	printf ("-- Iniciando servidor en %s:%d\n",anyaddr,p);

	bzero(&(my_addr.sin_zero), 8);
	/* Se crea el socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		printf("nro error= %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	/* Ligar a IP:PUERTO */
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		if (errno==99) printf("IP inválida. Pruebe -h para más información.\n");
		else{
			printf("nro error= %d.\n", errno);
			perror("bind");
		}
		exit(EXIT_FAILURE);
	}
	pid_t pid_hijo;
	listen(sockfd,MAX_LISTENBUF);
	addr_size= sizeof their_addr;
	while (1){
		/* Nueva conexión, se crea un hijo para que la maneje */
		new_fd=accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		pid_hijo=fork();
		/* En PADRE: descartar socket recién aceptado de hijo y continuar aceptando conexiones */
		if(pid_hijo!=0){
			close(new_fd);
			continue;
		}
		/*EN HIJO: Descartar socket de padre (de escucha) y atender solicitud a través del nuevo socket */
		close(sockfd);
		bzero(buffer,BUFFLEN);
		if ((numbytes=recv(new_fd, buffer, BUFFLEN, 0)) == -1) {
			perror("Recibiendo solicitud http");
			printf("nro error= %d.\n", errno);
			exit(EXIT_FAILURE);
		}

		response r;
		/* Procesar solicitud */
		procesarPedido(buffer,&r);
		/* Enviar respuesta: HEADER y OBJETO */
		enviarHeader(new_fd, &r);
		if (r.codigo==HTTP_OK)
			enviarArchivo(r.path,new_fd);
		if (r.codigo==HTTP_FNOTFND)
			enviarArchivo(ERROR_PATH,new_fd);
		if (r.codigo==HTTP_MNA)
			enviarArchivo(NOTIMPL_PATH,new_fd);
		/* Terminar con éxito */
		close(new_fd);
		free(r.path);
		exit(EXIT_SUCCESS);
		}
	close(sockfd);
	exit(EXIT_SUCCESS);
}
