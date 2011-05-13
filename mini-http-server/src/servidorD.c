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
#include <ctype.h>
#include "connHandler.c"

int puerto;
char* ip;
const char* DEMONIO="servidorHTTP";

void mostrarAyuda(char* argv[]) {
        printf("\nUso: %s [IP] [:PUERTO] [-h]\n", argv[0]);
        printf("  Opciones:\n");
        printf("\tIP\t IP sobre la cual debe escuchar el servidor\n");
        printf("\tPUERTO\t Puerto en el cual debe escuchar el servidor\n");
        printf("\t-h\t Ayuda\n\n");

        printf("Si no se especifica IP y PUERTO se usa 127.0.0.1:80\n\n");

}

int esNum(const char *val){
    int i = 0, s = 1;
    for(i = 0; i <strlen(val); i++)
        if(isdigit(val[i]) == 0)
            s = 0;
    return s;
}

int parsePuerto(char *portnum){
	char* portStr;
	portStr = portnum+1;
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
	int ret=0;
	char *ipnumCpy=malloc(strlen(ipnum));
	strcpy(ipnumCpy,ipnum);
	char *b1,*b2,*b3,*b4;
	b1=(char*)strsep(&ipnumCpy,".");
	b2=(char*)strsep(&ipnumCpy,".");
	b3=(char*)strsep(&ipnumCpy,".");
	b4=(char*)strsep(&ipnumCpy,".");

	if(b1==NULL || b2==NULL || b3==NULL || b4==NULL)
		ret= 0;
	else
		if(esNum(b1)&&esNum(b2)&&esNum(b3)&&esNum(b4)){
			int ip1,ip2,ip3,ip4;
			sscanf(ipnum,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
			if (ip1>=0 && ip1<=255 &&
				ip2>=0 && ip2<=255 &&
				ip3>=0 && ip3<=255 &&
				ip4>=0 && ip4<=255)
				ret=1;
			else
				ret=0;
		}
		else
			ret=0;
	free(ipnumCpy);
	return ret;
}

void signalHandler(int sig) {

    switch(sig) {
        case SIGUSR1:
			exit(EXIT_SUCCESS);
			break;
    }
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

