#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include <errno.h>



typedef struct req_struct {
	int codigo;
	char *path;
	char *mime_type;
} response ;

const int HTTP_OK=200;
const int HTTP_FNOTFND=404;
const int HTTP_MNA=405;

const char* HD_HTTP_OK="HTTP/1.0 200 OK\n";
const char* HD_HTTP_FNOTFND="HTTP/1.0 404 Not Found\n";
const char* HD_HTTP_MNA="HTTP/1.0 405 Method Not Allowed\n";

const char* HD_GIF="Content-Type: image/gif;\n";
const char* HD_PNG="Content-Type: image/png;\n";
const char* HD_JPG="Content-Type: image/jpeg;\n";
const char* HD_HTM="Content-Type: text/html; charset=utf-8\n";

const char* DEF_PATH_1="index.html";
const char* DEF_PATH_2="index.htm";
const char* DEF_PATH_3="index.php";
const char* ERROR_PATH="404.html";


char *ejecutarPHP(char *path, char *vars);

 char *extraerDominio(char* string){
	char* stringCpy=(char*)malloc(sizeof(char)*strlen(string));
	strcpy(stringCpy,string);
	strsep(&stringCpy,"/");
	strsep(&stringCpy,"/");
	char* extract;
	extract=strsep(&stringCpy,"/");
	return extract;
}
 //nuevo juan
 char *extraerParametrosPHP(char* string){
 	char* stringCpy=(char*)malloc(sizeof(char)*strlen(string));
 	strcpy(stringCpy,string);
 	if (strncmp(string, "http://",7)==0){ //Asumiendo direccion completa
 		strsep(&stringCpy,"/");
 		strsep(&stringCpy,"/");
 		strsep(&stringCpy,"/");
 		strsep(&stringCpy,"?");
 		return stringCpy;
 	}
 	else{
 		strsep(&stringCpy,"?");
 	}
 	return stringCpy;

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
		//nuevo,acomodar
		char *vars=extraerParametrosPHP(resp->path);
		if(strstr(resp->path,".php")){
			char *tempStr=malloc(strlen(resp->path));
			strcpy(tempStr,resp->path);
			char *path=strsep(&tempStr,"?");
			//char *vars=tempStr; // esta asi porque anaba...para probarlo
			resp->path=ejecutarPHP(path,vars);
		}
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
	resp->codigo=HTTP_MNA;
	resp->mime_type=0;

}

int existeArchivo(char* path){
	if(fopen(path,"r")==0)
		return 0;
	else return 1;
}

int enviarHeader(int flag, int sockfd, char *path){
	if(flag==HTTP_OK){
		send(sockfd, HD_HTTP_OK, strlen(HD_HTTP_OK), 0);
		if(strstr(path,".gif"))
			send(sockfd, HD_GIF, strlen(HD_GIF), 0);
		else if(strstr(path,".png"))
			send(sockfd, HD_PNG, strlen(HD_PNG), 0);
		else if(strstr(path,".jpg") || strstr(path,".jpeg"))
			send(sockfd, HD_JPG, strlen(HD_JPG), 0);
		else if(strstr(path,".html") || strstr(path,".htm")	)
			send(sockfd, HD_HTM, strlen(HD_HTM), 0);
	}
	if (flag==HTTP_FNOTFND){
		send(sockfd, HD_HTTP_FNOTFND, strlen(HD_HTTP_FNOTFND), 0);
		send(sockfd, HD_HTM, strlen(HD_HTM), 0);
	}
	if(flag==HTTP_MNA)
		send(sockfd, HD_HTTP_MNA, strlen(HD_HTTP_MNA), 0);
		send(sockfd, HD_HTM, strlen(HD_HTM), 0);
	send(sockfd,"\n",1, 0);
	return 0;

}

int enviarArchivo(char *path, int sockfd){
	FILE *src=fopen(path,"r");
	unsigned char *datos=(unsigned char *) malloc(sizeof(unsigned char)*1024);
	while (!feof(src)){
		int cant=fread(datos,sizeof(unsigned char),1024,src);
		send(sockfd, datos, sizeof(unsigned char)*cant, 0);
	}
	free(datos);
	fclose(src);
	if(strstr(path,".temp"))
		remove(path);
	return 0;
}



char *ejecutarPHP(char *path, char *vars){
	char *nomTemp=malloc(20);
	srand(time(NULL));
	int num=rand();
	sprintf(nomTemp,"temp%d.temp",num);
	pid_t hijo=fork();
	if(hijo==0){
		int temp= open(nomTemp, O_RDWR | O_CREAT);
		if(vars!=NULL)
			setenv("QUERY_STRING",vars,1);
		dup2(temp,STDOUT_FILENO);
		close(temp);
		if(execl("/usr/bin/php-cgi", "/usr/bin/php-cgi","-q",path,vars,(char *) 0)==-1){
			perror("execl");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
	if(hijo!=0){
		wait(NULL);
	}
	return nomTemp;
}

int dominioValido(char *dominio, char *ipres){

	int iplocal=0;
	struct addrinfo hints, *res, *p;
	char ip[INET_ADDRSTRLEN];

	bzero(&(hints), sizeof hints);

	if (getaddrinfo(dominio, NULL, &hints, &res) != 0)
		return 0;
	//ligamos la primer ip encontrada
	for(p = res;p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		inet_ntop(p->ai_family, addr, ip, sizeof ip);
		printf(ip);
		if (strcmp(ip,"127.0.0.1")==0) iplocal=1;
		if (p==res) strcpy(ipres,ip);
	}
	return iplocal;
}



