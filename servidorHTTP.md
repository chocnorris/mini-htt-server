# Estructura general del programa #

Para implementar el programa se pensó la solución en tres fases:
Invocación del programa, input de usuario, parámetros de entrada e inicio del servicio.
Manejo de las conexiones TCP, un módulo maneja todas las conexiones que se establecen para satisfacer los requerimientos.
Recuperación y envío de los archivos solicitados, incluye la ejecución de las páginas php, el envío de archivos HTML y de imágenes.
Se escribieron tres archivos fuente en los cuales se implementaron las funciones utilizadas, en un programa principal se encuentra la función main() (que se encarga del chequeo y del arranque del servidor) y en los otros dos archivos se implementaron los ítems restantes mencionados anteriormente.

# Funcionamiento del programa #

## Eecución ##
La sintaxis de invocación es la siguiente:
servidorHTTP [IP[:puerto]] | [puerto](puerto.md) | [-h]
NOTA: para poder ligar el servidor al puerto por defecto (80) será necesario ejecutarlo como root.
Una vez que el servidor es ejecutado, se validan los parámetros de entrada, chequeando que tengan la sintaxis correcta. De ser así se lleva el proceso a backrground (demonio), luego se intentan ligar el IP y el puerto, si esta operación tiene éxito, se crea un welcome socket en el puerto elegido (por defecto 80) y el servidor entra en un lazo infinito en el cual irá atendiendo los requerimientos a través del mecanismo de creación de hijos fork().
Cuando el programa entra en el ciclo se bloquea en la sentencia accept() esperando un nuevo requerimiento, al arribar uno el programa hace fork(), el pedido es atendido por el hijo creado y el programa principal se vuelve a bloquear en accept().

## Manejo de los pedidos ##
Luego de el fork(), el hijo está en condición de atender el requerimiento, para esto realiza un recv() en el cual se recibirá el HTTP Request correspondiente. En una primera instancia se procesa el pedido, verificando que tenga la sintaxis correcta y que corresponda con el único método implementado (HTTP GET) para luego conseguir el archivo correspondiente y enviarlo.

## Envío de archivos ##
Para enviar un archivo se busca el mismo en el directorio de trabajo, en este caso htdocs, si no se especifica ninguno se buscarán index.html, index.htm, index.php, en ese orden. Si se trata de un archivo con extensión php se invoca a una función que crea un archivo temporal con el contenido de la ejecución del php, el temporal será enviado como html y luego será eliminado. En caso de no ser encontrado el archivo se enviará un mensaje de error 404.
Una vez recuperado el archivo se determina su extensión, de acuerdo a este valor se envía el header correspondiente y luego se envía el archivo requerido dentro de un ciclo en el cual se leerán y se transmitirán de a un kilobyte.

## Terminación del servicio ##
El servidor finaliza de manera correcta cuando recibe la señal SIGUSR1 y mostrará un mensaje luego de terminar.

# Implementación #

servidorD.c
En este archivo se implementó la función main() que le da inicio, en la misma se realizan  chequeos de la entrada para luego invocar a correrServidor() que dará inicio al servicio. Las otras funciones implementadas son referentes a las validaciones y al manejo de señales.
Antes de llamar a correrServidor() se cambia el directorio de trabajo a htdocs, donde se encuentran las páginas almacenadas en el servidor y se pasa el proceso a background a través de la función daemon().

connHandler.c
En este caso la única operación implementada fue correrServidor(), que se encarga de manejar los sockets, tanto el de bienvenida como los sockets de cada conexión establecida.
Luego de ser invocada la función crea el socket y lo liga con la dirección y el puerto especificados, por defecto el programa atenderá en todas las direcciones asociadas y en el puerto 80. Tras el bind() se entra en un ciclo infinito en el cual se atenderán los pedidos. Cada pedido será atendido por un hijo con el objetivo de poder atender requerimientos en simultáneo. Al tratarse de HTTP/1.0 una vez que el pedido es satisfecho el hijo finaliza su ejecución.

