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
		//obtenidos = malloc(sizeof(tp_datosObtenidosDeProtocolo));
		recibir(sock,&(obtenidos->tamanio_total_archivo),sizeof(obtenidos->tamanio_total_archivo));
		recibir(sock,&(obtenidos->size),sizeof(&(obtenidos->size)));
		obtenidos->buffer = malloc(obtenidos->size);
		recibir(sock,obtenidos->buffer,obtenidos->size);
	}
	return obtenidos;
}

void prot_enviar_FS_DMA_datosObtenidos_serializado(t_datosObtenidos datosObtenidos, int sock){
	//s1 envia
	int sizeDeLaEsctructura=sizeof(t_datosObtenidosDeProtocolo);
	enviar(sock,&(datosObtenidos.resultado),sizeof(datosObtenidos.resultado));
	if(datosObtenidos.resultado==DatosObtenidos){
		int enviado=0;
		int sizeDeMallocsiezDeMalloc=(sizeof(long int))*2+datosObtenidos.size;
		//printf("\nsize de datos: %d\nsize de malloc: %d\n",datosObtenidos.size, sizeDeMallocsiezDeMalloc);
		char*datos=malloc(sizeDeMallocsiezDeMalloc);
		memcpy(datos+enviado,&(datosObtenidos.size),sizeof(long int));
		enviado=enviado+sizeof(int long);
		memcpy(datos+enviado,datosObtenidos.datos,datosObtenidos.size);
		//for(int i=0;i<datosObtenidos.size;i++)printf("%c",datosObtenidos.datos[i]);
		enviado=enviado+datosObtenidos.size;
		//printf("\ntam total del arch:%d\n",datosObtenidos.tamanio_total_archivo);
		memcpy(datos+enviado,&(datosObtenidos.tamanio_total_archivo),sizeof(long int));
		enviado=enviado+sizeof(int long);
		enviar(sock,datos,enviado);
		//printf("Enviando:%d=%d+%d\n",enviado,sizeof(long int),datosObtenidos.size);
	}
}

tp_datosObtenidos prot_recibir_FS_DMA_datosObtenidos_serializado(int sock){
	//s1 recibir
	int resultado;
	char*datos;
	tp_datosObtenidos obtenidos=malloc(sizeof(t_datosObtenidos));
	obtenidos->size=0;
	obtenidos->tamanio_total_archivo=0;
	obtenidos->resultado=0;
	obtenidos->datos=NULL;
	recibir(sock,&resultado,sizeof(resultado));
	if(resultado==DatosObtenidos){
		int tamanio_total_archivo;
		//en este caso tengo datos para recibir
		recibir(sock,&(obtenidos->size),sizeof(long int));
		//printf("\nsize: %d\n",obtenidos->size);
		obtenidos->datos = malloc(obtenidos->size);
		recibir(sock,obtenidos->datos,obtenidos->size);
		recibir(sock,&(obtenidos->tamanio_total_archivo),sizeof(long int));
		//printf("\nBufer:\n");
		//for(int i=0;i<obtenidos->size;i++)printf("%c",obtenidos->datos[i]);
		}
	//printf("\ntamanioTotal: %d, size:%d\n",obtenidos->tamanio_total_archivo,obtenidos->size);
	return obtenidos;
}


