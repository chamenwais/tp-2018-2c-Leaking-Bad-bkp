/*
 * contextoCPU.h
 *
 *  Created on: 30 nov. 2018
 *      Author: utnso
 */

#ifndef CONTEXTOCPU_H_
#define CONTEXTOCPU_H_

#include <stdio.h>
#include <stdlib.h> // Para malloc
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <pthread.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/string.h>
#include <lqvg/com.h>
#include <lqvg/protocolo.h>


//Nombres para el archivo de configuracion
#define ARCH_CONFIG_PUERTO_DIEGO "PUERTO_DIEGO"
#define ARCH_CONFIG_PUERTO_SAFA "PUERTO_SAFA"
#define ARCH_CONFIG_IP_SAFA "IP_SAFA"
#define ARCH_CONFIG_IP_DIEGO "IP_DIEGO"
#define ARCH_CONFIG_IP_MEM "IP_MEM"
#define ARCH_CONFIG_PUERTO_MEM "PUERTO_MEM"
#define ARCH_CONFIG_RETARDO "RETARDO"

//Configuracion
extern char * IP_SAFA;
extern char * PUERTO_SAFA;
extern char * IP_DIEGO;
extern char * PUERTO_DIEGO;
extern char * IP_MEM;
extern char * PUERTO_MEM;
extern char * RETARDO;

extern int serverSAFA;
extern int serverDIEGO;
extern int serverMEM;
t_log * logger;

typedef struct {
	enum {
		ABRIR,
		CONCENTRAR,
		ASIGNAR,
		WAIT,
		SIGNAL,
		FLUSH,
		CLOSE,
		CREAR,
		BORRAR
	} tipo_de_operacion;
	union {
		struct {
			char* path;
		} abrir;
		struct {
			char* path;
			char* linea;
			char* datos;
		} asignar;
		struct {
			char* recurso;
		} wait;
		struct {
			char* recurso;
		} signal;
		struct {
			char* path;
		} flush;
		struct {
			char* path;
		} close;
		struct {
			char* path;
			int cantidad_de_lineas;
		} crear;
		struct {
			char* path;
		} borrar;

	} parametros;
		char** liberar; //Para uso de la liberación
} t_operacion;

/*** Enums ***/
enum tipo_logueo { escribir, loguear, escribir_loguear, l_trace, l_debug, l_info, l_warning, l_error};

#endif /* CONTEXTOCPU_H_ */
