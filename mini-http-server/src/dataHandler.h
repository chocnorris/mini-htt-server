typedef struct req_struct {
	int codigo;
	char *path;
	char *mime_type;
} request ;

request procesarPedido(char *);
int enviarHeader(int, int);
int enviarHTML(char *, int);
int push(char *, int *,int );
