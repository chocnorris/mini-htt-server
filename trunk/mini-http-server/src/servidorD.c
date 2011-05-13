
/* .h de sistema */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

/* .h de aplicacion */

/* .c de aplicacion */
#include "connHandler.c"

int puerto;
char* ip;
const char* DEMONIO="servidorHTTP";

/**************************************************************************
    -mostrarAyuda-
    Descripcion:
        Mostrar ayuda y opciones
    Parametros:
        @argc	cantidad de argumentos
        @argv	lista de argumentos
    Ret:
        void
**************************************************************************/
void mostrarAyuda(char* argv[]) {
        printf("\nUso: %s [IP] [:PUERTO] [-h]\n", argv[0]);
        printf("  Opciones:\n");
        printf("\tIP\t IP sobre la cual debe escuchar el servidor\n");
        printf("\tPUERTO\t Puerto en el cual debe escuchar el servidor\n");
        printf("\t-h\t Ayuda\n\n");

        printf("Si no se especifica IP y PUERTO se usa 127.0.0.1:80\n\n");

}

int parsePuerto(char *portnum){
	//Conversion segura
	int port;
	sscanf(portnum,":%d",&port);
	if (port>=1 && port<=65535)
		return 1;
	else
		return 0;
}

int parseIP(char *ipnum){
	int b1,b2,b3,b4;
	sscanf(ipnum,"%d.%d.%d.%d",&b1,&b2,&b3,&b4);
	if (b1>=0 && b1<=255 &&
		b2>=0 && b2<=255 &&
		b3>=0 && b3<=255 &&
		b4>=0 && b4<=255)
		return 1;
	else
		return 0;
}

void signalHandler(int sig) {

    switch(sig) {
        case SIGUSR1:
			exit(EXIT_SUCCESS);
			break;
    }
}


/**************************************************************************
    -main-
    Descripcion:
        Funcion principal.
    Parametros:
        @argc	cantidad de parametros
        @argv	lista de parametros
    Returns:
        EXIT_SUCCESS si finaliza normalmente
**************************************************************************/
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
    	exit(1);
    }
    //Ok con parametros, vemos que tenemos:
    if (argc>2 && !parsePuerto(argv[2])){
    	printf ("Puerto inválido. Pruebe -h para mas información\n");
    	exit(EXIT_FAILURE);
    }
    if (argc>1 && !parseIP(argv[1])){
    	printf ("IP inválida. Pruebe -h para más información\n");
    	exit(EXIT_FAILURE);
    }

    if (argc>1)
    	ip=argv[1];
    else
    	ip="0.0.0.0";

    if (argc>2)
    	sscanf(argv[2],":%d",&puerto);
    else
    	puerto=80;

    printf ("-- Iniciando %s en %s:%d\n",DEMONIO,ip,puerto);

    /* Process ID y Session ID */
    pid_t pid, sid;
    //daemon(1,1);
    chdir("htdocs/");
    inicializarServidor(ip,puerto);
    printf("terminó un hijo\n");
    exit(EXIT_SUCCESS);
}

