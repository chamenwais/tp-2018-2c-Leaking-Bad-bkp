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

/*** Segmentacion paginada ***/

void crear_estructuras_esquema_segmentacion_paginada();
void cargar_parte_archivo_en_segmento_paginado(int DAM_fd);
void destruir_estructuras_esquema_segmentacion_paginada();
void buscar_informacion_administrativa_esquema_segmentacion_paginada();
void obtener_parte_archivo_con_segmentacion_paginada(int DAM_fd);
void darle_una_linea_al_cpu_segmentacion_paginada(int sock);
void asignar_datos_a_linea_segmentacion_paginada(int sock);
void liberar_archivo_abierto_segmentacion_paginada(int sock);
#endif /* SEGMENTACION_PAGINADA_H_ */
