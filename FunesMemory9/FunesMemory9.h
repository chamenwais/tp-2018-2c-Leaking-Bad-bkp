/*
 * FunesMemory9.h
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#ifndef FUNESMEMORY9_FUNESMEMORY9_H_
#define FUNESMEMORY9_FUNESMEMORY9_H_

/*** Includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>      // Para crear los sockets
#include <netdb.h> 			// Para getaddrinfo
#include <unistd.h> 		// Para close(socket)
#include <commons/log.h>    // Para Logger
#include <commons/string.h> // Para manejo de strings
#include <commons/config.h> // Para Archivo de configuración
#include <signal.h>			// Para manejo de señales
#include <lqvg/com.h>		// Para manejo de comunicaciones

/*** Defines ***/
#define MAX_CLIENTES 20


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


/*** Variables ***/
int MODO_EJECUCION;
int TAMANIO_MEMORIA;
int TAMANIO_MAX_LINEA;
int TAMANIO_PAGINA;
char * PUERTO_ESCUCHA;
t_log * logger;
int total_hilos = 0;
int GLOBAL_SEGUIR = 1;


/*** Funciones ***/
int iniciar_servidor(char * port);
int crear_listen_socket(char * puerto, int max_conexiones);
void iniciar_funes_memory_9();
void cargar_archivo_de_configuracion();
void logger_funesMemory9(int tipo_esc, int tipo_log, const char* mensaje, ...);
void configurar_signals(void);
void inicializar_logger();
void *escuchar_mensajes_entrantes(int socket_cliente);
void captura_sigpipe(int signo);
void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int));
void finalizar_funesMemory9();
struct addrinfo* crear_addrinfo();
int comunicarse_con_dam(int socket_escucha);

#endif /* FUNESMEMORY9_FUNESMEMORY9_H_ */
