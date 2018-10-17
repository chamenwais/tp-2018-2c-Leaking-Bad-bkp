/*
 * protocolo.c
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */
#include "protocolo.h"

void prot_enviar_DMA_FS_path(char* path,int sock){
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
}

char* prot_recibir_DMA_FS_path(int sock){
	char* path;
	int size;
	recibir(sock,&size,sizeof(size));
	path = malloc(size);
	recibir(sock,path,size);
	return path;
}

void prot_enviar_DMA_FS_obtenerDatos(char *path, int offset, int size, int sock){
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
	return;
}

tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(int sock){
	int tam;
	tp_obtenerDatos recibido = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	recibido->path = malloc(tam);
	recibir(sock,recibido->path,tam);
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	recibir(sock,&(recibido->size),sizeof(recibido->size));
	return recibido;
}

void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock){
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&pid,sizeof(pid));
}

tp_abrirPath prot_recibir_CPU_DMA_abrirPath(int sock){
	int size;
	tp_abrirPath recibido = malloc(sizeof(tp_abrirPath));
	recibir(sock,&size,sizeof(size));
	recibido->path = malloc(size);
	recibir(sock,recibido->path,size);
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
	return recibido;
}

void prot_enviar_FS_DMA_devolverDatos(void* buffer, int sock){
	int tam = sizeof(&buffer);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,buffer,tam);
}

void* prot_recibir_FS_DMA_devolverDatos(int sock){
	int size;
	void* buffer;
	recibir(sock,&size,sizeof(size));
	buffer = malloc(size);
	recibir(sock,buffer,size);
	return buffer;
}

void prot_enviar_DMA_FM9_cargarEnMemoria(char* path, void* buffer, int offset, int size, int sock){
	int path_size = sizeof(&path);
	int buffer_size = sizeof(&buffer);
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,path,path_size);
	enviar(sock,&buffer_size,sizeof(buffer_size));
	enviar(sock,buffer,buffer_size);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
}

tp_cargarEnMemoria prot_recibir_DMA_FM9_cargarEnMemoria(int sock){
	int path_size;
	int buffer_size;
	tp_cargarEnMemoria recibido = malloc(sizeof(tp_cargarEnMemoria));
	recibir(sock,&path_size,sizeof(path_size));
	recibido->path = malloc(path_size);
	recibir(sock,recibido->path,path_size);
	recibir(sock,&buffer_size,sizeof(buffer_size));
	recibido->buffer = malloc(buffer_size);
	recibir(sock,recibido->buffer,buffer_size);
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	recibir(sock,&(recibido->size),sizeof(recibido->size));
	return recibido;
}

void prot_enviar_FM9_DMA_cargaEnMemoria(int base, int offset, int sock){
	enviar(sock,&base,sizeof(base));
	enviar(sock,&offset,sizeof(offset));
}

tp_cargaEnMemoria prot_recibir_FM9_DMA_cargaEnMemoria(int sock){
	tp_cargaEnMemoria recibido = malloc(sizeof(tp_cargaEnMemoria));
	recibir(sock,&(recibido->base),sizeof(recibido->base));
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	return recibido;
}

void prot_enviar_DMA_SAFA_datosEnMemoria(char* path, int pid, int base, int offset, int sock){
	int path_size = sizeof(&path);
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,path,path_size);
	enviar(sock,&pid,sizeof(pid));
	enviar(sock,&base,sizeof(base));
	enviar(sock,&offset,sizeof(offset));
}

tp_datosEnMemoria prot_recibir_DMA_SAFA_datosEnMemoria(int sock){
	int path_size;
	tp_datosEnMemoria recibido = malloc(sizeof(tp_datosEnMemoria));
	recibir(sock,&path_size,sizeof(path_size));
	recibido->path = malloc(path_size);
	recibir(sock,recibido->path,path_size);
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
	recibir(sock,&(recibido->base),sizeof(recibido->base));
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	return recibido;
}
