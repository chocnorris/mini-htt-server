
/*****************************************************************************
 *
 * 	-dataHandler.c-
 * 	Descripción
 * 		Implementa funciones que involucran el procesamiento y envío de datos.
 *
 *****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include <errno.h>

/*
 * response: Estructura que encapsula la información de una respuesta
 * 	codigo	Código de respuesta según RFC1945 de HTTP
 * 	path	Ruta del archivo a enviar como respuesta
 *
 */
typedef struct req_struct {
	int codigo;
	char *path;
} response ;


/* Códigos de respuesta soportados */

const int HTTP_OK=200;
const int HTTP_FNOTFND=404;
const int HTTP_MNA=405;


/* HEADERs de respuesta soportados */

const char* HD_HTTP_OK="HTTP/1.0 200 OK\n";
const char* HD_HTTP_FNOTFND="HTTP/1.0 404 Not Found\n";
const char* HD_HTTP_MNA="HTTP/1.0 405 Method Not Allowed\n";
const char* HD_GIF="Content-Type: image/gif;\n";
const char* HD_PNG="Content-Type: image/png;\n";
const char* HD_JPG="Content-Type: image/jpeg;\n";
const char* HD_HTM="Content-Type: text/html; charset=utf-8\n";


/* Rutas predefinidas */

const char* DEF_PATH_1="index.html";
const char* DEF_PATH_2="index.htm";
const char* DEF_PATH_3="index.php";
const char* ERROR_PATH="404.html";
const char* NOTIMPL_PATH="405.html";


char *ejecutarPHP(char *path, char *vars);

/***************************************************************
 * 	-extraerDominio-
 * Descripción:
 * 		Dada una cadena en de la forma "http://dir/" extrae <dir>
 * Parámetros
 * 		@string		Cadena de la cual extraer <dir>
 * Retorno:
 * 		<dir>
 ***************************************************************/
char *extraerDominio(char* string){
	char* stringCpy=(char*)malloc(sizeof(char)*strlen(string));
	strcpy(stringCpy,string);
	strsep(&stringCpy,"/");
	strsep(&stringCpy,"/");
	char* extract;
	extract=strsep(&stringCpy,"/");
	return extract;
}

/********************************************************************
 * 	-extraerParametrosPHP-
 * 	Descripción
 * 		Dada una cadena en de la forma
 * 		"http://dir/archivo.php?param1=val1&param2=val2..." se extrae
 * 		param1=val1&param2=val2... (parámetros a php)
 * 	Parámetros
 * 		@string		Cadena de la cual extraer parámetros
 * 	Retorno
 * 		param1=val1&param2=val2...
 ***************************************************************/
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

 /********************************************************************
  *	 -pedidoPrincipal-
  *  Descripción
  *		Maneja la respuesta del parámetro ruta de archivo para los casos
  *		obtención de página principal
  *  Parámetros
  *  Retorno:
  * 		index.html (DEF_PATH_1) ó
  * 		index.htm (DEF_PATH_2) ó
  * 		index.php (DEF_PATH_3) ó
  * 		404.html (ERROR_PATH)
  *********************************************************************/
char* pedidoPrincipal(){
	if(existeArchivo(DEF_PATH_1))
		return DEF_PATH_1;
	if(existeArchivo(DEF_PATH_2))
		return DEF_PATH_2;
	if(existeArchivo(DEF_PATH_3))
		return DEF_PATH_3;
	return ERROR_PATH;
}

/***************************************************************************
 *	-procesarPedido-
 *	Descripción
 *		Dado un pedido lo procesa y brinda la información de respuesta.
 *	Parámetros
 *		@string		Pedido que envía el navegador.
 *		@resp		Información de respuesta que debe enviar el servidor.
 *
 ***************************************************************************/
void procesarPedido(char *string, response *resp){

	int largo=sizeof(char)*strlen(string)-4;
	resp->path=(char*)malloc(largo);
	if(strncmp(string, "GET / ",6)==0){
		resp->path=pedidoPrincipal();
		if(strcmp(resp->path,ERROR_PATH)==0)
			resp->codigo=HTTP_FNOTFND;
		else
			resp->codigo=HTTP_OK;
		return;
	}
	if(strncmp(string, "GET /", 5)==0){

		sscanf(string, "GET /%s", resp->path);
		char *vars=extraerParametrosPHP(resp->path);
		if(strstr(resp->path,".php")){
			char *tempStr=malloc(strlen(resp->path));
			strcpy(tempStr,resp->path);
			char *path=strsep(&tempStr,"?");
			resp->path=ejecutarPHP(path,vars);
		}
		if(!existeArchivo(resp->path)){
			resp->codigo=HTTP_FNOTFND;
		}
		else{
			resp->codigo=HTTP_OK;
		}
		return;
	}
	resp->codigo=HTTP_MNA;

}

/**************************************************************************************
 *	-existeArchivo-
 *	Descripción
 *		Dada la ruta a un archivo verifica que si este existe.
 *	Parámetros
 *		@path	Ruta al archivo
 *	Retorno
 *		1 si el archivo existe
 *		0 en caso contrario
 **************************************************************************************/