dataHandler.c
Este módulo contiene las funciones relativas a la manipulación de los pedidos y de sus respuestas. Se destacan procesarPedido() que determina el archivo que se debe enviar, enviarHeader() que envía el header correspondiente y enviarArchivo() que se encarga de transmitir el archivo en cuestión.
También se implementaron funciones de control y funciones para el procesamiento de archivos php.


# Funciones #
Responsabilidades principales


_servidorD.c_

`void mostrarAyuda(char* argv[]) `

Muestra una ayuda al usuario.


int esNum(const char **val)**

> Verifica si una cadena está representando un número.


int parsePuerto(char **portStr)**

> Dado un puerto se verifica si es válido según la RFC1700


int parseIP(char **ipnum)**

> Dada una IP verifica si es válida según IPv4.


int validarHostYPuerto(char **arg, char**ipport[2](2.md))

> Dada una cadena de la forma "HOST:PUERTO" se obtiene por separado HOST y 		PUERTO.


int dominioValido(char **dominio, char**ipres)

> Verifica que un nombre de dominio es válido realizando una consulta DNS.
> Se obtiene el IP en caso de un nombre válido.


void signalHandler(int sig)

> Manejador de señales del sistema operativo.


int main(int argc, char **argv[.md](.md))**

> Función main llamada al ejecutar el programa.
> Una finalización con éxito es una interrupción del usuario o que el programa
> llegue a la instrucción final. Lo último nunca sucederá porque el programa es un ciclo 		infinito.



connHandler.c

> int correrServidor(char **ip, int p)**

> Poner servidor a la espera de conexiones y manejar la interacción básica para la respuesta
de solicitudes.

_dataHandler.c_

char **extraerParametrosPHP(char** string)

Devuelve los parámetros a PHP desde la URL. Esto es, la cadena justo después del ‘?’ en 	el URL, para ser pasados al intérprete.


char**pedidoPrincipal()**

Define la ruta o nombre de archivo correspondiente a alguna de las páginas principales, 		devolviendo la primera que exista en el orden: index.html, index.htm, index.php.


void procesarPedido(char **string, response**resp)

Dado un string representando una solicitud HTTP, brinda la información necesaria para  la 	respuesta. Esta información se devuelve usando el tipo de dato struct “response”, en el 		cual definen los campos “codigo” y “path”.


int existeArchivo(char**path)**

Dada la ruta a un archivo, verifica si este existe.


void send2(int fd, const void**buf, size\_t n, int flags, char**action)

Extensión de send() para generalizar diagnóstico de errores. En caso de 	que la llamada a 	send() falle se termina el llamador y se reporta el tipo de error junto a la acción 			que se estaba llevando a cabo. La motivación de tal función fue por legilibilidad, debido a 		que la 	cantidad de llamadas a send() que se debían realizar con el control de errores 		correpondientes era alta.


int enviarHeader(int flag, int sockfd, char **path)**

De acuerdo tipo de respuesta (flag) se envía el header que corresponda al cliente. Estos headers estan definidos como constantes HD**_en dataHandler.c._**

int enviarArchivo(char **path, int sockfd)**

Lee un archivo y lo envía al cliente.


char **ejecutarPHP(char**path, char **vars)**

Crea un proceso hijo que realiza una llamada a php-cgi.


_servidorD.c_

void mostrarAyuda(char**argv[.md](.md))**

Muestra una ayuda al usuario.


int esNum(const char **val)**

> Verifica si una cadena está representando un número.


int parsePuerto(char **portStr)**

> Dado un puerto se verifica si es válido según la RFC1700


int parseIP(char **ipnum)**

> Dada una IP verifica si es válida según IPv4.


int validarHostYPuerto(char **arg, char**ipport[2](2.md))

> Dada una cadena de la forma "HOST:PUERTO" se obtiene por separado HOST y 		PUERTO.


int dominioValido(char **dominio, char**ipres)

> Verifica que un nombre de dominio es válido realizando una consulta DNS.
> Se obtiene el IP en caso de un nombre válido.


void signalHandler(int sig)

> Manejador de señales del sistema operativo.


int main(int argc, char **argv[.md](.md))**

> Función main llamada al ejecutar el programa.
> Una finalización con éxito es una interrupción del usuario o que el programa
> llegue a la instrucción final. Lo último nunca sucederá porque el programa es un ciclo 		infinito.