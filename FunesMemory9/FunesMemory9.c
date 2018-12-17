/*
 * FunesMemory9.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "FunesMemory9.h"

void stdin_no_bloqueante(void){
	  /* Set nonblock for stdin. */
	  int flag = fcntl(STDIN_FILENO, F_GETFL, 0);
	  flag |= O_NONBLOCK;
	  fcntl(STDIN_FILENO, F_SETFL, flag);
}

void iniciar_funes_memory_9(char * path){
	inicializar_logger();
	logger_funesMemory9(escribir_loguear, l_info,"Hola! Soy Funes Memory 9, a sus ordenes... :)\n");
	cargar_archivo_de_configuracion(path);
	configurar_signals();
}

int atender_nuevo_cpu(int serv_socket){
	struct sockaddr_in client_addr;

	//Setea la direccion en 0
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);
	if (new_client_sock < 0) {
		logger_funesMemory9(escribir_loguear,l_error,"Error al aceptar un nuevo cpu :(\n");
	  return -1;
	}

	logger_funesMemory9(escribir_loguear,l_trace,"\nSe aceptó un nuevo cpu, conexión (%d)\n", new_client_sock);

	//Lo agrego a la lista de conexiones con cpu actuales
	for (int i = 0; i < MAX_CLIENTES; ++i) {

		if (conexiones_cpu[i].socket == NO_SOCKET) {
			conexiones_cpu[i].socket = new_client_sock;
			conexiones_cpu[i].addres = client_addr;

	        return 0;
	    }
	 }

	logger_funesMemory9(escribir_loguear,l_error,"Demasiadas conexiones. Cerrando nueva conexion\n");
	 close(new_client_sock);

	 return -1;
}

void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
}

t_cabecera recibir_mensaje_cpu(t_conexion_cpu conexion_cpu){

	logger_funesMemory9(escribir_loguear,l_info,"Llegó un nuevo mensaje desde el CPU %d!\n",conexion_cpu.pid);
	t_cabecera cabecera=recibirCabecera(conexion_cpu.socket);
	interpretar_mensaje_cpu(cabecera.tipoDeMensaje, conexion_cpu.socket);

	return cabecera;
}

bool cabecera_es_invalida_dam(t_cabecera cabecera){
	if(sizeof(cabecera) != 0 && cabecera.tamanio>0){
		logger_funesMemory9(escribir_loguear,l_info,"El Diego dijo algo, vamos a tratar de entenderlo\n");
		return false;
	}
	logger_funesMemory9(escribir_loguear,l_error,"No se le entendio nada al Diego, se cerrara la conexion\n");
	return true;
}

bool cabecera_es_invalida_cpu(t_cabecera cabecera){
	if(sizeof(cabecera) != 0 && cabecera.tamanio>0){
		logger_funesMemory9(escribir_loguear,l_trace,"El cpu me mando una solicitud, pasara a ser atendida\n");
		return false;
	}
	logger_funesMemory9(escribir_loguear,l_error,"No se le entendio nada al cpu, se cerrara la conexion\n");
	return true;
}

void interpretar_mensaje_cpu(enum MENSAJES mensaje, int cpu_socket){
	switch(mensaje){
		case PedirLineaParaEjecutar:
			logger_funesMemory9(escribir_loguear, l_info, "El cpu me pidio una linea para ejecutar\n");
			(darle_una_linea_al_cpu[MODO_EJECUCION])(cpu_socket);
			break;
		case AsignarDatosALinea:
			logger_funesMemory9(escribir_loguear, l_info, "El cpu solicito asignar datos a una linea\n");
			(asignar_datos_a_linea[MODO_EJECUCION])(cpu_socket);
			break;
		case LiberarArchivoAbierto:
			logger_funesMemory9(escribir_loguear, l_info, "El cpu solicito liberar un archivo abierto\n");
			(liberar_archivo_abierto[MODO_EJECUCION])(cpu_socket);
			break;
		default:
			logger_funesMemory9(escribir_loguear, l_warning, "No se le entendio nada al cpu\n");
			break;
	}
}

