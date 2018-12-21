/*
 * segmentacion_pura.c
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#include "segmentacion_pura.h"

int agregar_nueva_tabla_segmentos_para_proceso(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite) {
	//Crea tabla de segmentos para el nuevo proceso y lo agrega a la lista
	t_list* nuevas_entradas_segmentos = list_create();
	int indice_entrada_tabla_segmentos=agregar_entrada_tabla_segmentos(parte_archivo, nuevas_entradas_segmentos, nueva_base, nuevo_limite);
	t_tabla_segmentos* nueva_tabla_segmentos=malloc(sizeof(t_tabla_segmentos));
	nueva_tabla_segmentos->pid = parte_archivo->pid;
	nueva_tabla_segmentos->entradas = nuevas_entradas_segmentos;
	list_add(tablas_de_segmentos, nueva_tabla_segmentos);
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
		hueco = (t_hueco*)list_get(lista_de_huecos, 0);
	} else {
		hueco = (t_hueco*)tomar_hueco_con_limite_mas_grande();
	}
	logger_funesMemory9(escribir_loguear, l_info,"Se eligio el hueco con base %d y limite %d\n", hueco->base, hueco->limite);
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
	if (el_proceso_tiene_tabla_de_segmentos(parte_archivo->pid)) {
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

bool tienen_el_mismo_nombre(void * arch1){
	return string_equals_ignore_case(path_archivo_para_comparar, (char*)arch1);
}

void darle_una_linea_al_cpu_segmentacion_pura(int sock){
	tp_lineaCPU paquete_asignar_linea;
	paquete_asignar_linea = prot_recibir_CPU_FM9_pedir_linea(sock);

	if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,paquete_asignar_linea->id_GTD)){
		t_entrada_tabla_segmentos * entrada_segmento;
		t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos(paquete_asignar_linea->id_GTD);
		path_archivo_para_comparar = paquete_asignar_linea->linea;
		entrada_segmento = list_find(tabla_segmentos->entradas, &tienen_el_mismo_nombre);

		if(paquete_asignar_linea->pc > entrada_segmento->limite){
			enviarCabecera(sock, HuboProblemaConLaLineaParaCpu, sizeof(HuboProblemaConLaLineaParaCpu));
		}else{
			int primer_byte_archivo = (entrada_segmento->base)*TAMANIO_MAX_LINEA;
			char * direccion_base_archivo = MEMORIA_FISICA + primer_byte_archivo;
			int cant_bytes_hasta_linea = (paquete_asignar_linea->pc)*TAMANIO_MAX_LINEA;
			char * linea = malloc(TAMANIO_MAX_LINEA+1);
			memcpy(linea,direccion_base_archivo+cant_bytes_hasta_linea,TAMANIO_MAX_LINEA);
			//TODO aca faltan mallocs
			char* linea_auxiliar = string_duplicate(linea);
			string_trim(&linea_auxiliar);
			char** split = string_n_split(linea_auxiliar, 2, "$");
			char * linea_para_cpu = split[0];
			int largo = strlen(linea_para_cpu);
			linea_para_cpu[largo]='\0';
			prot_enviar_CPU_FM9_linea_pedida(linea_para_cpu, sock);
		}
	}else{
		enviarCabecera(sock, HuboProblemaConLaLineaParaCpu, sizeof(HuboProblemaConLaLineaParaCpu));
	}

}

void asignar_datos_a_linea_segmentacion_pura(int sock){
/*	tp_asignarDatosLinea paquete_asignar_datos_linea;
		paquete_asignar_datos_linea = prot_recibir_CPU_FM9_asignar_datos_linea(sock);

		//se repite todo hasta obtener la linea
		//no hacer el malloc
		//ver cuanto queda entre el barra n y el tam max de linea, si alcanza para meter lo que piden

		if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,paquete_asignar_datos_linea->id_GDT)){
			t_entrada_tabla_segmentos * entrada_segmento;
			t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos(paquete_asignar_datos_linea->id_GDT);
			path_archivo_para_comparar = paquete_asignar_datos_linea->path;
			entrada_segmento = list_find(tabla_segmentos->entradas, &tienen_el_mismo_nombre);

			if(paquete_asignar_datos_linea->pc > entrada_segmento->limite){
				logger_funesMemory9(escribir_loguear, l_error,"20002");
				enviarCabecera(sock, FalloDeSegmentoMemoria, sizeof(FalloDeSegmentoMemoria));
			}else{
				int primer_byte_archivo = (entrada_segmento->base)*TAMANIO_MAX_LINEA;
				char * direccion_base_archivo = MEMORIA_FISICA + primer_byte_archivo;

				int cant_bytes_hasta_linea = paquete_asignar_datos_linea->pc *TAMANIO_MAX_LINEA;
				char * linea = malloc(TAMANIO_MAX_LINEA+1);
				memcpy(linea,direccion_base_archivo+cant_bytes_hasta_linea,TAMANIO_MAX_LINEA);
				linea[TAMANIO_MAX_LINEA]='\0';

				int cuanto_ocupa_lo_que_voy_a_meter = strlen(paquete_asignar_datos_linea->datos);
				char* linea_auxiliar = string_duplicate(linea);
				string_trim(&linea_auxiliar);
				char** split = string_n_split(linea_auxiliar, 2, "\n");
				char * info_linea = split[0];
				char * lo_que_resta = split[1];
				int size_lo_que_resta = strlen(lo_que_resta);
				lo_que_resta[size_lo_que_resta]="\n";

				int desde_barra_n_hasta_final_linea = TAMANIO_MAX_LINEA - (strlen(lo_que_resta)+1);
				//todo eso ver si es mayor o igual que lo tengo que meter
				//si esta todo bien, asigno los datos, sino seg fault 20003
				//int cuantos_signos_pesos = desde_barra_n_hasta_final_linea - cuanto_ocupa_lo_que_voy_a_meter;
				// char * sobrante = string_repeat($, cuantos_signos_pesos);
				// string_append(&datos, sobrante);
				//memcpy(direccion_base_archivo+cant_bytes_hasta_linea+    strlen de lo que resta +1,
				//          datos, desde_barra_n_hasta_final_linea);
				//
			}
		}
*/
}

