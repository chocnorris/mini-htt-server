#include <stdio.h>
#include <stdlib.h>
#include <time.h>


sprintf(msg,"HTTP/1.1 200 OK\nDate: %s\nServer: %s\nLast-Modified: %s\nConnection: close\nContent-Length: %d\nContent-Type: text/html\n",fecha_actual,fecha_modif,length);

void enviarPagina(String file){

}

char[] gettime(){
	char s[30];
	size_t i;
	struct tm tim;
	time_t now;
	now = time(NULL);
	tim = *(localtime(&now));
	i = strftime(s,30,"%b %d, %Y; %H:%M:%S\n",&tim);
//esto en perl obtiene como en protocolo HTTP:
	gmstrftime ("%A %d-%b-%y %T %Z", time ());
//obtains the current date and time and puts it into "s" in a format like

//Jan 10, 1987; 17:55:55\n
}
