typedef struct req_struct {
	int codigo;
	char *path;
	char *mime_type;
} response ;

void procesarPedido(char*, response*);
int enviarHeader(int,int);
int enviarHTML(char*,int);
int push(char*, int*,int );