void agregar_info_tabla_de_huecos(t_hueco * hueco){
	list_add(lista_de_huecos, hueco);
}

bool realizar_operacion_liberar(tp_liberarArchivo paquete_liberar){
	t_entrada_tabla_segmentos * entrada_segmento;

	if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,paquete_liberar->id_GDT)){
		t_hueco* hueco;
		t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos(paquete_liberar->id_GDT);
		path_archivo_para_comparar = paquete_liberar->path;
		entrada_segmento = list_find(tabla_segmentos->entradas, &tienen_el_mismo_nombre);
		hueco->base = entrada_segmento->base;
		hueco->limite = entrada_segmento->limite;

		list_remove_by_condition(tabla_segmentos->entradas, &tienen_el_mismo_nombre);
		//agregar hueco
		agregar_info_tabla_de_huecos(hueco);
		return true;
	}else{
		return false;
	}
}

void liberar_archivo_abierto_segmentacion_pura(int sock){
	tp_liberarArchivo paquete_liberar = prot_recibir_CPU_FM9_liberar_archivo(sock);
	if(realizar_operacion_liberar(paquete_liberar)){
		enviarCabecera(sock, LiberarArchivoAbiertoEjecutandose, sizeof(LiberarArchivoAbiertoEjecutandose));
		logger_funesMemory9(escribir_loguear, l_info,"Se le aviso al CPU solicitante que se esta liberando el archivo deseado\n");
	}else{
		logger_funesMemory9(escribir_loguear, l_error,"40002\n");
		enviarCabecera(sock, LiberarArchivoAbiertoNoFinalizado, sizeof(LiberarArchivoAbiertoNoFinalizado));
	}
	free(paquete_liberar);
}

void cargar_parte_archivo_en_segmento(int DAM_fd){
	tp_cargarEnMemoria parte_archivo=prot_recibir_DMA_FM9_cargarEnMemoria(DAM_fd);
	t_archivo_cargandose * archivo_de_proceso_cargandose = cargar_buffer_archivo(parte_archivo);
	//TODO agregar validaciones de posibles errores
	if(todavia_falta_mandar_pedazo_de_archivo(parte_archivo, archivo_de_proceso_cargandose)){
		prot_enviar_FM9_DMA_cargaEnMemoria(0, DAM_fd);
		return;
	}
	logger_funesMemory9(escribir_loguear, l_trace,
			"Ya se obtuvo el archivo y se intentara agregar el segmento en la memoria principal\n");
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
	//borra info del archivo cargandose
	borrar_info_archivo_cargandose(parte_archivo->pid);
	informar_carga_segmento_exitosa(indice_entrada_archivo_en_tabla_segmentos,
			parte_archivo, DAM_fd);
	return;
}