void prot_enviar_DMA_FS_obtenerDatos(char *path, long int offset, long int size, int sock){
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

void prot_enviar_DMA_FS_obtenerDatos_serializado(t_obtenerDatos datos, int sock){
	//s2 envia
	int tam = strlen(datos.path)+1;
	int sizeDeMallocsiezDeMalloc=sizeof(int)+sizeof(long int)*2+tam;
	/*printf("\nsize de datos: %d\nsize de malloc: %d\ntam:%d\nsize de un char:%d\ndatos:%s\n",
			datos.size, sizeDeMallocsiezDeMalloc,tam,sizeof(char),datos.path);
	*/
	char*datosAEnviar=malloc(sizeDeMallocsiezDeMalloc);
	int enviado=0;
	memcpy(datosAEnviar,&tam,sizeof(int));
	enviado=sizeof(int)+enviado;
	memcpy(datosAEnviar+enviado,datos.path,tam);
	enviado=tam+enviado;
	memcpy(datosAEnviar+enviado,&(datos.offset),sizeof(long int));
	enviado=sizeof(long int)+enviado;
	memcpy(datosAEnviar+enviado,&(datos.size),sizeof(long int));
	enviado=sizeof(long int)+enviado;
	enviar(sock,datosAEnviar,enviado);
	return;
}

tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos_serializado(int sock){
	//s2 recibe
	int tam;
	tp_obtenerDatos recibido = malloc(sizeof(t_obtenerDatos));
	recibir(sock,&tam,sizeof(tam));
	char*aux=malloc(tam);
	recibir(sock,aux,tam);
	recibido->path=aux;
	//for(int i=0;i<tam;i++)printf("%c",recibido->path[i]);
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

void prot_enviar_DMA_FS_guardarDatos(char *path, long int offset, long int size, char *buffer, int sock){
	//4 envia
	int tam2;
	int tam = strlen(path)+1;
	enviar(sock,&tam,sizeof(tam));
	enviar(sock,path,tam);
	enviar(sock,&offset,sizeof(offset));
	enviar(sock,&size,sizeof(size));
	tam2 = strlen(buffer);
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

void prot_enviar_DMA_FS_guardarDatos_serializado(t_obtenerDatos datos, int sock){
	//s4 envia
	int tamPath = strlen(datos.path)+1;
	int tamBuffer = strlen(datos.buffer);
	int sizeDeMallocsiezDeMalloc=sizeof(int)+sizeof(long int)*2+tamPath+tamBuffer;

	char*datosAEnviar=malloc(sizeDeMallocsiezDeMalloc);
	int enviado=0;
	memcpy(datosAEnviar+enviado,&tamPath,sizeof(int));
	enviado=sizeof(int)+enviado;
	memcpy(datosAEnviar+enviado,datos.path,tamPath);
	enviado=tamPath+enviado;
	memcpy(datosAEnviar+enviado,&(datos.offset),sizeof(long int));
	enviado=sizeof(long int)+enviado;
	memcpy(datosAEnviar+enviado,&(datos.size),sizeof(long int));
	enviado=sizeof(long int)+enviado;
	memcpy(datosAEnviar+enviado,&tamBuffer,sizeof(int));
	enviado=sizeof(int)+enviado;
	memcpy(datosAEnviar+enviado,datos.buffer,tamBuffer);
	enviado=tamBuffer+enviado;
	enviar(sock,datosAEnviar,enviado);
	printf("Enviando: Path(%d) %s | Offset %d | Size %d | Buffer(%d) %s\n",
		tamPath,datos.path,datos.offset,datos.size,tamBuffer,datos.buffer);
	return;
}

tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos_serializado(int sock){
	//s4 recibe
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
	//printf("Enviando: Path(%d) %s | Offset %d | Size %d | Buffer(%d) %s\n",
	//	tam,path,offset,size,tam2,buffer);
	return datos;
}

void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock){
	//5 envia
	int tam = strlen(path)+1;
	enviar(sock,&tam,4);
	enviar(sock,path,tam);
	enviar(sock,&pid,4);
}

tp_abrirPath prot_recibir_CPU_DMA_abrirPath(int sock){
	//5 recibe
	int size;
	int * size_aux=malloc(sizeof(int));
	tp_abrirPath recibido = malloc(sizeof(t_abrirPath));
	recibir(sock,size_aux,sizeof(int));
	size=*size_aux;
	free(size_aux);
	recibido->path = malloc(size);
	recibir(sock,recibido->path,size);
	int * pid_aux=malloc(sizeof(int));
	recibir(sock,pid_aux,sizeof(int));
	recibido->pid=*pid_aux;
	free(pid_aux);
	return recibido;
}

void prot_enviar_DMA_FM9_cargarEnMemoria(t_cargarEnMemoria cargar_en_memoria, int buffer_size, int sock){
	//6 envia
	int path_size = strlen(cargar_en_memoria.path)+1;
	enviar(sock,&cargar_en_memoria.pid, sizeof(int));
	enviar(sock,&path_size,sizeof(path_size));
	enviar(sock,cargar_en_memoria.path,path_size);
	enviar(sock,&buffer_size,sizeof(buffer_size));
	enviar(sock,cargar_en_memoria.buffer,buffer_size);
	enviar(sock,&cargar_en_memoria.offset,sizeof(int));
	enviar(sock,&cargar_en_memoria.transfer_size,sizeof(int));
	enviar(sock,&cargar_en_memoria.file_size,sizeof(int));
}

tp_cargarEnMemoria prot_recibir_DMA_FM9_cargarEnMemoria(int sock){
	//6 recibe
	int path_size;
	int buffer_size;
	tp_cargarEnMemoria recibido = malloc(sizeof(t_cargarEnMemoria));
	recibir(sock,&(recibido->pid),sizeof(recibido->pid));
	recibir(sock,&path_size,sizeof(path_size));
	recibido->path = malloc(path_size);
	recibir(sock,recibido->path,path_size);
	recibir(sock,&buffer_size,sizeof(buffer_size));
	recibido->buffer = malloc(buffer_size);
	recibir(sock,recibido->buffer,buffer_size);
	(recibido->buffer)[buffer_size]='\0';
	recibir(sock,&(recibido->offset),sizeof(recibido->offset));
	recibir(sock,&(recibido->transfer_size),sizeof(recibido->transfer_size));
	recibir(sock,&(recibido->file_size),sizeof(recibido->file_size));
	return recibido;
}

void prot_enviar_FM9_DMA_cargaEnMemoria(int memory_address, int sock){
	//7 envia
	enviar(sock,&memory_address,sizeof(memory_address));
}

void prot_enviar_CPU_FM9_linea_pedida(char * linea, int sock){
	int linea_size = strlen(linea)+1;
	enviar(sock,&linea_size,sizeof(linea_size));
	enviar(sock,linea,linea_size);
}

tp_lineaParaCPU prot_recibir_CPU_FM9_linea_pedida(int sock){
	int tam_linea;
	tp_lineaParaCPU recibido = malloc(sizeof(t_lineaParaCPU));
	recibir(sock, &tam_linea, sizeof(tam_linea));
	recibido->linea = malloc(tam_linea);
	recibir(sock,recibido->linea,tam_linea);
	return recibido;
}

tp_lineaCPU prot_recibir_CPU_FM9_pedir_linea(int sock){
	int tam_linea;
	int id_GDT;
	int pc;
	tp_lineaCPU recibido = malloc(sizeof(t_lineaCPU));
	recibir(sock, &tam_linea, sizeof(tam_linea));
	recibido->linea = malloc(tam_linea);
	(recibido->linea)[tam_linea]='\0';
	recibir(sock,recibido->linea,tam_linea);
	recibir(sock,&id_GDT,sizeof(int));
	recibido->id_GTD=id_GDT;
	recibir(sock,&pc,sizeof(int));
	recibido->pc=pc;
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

void prot_enviar_DMA_FS_CrearArchivo_serializado(t_crearArchivo dataDelArchivo,int sock){
	//s10 recibe
	int tam = strlen(dataDelArchivo.path)+1;
	int sizeDeMallocsiezDeMalloc=sizeof(long int)+tam+sizeof(int);
	char*datosAEnviar=malloc(sizeDeMallocsiezDeMalloc);
	int enviado=0;
	memcpy(datosAEnviar+enviado,&tam,sizeof(int));
	enviado=sizeof(int)+enviado;
	memcpy(datosAEnviar+enviado,dataDelArchivo.path,tam);
	enviado=tam+enviado;
	memcpy(datosAEnviar+enviado,&(dataDelArchivo.size),sizeof(int));
	enviado=sizeof(int)+enviado;
	enviar(sock,datosAEnviar,enviado);
	return;
}

tp_crearArchivo prot_recibir_DMA_FS_CrearArchivo_serializado(int sock){
	//s10 envia
	tp_crearArchivo data = malloc(sizeof(tp_crearArchivo));
	char* path;
	int size;
	recibir(sock,&size,sizeof(int));
	char*cadena = malloc(size);
	recibir(sock,cadena,size);
	data->path=cadena;
	recibir(sock,&(data->size),sizeof(int));
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

void prot_enviar_DMA_FM9_obtenerArchivo(char* path, int pid, int memory_address, int size, int sock){
	int tam_path = strlen(path);
	int size_of_int=sizeof(int);
	int tamanio_paquete_obtener = tam_path + (5 * size_of_int);
	char * paquete_obtener = malloc(tamanio_paquete_obtener);
	paquete_obtener[0]=tam_path;
	memcpy(paquete_obtener+4,path,tam_path);
	paquete_obtener[4+tam_path]=pid;
	paquete_obtener[4+tam_path+4]=memory_address;
	paquete_obtener[4+tam_path+4+4]=size;
	enviar(sock,paquete_obtener,tamanio_paquete_obtener);
	free(paquete_obtener);

}

void prot_enviar_CPU_FM9_asignar_datos_linea(char * datos, char * linea, char * path, int id_GDT, int pc, int sock){
	int tam_datos = strlen(datos);
	int tam_linea = strlen(linea);
	int tam_path = strlen(path);
	int size_of_int = sizeof(int);
	int tamanio_paquete_asignar_datos = tam_datos + tam_linea + tam_path + (2*size_of_int);
	char * paquete_asignar_datos = malloc(tamanio_paquete_asignar_datos);
	paquete_asignar_datos[0]=id_GDT;
	paquete_asignar_datos[4]=pc;
	paquete_asignar_datos[8]=tam_datos;
	memcpy(paquete_asignar_datos+4+4+4,datos,tam_datos);
	paquete_asignar_datos[4+4+4+tam_datos]=tam_linea;
	memcpy(paquete_asignar_datos+4+4+4+tam_datos+4,linea,tam_linea);
	paquete_asignar_datos[4+4+4+tam_datos+4+tam_linea]=tam_path;
	memcpy(paquete_asignar_datos+4+4+4+tam_datos+4+tam_linea+4,path,tam_path);
	enviar(sock,paquete_asignar_datos,tamanio_paquete_asignar_datos);
	free(paquete_asignar_datos);
}

tp_asignarDatosLinea prot_recibir_CPU_FM9_asignar_datos_linea(int sock){
	tp_asignarDatosLinea asignar_datos_linea;
	int tam_datos;
	int tam_linea;
	int tam_path;
	recibir(sock,&(asignar_datos_linea->id_GDT),sizeof(int));
	recibir(sock,&(asignar_datos_linea->pc),sizeof(int));
	recibir(sock,&tam_datos,sizeof(int));
	asignar_datos_linea->datos=malloc(tam_datos+1);
	recibir(sock,asignar_datos_linea->datos,tam_datos);
	(asignar_datos_linea->datos)[tam_datos]='\0';
	recibir(sock,&tam_linea,sizeof(int));
	asignar_datos_linea->linea=malloc(tam_linea+1);
	recibir(sock,asignar_datos_linea->linea,tam_linea);
	(asignar_datos_linea->linea)[tam_linea]='\0';
	recibir(sock,&tam_path,sizeof(int));
	asignar_datos_linea->path=malloc(tam_path+1);
	recibir(sock,asignar_datos_linea->path,tam_path);
	(asignar_datos_linea->path)[tam_path]='\0';
	return asignar_datos_linea;
}

void prot_enviar_CPU_SAFA_abortar_DTB(int id_GDT, int sock){
	enviar(sock,&id_GDT,sizeof(int));
}

int prot_recibir_CPU_SAFA_abortar_DTB(int sock){
	int id_GDT;
	recibir(sock,&id_GDT,sizeof(int));
	return id_GDT;
}

void prot_enviar_CPU_SAFA_bloquear_DTB(int id_GDT, int sock){
	enviar(sock,&id_GDT,sizeof(int));
}

int prot_recibir_CPU_SAFA_bloquear_DTB(int sock){
	int id_GDT;
	recibir(sock,&id_GDT,sizeof(int));
	return id_GDT;
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
	enviar(sock,&tam_path,size_of_int);
	enviar(sock,path,tam_path);
	enviar(sock,&id,size_of_int);
	enviar(sock,&pc,size_of_int);
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

void prot_enviar_CPU_SAFA_retener_recurso(char * recurso, int id_GDT, int sock){
	int tam_recurso = strlen(recurso);
	int size_of_int = sizeof(int);
	int tamanio_paquete_retener = tam_recurso + size_of_int;
	char * paquete_retener = malloc(tamanio_paquete_retener);
	paquete_retener[0]=tam_recurso;
	memcpy(paquete_retener+4,recurso,tam_recurso);
	paquete_retener[4+tam_recurso]=id_GDT;
	enviar(sock,paquete_retener,tamanio_paquete_retener);
	free(paquete_retener);
}

void prot_enviar_CPU_SAFA_liberar_recurso(char * recurso, int id_GDT, int sock){
	int tam_recurso = strlen(recurso);
	int size_of_int = sizeof(int);
	int tamanio_paquete_retener = tam_recurso + size_of_int;
	char * paquete_retener = malloc(tamanio_paquete_retener);
	paquete_retener[0]=tam_recurso;
	memcpy(paquete_retener+4,recurso,tam_recurso);
	paquete_retener[4+tam_recurso]=id_GDT;
	enviar(sock,paquete_retener,tamanio_paquete_retener);
	free(paquete_retener);
}

tp_tipoRecurso prot_recibir_CPU_SAFA_liberar_recurso(int sock){
	tp_tipoRecurso liberar_rec;
	int tam_recurso;
	recibir(sock,&tam_recurso,sizeof(int));
	liberar_rec->recurso=malloc(tam_recurso+1);
	recibir(sock,liberar_rec->recurso,tam_recurso);
	(liberar_rec->recurso)[tam_recurso]='\0';
	recibir(sock,&(liberar_rec->id_GDT),sizeof(int));
	return liberar_rec;
}

tp_tipoRecurso prot_recibir_CPU_SAFA_retener_recurso(int sock){
	tp_tipoRecurso retener_rec;
	int tam_recurso;
	recibir(sock,&tam_recurso,sizeof(int));
	retener_rec->recurso=malloc(tam_recurso+1);
	recibir(sock,retener_rec->recurso,tam_recurso);
	(retener_rec->recurso)[tam_recurso]='\0';
	recibir(sock,&(retener_rec->id_GDT),sizeof(int));
	return retener_rec;
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

void prot_enviar_CPU_FM9_liberar_archivo(char * path, int id_GDT, int sock){
	int tam_path = strlen(path);
	int size_of_int = sizeof(int);
	int tamanio_paquete_liberar_archivo = tam_path + size_of_int;
	char * paquete_liberar_archivo = malloc(tamanio_paquete_liberar_archivo);
	paquete_liberar_archivo[0]=tam_path;
	memcpy(paquete_liberar_archivo+4,path,tam_path);
	paquete_liberar_archivo[4+tam_path]=id_GDT;
	enviar(sock,paquete_liberar_archivo,tamanio_paquete_liberar_archivo);
	free(paquete_liberar_archivo);
}

void prot_enviar_CPU_DMA_crear_lineas_arch(char * path, int cant_lineas, int id_GDT, int sock){
	int tam_path = strlen(path)+1;
	int size_of_int = sizeof(int);
	enviar(sock,&tam_path,size_of_int);
	enviar(sock,path,tam_path);
	enviar(sock,&cant_lineas,size_of_int);
	enviar(sock,&id_GDT,size_of_int);
}

tp_crearLineasArch prot_recibir_CPU_DMA_crear_lineas_arch(int sock){
	tp_crearLineasArch crear_lineas_arch;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	crear_lineas_arch->path=malloc(tam_path+1);
	recibir(sock,crear_lineas_arch->path,tam_path);
	recibir(sock,&(crear_lineas_arch->cant_lineas),sizeof(int));
	recibir(sock,&(crear_lineas_arch->id_GDT),sizeof(int));
	return crear_lineas_arch;
}

tp_liberarArchivo prot_recibir_CPU_FM9_liberar_archivo(int sock){
	tp_liberarArchivo liberar_archivo;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	liberar_archivo->path=malloc(tam_path+1);
	recibir(sock,liberar_archivo->path,tam_path);
	(liberar_archivo->path)[tam_path]='\0';
	recibir(sock,&(liberar_archivo->id_GDT),sizeof(int));
	return liberar_archivo;
}

void prot_enviar_CPU_DAM_eliminar_arch_de_disco(char * path, int id_GDT, int sock){
	int tam_path = strlen(path);
	int size_of_int = sizeof(int);
	int tamanio_paquete_eliminar_arch = tam_path + size_of_int;
	char * paquete_eliminar_arch = malloc(tamanio_paquete_eliminar_arch);
	paquete_eliminar_arch[0]=tam_path;
	memcpy(paquete_eliminar_arch+4,path,tam_path);
	paquete_eliminar_arch[4+tam_path]=id_GDT;
	enviar(sock,paquete_eliminar_arch,tamanio_paquete_eliminar_arch);
	free(paquete_eliminar_arch);
}

tp_eliminarArch prot_recibir_CPU_DAM_eliminar_arch_de_disco(int sock){
	tp_eliminarArch eliminar_arch;
	int tam_path;
	recibir(sock,&tam_path,sizeof(int));
	eliminar_arch->path=malloc(tam_path+1);
	recibir(sock,eliminar_arch->path,tam_path);
	(eliminar_arch->path)[tam_path]='\0';
	recibir(sock,&(eliminar_arch->id_GDT),sizeof(int));
	return eliminar_arch;
}

void prot_enviar_SAFA_CPU_DTB(int id_GDT, int program_counter, int iniGDT, char* escriptorio, t_list* lista, int quantum, int sock){
	//mando id_GDT
	enviar(sock, &id_GDT, sizeof(id_GDT));
	//mando program counter
	enviar(sock, &program_counter, sizeof(program_counter));
	//mando iniGDT
	enviar(sock, &iniGDT, sizeof(iniGDT));
	//mando escriptorio
	int tam = strlen(escriptorio)+1;
	enviar(sock, &tam, sizeof(tam));
	enviar(sock, escriptorio, tam);
	//mando la lista
	int tamanio_elem;
	int cant_elem_lista;
	cant_elem_lista = list_size(lista);
	if(cant_elem_lista > 0){
		enviar(sock, &cant_elem_lista, sizeof(int));
		for(int i = 0; i < list_size(lista); i++){
			//calculo el tamaño de un elemento
			char* elem = list_remove(lista, 0);
			tamanio_elem = strlen(elem)+1;
			enviar(sock, &tamanio_elem, sizeof(tamanio_elem));
			enviar(sock, elem, tamanio_elem);
			free(elem);
	}
	}else{
		//en este caso cant elem es CERO
		enviar(sock, &cant_elem_lista, sizeof(cant_elem_lista));
	}
	//mando el quantum
	enviar(sock, &quantum, sizeof(quantum));
}

tp_DTB prot_recibir_SAFA_CPU_DTB(int sock){
	//reservo memoria
	tp_DTB DTB = malloc(sizeof(t_DTB));
	//recibo id_GDT
	recibir(sock, &(DTB->id_GDT), sizeof(int));
	//recibo program_counter
	recibir(sock, &(DTB->program_counter), sizeof(int));
	//recibo iniGDT
	recibir(sock, &(DTB->iniGDT), sizeof(int));
	//recibo escriptorio
	int tam;
	recibir(sock, &tam, sizeof(tam));
	char* buffer = malloc(tam);
	recibir(sock, buffer, tam);
	//buffer[tam]="/0";
	DTB->escriptorio = buffer;
	//recibir lista
	int cant_elem_lista_rec;
	int i;
	char* elem_rec;
	int tamanio_elem_rec;
	recibir(sock, &cant_elem_lista_rec, sizeof(int));
	DTB->tabla_dir_archivos = list_create();
	if(cant_elem_lista_rec > 0){
		for (i = 0; i < cant_elem_lista_rec; ++i) {
			recibir(sock, &tamanio_elem_rec, sizeof(tamanio_elem_rec));
			elem_rec=malloc(tamanio_elem_rec);
			recibir(sock, elem_rec, tamanio_elem_rec);
			list_add(DTB->tabla_dir_archivos, elem_rec);
		}
	}
	//recibir quantum
	recibir(sock, &(DTB->quantum), sizeof(int));
	return DTB;

}

void prot_enviar_DMA_SAFA_crearArchivo(int id_GDT, int sock){
	int * p_id_GDT=malloc(sizeof(int));
	*p_id_GDT=id_GDT;
	enviar(sock,p_id_GDT,sizeof(int));
	free(p_id_GDT);
}

int prot_recibir_DMA_SAFA_crearArchivo(int sock){
	int * p_id_GDT=malloc(sizeof(int));
	recibir(sock,p_id_GDT,sizeof(int));
	int retorno=*p_id_GDT;
	free(p_id_GDT);
	return retorno;
}

void prot_enviar_DMA_SAFA_eliminarArchivo(int id_GDT, int sock){
	int * p_id_GDT=malloc(sizeof(int));
	*p_id_GDT=id_GDT;
	enviar(sock,p_id_GDT,sizeof(int));
	free(p_id_GDT);
}

int prot_recibir_DMA_SAFA_eliminarArchivo(int sock){
	int * p_id_GDT=malloc(sizeof(int));
	recibir(sock,p_id_GDT,sizeof(int));
	int retorno=*p_id_GDT;
	free(p_id_GDT);
	return retorno;
}
