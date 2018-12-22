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
		logger_funesMemory9(escribir_loguear, l_info,"Se acumulo una parte del archivo en un buffer\n");
		prot_enviar_FM9_DMA_cargaEnMemoria(0, DAM_fd);
		return;
	}
	logger_funesMemory9(escribir_loguear, l_info,
			"Ya se obtuvo el archivo y se intentara agregar el segmento paginado en la memoria principal\n");
	if(hay_marcos_libres()==false){
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
	int indice_en_tabla=copiar_archivo_en_marcos_libres(archivo_separado_en_lineas, cantidad_de_lineas,
			marcos_libres, parte_archivo->path, parte_archivo->pid);
	informar_carga_segmento_exitosa(indice_en_tabla, parte_archivo, DAM_fd);
}

t_tabla_segmentos* obtener_tabla_segmentos_del_proceso(int pid){
	bool el_pid_tiene_tabla_de_segmentos(void * tabla_segmentos){
		return (*(t_tabla_segmentos*)tabla_segmentos).pid==pid;
	}
	return list_find(tablas_de_segmentos, &el_pid_tiene_tabla_de_segmentos);
}

int actualizar_tabla_segmentos_paginados(int pid, char * path, int marco, int ultima_linea_marco, int pagina){
	int indice_entrada_archivo_en_tabla_segmentos=-1;
	//distingue entre procesos que ya tienen tabla de segmentos y los que no
	if(el_proceso_tiene_tabla_de_segmentos(pid)){
		t_tabla_segmentos* p_tabla_segmentos = obtener_tabla_segmentos_del_proceso(pid);
		if(el_segmento_paginado_tiene_paginas(p_tabla_segmentos,path)){
			t_entrada_tabla_segmentos_pag * entrada_seg_pag=
						obtener_entrada_segmentos_relacionada_al_path(p_tabla_segmentos,path);
			list_add(entrada_seg_pag->base,crear_nueva_entrada_tabla_paginas(marco, pagina));
			indice_entrada_archivo_en_tabla_segmentos=list_size(p_tabla_segmentos->entradas)-1;
		} else {
			indice_entrada_archivo_en_tabla_segmentos=
					crear_nueva_entrada_tabla_de_segmentos_paginados(path, marco, ultima_linea_marco
							, pagina, p_tabla_segmentos);
		}
	} else {
		indice_entrada_archivo_en_tabla_segmentos=
				agregar_nueva_tabla_segmentos_paginados_para_proceso(pid, path, marco, ultima_linea_marco, pagina);
	}
	return indice_entrada_archivo_en_tabla_segmentos;
}

t_entrada_tabla_segmentos_pag * obtener_entrada_segmentos_relacionada_al_path(t_tabla_segmentos* p_tabla_segmentos
		, char * path){
	t_list * segmentos = p_tabla_segmentos->entradas;
	bool tabla_de_segmentos_tiene_archivo(void * segmento){
		return string_equals_ignore_case((*(t_entrada_tabla_segmentos_pag*)segmento).archivo,path);
	}
	return list_get(list_filter(segmentos, &tabla_de_segmentos_tiene_archivo),0);
}

bool el_segmento_paginado_tiene_paginas(t_tabla_segmentos* p_tabla_segmentos, char * path){
	t_entrada_tabla_segmentos_pag * entrada_seg_pag=
			obtener_entrada_segmentos_relacionada_al_path(p_tabla_segmentos,path);
	return !list_is_empty(entrada_seg_pag->base);
}

t_entrada_tabla_paginas* crear_nueva_entrada_tabla_paginas(int marco, int pagina) {
	t_entrada_tabla_paginas* entrada_tabla_paginas = malloc(
			sizeof(t_entrada_tabla_paginas));
	entrada_tabla_paginas->marco = marco;
	entrada_tabla_paginas->pagina = pagina;
	return entrada_tabla_paginas;
}

