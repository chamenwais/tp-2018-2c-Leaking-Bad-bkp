/*
 * protocolo.c
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */
#include "protocolo.h"

void prot_enviar_FS_DMA_datosObtenidos(char* datos, int resultado, int tamanioTotalDelArchivo, int sock){
	//1 envia
	enviar(sock,&resultado,sizeof(resultado));
	if(resultado==DatosObtenidos){
		//enviar(sock,&size,sizeof(size));
		enviar(sock,&tamanioTotalDelArchivo,sizeof(tamanioTotalDelArchivo));
		int tam = sizeof(&datos);
		enviar(sock,&tam,sizeof(tam));
		enviar(sock,datos,tam);
	}
}

tp_datosObtenidosDeProtocolo prot_recibir_FS_DMA_datosObtenidos(int sock){
	//1 recibir
	int resultado;
	tp_datosObtenidosDeProtocolo obtenidos=NULL;
	recibir(sock,&resultado,sizeof(resultado));
	if(resultado==DatosObtenidos){
		//en este caso tengo datos para recibir
		obtenidos = malloc(sizeof(tp_datosObtenidosDeProtocolo));
		recibir(sock,&(obtenidos->tamanio_total_archivo),sizeof(obtenidos->tamanio_total_archivo));
		recibir(sock,&(obtenidos->size),sizeof(&(obtenidos->size)));
		obtenidos->buffer = malloc(obtenidos->size);
		recibir(sock,obtenidos->buffer,obtenidos->size);
	}
	return obtenidos;
}

void prot_enviar_DMA_FS_obtenerDatos(char *path, int offset, int size, int sock){
	//2 envia
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
	return;
}

tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(int sock){
	//2 recibe
	int tam;
	tp_obtenerDatos recibido = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	recibido->path = malloc(tam);
	recibir(sock,recibido->path,tam);
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	recibir(sock,&(recibido->size),sizeof(recibido->size));
	return recibido;
}

void prot_enviar_DMA_FS_path(char* path,int sock){
	//3 recibe
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
}

char* prot_recibir_DMA_FS_path(int sock){
	//3 envia
	char* path;
	int size;
	recibir(sock,&size,sizeof(size));
	path = malloc(size);
	recibir(sock,path,size);
	return path;
}

void prot_enviar_int(int i,int sock){
	//9 recibe
	enviar(sock,&i,sizeof(i));
}

int prot_recibir_int(int sock){
	//9 envia
	int i;
	recibir(sock,&i,sizeof(i));
	return i;
}

void prot_enviar_DMA_FS_guardarDatos(char *path, int offset, int size, char *buffer, int sock){
	//4 envia
	int tam = strlen(path) + 1;
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
	int tam2 = strlen(buffer) + 1;
	enviar(sock,&tam2,sizeof(tam2));
	enviar(sock,buffer,tam2);
	//printf("enviando: %s, size:%d | %d | %d | %s size:%d\n",path,tam,offset,size,buffer,tam2);
	return;
}

tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos(int sock){
	//4 recibe
	int tam;
	tp_obtenerDatos datos = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	datos->path = malloc(tam);
	recibir(sock,(datos->path),tam);
	recibir(sock,&(datos->offset),sizeof(datos->offset));
	recibir(sock,&(datos->size),sizeof(datos->size));
	int tam2;
	recibir(sock,&tam2,sizeof(tam2));
	datos->buffer = malloc(tam2);
	recibir(sock,(datos->buffer),tam2);
	return datos;
}

void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock){
	//5 envia
	int tam = sizeof(&path);
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&pid,sizeof(pid));
}

tp_abrirPath prot_recibir_CPU_DMA_abrirPath(int sock){
	//5 recibe
	int size;
	tp_abrirPath recibido = malloc(sizeof(tp_abrirPath));
	recibir(sock,&size,sizeof(size));
	recibido->path = malloc(size);
	recibir(sock,recibido->path,size);
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
	return recibido;
}

void prot_enviar_DMA_FM9_cargarEnMemoria(int pid, char* path, char* buffer, int offset, int size, int sock){
	//6 envia
	int path_size = sizeof(&path);
	int buffer_size = sizeof(&buffer);
	enviar(sock,&pid, sizeof(pid));
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,path,path_size);
	enviar(sock,&buffer_size,sizeof(buffer_size));
	enviar(sock,buffer,buffer_size);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
}

tp_cargarEnMemoria prot_recibir_DMA_FM9_cargarEnMemoria(int sock){
	//6 recibe
	int path_size;
	int buffer_size;
	tp_cargarEnMemoria recibido = malloc(sizeof(tp_cargarEnMemoria));
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
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

void prot_enviar_FM9_DMA_cargaEnMemoria(int memory_address, int sock){
	//7 envia
	enviar(sock,&memory_address,sizeof(memory_address));
}

int prot_recibir_FM9_DMA_cargaEnMemoria(int sock){
	//7 recibe
	int recibido;
	recibir(sock,&recibido,sizeof(recibido));
	return recibido;
}

void prot_enviar_DMA_SAFA_datosEnMemoria(char* path, int pid, int memory_address, int sock){
	//8 envia
	int path_size = sizeof(&path);
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,path,path_size);
	enviar(sock,&pid,sizeof(pid));
	enviar(sock,&memory_address,sizeof(memory_address));
}

tp_datosEnMemoria prot_recibir_DMA_SAFA_datosEnMemoria(int sock){
	//8 recibe
	int path_size;
	tp_datosEnMemoria recibido = malloc(sizeof(tp_datosEnMemoria));
	recibir(sock,&path_size,sizeof(path_size));
	recibido->path = malloc(path_size);
	recibir(sock,recibido->path,path_size);
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
	recibir(sock,&(recibido->memory_address),sizeof(recibido->memory_address));
	return recibido;
}
