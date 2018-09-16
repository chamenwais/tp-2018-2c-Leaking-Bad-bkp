/*
 * ConexionesEntrantes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "ConexionesEntrantes.h"

pthread_t crear_hilo_conexiones_entrantes(int socket_fm9, int socket_mdj){
	pthread_t conexiones_entrantes;
	pthread_create(&conexiones_entrantes, NULL, comunicarse_con_cpu, NULL);
	return conexiones_entrantes;
}

void * comunicarse_con_cpu(){
	log_info(logger,"Se va a escuchar al CPU en el puerto %d", puerto_de_escucha);
	int socket_escucha=escucharEn(puerto_de_escucha);
	validar_comunicacion(socket_escucha, const_name_cpu);
	log_info(logger, "Se va a esperar al CPU");
	int socket_cpu=aceptarConexion(socket_escucha);
	validar_comunicacion(socket_cpu, const_name_cpu);
	realizar_handshake_con_cpu(socket_cpu);
	cerrar_socket_y_terminar(socket_cpu);
	return EXIT_SUCCESS;
}

void realizar_handshake_con_cpu(int socket_id){
	recibir_handshake_de(socket_id, CPU, const_name_cpu);
}
