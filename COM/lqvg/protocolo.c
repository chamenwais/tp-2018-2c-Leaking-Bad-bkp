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
		enviar(sock,&tamanioTotalDelArchivo,sizeof(tamanioTotalDelArchivo));
		int tam = strlen(datos);
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
	int tam = strlen(path)+1;
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
	int tam = strlen(path)+1;
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
	int tam = strlen(path)+1;
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
	int tam2 = strlen(buffer);
	enviar(sock,&tam2,sizeof(tam2));
	enviar(sock,buffer,tam2);
	printf("Enviando: Path(%d) %s | Offset %d | Size %d | Buffer(%d) %s\n",
		tam,path,offset,size,tam2,buffer);
	return;
}

tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos(int sock){
	//4 recibe
	int tam;
	tp_obtenerDatos datos = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	char*path = malloc(tam);
	recibir(sock,path,tam);
	datos->path=path;
	int size, offset;
	recibir(sock,&offset,sizeof(offset));
	datos->offset=offset;
	recibir(sock,&size,sizeof(size));
	datos->size=size;
	int tam2;
	recibir(sock,&tam2,sizeof(tam2));
	char*buffer = malloc(tam2);
	recibir(sock,buffer,tam2);
	datos->buffer=buffer;
	printf("Enviando: Path(%d) %s | Offset %d | Size %d | Buffer(%d) %s\n",
		tam,path,offset,size,tam2,buffer);
	return datos;
}

void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock){
	//5 envia
	int tam = strlen(path)+1;
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

void prot_enviar_DMA_FM9_cargarEnMemoria(int pid, char* path, char* buffer, int offset, int transfer_size, int file_size, int sock){
	//6 envia
	int path_size = strlen(path)+1;
	int buffer_size = strlen(buffer);//+1; no mando el /0
	enviar(sock,&pid, sizeof(pid));
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,path,path_size);
	enviar(sock,&buffer_size,sizeof(buffer_size));
	enviar(sock,buffer,buffer_size);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&transfer_size,sizeof(transfer_size));
	enviar(sock,&file_size,sizeof(file_size));
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
	recibir(sock,&(recibido->transfer_size),sizeof(recibido->transfer_size));
	recibir(sock,&(recibido->file_size),sizeof(recibido->file_size));
	return recibido;
}

void prot_enviar_FM9_DMA_cargaEnMemoria(int memory_address, int sock){
	//7 envia
	enviar(sock,&memory_address,sizeof(memory_address));
}

tp_lineaCPU prot_recibir_CPU_FM9_pedir_linea(int sock){
	int tam_linea;
	tp_lineaCPU recibido = malloc(sizeof(tp_lineaCPU));
	recibir(sock, &tam_linea, sizeof(tam_linea));
	recibido->linea = malloc(tam_linea);
	recibir(sock,recibido->linea,tam_linea);
	recibir(sock,&(recibido->pc),sizeof(recibido->pc));
	return recibido;
}

int prot_recibir_FM9_DMA_cargaEnMemoria(int sock){
	//7 recibe
	int recibido;
	recibir(sock,&recibido,sizeof(recibido));
	return recibido;
}

void prot_enviar_DMA_SAFA_datosEnMemoria(char* path, int pid, int memory_address, int sock){
	//8 envia
	int path_size = strlen(path)+1;
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

void prot_enviar_DMA_FS_CrearArchivo(char* path,int longitud,int sock){
	//10 recibe
	int tam = strlen(path)+1;
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&longitud,sizeof(longitud));
}

tp_crearArchivo prot_recibir_DMA_FS_CrearArchivo(int sock){
	//10 envia
	tp_crearArchivo data = malloc(sizeof(tp_crearArchivo));
	char* path;
	int size;
	recibir(sock,&size,sizeof(size));
	char*cadena = malloc(size);
	recibir(sock,cadena,size);
	data->path=cadena;
	recibir(sock,&(data->size),sizeof(data->size));
	return data;
}

void prot_enviar_CPU_DMA_flush(char* path, int pid, int memory_address, int sock){
	int tam_path = strlen(path);
	int size_of_int=sizeof(int);
	int tamanio_paquete_flush = tam_path + (3 * size_of_int);
	char * paquete_flush = malloc(tamanio_paquete_flush);
	paquete_flush[0]=tam_path;
	memcpy(paquete_flush+4,path,tam_path);
	paquete_flush[4+tam_path]=pid;
	paquete_flush[4+tam_path+4]=memory_address;
	enviar(sock,paquete_flush,tamanio_paquete_flush);
	free(paquete_flush);
}

