/*
 * FunesMemory9.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "FunesMemory9.h"


int iniciar_servidor(char * port){
	int server_socket = crear_listen_socket(port,MAX_CLIENTES_CPU);

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
	getaddrinfo(IP, PUERTO_ESCUCHA, &hints, &serverInfo);
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

int atender_nuevo_cpu(int serv_socket){
	struct sockaddr_in client_addr;

	//Setea la direccion en 0
	//memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);

	if (new_client_sock < 0) {
		logger_funesMemory9(escribir_loguear,l_error,"Error al aceptar un nuevo CPU :(\n");
	  return -1;
	}

	logger_funesMemory9(escribir_loguear,l_trace,"\nSe aceptó un nuevo CPU, conexión (%d)", new_client_sock);

	close(new_client_sock);

}

void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
}

void *escuchar_mensajes_entrantes(int socket_cliente){

    logger_funesMemory9(escribir_loguear,l_info, "Esperando mensajes... \n");

    total_hilos++;

    //interpretar_header(header, socket_cliente);

    sleep(10);

    //Si no recibo nada...
    logger_funesMemory9(escribir_loguear, l_info, "\nCerrada la conexión con socket cliente\n");

    close(socket_cliente);
}

void finalizar_funesMemory9(){
	//Por el momento solo esta para liberar el logger...
	log_destroy(logger);
}

void captura_sigpipe(int signo){
    int i;

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

void iniciar_funes_memory_9(){
	inicializar_logger();

	logger_funesMemory9(escribir_loguear, l_warning,"\nHola! Soy Funes Memory 9, a sus ordenes... :) \n");

	//cargar_archivo_de_configuracion();

	configurar_signals();
}

int main(){

	iniciar_funes_memory_9();

	int server_FM9 = iniciar_servidor(PUERTO_ESCUCHA);

	//Entra en un loop consecutivo hasta un ctrl+c...
	while (GLOBAL_SEGUIR){

		int socket_cliente = atender_nuevo_cpu(server_FM9);

		crear_hilo_conexion(socket_cliente, escuchar_mensajes_entrantes);
	}

	close(server_FM9);

	finalizar_funesMemory9();

}