int crear_nueva_entrada_tabla_de_segmentos_paginados(char * path, int marco,
		int ultima_linea_marco, int pagina, t_tabla_segmentos* p_tabla_segmentos){
	int entrada=-1;
	//Crea nueva entrada en la tabla de segmentos
	if(p_tabla_segmentos!=NULL){
		t_entrada_tabla_paginas* nueva_entrada_tabla_paginas=
				crear_nueva_entrada_tabla_paginas(marco, pagina);
		t_entrada_tabla_segmentos_pag* entrada_tabla_segmentos_pag=malloc(sizeof(t_entrada_tabla_segmentos_pag));
		entrada_tabla_segmentos_pag->archivo=path;
		entrada_tabla_segmentos_pag->base=list_create();
		list_add(entrada_tabla_segmentos_pag->base, nueva_entrada_tabla_paginas);
		entrada_tabla_segmentos_pag->limite=ultima_linea_marco;
		list_add(p_tabla_segmentos->entradas,entrada_tabla_segmentos_pag);
		return list_size(p_tabla_segmentos->entradas)-1;
	}
	return entrada;
}

int agregar_nueva_tabla_segmentos_paginados_para_proceso(int pid, char* path, int marco
		, int ultima_linea_marco, int pagina){
	t_tabla_segmentos* p_tabla_segmentos=malloc(sizeof(t_tabla_segmentos));
	p_tabla_segmentos->pid=pid;
	p_tabla_segmentos->entradas=list_create();
	crear_nueva_entrada_tabla_de_segmentos_paginados(path, marco, ultima_linea_marco, pagina, p_tabla_segmentos);
	return 0;
}

void inicializar_bitmap_de_marcos_libres() {
	bitmap_marcos_libres = list_create();
	int cantidad_de_marcos=(int)TAMANIO_MEMORIA / TAMANIO_PAGINA;
	t_disponibilidad_marco * marco_libre=malloc(sizeof(t_disponibilidad_marco));
	for(int marco=0;marco<cantidad_de_marcos;marco++){
		marco_libre->disponibilidad=0;
		marco_libre->indice=marco;
		list_add(bitmap_marcos_libres,marco_libre);
	}
}

bool el_marco_esta_libre(void* marco){
	return (*(t_disponibilidad_marco*)marco).disponibilidad==0;
}

bool hay_marcos_libres(){
	return list_any_satisfy(bitmap_marcos_libres, &el_marco_esta_libre);
}

int contar_marcos_libres(){
	return list_count_satisfying(bitmap_marcos_libres, &el_marco_esta_libre);
}

bool archivo_ocupa_mas_marcos_que_disponibles(int lineas){
	int marcos_disponibles=contar_marcos_libres();
	int lineas_disponibles=(marcos_disponibles*TAMANIO_PAGINA)/TAMANIO_MAX_LINEA;
	return lineas_disponibles<lineas;
}

t_list* obtener_marcos_libres() {
	return list_filter(bitmap_marcos_libres, &el_marco_esta_libre);
}

bool marco_de_menor_a_mayor(void* marco1, void* marco2){
	return ((*(t_disponibilidad_marco*)marco1).indice<(*(t_disponibilidad_marco*)marco2).indice);
}