void inicializar_lista_de_huecos() {
	lista_de_huecos = list_create();
	t_hueco * hueco_inicial=malloc(sizeof(t_hueco));
	hueco_inicial->base=0;
	hueco_inicial->limite=(int)TAMANIO_MEMORIA / TAMANIO_MAX_LINEA;
	list_add(lista_de_huecos,hueco_inicial);
}

void crear_estructuras_esquema_segmentacion(){
	tablas_de_segmentos=list_create();
	inicializar_lista_de_huecos();
	archivos_cargandose=list_create();
	archivos_devolviendose=list_create();
}

void eliminar_lista_de_entradas(void * tabla_segmentos){
	list_destroy((*(t_tabla_segmentos*)tabla_segmentos).entradas);
}

void eliminar_hueco_de_lista_de_huecos(void * hueco){
	free(hueco);
}

t_tabla_segmentos* buscar_tabla_de_segmentos(int pid) {
	return (t_tabla_segmentos*) list_get(list_filter_comparing(tablas_de_segmentos,
			&tiene_tabla_de_segmentos, pid),0);
}

void buscar_informacion_administrativa_esquema_segmentacion_y_mem_real(int id){
	if(id == -1){
		logger_funesMemory9(escribir_loguear, l_error,"Falta el ID del DTB, proba de nuevo\n");
	}else{
		t_entrada_tabla_segmentos * entrada_segmento;
		if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,id)){
			t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos(id);

			int cant_entradas = list_size(tabla_segmentos->entradas);
			logger_funesMemory9(escribir_loguear, l_info, "Utilizando el esquema de segmentacion pura y dado el ID del DTB indicado, el proceso tiene una tabla de "
					"segmentos con %d entradas\n", cant_entradas);

			for(int i = 0; i<cant_entradas; i++){
				logger_funesMemory9(escribir_loguear, l_info, "En la entrada %d, se encuentra la sgte informacion: ",i);
				entrada_segmento = list_get(tabla_segmentos->entradas,i);
				logger_funesMemory9(escribir_loguear, l_info, "El limite del segmento es %d.\n",entrada_segmento->limite);
				logger_funesMemory9(escribir_loguear, l_info, "Contiene el archivo %s.\n",entrada_segmento->archivo);
				logger_funesMemory9(escribir_loguear, l_info, "La base del segmento es %d.\n",entrada_segmento->base);

				int tamanio_archivo = (entrada_segmento->limite)*TAMANIO_MAX_LINEA;
				char * puntero_al_archivo = malloc(tamanio_archivo+1);

				memcpy(puntero_al_archivo,MEMORIA_FISICA + (entrada_segmento->base*TAMANIO_MAX_LINEA),tamanio_archivo);
				puntero_al_archivo[tamanio_archivo]='\0';

				char ** lineas=string_split(puntero_al_archivo, "\n");
				logger_funesMemory9(escribir_loguear, l_info,
						"La información contenida en la memoria fisica para la entrada correspondiente es: \n");
				for(int cant_lineas=0;lineas[cant_lineas]!=NULL;cant_lineas++){
					char * linea=lineas[cant_lineas];
					remover_caracter(linea,'$');
					logger_funesMemory9(escribir_loguear, l_info,"%s\n",linea);
				}

				free(puntero_al_archivo);
			}

		}else{
			logger_funesMemory9(escribir_loguear, l_warning, "No hay información administrativa para el ID %d del DTB indicado.\n",id);
		}
	}
}

void destruir_estructuras_esquema_segmentacion(){
	if(tablas_de_segmentos!=NULL){
		list_destroy_and_destroy_elements(tablas_de_segmentos,&eliminar_lista_de_entradas);
	}
	if(lista_de_huecos!=NULL){
		list_destroy_and_destroy_elements(lista_de_huecos,&eliminar_hueco_de_lista_de_huecos);
	}
	if(archivos_cargandose!=NULL){
		list_destroy(archivos_cargandose);
	}
	if(archivos_devolviendose!=NULL){
		list_destroy(archivos_devolviendose);
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
		t_entrada_tabla_segmentos* nueva_entrada_segmento=malloc(sizeof(t_entrada_tabla_segmentos));
		//actualiza con la base y segmento que resulta de agregar en la memoria fisica
		nueva_entrada_segmento->base = nueva_base;
		nueva_entrada_segmento->limite = nuevo_limite;
		nueva_entrada_segmento->archivo = nombre_archivo->path;
		list_add(entradas_segmentos, nueva_entrada_segmento);
	}
	//Devolvemos el índice de la nueva entrada de la tabla de segmentos que es el tamanio menos uno por comenzar desde cero
	return list_size(entradas_segmentos)-1;
}