void interpretar_mensaje_del_diego(enum MENSAJES mensaje, int DMA_socket){
	switch(mensaje){
		case CargarParteEnMemoria:
			logger_funesMemory9(escribir_loguear, l_info, "El Diego quiere cargar una parte de un archivo\n");
			(cargar_parte_archivo[MODO_EJECUCION])(DMA_socket);
			break;
		case ObtenerDatos:
			logger_funesMemory9(escribir_loguear, l_info, "El Diego quiere hacerle flush a un archivo\n");
			(obtener_parte_archivo[MODO_EJECUCION])(DMA_socket);
			break;
		default:
			logger_funesMemory9(escribir_loguear, l_warning, "El Diego esta re duro y no se le entiende nada\n");
			break;
	}
}

t_cabecera recibir_mensaje_dam(int socket_DMA){

	logger_funesMemory9(escribir_loguear,l_info, "Esperando que el Diego termine una frase... \n");
	t_cabecera cabecera=recibirCabecera(socket_DMA);
	interpretar_mensaje_del_diego(cabecera.tipoDeMensaje, socket_DMA);

	return cabecera;
}

int comunicarse_con_dam(int socket_escucha){
	logger_funesMemory9(escribir_loguear, l_info,"Voy a esperar al Diego\n");

	int socket_dam=aceptarConexion(socket_escucha);

	logger_funesMemory9(escribir_loguear, l_trace,"Voy a realizar un handshake con el Diego\n");

	if(socket_dam > 0 && (recibirHandshake(MEMORIA, DMA, socket_dam) != 0)){
		logger_funesMemory9(escribir_loguear, l_trace,"Handshake con el Diego hecho!\n");
		return socket_dam;
	}

	logger_funesMemory9(escribir_loguear, l_error," No se pudo realizar handshake con el Diego\n");

	close(socket_escucha);

	finalizar_funesMemory9();

	return -1;
}

char *reservar_total_memoria(){
	return malloc(TAMANIO_MEMORIA);
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

	buscar_informacion_administrativa[SEGMENTACION_PURA]=&buscar_informacion_administrativa_esquema_segmentacion_y_mem_real;
	buscar_informacion_administrativa[SEGMENTACION_PAGINADA]=&buscar_informacion_administrativa_esquema_segmentacion_paginada;
	buscar_informacion_administrativa[TABLA_PAGINAS_INVERTIDA]=&buscar_informacion_administrativa_esquema_paginacion_invertida;

	obtener_parte_archivo[SEGMENTACION_PURA]=&obtener_parte_archivo_con_segmentacion;
	obtener_parte_archivo[SEGMENTACION_PAGINADA]=&obtener_parte_archivo_con_segmentacion_paginada;
	obtener_parte_archivo[TABLA_PAGINAS_INVERTIDA]=&obtener_parte_archivo_con_paginacion_invertida;

	darle_una_linea_al_cpu[SEGMENTACION_PURA]=&darle_una_linea_al_cpu_segmentacion_pura;
	darle_una_linea_al_cpu[SEGMENTACION_PAGINADA]=&darle_una_linea_al_cpu_segmentacion_paginada;
	darle_una_linea_al_cpu[TABLA_PAGINAS_INVERTIDA]=&darle_una_linea_al_cpu_paginacion_invertida;

	asignar_datos_a_linea[SEGMENTACION_PURA]=&asignar_datos_a_linea_segmentacion_pura;
	asignar_datos_a_linea[SEGMENTACION_PAGINADA]=&asignar_datos_a_linea_segmentacion_paginada;
	asignar_datos_a_linea[TABLA_PAGINAS_INVERTIDA]=&asignar_datos_a_linea_paginacion_invertida;

	liberar_archivo_abierto[SEGMENTACION_PURA]=&liberar_archivo_abierto_segmentacion_pura;
	liberar_archivo_abierto[SEGMENTACION_PAGINADA]=&liberar_archivo_abierto_segmentacion_paginada;
	liberar_archivo_abierto[TABLA_PAGINAS_INVERTIDA]=&liberar_archivo_abierto_paginacion_invertida;
}

int consola_obtener_key_comando(char* comando)
{
	int key = -1;

	if(comando == NULL)
		return key;

	if(!strcmp(comando, "dump")){
		key = dump;
	}else{
		logger_funesMemory9(escribir_loguear, l_error,"No conozco ese comando, proba de nuevo\n");
	}

	return key;
}

