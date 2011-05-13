#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <regex.h>
#include "dataHandler.h"

const int HTTP_OK=200;
const int HTTP_FNOTFND=404;

const char* HD_HTTP_OK="HTTP/1.0 200 OK\n";
const char* HD_HTTP_FNOTFND="HTTP/1.0 404 Not Found\n";

const char* DEF_PATH_1="index.html";
const char* DEF_PATH_2="index.htm";
const char* DEF_PATH_3="index.php";
const char* ERROR_PATH="404.html";

char* pedidoPrincipal(){
	if(existeArchivo(DEF_PATH_1))
		return DEF_PATH_1;
	if(existeArchivo(DEF_PATH_2))
		return DEF_PATH_2;
	if(existeArchivo(DEF_PATH_3))
		return DEF_PATH_3;
	return ERROR_PATH;
}

void procesarPedido(char *string, response *resp){

	int largo=sizeof(char)*strlen(string)-4;
	resp->path=(char*)malloc(largo);
	if(strncmp(string, "GET http://", 7)==0){
		sscanf(string, "GET http://%*d.%*d.%*d.%*d/%s", resp->path);
		if(!existeArchivo(resp->path)){
			resp->codigo=HTTP_FNOTFND;
			resp->mime_type=0;
		}
		else{
			resp->codigo=HTTP_OK;
			resp->mime_type=0;
		}
		return;
	}
	if(strncmp(string, "GET / ",6)==0){
		resp->path=pedidoPrincipal();
		if(strcmp(resp->path,ERROR_PATH)==0)
			resp->codigo=HTTP_FNOTFND;
		else{
			resp->codigo=HTTP_OK;
			resp->mime_type=0;
		}
		return;
	}
	if(strncmp(string, "GET /", 5)==0){
		sscanf(string, "GET /%s", resp->path);
		if(!existeArchivo(resp->path)){
			resp->codigo=HTTP_FNOTFND;
			resp->mime_type=0;
		}
		else{
			resp->codigo=HTTP_OK;
			resp->mime_type=0;
		}
		return;
	}
}

int existeArchivo(char* path){
	if(fopen(path,"r")==0)
		return 0;
	else return 1;
}

int enviarHeader(int flag, int sockfd){
	//flag=codigo de error
	if(flag==HTTP_OK)
		send(sockfd, HD_HTTP_OK, strlen(HD_HTTP_OK), 0);
	else if (flag==HTTP_FNOTFND)
		send(sockfd, HD_HTTP_FNOTFND, strlen(HD_HTTP_FNOTFND), 0);
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



/*
int main(){

	response r;
	procesarPedido("GET http://127.12.3.2/cococ",&r);
	printf ("%d\n",r.codigo);
	printf("%s\n",r.path);


}

*/
