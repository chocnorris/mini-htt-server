#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <regex.h>
#include "dataHandler.h"


 char *extraerDominio(char* string){
	char* stringCpy=(char*)malloc(sizeof(char)*strlen(string));
	strcpy(stringCpy,string);
	strsep(&stringCpy,"/");
	strsep(&stringCpy,"/");
	char* extract;
	extract=strsep(&stringCpy,"/");
	return extract;
}

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

int ejecutarPHP(char *path, char* vars){
	pid_t hijo=fork();
	if(hijo==0){
		setenv("QUERY_STRING",vars,1);
		int num=rand();
		char *nomTemp=malloc(20);
		sprintf(nomTemp,"temp%d",num);
		FILE *temp=fopen(nomTemp,"w+");
		dup2(temp,stdout);
		execl("php-cgi",path);
		free(nomTemp);
	}
	if(hijo!=0){
		int status;
		wait(&status);
	}
}

/*
int main(){

	response r;
	procesarPedido("GET http://127.12.3.2/cococ",&r);
	printf ("%d\n",r.codigo);
	printf("%s\n",r.path);


}

*/
