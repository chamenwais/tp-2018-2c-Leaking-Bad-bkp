/*
 * ConexionesSalientes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ConexionesSalientes.h"

pthread_t crear_hilo_conexiones_salientes(int socket_fm9, int socket_mdj){
	pthread_t conexiones_salientes;
	pthread_create(&conexiones_salientes, NULL, comunicarse_con_safa, NULL);
	return conexiones_salientes;
}

void mostrar_mensaje_previa_conexion_con_safa() {
	char* mensaje_informativo_previa_conexion_con_safa =
			mensaje_informativo_previa_conexion_con(const_name_safa);
	log_info(logger, mensaje_informativo_previa_conexion_con_safa, ip_safa,
			puerto_safa);
	free(mensaje_informativo_previa_conexion_con_safa);
}

void * comunicarse_con_safa(){
	mostrar_mensaje_previa_conexion_con_safa();
	int socket_safa=conectarseA(ip_safa, puerto_safa);
	validar_comunicacion(socket_safa, const_name_safa);
	realizar_handshake_con_safa(socket_safa);
	cerrar_socket_y_terminar(socket_safa);
	return EXIT_SUCCESS;
}

void realizar_handshake_con_safa(int socket_id){
	mandar_handshake_a(socket_id, PLANIFICADOR, const_name_safa);
}
