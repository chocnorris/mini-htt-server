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



int puerto;
char* ip;
const char* DEMONIO="servidorHTTP";

void mostrarAyuda(char* argv[]) {
        printf("\nUso: %s [IP][:PUERTO] [-h]\n", argv[0]);
        printf("  Opciones:\n");
        printf("\tIP\t IP sobre la cual debe escuchar el servidor\n");
        printf("\tPUERTO\t Puerto en el cual debe escuchar el servidor\n");
        printf("\t-h\t Ayuda\n\n");

        printf("Si no se especifica IP y PUERTO se usan todas las direcciones IP y el puerto 80\n\n");

}

int esNum(const char *val){
    int i = 0, s = 1;
    for(i = 0; i <strlen(val); i++)
        if(isdigit(val[i]) == 0)
            s = 0;
    return s;
}

/*
char* separar(char *arg, char ){
	//PRE: ":" existe en el arg
	char* argcopia=(char*)malloc(sizeof(char)*strlen(arg));
	strcpy(argcopia, arg);
	strsep(&argcopia,":");

}
*/


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

int parseIP(char *ipnum){
	int i=inet_addr(ipnum);
	if(i==INADDR_NONE)
		return 0;
	else
		return 1;
}

int validarHostYPuerto(char *arg, char *ipport[2]){
	//Separar en HOST:IP
	char *argcopia2=strdup(arg);
	char *sup_host=strsep(&argcopia2,":");
	char unaip[INET_ADDRSTRLEN];
	char *sup_port=argcopia2;
	printf("%s %s",sup_host,sup_port);

	if ((strcmp(sup_host,"")!=0) && !dominioValido(sup_host, unaip))
		{
		printf ("Dominio no existe.\n");
		return 1;
		}
	if (!parsePuerto(sup_port)){
		printf ("Puerto invalido\n");
		return 1;
	}
	ipport[0]=&unaip[0];
	ipport[1]=sup_port;
}

void signalHandler(int sig) {
	printf("El servidor terminó correctamente\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    //Manejo de señales
	//ver cuales vamos a manejar
    signal(SIGUSR1, signalHandler);
    signal(SIGCHLD, SIG_IGN);

    if (argc>0 && argc<=4){
    	for (int i=0;i<sizeof(argv);i++){ //Buscar -h
    		if (argv[i]!=0)
    			if (strcmp(argv[i],"-h")==0){
    				mostrarAyuda(argv);
    				exit(1);
    			}
    	}
    }
    else{
    	printf ("Pruebe -h para mas información.\n");
    	exit(EXIT_FAILURE);
    }

    if (argc==2){
    	char *param=strdup(argv[1]);
    	if (strstr(strdup(param),":")){
    		char** ipport = calloc(2, sizeof(char*));
    		validarHostYPuerto(param,ipport);
    		ip=ipport[0];
    		sscanf(ipport[1],"%d",&puerto);
    	}
    	else
    	{//Solo un puerto o solo un host
    		if (esNum(param)){
    			if (!parsePuerto(param)){
    				printf ("Puerto inválido. Pruebe -h para mas información\n");
    		    	exit(EXIT_FAILURE);
    		    }
			ip="0.0.0.0";
			sscanf(param,"%d",&puerto);
    		}else{
    			char unaip[INET_ADDRSTRLEN];
    			if (!dominioValido(param, unaip)){
    				printf ("HOST inválido. Pruebe -h para más información\n");
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

    printf ("-- Iniciando %s en %s:%d --\n",DEMONIO,ip,puerto);

//    daemon(1,1);
    chdir("htdocs/");
    inicializarServidor(ip,puerto);
    exit(EXIT_SUCCESS);
}

