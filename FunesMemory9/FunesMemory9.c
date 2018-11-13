/*
 * FunesMemory9.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "FunesMemory9.h"

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
		logger_funesMemory9(escribir_loguear, l_info, "Se obtuvo configuración 'Tamanio de la memoria': %d \n", TAMANIO_MEMORIA);
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
		logger_funesMemory9(escribir_loguear,l_error,"\nFalló la creación del socket servidor");
		exit(1);
	}
	else
	{
		logger_funesMemory9(escribir_loguear,l_trace,"Socket servidor (%d) escuchando", server_socket);
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
		//printf("\n");

		console_buffer = string_from_format("%s%s%s",log_colors[tipo_log],msj_salida, log_colors[0]);
		printf("%s",console_buffer);
		printf("\n");
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

	log_destroy(logger);

	free(MEMORIA_FISICA);
}

void captura_sigpipe(int signo){

    if(signo == SIGINT)
    {
    	logger_funesMemory9(escribir_loguear, l_warning,"\nFinalizando proceso... Gracias vuelva prontos.\n");
    	GLOBAL_SEGUIR = 0;
    	finalizar_funesMemory9();
    	exit(EXIT_FAILURE);
    }
    else if(signo == SIGPIPE)
    {
    	logger_funesMemory9(escribir_loguear, l_error,"\n Se desconectó un proceso al que se quizo enviar.\n");
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
    	logger_funesMemory9(escribir_loguear, l_error,"\n SIGACTION error \n");
    }

    sigaddset(&signal_struct.sa_mask, SIGINT);
    if (sigaction(SIGINT, &signal_struct, NULL) < 0)
    {
    	logger_funesMemory9(escribir_loguear, l_error,"\n SIGACTION error \n");
    }

}

void inicializar_logger(){
	logger = log_create("Log_FunesMemory9.txt", "FunesMemory9", false, LOG_LEVEL_TRACE);
}

void iniciar_funes_memory_9(char * path){
	inicializar_logger();

	logger_funesMemory9(escribir_loguear, l_warning,"\nHola! Soy Funes Memory 9, a sus ordenes... :) \n");

	cargar_archivo_de_configuracion(path);

	configurar_signals();
}

t_list* list_filter_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	t_list* filtered = list_create();

	void _add_if_apply(void* element) {
		if (condition_equal_to(element, to_compare)) {
			list_add(filtered, element);
		}
	}

	list_iterate(self, _add_if_apply);
	return filtered;
}

int list_count_satisfying_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	t_list *satisfying = list_filter_comparing(self, condition_equal_to, to_compare);
	int result = satisfying->elements_count;
	list_destroy(satisfying);
	return result;
}

bool list_any_satisfy_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	return list_count_satisfying_comparing(self, condition_equal_to, to_compare) > 0;
}

int atender_nuevo_cpu(int serv_socket){
	struct sockaddr_in client_addr;

	//Setea la direccion en 0
	//memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);

	if (new_client_sock < 0) {
		logger_funesMemory9(escribir_loguear,l_error,"Error al aceptar un nuevo cpu :(\n");
	  return -1;
	}

	logger_funesMemory9(escribir_loguear,l_trace,"\nSe aceptó un nuevo cpu, conexión (%d)", new_client_sock);

	return new_client_sock;

}

void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
}

void *escuchar_mensajes_de_cpus(int socket_cpu){

    logger_funesMemory9(escribir_loguear,l_info, "Esperando mensajes de un cpu... \n");

    total_hilos++;

    //Si no recibo nada...
    logger_funesMemory9(escribir_loguear, l_info, "\nCerrada la conexión con socket de cpu\n");
}

bool cabecera_es_invalida(t_cabecera* cabecera){
	if(sizeof(*cabecera) != 0 && cabecera->tamanio>0){
		logger_funesMemory9(escribir_loguear,l_trace,"\nEl Diego dijo algo, vamos a tratar de entenderlo\n");
		return false;
	}
	logger_funesMemory9(escribir_loguear,l_error,"No se le entendio nada al Diego, se seguira escuchando\n");
	return true;
}

void interpretar_mensaje_del_diego(enum MENSAJES mensaje, int DMA_socket){
	switch(mensaje){
		case CargarParteEnMemoria:
			logger_funesMemory9(escribir_loguear, l_info, "\nEl Diego quiere cargar una parte de un archivo\n");
			(cargar_parte_archivo[MODO_EJECUCION])(DMA_socket);
			break;
		//TODO agregar otros pedidos de DMA
		default:
			break;
	}
}

void *escuchar_al_diego(int socket_DMA){

	logger_funesMemory9(escribir_loguear,l_info, "Esperando que el Diego termine una frase... \n");

	while(true){
		t_cabecera cabecera=recibirCabecera(socket_DMA);

		if(cabecera_es_invalida(&cabecera)) continue;

		interpretar_mensaje_del_diego(cabecera.tipoDeMensaje, socket_DMA);

	}

}

int comunicarse_con_dam(int socket_escucha){
	logger_funesMemory9(escribir_loguear, l_trace,"\nVoy a esperar al Diego \n");

	int socket_dam=aceptarConexion(socket_escucha);

	logger_funesMemory9(escribir_loguear, l_trace,"\nVoy a realizar un handshake con el Diego \n");

	if(socket_dam > 0 && (recibirHandshake(MEMORIA, DMA, socket_dam) != 0)){
		logger_funesMemory9(escribir_loguear, l_trace,"\nHandshake con el Diego hecho! \n");
		return socket_dam;
	}

	logger_funesMemory9(escribir_loguear, l_error,"\n No se pudo realizar handshake con el Diego \n");

	close(socket_escucha);

	finalizar_funesMemory9();

	return -1;
}

char *reservar_total_memoria(){
	return malloc(TAMANIO_MEMORIA);
}

void cerrar_sockets(int server_FM9, int socket_cpu, int cliente_DAM) {
	close(server_FM9);
	close(socket_cpu);
	close(cliente_DAM);
}

bool el_proceso_tiene_archivo_cargandose(void * archivo_cargandose, int pid){
	return (*(t_archivo_cargandose*)archivo_cargandose).pid==pid;
}

bool es_un_proceso_conocido(void * tabla_segmentos){
	return list_any_satisfy_comparing(archivos_cargandose, &el_proceso_tiene_archivo_cargandose, (*(t_tabla_segmentos*)tabla_segmentos).pid);
}

void agregar_entrada_tabla_segmentos(tp_cargarEnMemoria nombre_archivo, t_list* entradas_segmentos) {
	t_entrada_tabla_segmentos nueva_entrada_segmento;
	//TODO actualizar con la base y segmento que resulta de agregar en la memoria fisica
	nueva_entrada_segmento.base = 0;
	nueva_entrada_segmento.limite = 0;
	nueva_entrada_segmento.archivo = nombre_archivo->path;
	list_add(entradas_segmentos, &nueva_entrada_segmento);
}

void crear_nueva_entrada_tabla_de_segmentos(tp_cargarEnMemoria parte_archivo) {
	//Crea nueva entrada en la tabla de segmentos
	//TODO validar que la lista de entradas de segmentos no este vacia
	t_tabla_segmentos* p_tabla_segmentos = list_find(tablas_de_segmentos,
			&es_un_proceso_conocido);
	agregar_entrada_tabla_segmentos(parte_archivo, (*p_tabla_segmentos).entradas);
}

int el_proceso_tiene_tabla_de_segmentos() {
	return !list_is_empty(tablas_de_segmentos)
			&& list_any_satisfy(tablas_de_segmentos,
					&es_un_proceso_conocido);
}

void agregar_nueva_tabla_segmentos_para_proceso(tp_cargarEnMemoria parte_archivo) {
	//Crea tabla de segmentos para el nuevo proceso y lo agrega a la lista
	t_list* nuevas_entradas_segmentos = list_create();
	agregar_entrada_tabla_segmentos(parte_archivo, nuevas_entradas_segmentos);
	t_tabla_segmentos nueva_tabla_segmentos;
	nueva_tabla_segmentos.pid = parte_archivo->pid;
	nueva_tabla_segmentos.entradas = nuevas_entradas_segmentos;
	list_add(tablas_de_segmentos, &nueva_tabla_segmentos);
}

void separar_en_lineas(char * trozo_de_archivo, char ** buffer_de_lineas){
	double trozo_dividido_linea=sizeof(&trozo_de_archivo)/TAMANIO_MAX_LINEA;
	if(trozo_dividido_linea>1){
		//TODO ir copiando desde el trozo de archivo al buffer de lineas de a tamanios de linea con memcpy
		//hasta que te quede un pedazo menor al tamanio de una linea, completar con $
	} else if (trozo_dividido_linea<1){
		//TODO como el tamanio del pedazo menor al tamanio de una linea, completar con $
	} else {
		//TODO simplemente copiar el pedazo de archivo al buffer porque es igual a una linea
	}
}

void cargar_parte_archivo_en_segmento(int DAM_fd){
	tp_cargarEnMemoria parte_archivo=prot_recibir_DMA_FM9_cargarEnMemoria(DAM_fd);
	t_archivo_cargandose archivo_de_proceso_cargandose;
	archivo_de_proceso_cargandose.pid=parte_archivo->pid;
	separar_en_lineas(parte_archivo->buffer, &(archivo_de_proceso_cargandose.buffer_archivo));
	//TODO DAM va a mandar un pedazo de archivo= true
	while(/* TODO DAM va a mandar un pedazo de archivo y todavia le falta mandar*/){
		//TODO recibo el pedazo y cargo el buffer
		//TODO pregunto si DAM mando otro pedazo de archivo
	}
	//TODO si DAM mando a cargar un pedazo de archivo actualizamos la tabla de segmentos, si no que fluya

	//TODO hay que mantener una lista con los segmentos asignados en memoria y huecos libres
	//de ahí saldra la base y límite

	//Hacer esto una vez que se cargue el segmento en un hueco
	if (el_proceso_tiene_tabla_de_segmentos()) {
		crear_nueva_entrada_tabla_de_segmentos(parte_archivo);
	} else {
		agregar_nueva_tabla_segmentos_para_proceso(parte_archivo);
	}

	//TODO
	//Ír cargando el buffer_archivo con lo que viene en la parte_archivo
	//Al principio hay que hacer un malloc para el tamanio de la parte y luego ir agrandando el heap con realloc
	//por ser el tamanio variable y no conocer la cantidad de antemano
	//(tener cuidado de los comportamientos de realloc)
	//si se termina la carga del archivo, liberar el buffer_archivo

	//recibir pid y buscarlo en la tabla de segmentos para ver si ya existía, si no crearle la tabla. Actualizarla con la info de lo que
	//se esta cargando

	//Cuando ya se tenga todo el archivo en el buffer, hay que desmenuzarlo en líneas, cargarlo en el malloc gigante
	//con algo como: char * direccion_linea_3 = MEMORIA_FISICA + (3*TAMANIO_MAX_LINEA);
}

