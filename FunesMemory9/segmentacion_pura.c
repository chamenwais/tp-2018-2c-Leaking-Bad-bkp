/*
 * segmentacion_pura.c
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#include "segmentacion_pura.h"

t_list * tablas_de_segmentos=NULL;
t_list * lista_de_huecos=NULL;

int el_proceso_tiene_tabla_de_segmentos() {
	return !list_is_empty(tablas_de_segmentos)
			&& list_any_satisfy(tablas_de_segmentos,
					&es_un_proceso_conocido);
}

int agregar_nueva_tabla_segmentos_para_proceso(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite) {
	//Crea tabla de segmentos para el nuevo proceso y lo agrega a la lista
	t_list* nuevas_entradas_segmentos = list_create();
	int indice_entrada_tabla_segmentos=agregar_entrada_tabla_segmentos(parte_archivo, nuevas_entradas_segmentos, nueva_base, nuevo_limite);
	t_tabla_segmentos nueva_tabla_segmentos;
	nueva_tabla_segmentos.pid = parte_archivo->pid;
	nueva_tabla_segmentos.entradas = nuevas_entradas_segmentos;
	list_add(tablas_de_segmentos, &nueva_tabla_segmentos);
	return indice_entrada_tabla_segmentos;
}

bool colocar_primero_hueco_de_limite_mayor(void * primer_hueco, void * segundo_hueco){
	return (*(t_hueco*)primer_hueco).limite>(*(t_hueco*)segundo_hueco).limite;
}

t_hueco *  tomar_hueco_con_limite_mas_grande(){
	list_sort(lista_de_huecos, &colocar_primero_hueco_de_limite_mayor);
	return list_get(lista_de_huecos,0);
}

bool archivo_mas_grande_que_hueco(
		int tamanio_en_lineas_archivo,
		const t_hueco* hueco) {
	return tamanio_en_lineas_archivo
			> hueco->limite;
}

bool archivo_igual_al_hueco(
		int tamanio_archivo_en_linas,
		const t_hueco* hueco_usado) {
	return tamanio_archivo_en_linas
			== hueco_usado->limite;
}

t_hueco * tomar_hueco() {
	t_hueco * hueco;
	if (list_size(lista_de_huecos) == 1) {
		hueco = list_get(lista_de_huecos, 0);
	} else {
		hueco = tomar_hueco_con_limite_mas_grande();
	}
	return hueco;
}

void actualizar_info_tabla_de_huecos(int tamanio_archivo_en_memoria, t_hueco* hueco) {
	if (archivo_igual_al_hueco(tamanio_archivo_en_memoria, hueco)) {
		list_remove(lista_de_huecos, 0);
	} else {
		hueco->base += tamanio_archivo_en_memoria;
		hueco->limite -= tamanio_archivo_en_memoria;
	}
}

int actualizar_tabla_segmentos(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite) {
	int indice_entrada_archivo_en_tabla_segmentos=-1;
	if (el_proceso_tiene_tabla_de_segmentos()) {
		indice_entrada_archivo_en_tabla_segmentos=crear_nueva_entrada_tabla_de_segmentos(parte_archivo, nueva_base, nuevo_limite);
	} else {
		indice_entrada_archivo_en_tabla_segmentos=agregar_nueva_tabla_segmentos_para_proceso(parte_archivo, nueva_base, nuevo_limite);
	}
	return indice_entrada_archivo_en_tabla_segmentos;
}

void copiar_archivo_a_memoria_fisica(size_t tamanio_archivo_en_memoria,
		t_hueco* hueco, char* archivo_separado_en_lineas) {
	memcpy(MEMORIA_FISICA + (hueco->base*TAMANIO_MAX_LINEA), archivo_separado_en_lineas,
			tamanio_archivo_en_memoria);
	free(archivo_separado_en_lineas);
}

void informar_carga_segmento_exitosa(int indice_entrada_archivo_en_tabla_segmentos, tp_cargarEnMemoria parte_archivo, int DAM_fd) {
	logger_funesMemory9(escribir_loguear, l_info,
			"\nSe creo el segmento %d para el archivo %s\n",
			indice_entrada_archivo_en_tabla_segmentos, parte_archivo->path);
	prot_enviar_FM9_DMA_cargaEnMemoria(
			indice_entrada_archivo_en_tabla_segmentos, DAM_fd);
}

t_archivo_cargandose * cargar_buffer_archivo(tp_cargarEnMemoria parte_archivo) {
	t_archivo_cargandose * archivo_de_proceso_cargandose;
	size_t tamanio_parte_archivo = strlen(parte_archivo->buffer);
	if (el_archivo_ya_se_estaba_cargando(parte_archivo)) {
		//El pedazo de archivo es conocido, hay que obtener la info que ya veníamos cargando
		obtener_archivo_en_curso_de_carga(parte_archivo, &archivo_de_proceso_cargandose);
		//TODO agregar algunas validaciones para no tener efectos adversos en el realloc
		size_t tamanio_archivo_cargandose= strlen(archivo_de_proceso_cargandose->buffer_archivo);
		archivo_de_proceso_cargandose->buffer_archivo=realloc(archivo_de_proceso_cargandose->buffer_archivo,
				tamanio_archivo_cargandose+tamanio_parte_archivo);
	} else {
		//El pedazo de archivo es nuevo, hay que crear el structure del mismo
		archivo_de_proceso_cargandose->pid = parte_archivo->pid;
		archivo_de_proceso_cargandose->recibido_actualmente=0;
		archivo_de_proceso_cargandose->buffer_archivo=malloc(tamanio_parte_archivo);
	}
	archivo_de_proceso_cargandose->recibido_actualmente +=tamanio_parte_archivo;
	memcpy(archivo_de_proceso_cargandose->buffer_archivo, parte_archivo->buffer,tamanio_parte_archivo);
	free(parte_archivo->buffer);
	return archivo_de_proceso_cargandose;
}

void cargar_parte_archivo_en_segmento(int DAM_fd){
	tp_cargarEnMemoria parte_archivo=prot_recibir_DMA_FM9_cargarEnMemoria(DAM_fd);
	t_archivo_cargandose * archivo_de_proceso_cargandose = cargar_buffer_archivo(parte_archivo);
	//TODO agregar validaciones de posibles errores
	if(todavia_falta_mandar_pedazo_de_archivo(parte_archivo, archivo_de_proceso_cargandose)){
		logger_funesMemory9(escribir_loguear, l_trace,"\nSe acumulo una parte del archivo en un buffer\n");
		prot_enviar_FM9_DMA_cargaEnMemoria(0, DAM_fd);
		return;
	}
	logger_funesMemory9(escribir_loguear, l_trace,"\nYa se obtuvo el archivo y se intentara agregar el segmento en la memoria principal\n");
	if(lista_de_huecos==NULL||list_is_empty(lista_de_huecos)){
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	t_hueco * hueco = tomar_hueco();
	char * archivo_separado_en_lineas=NULL;
	int cantidad_de_lineas=separar_en_lineas(archivo_de_proceso_cargandose,&archivo_separado_en_lineas);
	if(cantidad_de_lineas<0){
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	if (archivo_mas_grande_que_hueco(cantidad_de_lineas, hueco)) {
		informar_espacio_insuficiente(DAM_fd);
		return;
	}
	size_t tamanio_archivo_en_memoria =
			strlen(archivo_separado_en_lineas);

	copiar_archivo_a_memoria_fisica(tamanio_archivo_en_memoria, hueco,
			archivo_separado_en_lineas);

	actualizar_info_tabla_de_huecos(cantidad_de_lineas, hueco);

	int indice_entrada_archivo_en_tabla_segmentos=actualizar_tabla_segmentos(parte_archivo, hueco->base-cantidad_de_lineas, cantidad_de_lineas);
	//TODO borrar info del archivo cargandose
	informar_carga_segmento_exitosa(indice_entrada_archivo_en_tabla_segmentos,
			parte_archivo, DAM_fd);
	return;
}

/**
 * @DESC:Va sacando lineas desde el buffer del archivo.
 * Por cada linea, reservar memoria, y copia la linea al buffer del archivo separado en lineas de a tamanios de linea
 * hasta que queda un pedazo menor al tamanio de una linea, el cual tambien copia.
 */
