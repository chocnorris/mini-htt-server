#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <regex.h>
#include "dataHandler.h"

const int HTTP_OK=200;
const int HTTP_FNOTFND=404;

const char* HD_HTTP_OK="HTTP/1.0 200 OK\n";


request procesarPedido(char *string){
	request ret;
	char *path;
	sprintf(string,"GET %s",path);
	return ret;
}

int enviarHeader(int flag, int sockfd){
	//flag=codigo de error
	if(flag==HTTP_OK)
		send(sockfd, HD_HTTP_OK, strlen(HD_HTTP_OK), 0);
	send(sockfd,"\n",1, 0);
	return 0;

}

int enviarHTML(char *path, int sockfd){

	FILE *html=fopen(path,"r");
	char *datos=(char *) malloc(sizeof(char)*1024);
	while (!feof(html)){
		int cant=fread(datos,sizeof(char),1024,html);
		send(sockfd, datos, strlen(datos), 0);
	}
	free(datos);
	fclose(html);
	return 0;
}


int push(char *datos, int *longitud,int sockfd)
{
    int total = 0;     //enviados
    int enviados;
    int restantes = *longitud; // datos que faltan


    while(total < *longitud) {
        enviados = send(sockfd, datos+total, restantes, 0);
        if (enviados == -1) { break; }
        total += enviados;
        restantes -= enviados;
    }

    *longitud = total; // return number actually sent here

    if (enviados==-1) return -1;
    else return 0;
 }