int crear_nueva_entrada_tabla_de_segmentos(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite) {
	//Crea nueva entrada en la tabla de segmentos
	bool el_pid_tiene_tabla_de_segmentos(void * tabla_segmentos){
		return (*(t_tabla_segmentos*)tabla_segmentos).pid==parte_archivo->pid;
	}
	t_tabla_segmentos* p_tabla_segmentos = list_find(tablas_de_segmentos, &el_pid_tiene_tabla_de_segmentos);
	if(p_tabla_segmentos!=NULL){
		 return agregar_entrada_tabla_segmentos(parte_archivo, (*p_tabla_segmentos).entradas, nueva_base, nuevo_limite);
	}
	return -1;
}

t_archivo_devolviendose* filtrar_info_archivo_devolviendose(int id_dtb){
	return (t_archivo_devolviendose*)(list_get(list_filter_comparing(archivos_devolviendose,&el_proceso_tiene_archivo_devolviendose, id_dtb),0));
}

int obtener_tamanio_trozo_faltante(int tamanio_archivo_en_memoria,
		int bytes_transferidos) {
	return tamanio_archivo_en_memoria - bytes_transferidos;
}

bool trozo_faltante_es_menor_que_transfersize(t_archivo_devolviendose* info_trozo_archivo, int transfer_size){
	int tamanio_archivo_en_memoria =
			info_trozo_archivo->tamanio_archivo_en_memoria;
	int bytes_transferidos = info_trozo_archivo->bytes_transferidos;
	int tamanio_trozo_faltante = obtener_tamanio_trozo_faltante(
			tamanio_archivo_en_memoria, bytes_transferidos);
	return tamanio_trozo_faltante<transfer_size;
}

void devolver_trozo_faltante_archivo(tp_obtenerArchivo pedido_obtencion,
		int DAM_fd, t_archivo_devolviendose* info_archivo_devolviendose) {
	int tamanio_trozo_faltante = obtener_tamanio_trozo_faltante(
			info_archivo_devolviendose->tamanio_archivo_en_memoria,
			pedido_obtencion->size);
	char* trozo_archivo = malloc(tamanio_trozo_faltante);
	memcpy(trozo_archivo,
			(info_archivo_devolviendose->buffer_en_memoria)
					+ (info_archivo_devolviendose->bytes_transferidos),
			tamanio_trozo_faltante);
	prot_enviar_FM9_DMA_devolverDatos(trozo_archivo, tamanio_trozo_faltante,
			info_archivo_devolviendose->tamanio_archivo_en_memoria, DAM_fd);
	free(trozo_archivo);
	free(info_archivo_devolviendose->buffer_en_memoria);
	free(pedido_obtencion->path);
	free(info_archivo_devolviendose);
	logger_funesMemory9(escribir_loguear, l_info, "Se devolvio el ultimo pedazo del archivo %s de %d bytes\n",
			pedido_obtencion->path, tamanio_trozo_faltante);
}

void devolver_trozo_con_tamanio_transfersize(tp_obtenerArchivo pedido_obtencion,
		int DAM_fd, t_archivo_devolviendose* info_archivo_devolviendose) {
	int transfer_size = pedido_obtencion->size;
	char* trozo_archivo = malloc(transfer_size);
	memcpy(trozo_archivo,
				(info_archivo_devolviendose->buffer_en_memoria)
						+ (info_archivo_devolviendose->bytes_transferidos),
						transfer_size);
	info_archivo_devolviendose->bytes_transferidos += transfer_size;
	prot_enviar_FM9_DMA_devolverDatos(trozo_archivo, transfer_size, info_archivo_devolviendose->tamanio_archivo_en_memoria
			, DAM_fd);
	free(trozo_archivo);
	free(pedido_obtencion->path);
	logger_funesMemory9(escribir_loguear, l_info, "Se devolvio un pedazo del archivo %s de transfer size %d bytes\n",
				pedido_obtencion->path, transfer_size);
}