int separar_en_lineas(t_archivo_cargandose * archivo_cargado, char** archivo_separado_en_lineas){
	//Convierto el stream a string para poder usar funciones de string de las commons
	archivo_cargado->buffer_archivo=realloc(archivo_cargado->buffer_archivo, archivo_cargado->recibido_actualmente+1);
	archivo_cargado->buffer_archivo[archivo_cargado->recibido_actualmente]='\0';
	char ** lineas=string_split(archivo_cargado->buffer_archivo,"\n");
	int cant_lineas=-1;
	for(cant_lineas=0;lineas[cant_lineas]!=NULL;cant_lineas++){
		if(cant_lineas==0){
			//se esta creando la primer linea, se usa malloc
			*archivo_separado_en_lineas=malloc(TAMANIO_MAX_LINEA);
		} else {
			//no es la primer linea, hay que usar realloc
			*archivo_separado_en_lineas=realloc(*archivo_separado_en_lineas,TAMANIO_MAX_LINEA);
		}
		string_append(archivo_separado_en_lineas,lineas[cant_lineas]);
		//Le tengo que agregar nuevamente el \n porque el split se lo quita
		string_append(archivo_separado_en_lineas,"\n");
		//Le sumo uno por el \n que tenia originalmente
		int tamanio_linea_separada = string_length(lineas[cant_lineas])+1;
		if (tamanio_linea_separada < TAMANIO_MAX_LINEA) {
			//Si la linea del archivo es mas chica que el tamanio de linea, completo con $
			int cantidad_sobrante=TAMANIO_MAX_LINEA-tamanio_linea_separada;
			char * relleno_sobrante=string_repeat('$',cantidad_sobrante);
			string_append(archivo_separado_en_lineas,relleno_sobrante);
		} else if (tamanio_linea_separada>TAMANIO_MAX_LINEA){
			logger_funesMemory9(escribir_loguear, l_warning,
						"\nSe exedio el tamanio de una linea\n");
			return -1;
		}

	}
	free(archivo_cargado->buffer_archivo);
	return cant_lineas;
}


