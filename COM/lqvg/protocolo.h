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
	char* buffer;
} t_obtenerDatos;
typedef t_obtenerDatos* tp_obtenerDatos;

typedef struct datosObtenidos{
	int tamanio_total_archivo;
	char* buffer;
	int size;
} t_datosObtenidosDeProtocolo;
typedef t_datosObtenidosDeProtocolo* tp_datosObtenidosDeProtocolo;

typedef struct stru_abrirPath{
	char* path;
	int pid;
} t_abrirPath;
typedef t_abrirPath* tp_abrirPath;

typedef struct stru_cargarEnMemoria{
	int pid;
	char* path;
	void* buffer;
	int offset;
	int size;
} t_cargarEnMemoria;
typedef t_cargarEnMemoria* tp_cargarEnMemoria;

typedef struct stru_datosEnMemoria{
	char* path;
	int pid;
	int memory_address;
} t_datosEnMemoria;
typedef t_datosEnMemoria* tp_datosEnMemoria;

void prot_enviar_FS_DMA_datosObtenidos(char* datos, int resultado, int tamanioTotalDelArchivo, int sock);
tp_datosObtenidosDeProtocolo prot_recibir_FS_DMA_datosObtenidos(int sock);
void prot_enviar_DMA_FS_path(char* path,int sock);
char* prot_recibir_DMA_FS_path(int sock);
void prot_enviar_DMA_FS_guardarDatos(char *path, int offset, int size, char *buffer, int sock);
tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos(int sock);
void prot_enviar_DMA_FS_obtenerDatos(char *path, int offset, int size, int sock);
tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(int sock);
void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock);
tp_abrirPath prot_recibir_CPU_DMA_abrirPath(int sock);
void prot_enviar_DMA_FM9_cargarEnMemoria(int pid, char* path, void* buffer, int offset, int size, int sock);
tp_cargarEnMemoria prot_recibir_DMA_FM9_cargarEnMemoria(int sock);
void prot_enviar_FM9_DMA_cargaEnMemoria(int memory_address, int sock);
int prot_recibir_FM9_DMA_cargaEnMemoria(int sock);
void prot_enviar_DMA_SAFA_datosEnMemoria(char* path, int pid, int memory_address, int sock);
tp_datosEnMemoria prot_recibir_DMA_SAFA_datosEnMemoria(int sock);

#endif /* LQVG_PROTOCOLO_H_ */