void informar_archivo_no_abierto(int DAM_fd) {
	enviarCabecera(DAM_fd, ElArchivoNoExiste, sizeof(ElArchivoNoExiste));
	logger_funesMemory9(escribir_loguear, l_warning,
			"Se informa a Safa que el archivo no se encuentra abierto\n");
}

t_archivo_devolviendose* crear_elemento_en_archivos_devolviendose(t_entrada_tabla_segmentos* segmento_archivo, int pid){
	t_archivo_devolviendose* nueva_devolucion=malloc(sizeof(t_archivo_devolviendose));
	int lineas_archivo=segmento_archivo->limite;
	int bytes_brutos=lineas_archivo*TAMANIO_MAX_LINEA;
	nueva_devolucion->buffer_en_memoria=malloc(bytes_brutos+1);
	memcpy(nueva_devolucion->buffer_en_memoria, MEMORIA_FISICA+(segmento_archivo->base*TAMANIO_MAX_LINEA)
			, bytes_brutos);
	nueva_devolucion->buffer_en_memoria[bytes_brutos]='\0';
	//Quitamos el relleno para finalizar la linea
	remover_caracter(nueva_devolucion->buffer_en_memoria, '$');
	nueva_devolucion->tamanio_archivo_en_memoria=string_length(nueva_devolucion->buffer_en_memoria);
	//inicializa cantidad de bytes copiados en cero
	nueva_devolucion->bytes_transferidos=0;
	nueva_devolucion->pid=pid;
	return nueva_devolucion;
}

void obtener_parte_archivo_con_segmentacion(int DAM_fd){
	tp_obtenerArchivo pedido_obtencion=prot_recibir_DMA_FM9_obtenerArchivo(DAM_fd);
	t_archivo_devolviendose* info_archivo_devolviendose;
	if(list_any_satisfy_comparing(archivos_devolviendose, &el_proceso_tiene_archivo_devolviendose, pedido_obtencion->pid)){
		enviarCabecera(DAM_fd, ElArchivoExiste, sizeof(ElArchivoExiste));
		info_archivo_devolviendose=filtrar_info_archivo_devolviendose(pedido_obtencion->pid);
		if(trozo_faltante_es_menor_que_transfersize(info_archivo_devolviendose, pedido_obtencion->size)){
			devolver_trozo_faltante_archivo(pedido_obtencion, DAM_fd, info_archivo_devolviendose);
			//elimina elemento de lista de archivos devolviendose
			borrar_info_archivo_devolviendose(pedido_obtencion->pid);
			return;
		} else {
			devolver_trozo_con_tamanio_transfersize(pedido_obtencion, DAM_fd, info_archivo_devolviendose);
			return;
		}
	} else if(list_any_satisfy_comparing(tablas_de_segmentos,&tiene_tabla_de_segmentos,pedido_obtencion->pid)){
		//Obtiene tabla de segmentos
		t_tabla_segmentos * tabla_segmentos = buscar_tabla_de_segmentos(pedido_obtencion->pid);
		t_entrada_tabla_segmentos* segmento;
		t_list* segmentos = tabla_segmentos->entradas;
		if (list_get(segmentos, pedido_obtencion->memory_address) != NULL) {
			segmento=(t_entrada_tabla_segmentos*)
					list_get(segmentos, pedido_obtencion->memory_address);
			t_archivo_devolviendose* nuevo_archivo_devolviendose;
			if(string_equals_ignore_case(segmento->archivo,pedido_obtencion->path)){
				//crea elemento en archivos devolviendose
				nuevo_archivo_devolviendose=crear_elemento_en_archivos_devolviendose(segmento, pedido_obtencion->pid);
				devolver_trozo_con_tamanio_transfersize(pedido_obtencion, DAM_fd, nuevo_archivo_devolviendose);
			}
			bool es_el_segmento_buscado(void * segmento){
				return string_equals_ignore_case((*(t_entrada_tabla_segmentos*)segmento).archivo,pedido_obtencion->path);
			}
			if(list_any_satisfy(segmentos,&es_el_segmento_buscado)){
				segmento=(t_entrada_tabla_segmentos*)
						list_get(list_filter(segmentos, &es_el_segmento_buscado),0);
				nuevo_archivo_devolviendose=crear_elemento_en_archivos_devolviendose(segmento, pedido_obtencion->pid);
				devolver_trozo_con_tamanio_transfersize(pedido_obtencion, DAM_fd, nuevo_archivo_devolviendose);
			}
		}
	}
	//informa archivo no abierto
	informar_archivo_no_abierto(DAM_fd);
}
