
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
//#include "signalHandler.h"
#include "connHandler.c"

#define DEMONIO "servidorHTTPd"
#define PID_FILE "/var/run/servidorHTTPd.pid"

int mododebug=0;
int PUERTO=80;
char* IP="127.0.0.1";


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

int parsePuerto(char *puerto){
	//Conversion segura
	//long port = strtol(puerto,puerto+sizeof(puerto),10);

	//if (port>=1 && port<=65535) return 1;
	//else return 0;
	return 1;
}

int parseIP(char *ip){

	return 1;
}

void info(char *msg){
	printf(" -- %s\n", msg);
	syslog(LOG_INFO, msg, DEMONIO);
}

void signalHandler(int sig) {

    switch(sig) {
        case SIGHUP:
            syslog(LOG_WARNING, "Recibida señal SIGHUP");
            break;
        case SIGTERM:
            syslog(LOG_WARNING, "Recibida señal SIGTERM");
            break;
        case SIGUSR1:
			syslog(LOG_WARNING, "Recibida señal SIGUSR1");
        default:
            syslog(LOG_WARNING, "Señal no manejada (%d) %s", strsignal(sig));
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
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGHUP, signalHandler);
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
    if (!parsePuerto(argv[2])) printf ("Puerto inválido. Pruebe -h para mas información\n");
    if (!parseIP(argv[1])) printf ("IP inválida. Pruebe -h para más información\n");

    if (argv[1]) IP=argv[1];
    if (argv[2]) PUERTO=*argv[2];

    printf ("-- Iniciando %s en %s:%d\n",DEMONIO,IP,PUERTO);

    /* Process ID y Session ID */
    pid_t pid, sid;
    //daemon(1,1);
    chdir("htdocs/");
    inicializarServidor(IP,PUERTO);
    printf("terminó un hijo\n");
    exit(EXIT_SUCCESS);
}

