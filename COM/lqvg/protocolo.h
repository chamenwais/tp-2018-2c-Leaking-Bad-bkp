/*
 * protocolo.h
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */

#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"

typedef struct stru_obtenerDatos{
	char* path;
	int offset;
	int size;
} t_obtenerDatos;
typedef t_obtenerDatos* tp_obtenerDatos;

void prot_enviar_DMA_FS_path(char* path,int sock);
char* prot_recibir_DMA_FS_path(int sock);
void prot_enviar_DMA_FS_obtenerDatos(char *path, int offset, int size, int sock);
tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(int sock);

#endif /* LQVG_PROTOCOLO_H_ */
