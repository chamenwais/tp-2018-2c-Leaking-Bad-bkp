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
char * MEMORIA_FISICA;

/*** Funciones ***/
int iniciar_servidor(char * port);
int crear_listen_socket(char * puerto, int max_conexiones);
void iniciar_funes_memory_9();
void cargar_archivo_de_configuracion();
void logger_funesMemory9(int tipo_esc, int tipo_log, const char* mensaje, ...);
void configurar_signals(void);
void inicializar_logger();
void *escuchar_mensajes_de_cpus(int socket_cpu);
void *escuchar_al_diego(int socket_DMA);
void captura_sigpipe(int signo);
void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int));
void finalizar_funesMemory9();
struct addrinfo* crear_addrinfo();
int comunicarse_con_dam(int socket_escucha);
char * reservar_total_memoria();
int atender_nuevo_cpu(int serv_socket);
void interpretar_mensaje_del_diego(enum MENSAJES mensaje, int DMA_socket);
void cerrar_sockets(int server_FM9, int socket_cpu, int cliente_DAM);
void inicializar_funciones_variables_por_segmento();
int list_count_satisfying_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);
bool list_any_satisfy_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);
t_list* list_filter_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);

/*** Funciones y variables necesarias a intercambiar segun el esquema de memoria utilizado ***/
void (*crear_estructuras_esquema[3])();
void (*cargar_parte_archivo[3])(int);
void (*destruir_estructuras_esquema[3])();

/*** Segmentacion pura ***/
struct entrada_tabla_segmentos{
	int base;
	int limite;
	char * archivo;
};
typedef struct entrada_tabla_segmentos t_entrada_tabla_segmentos;

struct tabla_segmentos{
	int pid;
	t_list * entradas;
};
typedef struct tabla_segmentos t_tabla_segmentos;

struct hueco{
	int base;
	int limite;
};
typedef struct hueco t_hueco;

struct archivo_cargandose{
	int pid;
	char * buffer_archivo;
	int recibido_actualmente;
};
typedef struct archivo_cargandose t_archivo_cargandose;

t_list * tablas_de_segmentos=NULL;
t_list * lista_de_huecos=NULL;
t_list * archivos_cargandose=NULL;
void inicializar_lista_de_huecos();
bool es_un_proceso_conocido(void * tabla_segmentos);
void agregar_entrada_tabla_segmentos(tp_cargarEnMemoria nombre_archivo, t_list* entradas_segmentos);
int el_proceso_tiene_tabla_de_segmentos();
void agregar_nueva_tabla_segmentos_para_proceso(tp_cargarEnMemoria parte_archivo);
void crear_estructuras_esquema_segmentacion();
void cargar_parte_archivo_en_segmento(int DAM_fd);
void destruir_estructuras_esquema_segmentacion();
void eliminar_lista_de_entradas(void * tabla_segmentos);
t_archivo_cargandose* cargar_buffer_archivo(tp_cargarEnMemoria parte_archivo);
void informar_espacio_insuficiente(int DAM_fd);
t_hueco* tomar_hueco_con_limite_mas_grande();
size_t archivo_mas_grande_que_hueco(const t_archivo_cargandose* archivo_de_proceso_cargandose,const t_hueco* hueco);
size_t archivo_igual_al_hueco(const t_archivo_cargandose* archivo_cargado,const t_hueco* hueco_usado);
t_hueco* tomar_hueco();
void actualizar_info_tabla_de_huecos(size_t tamanio_archivo_en_memoria,t_archivo_cargandose* archivo_de_proceso_cargandose, t_hueco* hueco);
char * separar_en_lineas(char * buffer_archivo);

/*** Segmentacion paginada ***/

void crear_estructuras_esquema_segmentacion_paginada();
void cargar_parte_archivo_en_segmento_paginado(int DAM_fd);
void destruir_estructuras_esquema_segmentacion_paginada();

/*** Paginacion invertida ***/
void crear_estructuras_esquema_paginacion_invertida();
void cargar_parte_archivo_en_pagina_invertida(int DAM_fd);
void destruir_estructuras_esquema_paginacion_invertida();

#endif /* FUNESMEMORY9_FUNESMEMORY9_H_ */