int copiar_archivo_en_marcos_libres(char * archivo, int lineas, t_list * marcos_libres, char * path, int dtb){
	int indice_en_tabla=-1;
	int cantidad_lineas_de_marco=TAMANIO_PAGINA/TAMANIO_MAX_LINEA;
	list_sort(marcos_libres, &marco_de_menor_a_mayor);
	int cont_marco_libre=0;
	int offset=0;
	while(lineas>cantidad_lineas_de_marco){
		cont_marco_libre++;
		t_disponibilidad_marco* marco_libre=list_get(marcos_libres,cont_marco_libre);
		int indice_marco_libre=marco_libre->indice;
		int primer_byte_de_mp_del_marco=indice_marco_libre*TAMANIO_PAGINA;
		memcpy(MEMORIA_FISICA+primer_byte_de_mp_del_marco,archivo+offset,TAMANIO_PAGINA);
		lineas-=cantidad_lineas_de_marco;
		offset+=TAMANIO_PAGINA;
		marco_libre->disponibilidad=1;
		indice_en_tabla=actualizar_tabla_segmentos_paginados(dtb, path
				, indice_marco_libre, cantidad_lineas_de_marco, cont_marco_libre);
	}
	if(lineas>0){
		cont_marco_libre++;
		t_disponibilidad_marco* marco_libre=list_get(marcos_libres,cont_marco_libre);
		int indice_marco_libre=marco_libre->indice;
		int primer_byte_de_mp_del_marco=indice_marco_libre*TAMANIO_PAGINA;
		memcpy(MEMORIA_FISICA+primer_byte_de_mp_del_marco,archivo+offset,TAMANIO_PAGINA);
		marco_libre->disponibilidad=1;
		indice_en_tabla=actualizar_tabla_segmentos_paginados(dtb, path
				, indice_marco_libre, lineas, cont_marco_libre);
	}
	list_destroy(marcos_libres);
	return indice_en_tabla;
}

void eliminar_lista_de_entradas_seg_pag(void * tabla_segmentos){
	//list_destroy_and_destroy_elements((*(t_tabla_segmentos*)tabla_segmentos).entradas,);
}

void destruir_estructuras_esquema_segmentacion_paginada(){
	if(tablas_de_segmentos!=NULL){
		//list_destroy_and_destroy_elements(tablas_de_segmentos,&eliminar_lista_de_entradas_seg_pag);
	}
	if(bitmap_marcos_libres!=NULL){
		list_destroy(bitmap_marcos_libres);
	}
	if(archivos_cargandose!=NULL){
		list_destroy(archivos_cargandose);
	}
	if(archivos_devolviendose!=NULL){
		list_destroy(archivos_devolviendose);
	}
}

t_tabla_segmentos* buscar_tabla_de_segmentos_seg_pag(int pid) {
	return (t_tabla_segmentos*) list_get(list_filter_comparing(tablas_de_segmentos,
			&tiene_tabla_de_segmentos_seg_pag, pid),0);
}

void buscar_informacion_administrativa_esquema_segmentacion_paginada(int id){
	if(id == -1){
		logger_funesMemory9(escribir_loguear, l_error,"Falta el ID del DTB, proba de nuevo\n");
	}else{
		t_entrada_tabla_segmentos_pag * entrada_segmento;
		if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos_seg_pag,id)){
			t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos_seg_pag(id);

			int cant_entradas = list_size(tabla_segmentos->entradas);
			logger_funesMemory9(escribir_loguear, l_info, "Utilizando el esquema de segmentacion paginada y dado el ID del DTB indicado, el proceso tiene una tabla de "
					"segmentos con %d entradas\n", cant_entradas);

			for(int i = 0; i<cant_entradas; i++){
				logger_funesMemory9(escribir_loguear, l_info, "En la entrada %d, se encuentra la sgte informacion: \n",i);
				entrada_segmento = list_get(tabla_segmentos->entradas,i);
				logger_funesMemory9(escribir_loguear, l_info, "Contiene el archivo %s.\n",entrada_segmento->archivo);
				int cant_de_paginas = list_size(entrada_segmento->base);
				logger_funesMemory9(escribir_loguear, l_info, "Contiene una tabla de paginas con %d entradas, y en c/u hay: \n",cant_de_paginas);

				for(int i=0;i<cant_de_paginas;i++){
					t_entrada_tabla_paginas * tabla_de_paginas = list_get(entrada_segmento->base,i);
					logger_funesMemory9(escribir_loguear, l_info, "En la entrada %d esta la pagina nro %d y tiene asociado el marco"
							"%d \n",i,tabla_de_paginas->pagina, tabla_de_paginas->marco);
				}

				for(int i=0;i<cant_de_paginas;i++){
					t_entrada_tabla_paginas * tabla_de_paginas = list_get(entrada_segmento->base,i);
					int indice_marco = tabla_de_paginas->marco;
					int primer_byte_de_mp_del_marco=indice_marco*TAMANIO_PAGINA;

					char * lo_que_esta_en_mem_fisica_del_marco = malloc(TAMANIO_PAGINA);
					memcpy(lo_que_esta_en_mem_fisica_del_marco,MEMORIA_FISICA+primer_byte_de_mp_del_marco,TAMANIO_PAGINA);
					lo_que_esta_en_mem_fisica_del_marco[TAMANIO_PAGINA]='\0';

					//no estoy considerando el limite de la ultima pagina

					char ** lineas=string_split(lo_que_esta_en_mem_fisica_del_marco, "\n");
					logger_funesMemory9(escribir_loguear, l_info,
							"La información contenida en la memoria fisica para el marco %d es: \n", tabla_de_paginas->marco);
					for(int cant_lineas=0;lineas[cant_lineas]!=NULL;cant_lineas++){
						char * linea=lineas[cant_lineas];
						remover_caracter(linea,'$');
						logger_funesMemory9(escribir_loguear, l_info,"%s\n",linea);
					}

					free(lo_que_esta_en_mem_fisica_del_marco);
				}
			}

		}else{
			logger_funesMemory9(escribir_loguear, l_warning, "No hay información administrativa para el ID %d del DTB indicado.\n",id);
		}
	}
}

