#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <regex.h>
#include <unistd.h>

typedef struct req_struct {
	int codigo;
	char *path;
	char *mime_type;
} response ;

const int HTTP_OK=200;
const int HTTP_FNOTFND=404;

const char* HD_HTTP_OK="HTTP/1.0 200 OK\n";
const char* HD_HTTP_FNOTFND="HTTP/1.0 404 Not Found\n";

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

 int dominioValido(char *dominio){

	int iplocal=0;
	struct addrinfo hints, *res, *p;
	char ip[INET_ADDRSTRLEN];
	bzero(&(hints), sizeof hints);

	if (getaddrinfo(dominio, NULL, &hints, &res) != 0)
		return 0;
	//IPs
	for(p = res;p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;
		//Obtener IPs
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		inet_ntop(p->ai_family, addr, ip, sizeof ip);
		if (strcmp(ip,"127.0.0.1")==0) iplocal=1;
	}
	return iplocal;
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
		if(strstr(resp->path,".php")){
			char *tempStr=malloc(strlen(resp->path));
			strcpy(tempStr,resp->path);
			char *path=strsep(&tempStr,"?");
			char *vars=tempStr;
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

char *ejecutarPHP(char *path, char *vars){
	char *nomTemp=malloc(20);
	srand(time(NULL));
	int num=rand();
	sprintf(nomTemp,"temp%d",num);
	printf("\n%s\n",nomTemp);
	pid_t hijo=fork();
	if(hijo==0){
		int temp= open(nomTemp, O_RDWR | O_CREAT);
		if(vars!=NULL)
			setenv("QUERY_STRING",vars,1);
		dup2(temp,STDOUT_FILENO);
		close(temp);
		execl("/usr/bin/php5-cgi", "/usr/bin/php5-cgi",path,(char *) 0);
		exit(EXIT_SUCCESS);
	}
	if(hijo!=0){
		wait(NULL);
	}
	return nomTemp;
}



