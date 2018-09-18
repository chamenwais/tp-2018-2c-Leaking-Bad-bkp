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

tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(char *path, int offset, int size, int sock){
	int tam;
	tp_obtenerDatos recibido = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	recibido->path = malloc(tam);
	recibir(sock,recibido->path,tam);
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	recibir(sock,&(recibido->size),sizeof(recibido->size));
	return recibido;
}