tp_datosEnMemoria prot_recibir_CPU_DMA_flush(int sock){
	tp_datosEnMemoria datos_flush;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	datos_flush->path=malloc(tam_path+1);
	recibir(sock,datos_flush->path,tam_path);
	(datos_flush->path)[tam_path]='\0';
	recibir(sock,&(datos_flush->pid),sizeof(int));
	recibir(sock,&(datos_flush->memory_address),sizeof(int));
	return datos_flush;
}

void prot_enviar_DMA_FM9_obtenerArchivo(char* path, int pid, int memory_address, int offset, int size, int sock){
	int tam_path = strlen(path);
	int size_of_int=sizeof(int);
	int tamanio_paquete_obtener = tam_path + (5 * size_of_int);
	char * paquete_obtener = malloc(tamanio_paquete_obtener);
	paquete_obtener[0]=tam_path;
	memcpy(paquete_obtener+4,path,tam_path);
	paquete_obtener[4+tam_path]=pid;
	paquete_obtener[4+tam_path+4]=memory_address;
	paquete_obtener[4+tam_path+4+4]=offset;
	paquete_obtener[4+tam_path+4+4+4]=size;
	enviar(sock,paquete_obtener,tamanio_paquete_obtener);
	free(paquete_obtener);

}

tp_obtenerArchivo prot_recibir_DMA_FM9_obtenerArchivo(int sock){
	tp_obtenerArchivo obtener_archivo;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	obtener_archivo->path=malloc(tam_path+1);
	recibir(sock,obtener_archivo->path,tam_path);
	(obtener_archivo->path)[tam_path]='\0';
	recibir(sock,&(obtener_archivo->pid),sizeof(int));
	recibir(sock,&(obtener_archivo->memory_address),sizeof(int));
	recibir(sock,&(obtener_archivo->offset),sizeof(int));
	recibir(sock,&(obtener_archivo->size),sizeof(int));
	return obtener_archivo;
}

void prot_enviar_FM9_DMA_devolverDatos(char* datos, int tamanio_trozo, int tamanio_total_archivo, int sock){
	int tam_datos = strlen(datos);
	int size_of_int=sizeof(int);
	int tamanio_paquete_devolver = tam_datos + (3 * size_of_int);
	char * paquete_devolver = malloc(tamanio_paquete_devolver);
	paquete_devolver[0]=tam_datos;
	memcpy(paquete_devolver+4,datos,tam_datos);
	paquete_devolver[4+tam_datos]=tamanio_trozo;
	paquete_devolver[4+tam_datos+4]=tamanio_total_archivo;
	enviar(sock,paquete_devolver,tamanio_paquete_devolver);
	free(paquete_devolver);
}

void prot_enviar_CPU_FM9_pedir_linea(char * path, int id, int pc, int sock){
	int tam_path = strlen(path);
	int size_of_int=sizeof(int);
	int tamanio_paquete_devolver = tam_path + (3 * size_of_int);
	char * paquete_pedir_linea = malloc(tamanio_paquete_devolver);
	paquete_pedir_linea[0]=tam_path;
	memcpy(paquete_pedir_linea+4,path,tam_path);
	paquete_pedir_linea[4+tam_path]=id;
	paquete_pedir_linea[4+tam_path+4]=pc;
	enviar(sock,paquete_pedir_linea,tamanio_paquete_devolver);
	free(paquete_pedir_linea);
}

tp_datosObtenidosDeProtocolo prot_recibir_FM9_DMA_devolverDatos(int sock){
	tp_datosObtenidosDeProtocolo devolver_datos;
	int tam_datos;
	recibir(sock,&tam_datos,sizeof(int));
	devolver_datos->buffer=malloc(tam_datos+1);
	recibir(sock,devolver_datos->buffer,tam_datos);
	(devolver_datos->buffer)[tam_datos]='\0';
	recibir(sock,&(devolver_datos->size),sizeof(int));
	recibir(sock,&(devolver_datos->tamanio_total_archivo),sizeof(int));
	return devolver_datos;
}

void prot_enviar_DMA_SAFA_finFlush(char* path, int pid, int sock){
	int tam_path = strlen(path);
	int size_of_int=sizeof(int);
	int tamanio_paquete_flush = tam_path + (2 * size_of_int);
	char * paquete_flush = malloc(tamanio_paquete_flush);
	paquete_flush[0]=tam_path;
	memcpy(paquete_flush+4,path,tam_path);
	paquete_flush[4+tam_path]=pid;
	enviar(sock,paquete_flush,tamanio_paquete_flush);
	free(paquete_flush);
}

tp_pathPid prot_recibir_DMA_SAFA_finFlush(int sock){
	tp_pathPid fin_flush;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	fin_flush->path=malloc(tam_path+1);
	recibir(sock,fin_flush->path,tam_path);
	(fin_flush->path)[tam_path]='\0';
	recibir(sock,&(fin_flush->pid),sizeof(int));
	return fin_flush;
}
