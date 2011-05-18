/********************************************************************************************
 *
 * -servidorHTTP.c-
 * Descripción:
 * 		Contiene la función main() con funciones para la validación de parametros al programa
 * 		principal.
 *
 ********************************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include "connHandler.c"



const char *HOST_ERRORMSG="Host desconocido.";
const char *PORT_ERRORMSG="Puerto inválido.";
const char *HELP_MSG="Pruebe -h para mas información.";

const char* DEMONIO="servidorHTTP";

int puerto;
char* ip;

/**************************************************************************************
 *	-mostrarAyuda-
 *	Descripción
 *		muestra una ayuda al usuario
 *	Parámetros
 *		@argv	argumentos pasados al programa principal
 *	Retorno
 *		void
 **************************************************************************************/
void mostrarAyuda(char* argv[]) {
        printf("\nUso: %s [IP][:PUERTO] [-h]\n", argv[0]);
        printf("  Opciones:\n");
        printf("\tIP\t IP sobre la cual debe escuchar el servidor\n");
        printf("\tPUERTO\t Puerto en el cual debe escuchar el servidor\n");
        printf("\t-h\t Ayuda\n\n");

        printf("Si sólo se especifica IP se usa el puerto 80 por defecto\n");
        printf("Si sólo se especifica PUERTO se usa cualquier IP local por defecto \n");
        printf("Si no se especifica nada se usa cualquier IP local y puerto 80 por defecto\n");

}

/**************************************************************************************
 *	-esNum-
 *	Descripción
 *		Verifica si una cadena está representando un número.
 *	Parámetros
 *		@val cadena a verificar
 *	Retorno
 *		0 si val es un número
 *		1 en caso contrario
 **************************************************************************************/
int esNum(const char *val){
    int i = 0, s = 1;
    for(i = 0; i <strlen(val); i++)
        if(isdigit(val[i]) == 0)
            s = 0;
    return s;
}

/**************************************************************************************
 *	-parsePuerto-
 *	Descripción
 *		Dado un puerto se verifica si es válido según la RFC1700
 *	Parámetros
 *		@portStr	Cadena a verificar
 *	Retorno
 *		1 si se valida el puerto
 *		0 en caso contrario
 **************************************************************************************/
