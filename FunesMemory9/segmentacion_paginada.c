/*
 * segmentacion_paginada.c
 *
 *  Created on: 8 dic. 2018
 *      Author: utnso
 */
#include "segmentacion_paginada.h"

void crear_estructuras_esquema_segmentacion_paginada(){
	tablas_de_segmentos=list_create();
	inicializar_bitmap_de_marcos_libres();
	archivos_cargandose=list_create();
	archivos_devolviendose=list_create();
}

void cargar_parte_archivo_en_segmento_paginado(int DAM_fd){
	tp_cargarEnMemoria parte_archivo=prot_recibir_DMA_FM9_cargarEnMemoria(DAM_fd);
	t_archivo_cargandose * archivo_de_proceso_cargandose = cargar_buffer_archivo(parte_archivo);
	if(todavia_falta_mandar_pedazo_de_archivo(parte_archivo, archivo_de_proceso_cargandose)){
		logger_funesMemory9(escribir_loguear, l_trace,"Se acumulo una parte del archivo en un buffer\n");
		prot_enviar_FM9_DMA_cargaEnMemoria(0, DAM_fd);
		return;
	}
	logger_funesMemory9(escribir_loguear, l_trace,
			"Ya se obtuvo el archivo y se intentara agregar el segmento paginado en la memoria principal\n");
	if(!hay_marcos_libres()){
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	char * archivo_separado_en_lineas=NULL;
	int cantidad_de_lineas=separar_en_lineas(archivo_de_proceso_cargandose,&archivo_separado_en_lineas);
	if(cantidad_de_lineas<0){
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	if (archivo_ocupa_mas_marcos_que_disponibles(cantidad_de_lineas)) {
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	t_list * marcos_libres = obtener_marcos_libres();
	copiar_archivo_en_marcos_libres(archivo_separado_en_lineas, cantidad_de_lineas, marcos_libres);
}

void destruir_estructuras_esquema_segmentacion_paginada(){

}

void buscar_informacion_administrativa_esquema_segmentacion_paginada(int pid){

}

void obtener_parte_archivo_con_segmentacion_paginada(int DAM_fd){

}

void darle_una_linea_al_cpu_segmentacion_paginada(int sock){

}

void asignar_datos_a_linea_segmentacion_paginada(int sock){

}

void liberar_archivo_abierto_segmentacion_paginada(int sock){

}