void obtener_parte_archivo_con_segmentacion_paginada(int DAM_fd){

}

void darle_una_linea_al_cpu_segmentacion_paginada(int sock){

}

void asignar_datos_a_linea_segmentacion_paginada(int sock){

}

bool tiene_tabla_de_segmentos_seg_pag(void * tabla_segmentos, int pid){
	return pid == (*(t_tabla_segmentos*)tabla_segmentos).pid;
}

bool tienen_el_mismo_nombre_seg_pag(void * arch1){
	return string_equals_ignore_case(path_archivo_para_comparar, (*(t_entrada_tabla_segmentos_pag*)arch1).archivo);
}

bool realizar_operacion_liberar_seg_paginada(tp_liberarArchivo paquete_liberar){
	t_entrada_tabla_segmentos_pag * entrada_segmento;
	path_archivo_para_comparar = paquete_liberar->path;

	if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos_seg_pag,paquete_liberar->id_GDT)){
		entrada_segmento = obtener_entrada_segmentos_relacionada_al_path(tablas_de_segmentos,paquete_liberar->path);
		int cant_de_paginas = list_size(entrada_segmento->base);

		for(int i=0;i<cant_de_paginas;i++){
			t_entrada_tabla_paginas * tabla_de_paginas = list_get(entrada_segmento->base,i);
			t_disponibilidad_marco marco_nuevo;
			marco_nuevo.disponibilidad = 0;
			marco_nuevo.indice = tabla_de_paginas->marco;
			list_add(bitmap_marcos_libres,&marco_nuevo);
		}

		list_remove_by_condition(tablas_de_segmentos, &tienen_el_mismo_nombre_seg_pag);

		return true;
	}else{
		return false;
	}
}

void liberar_archivo_abierto_segmentacion_paginada(int sock){
	tp_liberarArchivo paquete_liberar = prot_recibir_CPU_FM9_liberar_archivo(sock);
	if(realizar_operacion_liberar_seg_paginada(paquete_liberar)){
		enviarCabecera(sock, LiberarArchivoAbiertoEjecutandose, sizeof(LiberarArchivoAbiertoEjecutandose));
		logger_funesMemory9(escribir_loguear, l_info,"Se le aviso al CPU solicitante que se esta liberando el archivo deseado\n");
	}else{
		logger_funesMemory9(escribir_loguear, l_error,"40002\n");
		enviarCabecera(sock, LiberarArchivoAbiertoNoFinalizado, sizeof(LiberarArchivoAbiertoNoFinalizado));
	}
	free(paquete_liberar);
}
