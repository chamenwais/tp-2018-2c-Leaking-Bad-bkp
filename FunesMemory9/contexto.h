/*
 * contexto.h
 *
 *  Created on: 29 nov. 2018
 *      Author: utnso
 */

#ifndef CONTEXTO_H_
#define CONTEXTO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>      // Para crear los sockets
#include <netdb.h> 			// Para getaddrinfo
#include <unistd.h> 		// Para close(socket)
#include <commons/log.h>    // Para Logger
#include <commons/string.h> // Para manejo de strings
#include <commons/config.h> // Para Archivo de configuración
#include <commons/collections/list.h>	// Para manejo de listas
#include <signal.h>			// Para manejo de señales
#include <lqvg/com.h>		// Para manejo de comunicaciones
#include <lqvg/protocolo.h>		// Para el protocolo de comunicaciones
#include <readline/readline.h>
#include <readline/history.h>

/*** Nombres claves de archivo de configuración ***/
#define ARCH_CONFIG_MODO_EJECUCION "modo_ejecucion"
#define ARCH_CONFIG_PUERTO_ESCUCHA "puerto_escucha"
#define ARCH_CONFIG_TAMANIO_MEMORIA "tamanio_memoria"
#define ARCH_CONFIG_TAMANIO_MAX_LINEA "tamanio_max_linea"
#define ARCH_CONFIG_TAMANIO_PAGINA "tamanio_pagina"

/*** Modos de ejecucion ***/
#define SEGMENTACION_PURA 0
#define TABLA_PAGINAS_INVERTIDA 1
#define SEGMENTACION_PAGINADA 2

/*** Enums ***/
enum tipo_logueo { escribir, loguear, escribir_loguear, l_trace, l_debug, l_info, l_warning, l_error};

/*** Defines ***/
#define MAX_CLIENTES 20

extern int MODO_EJECUCION;
extern int TAMANIO_MEMORIA;
extern int TAMANIO_MAX_LINEA;
extern int TAMANIO_PAGINA;
extern char * PUERTO_ESCUCHA;
extern t_log * logger;
extern char * MEMORIA_FISICA;
extern int GLOBAL_SEGUIR;
extern pthread_t threadConsola;

struct archivo_cargandose{
	int pid;
	char * buffer_archivo;
	int recibido_actualmente;
};
typedef struct archivo_cargandose t_archivo_cargandose;

extern t_list * archivos_cargandose;

#endif /* CONTEXTO_H_ */