void inicializar_lista_de_huecos() {
	lista_de_huecos = list_create();
	t_hueco hueco_inicial;
	hueco_inicial.base=0;
	hueco_inicial.limite=(int)TAMANIO_MEMORIA / TAMANIO_MAX_LINEA;
	list_add(lista_de_huecos,&hueco_inicial);
}

void crear_estructuras_esquema_segmentacion(){
	tablas_de_segmentos=list_create();
	inicializar_lista_de_huecos();
	archivos_cargandose=list_create();
}

void eliminar_lista_de_entradas(void * tabla_segmentos){
	list_destroy((*(t_tabla_segmentos*)tabla_segmentos).entradas);
}

void buscar_informacion_administrativa_esquema_segmentacion_y_mem_real(int id){
	t_entrada_tabla_segmentos * entrada_segmento;
	if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,id)){
		t_tabla_segmentos * tabla_segmentos = (t_tabla_segmentos *)list_filter_comparing(tablas_de_segmentos, &tiene_tabla_de_segmentos,id);

		int cant_entradas = list_size(tabla_segmentos->entradas);
		logger_funesMemory9(escribir_loguear, l_info, "Utilizando el esquema de segmentacion pura y dado el ID del DTB indicado, el proceso tiene una tabla de "
				"segmentos con %d entradas\n", cant_entradas);

		for(int i = 0; i<cant_entradas; i++){
			logger_funesMemory9(escribir_loguear, l_info, "En la entrada %d, se encuentra la sgte informacion: \n",i);
			entrada_segmento = list_get(tabla_segmentos->entradas,i);
			logger_funesMemory9(escribir_loguear, l_info, "El limite del segmento es %d. \n",entrada_segmento->limite);
			logger_funesMemory9(escribir_loguear, l_info, "Contiene el archivo %s. \n\n",entrada_segmento->archivo);
			logger_funesMemory9(escribir_loguear, l_info, "La base del segmento es %d. \n",entrada_segmento->base);

			int tamanio_archivo = (entrada_segmento->limite)*TAMANIO_MAX_LINEA;
			char * puntero_al_archivo = malloc(sizeof(tamanio_archivo)+1);

			memcpy(puntero_al_archivo,MEMORIA_FISICA + (entrada_segmento->base*TAMANIO_MAX_LINEA),tamanio_archivo);
			puntero_al_archivo[tamanio_archivo]='\0';

			logger_funesMemory9(escribir_loguear, l_info, "La información contenida en la memoria fisica para la entrada correspondiente es:"
					" %s \n",&puntero_al_archivo);
		}

	}else{
		logger_funesMemory9(escribir_loguear, l_warning, "No hay información administrativa para el ID %d del DTB indicado. \n",id);
	}
}

