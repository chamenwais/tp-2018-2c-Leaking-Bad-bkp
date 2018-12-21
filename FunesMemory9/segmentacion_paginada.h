/*
 * segmentacion_paginada.h
 *
 *  Created on: 8 dic. 2018
 *      Author: utnso
 */

#ifndef SEGMENTACION_PAGINADA_H_
#define SEGMENTACION_PAGINADA_H_

#include "contexto.h"
#include "utilidades_fm9.h"
#include "utilidades_para_listas.h"

struct entrada_tabla_segmentos_pag{
	t_list * base;
	int limite;
	char * archivo;
};
typedef struct entrada_tabla_segmentos_pag t_entrada_tabla_segmentos_pag;


/*** Segmentacion paginada ***/

void crear_estructuras_esquema_segmentacion_paginada();
void cargar_parte_archivo_en_segmento_paginado(int DAM_fd);
void destruir_estructuras_esquema_segmentacion_paginada();
void buscar_informacion_administrativa_esquema_segmentacion_paginada();
void obtener_parte_archivo_con_segmentacion_paginada(int DAM_fd);
void darle_una_linea_al_cpu_segmentacion_paginada(int sock);
void asignar_datos_a_linea_segmentacion_paginada(int sock);
void liberar_archivo_abierto_segmentacion_paginada(int sock);
int actualizar_tabla_segmentos_paginados(int pid, char * path, int marco, int ultima_linea_marco, int pagina);
void inicializar_bitmap_de_marcos_libres();
bool hay_marcos_libres();
bool el_marco_esta_libre(void* marco);
int contar_marcos_libres();
bool archivo_ocupa_mas_marcos_que_disponibles(int lineas);
t_list* obtener_marcos_libres();
int copiar_archivo_en_marcos_libres(char * archivo, int lineas, t_list * marcos_libres, char * path, int dtb);
bool marco_de_menor_a_mayor(void* marco1, void* marco2);
int crear_nueva_entrada_tabla_de_segmentos_paginados(char * path, int marco
		, int ultima_linea_marco, int pagina, t_tabla_segmentos* p_tabla_segmentos);
int agregar_nueva_tabla_segmentos_paginados_para_proceso(int pid, char* path, int marco
		, int ultima_linea_marco, int pagina);
bool el_segmento_paginado_tiene_paginas(t_tabla_segmentos* p_tabla_segmentos, char * path);
t_entrada_tabla_segmentos_pag * obtener_entrada_segmentos_relacionada_al_path(t_tabla_segmentos* p_tabla_segmentos
		, char * path);
t_entrada_tabla_paginas* crear_nueva_entrada_tabla_paginas(int marco, int pagina);

#endif /* SEGMENTACION_PAGINADA_H_ */
