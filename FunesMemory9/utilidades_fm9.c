/*
 * utilidades_fm9.c
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#include "utilidades_fm9.h"

void cargar_archivo_de_configuracion(char * path){

	logger_funesMemory9(escribir_loguear, l_info, "Cargando archivo de configuración...\n");

	t_config * config_file = config_create(path);
	char * modo;

	if(config_file==NULL){
		logger_funesMemory9(escribir_loguear, l_error, "No se pudo cargar el archivo de configuracion.\n");
		finalizar_funesMemory9();
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config_file,ARCH_CONFIG_MODO_EJECUCION)){
		modo = strdup(config_get_string_value(config_file, ARCH_CONFIG_MODO_EJECUCION));
		if (string_equals_ignore_case(modo, "SEG")){
			//Segmentacion pura
			MODO_EJECUCION = SEGMENTACION_PURA;
			logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Modo de ejecucion': %s - %d\n", modo, MODO_EJECUCION);
		}
		else if (string_equals_ignore_case(modo, "TPI")){
			//Tabla de paginas invertida
			MODO_EJECUCION = TABLA_PAGINAS_INVERTIDA;
			logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Modo de ejecucion': %s - %d\n", modo, MODO_EJECUCION);
		}
		else if (string_equals_ignore_case(modo, "SPA")){
			//Segmentacion paginada
			MODO_EJECUCION = SEGMENTACION_PAGINADA;
			logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Modo de ejecucion': %s - %d\n", modo, MODO_EJECUCION);
		}
	}

	if (config_has_property(config_file,ARCH_CONFIG_TAMANIO_MEMORIA)){
		TAMANIO_MEMORIA = config_get_int_value(config_file, ARCH_CONFIG_TAMANIO_MEMORIA);
		logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Tamanio de la memoria': %d\n", TAMANIO_MEMORIA);
	}

	if (config_has_property(config_file,ARCH_CONFIG_PUERTO_ESCUCHA)){
		PUERTO_ESCUCHA = strdup(config_get_string_value(config_file, ARCH_CONFIG_PUERTO_ESCUCHA));
		logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Puerto de escucha': %s\n",PUERTO_ESCUCHA);
	}

	if (config_has_property(config_file,ARCH_CONFIG_TAMANIO_MAX_LINEA)){
		TAMANIO_MAX_LINEA = config_get_int_value(config_file, ARCH_CONFIG_TAMANIO_MAX_LINEA);
		logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Tamaño maximo de linea': %d\n",TAMANIO_MAX_LINEA);
	}

	if (config_has_property(config_file,ARCH_CONFIG_TAMANIO_PAGINA)){
		TAMANIO_PAGINA = config_get_int_value(config_file, ARCH_CONFIG_TAMANIO_PAGINA);
		logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Tamanio de pagina': %d\n",TAMANIO_PAGINA);
	}

	free(modo);
	config_destroy(config_file);
}

int iniciar_servidor(char * port){
	int server_socket = crear_listen_socket(port,MAX_CLIENTES);

	if(server_socket < 0)
	{
		logger_funesMemory9(escribir_loguear,l_error,"Falló la creación del socket servidor\n");
		exit(1);
	}
	else
	{
		logger_funesMemory9(escribir_loguear,l_trace,"Socket servidor (%d) escuchando\n", server_socket);
	}

	return server_socket;
}

struct addrinfo* crear_addrinfo(){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	getaddrinfo("127.0.0.1", PUERTO_ESCUCHA, &hints, &serverInfo);
	return serverInfo;
}

int crear_listen_socket(char * puerto, int max_conexiones){
	struct sockaddr_in dir_sock;

	//Convierto el string a INT para htons
	unsigned int puerto_i = atoi(puerto);

	dir_sock.sin_family = AF_INET;
	dir_sock.sin_addr.s_addr = INADDR_ANY;
	dir_sock.sin_port = htons(puerto_i);

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		return -1;
	}

	int activado = 1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(server_socket, (void*)&dir_sock, sizeof(dir_sock)) != 0)
	{
		return -1;
	}

	listen(server_socket, max_conexiones);

	return server_socket;

}

void logger_funesMemory9(int tipo_esc, int tipo_log, const char* mensaje, ...){

	//Colores (reset,vacio,vacio,cian,verde,vacio,amarillo,rojo)
	static char *log_colors[8] = {"\x1b[0m","","","\x1b[36m", "\x1b[32m", "", "\x1b[33m", "\x1b[31m" };
	char *console_buffer=NULL;

	char *msj_salida = malloc(sizeof(char) * 256);

	//Captura los argumentos en una lista
	va_list args;
	va_start(args, mensaje);

	//Arma el mensaje formateado con sus argumentos en msj_salida.
	vsprintf(msj_salida, mensaje, args);

	//ESCRIBE POR PANTALLA
	if((tipo_esc == escribir) || (tipo_esc == escribir_loguear)){
		//printf("%s",msj_salida);
		//printf("");

		console_buffer = string_from_format("%s%s%s",log_colors[tipo_log],msj_salida, log_colors[0]);
		printf("%s",console_buffer);
		fflush(stdout);
		free(console_buffer);
	}

	//LOGUEA
	if((tipo_esc == loguear) || (tipo_esc == escribir_loguear)){

		if(tipo_log == l_info){
			log_info(logger, msj_salida);
		}
		else if(tipo_log == l_warning){
			log_warning(logger, msj_salida);
		}
		else if(tipo_log == l_error){
			log_error(logger, msj_salida);
		}
		else if(tipo_log == l_debug){
			log_debug(logger, msj_salida);
		}
		else if(tipo_log == l_trace){
			log_trace(logger, msj_salida);
		}
	}

	va_end(args);
	free(msj_salida);

	return;
}

void finalizar_funesMemory9(){
	//(destruir_estructuras_esquema[MODO_EJECUCION])();
	log_destroy(logger);
	free(MEMORIA_FISICA);
}

void captura_sigpipe(int signo){

    if(signo == SIGINT)
    {
    	logger_funesMemory9(escribir_loguear, l_warning,"Finalizando proceso... Gracias vuelva prontos.");
    	GLOBAL_SEGUIR = 0;
    	finalizar_funesMemory9();
    	exit(EXIT_FAILURE);
    }
    else if(signo == SIGPIPE)
    {
    	logger_funesMemory9(escribir_loguear, l_error," Se desconectó un proceso al que se quizo enviar.");
    }

}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
	signal_struct.sa_flags   = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
    if (sigaction(SIGPIPE, &signal_struct, NULL) < 0)
    {
    	logger_funesMemory9(escribir_loguear, l_error," SIGACTION error ");
    }

    sigaddset(&signal_struct.sa_mask, SIGINT);
    if (sigaction(SIGINT, &signal_struct, NULL) < 0)
    {
    	logger_funesMemory9(escribir_loguear, l_error," SIGACTION error ");
    }

}

void inicializar_logger(){
	logger = log_create("Log_FunesMemory9.txt", "FunesMemory9", false, LOG_LEVEL_TRACE);
}

bool el_proceso_tiene_archivo_cargandose(void * archivo_cargandose, int pid){
	return (*(t_archivo_cargandose*)archivo_cargandose).pid==pid;
}

bool el_archivo_ya_se_estaba_cargando(tp_cargarEnMemoria process_id) {
	return list_any_satisfy_comparing(archivos_cargandose,
			&el_proceso_tiene_archivo_cargandose, process_id->pid);
}

void obtener_archivo_en_curso_de_carga(tp_cargarEnMemoria parte_archivo,
		t_archivo_cargandose** archivo_de_proceso_cargandose) {
	t_list* archivo_cargandose_filtrado = list_filter_comparing(
			archivos_cargandose, &el_proceso_tiene_archivo_cargandose,
			parte_archivo->pid);
	*archivo_de_proceso_cargandose = list_get(archivo_cargandose_filtrado, 0);
}

void borrar_info_archivo_cargandose(int pid){
	t_archivo_cargandose * info_archivo_cargandose =
			(t_archivo_cargandose*) list_remove_by_condition_comparing(
					archivos_cargandose, &el_proceso_tiene_archivo_cargandose,
					pid);
	logger_funesMemory9(escribir_loguear, l_trace,"Se borra el elemento del archivo cargandose del proceso %d\n"
			,
			info_archivo_cargandose->pid);
	free(info_archivo_cargandose);
}

void borrar_info_archivo_devolviendose(int pid){
	logger_funesMemory9(escribir_loguear, l_trace,"Se borra el elemento del archivo devolviendose del proceso %d\n"
			,(*(t_archivo_devolviendose*)list_remove_by_condition_comparing(archivos_cargandose,
					&el_proceso_tiene_archivo_devolviendose, pid)).pid);
}

void informar_espacio_insuficiente(int DAM_fd) {
	logger_funesMemory9(escribir_loguear, l_warning,
			"Espacio insuficiente para albergar el archivo. Error 10002\n");
	prot_enviar_FM9_DMA_cargaEnMemoria(-1, DAM_fd);
}

bool el_proceso_tiene_archivo_devolviendose(void * archivo_devolviendose, int pid){
	return (*(t_archivo_devolviendose*)archivo_devolviendose).pid==pid;
}

void remover_caracter(char * string_afectado, char caracter){
	char *src, *dst;
	for (src = dst = string_afectado; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != caracter) dst++;
	}
	*dst = '\0';
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
		//El pedazo de archivo es nuevo, hay que crear el structure del mismo.
		archivo_de_proceso_cargandose=malloc(sizeof(t_archivo_cargandose));
		archivo_de_proceso_cargandose->pid = parte_archivo->pid;
		archivo_de_proceso_cargandose->recibido_actualmente=0;
		archivo_de_proceso_cargandose->buffer_archivo=malloc(tamanio_parte_archivo);
		//Tambien se agrega elemento de archivo cargandose a la lista
		logger_funesMemory9(escribir_loguear, l_trace,"Se agrega a la lista, un elemento por archivo cargandose del proceso %d\n"
					,parte_archivo->pid);
		list_add(archivos_cargandose,archivo_de_proceso_cargandose);
	}
	memcpy(archivo_de_proceso_cargandose->buffer_archivo+archivo_de_proceso_cargandose->recibido_actualmente,
			parte_archivo->buffer,tamanio_parte_archivo);
	archivo_de_proceso_cargandose->recibido_actualmente +=tamanio_parte_archivo;
	free(parte_archivo->buffer);
	return archivo_de_proceso_cargandose;
}

int todavia_falta_mandar_pedazo_de_archivo(tp_cargarEnMemoria pedazo_actual, t_archivo_cargandose * archivo_cargandose){
	return archivo_cargandose->recibido_actualmente<pedazo_actual->file_size;
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
		//Le sumo uno por el  que tenia originalmente
		int tamanio_linea_separada = string_length(lineas[cant_lineas])+1;
		if (tamanio_linea_separada < TAMANIO_MAX_LINEA) {
			//Si la linea del archivo es mas chica que el tamanio de linea, completo con $
			int cantidad_sobrante=TAMANIO_MAX_LINEA-tamanio_linea_separada;
			char * relleno_sobrante=string_repeat('$',cantidad_sobrante);
			string_append(archivo_separado_en_lineas,relleno_sobrante);
		} else if (tamanio_linea_separada>TAMANIO_MAX_LINEA){
			logger_funesMemory9(escribir_loguear, l_warning,
						"Se exedio el tamanio de una linea\n");
			return -1;
		}
	}
	free(archivo_cargado->buffer_archivo);
	return cant_lineas;
}

void informar_carga_segmento_exitosa(int indice_entrada_archivo_en_tabla_segmentos, tp_cargarEnMemoria parte_archivo, int DAM_fd) {
	logger_funesMemory9(escribir_loguear, l_info,
			"Se creo el segmento %d para el archivo %s\n",
			indice_entrada_archivo_en_tabla_segmentos, parte_archivo->path);
	prot_enviar_FM9_DMA_cargaEnMemoria(
			indice_entrada_archivo_en_tabla_segmentos, DAM_fd);
}

void inicializar_bitmap_de_marcos_libres() {
	bitmap_marcos_libres = list_create();
	int cantidad_de_marcos=(int)TAMANIO_MEMORIA / TAMANIO_PAGINA;
	t_disponibilidad_marco marco_libre;
	marco_libre.disponibilidad=0;
	for(int marco=0;marco<cantidad_de_marcos;marco++){
		marco_libre.indice=marco;
		list_add(bitmap_marcos_libres,&marco_libre);
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

void copiar_archivo_en_marcos_libres(char * archivo, int lineas, t_list * marcos_libres){
	//TODO terminar

}