int existeArchivo(char* path){
	FILE *a;
	if ( (a=fopen(path,"r"))==NULL)
		return 0;
	else {
		fclose(a);
		return 1;
	}
}

/**************************************************************************************
 *	-send2-
 *	Descripción
 *		Extensión de send() para generalizar diagnóstico de errores en esta llamada.
 *		En caso de que la llamada a send() felle se termina el llamador y se reporta el
 *		tipo de error junto a la acción que se estaba llevando a cabo.
 *	Parámetros
 *		@fd
 *		@buf
 *		@n
 *		@flags	Como send(fd,buf,n,flags)
 *		@action	String que contiene un mensaje de reporte de la acción que
 *				se está realizando
 *	Retorno
 *		void
 **************************************************************************************/
void send2(int fd, const void* buf, size_t n, int flags, char *action){
	if (send(fd,buf,n,flags)==-1){
		perror(action);
		printf("nro error= %d.\n", errno);
		exit(EXIT_FAILURE);
	}
}

/**************************************************************************************
 *	-enviarHeader-
 *	Descripción
 *		De acuerdo tipo de respuesta se envía el header que corresponda.
 *		Ver flags de HD.
 *	Parámetros
 *		@flag	Tipo de respuesta según protocolo HTTP.
 *		@sockfd	Descriptor de archivo de la conexión actual.
 *		@path	Ruta al archivo que requirió el cliente.
 *	Retorno
 *		0 en caso de éxito
 *		indefinido en caso contrario
 **************************************************************************************/
int enviarHeader(int flag, int sockfd, char *path){
	if(flag==HTTP_OK){
		send2(sockfd, HD_HTTP_OK, strlen(HD_HTTP_OK), 0,"Enviando header.");

		if(strstr(path,".gif"))
			send2(sockfd, HD_GIF, strlen(HD_GIF), 0,"Enviando GIF.");
		else if(strstr(path,".png"))
			send2(sockfd, HD_PNG, strlen(HD_PNG), 0,"Enviando PNG.");
		else if(strstr(path,".jpg") || strstr(path,".jpeg"))
			send2(sockfd, HD_JPG, strlen(HD_JPG), 0, "Enviando JPG.");
		else if(strstr(path,".html") || strstr(path,".htm") || strstr(path,".temp")	)
			send2(sockfd, HD_HTM, strlen(HD_HTM), 0, "Enviando HTML.");
	}
	if (flag==HTTP_FNOTFND){
		send2(sockfd, HD_HTTP_FNOTFND, strlen(HD_HTTP_FNOTFND), 0, "Enviando header.");
		send2(sockfd, HD_HTM, strlen(HD_HTM), 0, "Enviando HTML.");
	}
	if(flag==HTTP_MNA)
		send2(sockfd, HD_HTTP_MNA, strlen(HD_HTTP_MNA), 0, "Enviando header.");
		send2(sockfd, HD_HTM, strlen(HD_HTM), 0, "Enviando HTML.");
	send2(sockfd,"\n",1, 0, "Enviando CR.");
	return 0;

}

/**************************************************************************************
 *	-enviarArchivo-
 *	Descripción
 *		Lee un archivo y lo envía al cliente.
 *	Parámetros
 *		@path	Ruta del archivo a enviar
 *		@sockfd	Descriptor de archivo a través del cual enviar el archivo
 *	Retorno
 *		0 en caso de éxito
 *		indefinido en caso contrario
 **************************************************************************************/
int enviarArchivo(char *path, int sockfd){
	FILE *src=fopen(path,"r");

	if (src==NULL){
		perror("Leyendo archivo.");
		printf("nro error= %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	unsigned char *datos=(unsigned char *) malloc(sizeof(unsigned char)*1024);
	while (!feof(src)){
		int cant=fread(datos,sizeof(unsigned char),1024,src);
		send2(sockfd, datos, sizeof(unsigned char)*cant, 0,"Enviando archivo.");
	}
	free(datos);
	fclose(src);
	if(strstr(path,".temp"))
		remove(path);
	return 0;
}



/**************************************************************************************
 *	-ejecutarPHP-
 *	Descripción
 *		Crea un proceso hijo que realiza una llamada a php-cgi
 *	Parámetros
 *		@path Ruta del archivo .php a interpretar.
 *		@vars param1=val&param2=val... (parámetros para el archivo php)
 *	Retorno
 *		nombre del archivo temporal en el cual se guardó la salida que devolvió la
 *		interpretación del código php
 **************************************************************************************/
char *ejecutarPHP(char *path, char *vars){
	char *nomTemp=malloc(20);
	srand(time(NULL));
	int num=rand();
	sprintf(nomTemp,"temp%d.temp",num,S_IRWXU);
	pid_t hijo=fork();
	if(hijo==0){
		int temp= open(nomTemp, O_RDWR | O_CREAT);
		dup2(temp,STDOUT_FILENO);
		close(temp);
		freopen("/dev/null", "w", stderr);
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