void destruir_estructuras_esquema_segmentacion(){
	if(tablas_de_segmentos!=NULL){
		list_destroy_and_destroy_elements(tablas_de_segmentos,&eliminar_lista_de_entradas);
	}
	if(lista_de_huecos!=NULL){
		list_destroy(lista_de_huecos);
	}
	if(archivos_cargandose!=NULL){
		list_destroy(archivos_cargandose);
	}
}

bool es_un_proceso_conocido(void * tabla_segmentos){
	return list_any_satisfy_comparing(archivos_cargandose, &el_proceso_tiene_archivo_cargandose, (*(t_tabla_segmentos*)tabla_segmentos).pid);
}

bool tiene_tabla_de_segmentos(void * tabla_segmentos, int pid){
	return pid == (*(t_tabla_segmentos*)tabla_segmentos).pid;
}

int agregar_entrada_tabla_segmentos(tp_cargarEnMemoria nombre_archivo, t_list* entradas_segmentos, int nueva_base, int nuevo_limite) {
	if(entradas_segmentos!=NULL){
		t_entrada_tabla_segmentos nueva_entrada_segmento;
		//actualiza con la base y segmento que resulta de agregar en la memoria fisica
		nueva_entrada_segmento.base = nueva_base;
		nueva_entrada_segmento.limite = nuevo_limite;
		nueva_entrada_segmento.archivo = nombre_archivo->path;
		list_add(entradas_segmentos, &nueva_entrada_segmento);
	}
	//Devolvemos el índice de la nueva entrada de la tabla de segmentos que es el tamanio menos uno por comenzar desde cero
	return list_size(entradas_segmentos)-1;
}

int crear_nueva_entrada_tabla_de_segmentos(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite) {
	//Crea nueva entrada en la tabla de segmentos
	t_tabla_segmentos* p_tabla_segmentos = list_find(tablas_de_segmentos, &es_un_proceso_conocido);
	if(p_tabla_segmentos!=NULL){
		 return agregar_entrada_tabla_segmentos(parte_archivo, (*p_tabla_segmentos).entradas, nueva_base, nuevo_limite);
	}
	return -1;
}

int todavia_falta_mandar_pedazo_de_archivo(tp_cargarEnMemoria pedazo_actual, t_archivo_cargandose * archivo_cargandose){
	return archivo_cargandose->recibido_actualmente<pedazo_actual->file_size;
}

void obtener_parte_archivo_con_segmentacion(int DAM_fd){

}