void consola_obtener_parametros(char* buffer, char** comando, char** parametro1){
	char** comandos;
	int i,j=0;

	comandos = string_n_split(buffer,3," ");

	while(comandos[j])
	{
		switch(j)
		{
			case 0:
				*comando = comandos[j];
				break;
			case 1:
				*parametro1 = comandos[j];
				break;
		}

		j++;
	}

	for(i=0;i>j;i++)
	{
		//log_info(logger,"parte %d: %s\n", j,comandos[j]);
		free(comandos[j]);
	}

	free(comandos);
}

void realizar_dump(int id){
	(buscar_informacion_administrativa[MODO_EJECUCION])(id);
}

void validar_parametro_consola(char ** parametro){
	if(*parametro!=NULL){
		return;
	}
	*parametro="-1";
}

void limpiar_token_consola(char* token) {
	if (token != NULL) {
		free(token);
		token = NULL;
	}
}

int consola_derivar_comando(char * buffer){

	int comando_key;
	char *comando = NULL;
	char *parametro1 = NULL;
	int res = 0;

	// Separa la linea de consola en comando y sus parametros
	consola_obtener_parametros(buffer, &comando, &parametro1);

	// Obtiene la clave del comando a ejecutar para el switch
	comando_key = consola_obtener_key_comando(comando);

	validar_parametro_consola(&parametro1);

	switch(comando_key){
		case dump:
			realizar_dump((int)atoi(parametro1));
			break;
	}

	//Limpio el parametro 1
	limpiar_token_consola(parametro1);
	limpiar_token_consola(comando);

	return res;
}

void inicializar_conexiones_cpu(void){
	for (int i = 0; i < MAX_CLIENTES; ++i){
		conexiones_cpu[i].socket = NO_SOCKET;
	}
}

void *consola() {

	int res = 0;
	char *buffer = NULL;

	logger_funesMemory9(escribir_loguear,l_info,"\nAbriendo consola...\n");

	while(TRUE){

		//Trae la linea de consola
		buffer = readline(">");
		//consola_leer_stdin(buffer, MAX_LINEA);

		res = consola_derivar_comando(buffer);

		free(buffer);

		//Sale de la consola con exit
		if(res)
			break;
	}

	pthread_exit(0);
	return 0;
}

int consola_leer_stdin(char *read_buffer, size_t max_len)
{
	char c = '\0';
	int i = 0;

	memset(read_buffer, 0, max_len);

	ssize_t read_count = 0;
	ssize_t total_read = 0;

	do{
		read_count = read(STDIN_FILENO, &c, 1);

		if (read_count < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			logger_funesMemory9(escribir_loguear,l_error,"Error en read() desde STDIN");
		  return -1;
		}
		else if (read_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		  break;
		}
		else if (read_count > 0) {
		  total_read += read_count;

		  read_buffer[i] = c;
		  i++;

		  if (total_read > max_len) {
			//log_info(logger,"Message too large and will be chopped. Please try to be shorter next time.\n");
			fflush(STDIN_FILENO);
			break;
		  }
		}

	}while ((read_count > 0) && (total_read < max_len) && (c != '\n'));

	size_t len = strlen(read_buffer);
	if (len > 0 && read_buffer[len - 1] == '\n'){
		read_buffer[len - 1] = '\0';
	}

	//log_info(logger,"Read from stdin %zu bytes. Let's prepare message to send.\n", strlen(read_buffer));

	return 0;
}

