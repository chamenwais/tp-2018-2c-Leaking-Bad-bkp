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
#include "segmentacion_paginada.h"
#include "paginacion_invertida.h"
#include "utilidades_para_listas.h"

/*** Funciones ***/
void stdin_no_bloqueante(void);
void iniciar_funes_memory_9(char * path);
int atender_nuevo_cpu(int serv_socket);
void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int));
t_cabecera recibir_mensaje_cpu(t_conexion_cpu conexion_cpu);
bool cabecera_es_invalida_dam(t_cabecera cabecera);
bool cabecera_es_invalida_cpu(t_cabecera cabecera);
void interpretar_mensaje_cpu(enum MENSAJES mensaje, int cpu_socket);
void interpretar_mensaje_del_diego(enum MENSAJES mensaje, int DMA_socket);
t_cabecera recibir_mensaje_dam(int socket_DMA);
int comunicarse_con_dam(int socket_escucha);
char *reservar_total_memoria();
void inicializar_funciones_variables_por_segmento();
int consola_obtener_key_comando(char* comando);
void consola_obtener_parametros(char* buffer, char** comando, char** parametro1);
void realizar_dump(int id);
int consola_derivar_comando(char * buffer);
void inicializar_conexiones_cpu(void);
void *consola();
int consola_leer_stdin(char *read_buffer, size_t max_len);
void validar_parametro_consola(char ** parametro);
void limpiar_token_consola(char* token);

/*** Funciones y variables necesarias a intercambiar segun el esquema de memoria utilizado ***/
void (*crear_estructuras_esquema[3])();
void (*cargar_parte_archivo[3])(int);
void (*buscar_informacion_administrativa[3])(int);
void (*obtener_parte_archivo[3])(int);
void (*darle_una_linea_al_cpu[3])(int);
void (*asignar_datos_a_linea[3])(int);
void (*liberar_archivo_abierto[3])(int);

#endif /* FUNESMEMORY9_FUNESMEMORY9_H_ */
