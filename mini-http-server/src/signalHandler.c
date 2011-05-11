/*
 * signalHandler.c
 *
 *  Creado: 10/05/2011
 *  Autor: team
 */
#include <syslog.h>
#include <stdlib.h>


/**************************************************************************
    -signalHandler-
    Descripcion:
        Maneja las señales recibidas
    Parametros:
        @sig	Señal recibida
    Ret:
        void
**************************************************************************/
void signalHandler(int sig) {

    switch(sig) {
        case SIGHUP:
            syslog(LOG_WARNING, "Recibida señal SIGHUP");
            break;
        case SIGTERM:
            syslog(LOG_WARNING, "Recibida señal SIGTERM");
            break;
        case SIGUSR1:
			syslog(LOG_WARNING, "Recibida señal SIGUSR1");
        default:
            syslog(LOG_WARNING, "Señal no manejada (%d) %s", strsignal(sig));
            break;
    }
}