int main(int argc, char **argv) {

	fd_set readset, writeset, exepset;
	int max_fd;
	char read_buffer[MAX_LINEA];
	struct timeval tv = {0, 500};

	iniciar_funes_memory_9(argv[1]);
	inicializar_conexiones_cpu();
	stdin_no_bloqueante();

	int server_FM9 = iniciar_servidor(PUERTO_ESCUCHA);
	int cliente_DAM = comunicarse_con_dam(server_FM9);

	MEMORIA_FISICA = reservar_total_memoria();
	inicializar_funciones_variables_por_segmento();
	(crear_estructuras_esquema[MODO_EJECUCION])();

	while(TRUE){
		//Inicializa los file descriptor
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&exepset);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		//Agrega el fd del socket servidor al set de lectura y excepciones
		FD_SET(server_FM9, &readset);
		FD_SET(server_FM9, &exepset);

		//Agrega el fd del socket DAM al set de lectura y excepciones
		FD_SET(cliente_DAM, &readset);
		FD_SET(cliente_DAM, &exepset);

		//Agrega el stdin para leer la consola
		FD_SET(STDIN_FILENO, &readset);

		//Seteo el maximo file descriptor necesario para el select
		max_fd = server_FM9;

		//Agrega los conexiones existentes
		if (cliente_DAM != NO_SOCKET){
			FD_SET(cliente_DAM, &readset);
			FD_SET(cliente_DAM, &exepset);
		}

		if (cliente_DAM > max_fd){
			max_fd = cliente_DAM;
		}

		for (int i = 0; i < MAX_CLIENTES; i++){
			if (conexiones_cpu[i].socket != NO_SOCKET){
				FD_SET(conexiones_cpu[i].socket, &readset);
				FD_SET(conexiones_cpu[i].socket, &exepset);
			}

			if (conexiones_cpu[i].socket > max_fd){
				max_fd = conexiones_cpu[i].socket;
			}
		}

		int result = select(max_fd+1, &readset, &writeset, &exepset, &tv);
		logger_funesMemory9(loguear, l_debug,"Resultado del select: %d\n",result); //Revisar rendimiento del CPU cuando select da > 1

		if(result < 0 ){
			logger_funesMemory9(escribir_loguear, l_error,"Error en select\n");
			break;
		}
		else if(errno == EINTR){
			logger_funesMemory9(escribir_loguear, l_error,"Me mataron! salgo del select\n");
			break;
		}
		else if(result > 0) //Hubo un cambio en algun fd
		{
			//Aceptar nuevas conexiones de CPU
			if (FD_ISSET(server_FM9, &readset)){
				atender_nuevo_cpu(server_FM9);
			}

			//Se ingresó algo a la consola
			if(FD_ISSET(STDIN_FILENO, &readset)){
				consola_leer_stdin(read_buffer, MAX_LINEA);

				int res = consola_derivar_comando(read_buffer);
				if(res)	{
					finalizar_funesMemory9();
					break;
				}
			}

			//Manejo de conexiones ya existentes
			if(cliente_DAM != NO_SOCKET ){
				//Mensajes nuevo de dam
				if (FD_ISSET(cliente_DAM, &readset)) {
					if(cabecera_es_invalida_dam(recibir_mensaje_dam(cliente_DAM)))
					{
						logger_funesMemory9(escribir_loguear,l_trace,"Conexión con DAM cerrada\n");
						close(cliente_DAM);
						finalizar_funesMemory9();
					}
				}

				//Excepciones del dam, para la desconexion
				if (FD_ISSET(cliente_DAM, &exepset)) {
					if(cabecera_es_invalida_dam(recibir_mensaje_dam(cliente_DAM)))
					{
						logger_funesMemory9(escribir_loguear,l_trace,"Conexión con DAM cerrada\n");
						close(cliente_DAM);
						finalizar_funesMemory9();
					}
				}//if isset
			} // if NO_SOCKET

			for(int i = 0; i < MAX_CLIENTES; ++i){
				if(conexiones_cpu[i].socket != NO_SOCKET ){
					//Mensajes nuevos de algun cpu
					if (FD_ISSET(conexiones_cpu[i].socket, &readset)) {
						if(cabecera_es_invalida_cpu(recibir_mensaje_cpu(conexiones_cpu[i])))
						{
							logger_funesMemory9(escribir_loguear,l_trace,"Conexión con CPU %d cerrada\n",conexiones_cpu[i].pid);
							close(conexiones_cpu[i].socket);
							conexiones_cpu[i].socket = NO_SOCKET;
							continue;
						}
					}

					//Excepciones del cpu, para la desconexion
					if (FD_ISSET(conexiones_cpu[i].socket, &exepset)) {
						if(cabecera_es_invalida_cpu(recibir_mensaje_cpu(conexiones_cpu[i])))
						{
							logger_funesMemory9(escribir_loguear,l_trace,"Conexión con CPU %d cerrada\n",conexiones_cpu[i].pid);
							close(conexiones_cpu[i].socket);
							conexiones_cpu[i].socket = NO_SOCKET;
							continue;
						}
					}//if isset
				} // if NO_SOCKET
			} //for conexiones_cpu
		} //if result select
	} //while

	//pthread_exit(0);
	return EXIT_SUCCESS;
}
