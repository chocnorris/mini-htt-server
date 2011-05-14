/*
 * regulares.c
 *
 *  Created on: 13/05/2011
 *      Author: lsf
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <math.h>
/*
int main () {

		char *pattern="hello";
		regex_t expr;
		regmatch_t matched[10];
		int status;
		char *string="helloo";

		regcomp(&expr,pattern,REG_EXTENDED);
		status=regexec(&expr,string,(size_t)10,matched,0);
		regfree(&expr);

		if (status) printf ("no matcheo\n");
		else {
			int fin=matched[0].rm_eo;
			int ini=matched[0].rm_so;
			int cant=abs(ini-fin);
			char *astring=(char*)malloc(sizeof(char)*cant);
			strcpy(astring, &string[ini]);
			printf("matcheo\n");
			printf("%s\n",astring);
			free(astring);
		}

	char** strings = calloc(100, sizeof(char*));
	int i;
	for (i = 0; i < 100; ++i)
	  strings[i] = malloc(100);

	int cant= MatchYObtenerStrings("MAIL FROM:<(.*)>","012345MAIL FROM:<abcd>$", strings);
	printf("%d\n", cant);
	for (i = 0; i < cant; i++){
		printf ("%s\n",strings[i]);
	}


}
*/
int MatchYObtenerStrings(char *pattern, char* toMatch, char **str){
	regex_t expr;
	regmatch_t matched[100];
	int status;
	int cant_matched=0;

	status=regcomp(&expr,pattern,REG_EXTENDED);
	status=regexec(&expr,toMatch,(size_t)100,matched,0);

	if (status) printf ("no matcheo\n");
	else {
		int i;
		for (i=0;i<100;i++){
			int ini=matched[i].rm_so; //puntero a inicio de cadena
			if (ini!=-1 && i!=0){
				int fin=matched[i].rm_eo; //offset final de cadena
				int cant=abs(ini-fin); //obtener longitud
				char *astring=(char*)malloc(sizeof(char)*cant); //alocar segun longitud
				strcpy(astring, &toMatch[ini]); //Copiar cadena en el nuevo buffer
				printf ("%s\n",astring);
				strcpy(str[i],astring);
				cant_matched++;
			}
		}
	}
	return cant_matched;
}


