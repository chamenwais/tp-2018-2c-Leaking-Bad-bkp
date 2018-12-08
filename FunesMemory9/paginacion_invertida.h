/*
 * paginacion_invertida.h
 *
 *  Created on: 8 dic. 2018
 *      Author: utnso
 */

#ifndef PAGINACION_INVERTIDA_H_
#define PAGINACION_INVERTIDA_H_

#include "contexto.h"
#include "utilidades_fm9.h"
#include "utilidades_para_listas.h"

/*** Paginacion invertida ***/
void crear_estructuras_esquema_paginacion_invertida();
void cargar_parte_archivo_en_pagina_invertida(int DAM_fd);
void destruir_estructuras_esquema_paginacion_invertida();
void buscar_informacion_administrativa_esquema_paginacion_invertida();
void obtener_parte_archivo_con_paginacion_invertida(int DAM_fd);

#endif /* PAGINACION_INVERTIDA_H_ */
