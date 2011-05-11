
/* .h de sistema */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

/* .h de aplicacion */

/* .c de aplicacion */
#include <signalHandler.c>

#define DEMONIO "servidorHTTPd"
#define PID_FILE "/var/run/servidorHTTPd.pid"


/**************************************************************************
    -mostrarAyuda-
    Descripcion:
        Mostrar ayuda y opciones
    Parametros:
        @argc	cantidad de argumentos
        @argv	lista de argumentos
    Ret:
        void
**************************************************************************/
void mostrarAyuda(int argc, char *argv[]) {
    if (argc >=1) {
        printf("Uso: %s -h -nn", argv[0]);
        printf("  Options:n");
        printf("      -ntModo Debug.n");
        printf("      -htMostrar ayuda.n");
        printf("n");
    }
}


/**************************************************************************
    -main-
    Descripcion:
        Funcion principal.
    Parametros:
        @argc	cantidad de parametros
        @argv	lista de parametros
    Returns:
        EXIT_SUCCESS si finaliza normalmente
**************************************************************************/
int main(int argc, char *argv[]) {

    //Manejo de señales
	//ver cuales vamos a manejar
    signal(SIGUSR1, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGHUP, signalHandler);
    mododebug=0;
    int c;
    while( (c = getopt(argc, argv, "nh|help")) != -1) {
        switch(c){
            case 'h':
                mostrarAyuda(argc, argv);
                exit(0);
                break;
            case 'n':
                mododebug = 1;
                break;
            default:
                mostrarAyuda(argc, argv);
                exit(0);
                break;
        }
    }

    syslog(LOG_INFO, "iniciando %s...", DEMONIO);

    // LOG a SYSLOG
#if defined(DEBUG)
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog(DEMONIO, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#else
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DEMONIO, LOG_CONS, LOG_USER);
#endif

    /* Process ID y Session ID */
    pid_t pid, sid;

    if (!mododebug) {

        /* Anular padre */
        pid = fork();
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }
        //pid válido
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }

        /* Cambiar umask ??*/
        umask(0);

        /* Setear Session ID para el hijo */
        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }

        /* Cambiar dir -noseparaque */
        if ((chdir("/")) < 0) {
            /* Log the failure */
            exit(EXIT_FAILURE);
        }

        /* Finalizar descriptores de archivos standard */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    //****************************************************
    // TODO: Insert core of your daemon processing here
    //****************************************************

    syslog(LOG_INFO, "%s daemon exiting", DEMONIO);

    //****************************************************
    // TODO: Free any allocated resources before exiting
    //****************************************************

    return exit(EXIT_SUCCESS);
}