void cargar_parte_archivo_en_segmento_paginado(int DAM_fd){

}

void cargar_parte_archivo_en_pagina_invertida(int DAM_fd){

}

void inicializar_lista_de_huecos() {
	lista_de_huecos = list_create();
	t_hueco hueco_inicial;
	hueco_inicial.base=0;
	hueco_inicial.limite=TAMANIO_MEMORIA / TAMANIO_MAX_LINEA;
	list_add(lista_de_huecos,&hueco_inicial);
}

void crear_estructuras_esquema_segmentacion(){
	tablas_de_segmentos=list_create();
	inicializar_lista_de_huecos();
	archivos_cargandose=list_create();
}

void crear_estructuras_esquema_segmentacion_paginada(){

}

void crear_estructuras_esquema_paginacion_invertida(){

}

void destruir_estructuras_esquema_segmentacion(){

}

void destruir_estructuras_esquema_segmentacion_paginada(){

}

void eliminar_lista_de_entradas(void * tabla_segmentos){
	list_destroy((*(t_tabla_segmentos*)tabla_segmentos).entradas);
}

void destruir_estructuras_esquema_paginacion_invertida(){
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

void inicializar_funciones_variables_por_segmento(){
	crear_estructuras_esquema[SEGMENTACION_PURA]=&crear_estructuras_esquema_segmentacion;
	crear_estructuras_esquema[SEGMENTACION_PAGINADA]=&crear_estructuras_esquema_segmentacion_paginada;
	crear_estructuras_esquema[TABLA_PAGINAS_INVERTIDA]=&crear_estructuras_esquema_paginacion_invertida;
	cargar_parte_archivo[SEGMENTACION_PURA]=&cargar_parte_archivo_en_segmento;
	cargar_parte_archivo[SEGMENTACION_PAGINADA]=&cargar_parte_archivo_en_segmento_paginado;
	cargar_parte_archivo[TABLA_PAGINAS_INVERTIDA]=&cargar_parte_archivo_en_pagina_invertida;
	destruir_estructuras_esquema[SEGMENTACION_PURA]=&destruir_estructuras_esquema_segmentacion;
	destruir_estructuras_esquema[SEGMENTACION_PAGINADA]=&destruir_estructuras_esquema_segmentacion_paginada;
	destruir_estructuras_esquema[TABLA_PAGINAS_INVERTIDA]=&destruir_estructuras_esquema_paginacion_invertida;
}

int main(int argc, char **argv){

	iniciar_funes_memory_9(argv[1]);

	int server_FM9, socket_cpu, cliente_DAM;

	server_FM9 = iniciar_servidor(PUERTO_ESCUCHA);

	MEMORIA_FISICA = reservar_total_memoria();

	inicializar_funciones_variables_por_segmento();

	(crear_estructuras_esquema[MODO_EJECUCION])();

	cliente_DAM = comunicarse_con_dam(server_FM9);

	crear_hilo_conexion(cliente_DAM, escuchar_al_diego);

	//Entra en un loop consecutivo hasta un ctrl+c...
	while (GLOBAL_SEGUIR && cliente_DAM>0){

		socket_cpu = atender_nuevo_cpu(server_FM9);

		crear_hilo_conexion(socket_cpu, escuchar_mensajes_de_cpus);
	}

	cerrar_sockets(server_FM9, socket_cpu, cliente_DAM);

	(destruir_estructuras_esquema[MODO_EJECUCION])();

	finalizar_funesMemory9();
}

