/*
 * signalHandler.c
 *
 *  Creado: 10/05/2011
 *  Autor: team
 */
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
#include "signalHandler.h"


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
