/*
 * protocolo.c
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */


void prot_enviar_DMA_FS_path(char* path,int sock){
	int size = sizeof(&path);
	enviar(sock,&size,sizeof(size));
	enviar(sock,path,size);
}

char* prot_recibir_DMA_FS_path(int sock){
	char* path;
	int size;
	recibir(sock,&size,sizeof(size));
	path = malloc(size);
	recibir(sock,path,size);
	return path;
}
