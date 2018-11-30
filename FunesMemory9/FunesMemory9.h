/*
 * FunesMemory9.h
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#ifndef FUNESMEMORY9_FUNESMEMORY9_H_
#define FUNESMEMORY9_FUNESMEMORY9_H_

/*** Includes ***/
#include "contexto.h"
#include "utilidades_fm9.h"
#include "segmentacion_pura.h"
#include "utilidades_para_listas.h"

/*** Funciones ***/
void iniciar_funes_memory_9();
void *escuchar_mensajes_de_cpus(int socket_cpu);
void *escuchar_al_diego(int socket_DMA);
void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int));
int comunicarse_con_dam(int socket_escucha);
char * reservar_total_memoria();
int atender_nuevo_cpu(int serv_socket);
void interpretar_mensaje_del_diego(enum MENSAJES mensaje, int DMA_socket);
void inicializar_funciones_variables_por_segmento();
void iniciar_consola();
char** parser_instruccion(char* linea);
void *funcionHiloConsola(void *arg);
void realizar_dump();
void buscar_informacion_administrativa_esquema_segmentacion_paginada();
void buscar_informacion_administrativa_esquema_paginacion_invertida();
void buscar_memoria_real_contenido_relacionado(int id);

/*** Funciones y variables necesarias a intercambiar segun el esquema de memoria utilizado ***/
void (*crear_estructuras_esquema[3])();
void (*cargar_parte_archivo[3])(int);
void (*destruir_estructuras_esquema[3])();

/*** Segmentacion paginada ***/

void crear_estructuras_esquema_segmentacion_paginada();
void cargar_parte_archivo_en_segmento_paginado(int DAM_fd);
void destruir_estructuras_esquema_segmentacion_paginada();

/*** Paginacion invertida ***/
void crear_estructuras_esquema_paginacion_invertida();
void cargar_parte_archivo_en_pagina_invertida(int DAM_fd);
void destruir_estructuras_esquema_paginacion_invertida();

#endif /* FUNESMEMORY9_FUNESMEMORY9_H_ */
