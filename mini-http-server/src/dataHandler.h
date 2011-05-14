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

void procesarPedido(char*, response*);
int enviarHeader(int,int);
int enviarHTML(char*,int);
int push(char*, int*,int );