int parsePuerto(char *portStr){
	if(esNum(portStr)){
		int port;
		sscanf(portStr,"%d",&port);
		if (port>=1 && port<=65535)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

/**************************************************************************************
 *	-parseIP-
 *	Descripción
 *		Dada una IP verifica si es válida según IPv4.
 *	Parámetros
 *		@ipnum	Cadena que contiene la IP a verificar.
 *	Retorno
 *		1 en caso de que se valide la IP
 *		0 en caso contrario
 **************************************************************************************/
int parseIP(char *ipnum){
	int i=inet_addr(ipnum);
	if(i==INADDR_NONE)
		return 0;
	else
		return 1;
}

/**************************************************************************************
 *	-validarHostYPuerto-
 *	Descripción
 *		Dada una cadena de la forma "HOST:PUERTO" se obtiene por separado HOST y PUERTO.
 *		PUERTO se valida.
 *		Si HOST no es una IP se lo toma como nombre de dominio, se verifica y obtiene
 *		la IP asociada a él.
 *		Si HOST es una IP se posterga la validación hasta el momento de asignar la IP
 *		en inicializarServidor.
 *	Parámetros
 *		@arg	1er parámetro de entrada al programa. Es Precondición que éste contiene
 *				un ":" como separador.
 *	Retorno
 *		Arreglo conteniendo IP y PUERTO
 **************************************************************************************/
int validarHostYPuerto(char *arg, char *ipport[2]){
	//Separar en HOST:IP
	char *argcopia2=strdup(arg);
	char *sup_host=strsep(&argcopia2,":");
	char *sup_port=argcopia2;

	printf("%s %s",sup_host,sup_port);

	if (!parseIP(sup_host)){ //debe ser un nombre de dominio
		char* unaip=(char*)malloc(sizeof(char)*INET_ADDRSTRLEN);
		if ((strcmp(sup_host,"")!=0) && !dominioValido(sup_host, unaip))
		{
			printf ("%s\n%s", HOST_ERRORMSG, HELP_MSG);
			return 0;
		}
		ipport[0]=unaip;
	}
	else ipport[0]=sup_host;

	if (!parsePuerto(sup_port)){
		printf ("%s\n%s", PORT_ERRORMSG, HELP_MSG);
		return 0;
	}
	else ipport[1]=sup_port;
	return 1;
}
/**************************************************************************************
 *	-dominioValido-
 *	Descripción
 *		Verifica que un nombre de dominio es válido realizando una consulta DNS.
 *		Se obtiene el IP en caso de un nombre válido.
 *	Parámetros
 *		@dominio	Nombre de dominio a consultar
 *		@ipres		IP del nombre de dominio
 *	Retorno
 *		IP asociada al nombre de dominio
 **************************************************************************************/
int dominioValido(char *dominio, char *ipres){

	struct addrinfo hints, *res, *p;
	char ip[INET_ADDRSTRLEN];

	bzero(&(hints), sizeof hints);

	if (getaddrinfo(dominio, NULL, &hints, &res) != 0)
		return 0;
	/* Ligamos la primer ip encontrada */
	for(p = res;p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		inet_ntop(p->ai_family, addr, ip, sizeof ip);
		if (p==res) strcpy(ipres,ip);
		return 1;
	}
	return 0;
}
/**************************************************************************************
 *	-signalHandler-
 *	Descripción
 *		Manejador de señales del sistema operativo.
 *	Parámetros
 *		@sig	Número de señal recibida.
 *	Retorno
 *		void
 **************************************************************************************/

void signalHandler(int sig) {
	printf("El servidor terminó correctamente\n");
	free(ip);
	exit(EXIT_SUCCESS);
}

/**************************************************************************************
 *	-main-
 *	Descripción
 *		Función main llamada al ejecutar el programa.
 *		Una finalización con éxito es una interrupción del usuario o que el programa
 *		llegue a la instrucción final.
 *	Parámetros
 *		@argc	Cantidad de parámetros de entrada
 *		@argv	Parámetros de entrada
 *	Retorno
 *		EXIT_SUCCESS si finaliza con éxito.
 *		EXIT_FAILURE en caso contrario.
 **************************************************************************************/
int main(int argc, char *argv[]) {

	/* Manejar señales */
    signal(SIGUSR1, signalHandler);
    signal(SIGCHLD, SIG_IGN);

    if (argc>0 && argc<=3){
    	/* Buscar -h para mostrar ayuda y salir */
    	for (int i=0;i<sizeof(argv);i++){
    		if (argv[i]!=0)
    			if (strcmp(argv[i],"-h")==0){
    				mostrarAyuda(argv);
    				exit(EXIT_FAILURE);
    			}
    	}
    }
    else{ /* Número de argumentos inválido, sugerir ayuda y salir */
    	printf ("%s\n",HELP_MSG);
    	exit(EXIT_FAILURE);
    }


    if (argc==2){
    	char *param=strdup(argv[1]);
    	if (strstr(strdup(param),":")){
    		/* Se verifica precondicion de validarHostYPuerto: HOST:PUERTO */
    		char** ipport= calloc(2, sizeof(char*));
    		if (!validarHostYPuerto(param,ipport)) exit(EXIT_FAILURE);
    		ip=strdup(ipport[0]);
    		sscanf(ipport[1],"%d",&puerto);
    	}
    	else
    	{
    	/* No es HOST:PUERTO, soloun HOST o solo PUERTO */
    		if (esNum(param)){
    			/* Debe ser solo PUERTO, establecer cualquier IP local */
    			if (!parsePuerto(param)){
    				printf ("%s\n%s\n", PORT_ERRORMSG, HELP_MSG);
    		    	exit(EXIT_FAILURE);
    		    }
			ip="0.0.0.0";
			sscanf(param,"%d",&puerto);
    		}else{
    			/* Debe ser solo HOST, establecer puerto por defecto */
    			char unaip[INET_ADDRSTRLEN];
    			if (!dominioValido(param, unaip)){
    				printf ("%s\n%s\n", HOST_ERRORMSG, HELP_MSG);
    				exit(EXIT_FAILURE);
    			}
    			ip=&unaip[0];
    			puerto=80;
    		}
    	}
    }
    if(argc==1){
    	ip="0.0.0.0";
    	puerto=80;
    }


    daemon(1,1); /* Modo demonio: programa principal en background */
    if ( (chdir("htdocs/")==-1) ){
    	perror("Cambiando a /htdocs");
    	printf("nro error= %d.\n", errno);
    	printf ("Cree la carpeta htdocs en la carpeta donde está ubicado el ejecutable y ponga allí los documentos.\n");
    	exit(EXIT_FAILURE);
    }
    correrServidor(ip,puerto);
    exit(EXIT_SUCCESS);
}

