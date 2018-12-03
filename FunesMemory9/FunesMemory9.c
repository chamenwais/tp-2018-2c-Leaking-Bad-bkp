/*
 * FunesMemory9.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "FunesMemory9.h"

void iniciar_funes_memory_9(char * path){
	inicializar_logger();

	logger_funesMemory9(escribir_loguear, l_warning,"\nHola! Soy Funes Memory 9, a sus ordenes... :) \n");

	cargar_archivo_de_configuracion(path);

	configurar_signals();
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

    //Si no recibo nada...
    logger_funesMemory9(escribir_loguear, l_info, "\nCerrada la conexión con socket de cpu\n");
    return EXIT_SUCCESS;
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

void cargar_parte_archivo_en_segmento_paginado(int DAM_fd){

}

void cargar_parte_archivo_en_pagina_invertida(int DAM_fd){

}

void crear_estructuras_esquema_segmentacion_paginada(){

}

void crear_estructuras_esquema_paginacion_invertida(){

}

void destruir_estructuras_esquema_segmentacion_paginada(){

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

void buscar_informacion_administrativa_esquema_segmentacion_paginada(int pid){

}

void buscar_informacion_administrativa_esquema_paginacion_invertida(int pid){

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
}

char** parser_instruccion(char* linea){
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

void *funcionHiloConsola(void *arg){
	char * linea;
	char *ret="Cerrando hilo";
	char** instruccion;

	logger_funesMemory9(escribir_loguear, l_info, "Consola lista\n");

	while(1){
		linea = readline(">");

		if(strlen(linea)>0){
			add_history(linea);
			instruccion = parser_instruccion(linea);

			if(instruccion[0] == NULL) continue;

			if(strcmp(instruccion[0],"exit")==0){//Mata al hilo de la consola

				//salir_true();
				for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
				free(instruccion);
				free(linea);

				logger_funesMemory9(escribir_loguear, l_info, "Cerrando consola\n");
				pthread_exit(ret);
				}else{
					if(strcmp(instruccion[0],"dump")==0){
						if(instruccion[1]!=NULL){
							logger_funesMemory9(escribir_loguear, l_info, "Se procederá a imprimir por pantalla y en log la totalidad"
									"de los datos administrativos guardados y los datos en memoria real del ID: %d \n", instruccion[1]);

							realizar_dump((int)instruccion[1]);

							}else{
								logger_funesMemory9(escribir_loguear, l_error, "Falta el ID del DTB, reintentar\n");
							}
						}
					}
				}
		}
}


void realizar_dump(int id){

	(buscar_informacion_administrativa[MODO_EJECUCION])(id);
}

void iniciar_consola(){
	logger_funesMemory9(escribir_loguear, l_info, "\nIniciando hilo de consola\n");

	int resultadoDeCrearHilo = pthread_create(&threadConsola, NULL, funcionHiloConsola, "Hilo consola");

	if(resultadoDeCrearHilo){
		logger_funesMemory9(escribir_loguear, l_error, "No se pudo crear el hilo de la consola correctamente\n");
		finalizar_funesMemory9();
		exit(EXIT_FAILURE);
			}
		else{
			logger_funesMemory9(escribir_loguear, l_info, "La consola fue creada correctamente\n");
			}
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

	iniciar_consola();

	//Entra en un loop consecutivo hasta un ctrl+c...
	while (GLOBAL_SEGUIR && cliente_DAM>0){

		socket_cpu = atender_nuevo_cpu(server_FM9);

		crear_hilo_conexion(socket_cpu, escuchar_mensajes_de_cpus);
	}

	cerrar_sockets(server_FM9, socket_cpu, cliente_DAM);

	(destruir_estructuras_esquema[MODO_EJECUCION])();

	finalizar_funesMemory9();
}
