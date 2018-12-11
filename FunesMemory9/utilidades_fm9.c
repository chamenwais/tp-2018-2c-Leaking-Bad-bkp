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
	logger_funesMemory9(escribir_loguear, l_trace,"Se borra el elemento del archivo cargandose del proceso %d\n"
			,(*(t_archivo_cargandose*)list_remove_by_condition_comparing(archivos_cargandose, &el_proceso_tiene_archivo_cargandose, pid)).pid);
}

void informar_espacio_insuficiente(int DAM_fd) {
	logger_funesMemory9(escribir_loguear, l_warning,
			"Espacio insuficiente para albergar el archivo. Error 10002\n");
	prot_enviar_FM9_DMA_cargaEnMemoria(-1, DAM_fd);
}

bool el_proceso_tiene_archivo_devolviendose(void * archivo_devolviendose, int pid){
	return (*(t_archivo_cargandose*)archivo_devolviendose).pid==pid;
}
