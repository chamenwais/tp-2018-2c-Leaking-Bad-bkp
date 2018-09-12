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

void * comunicarse_con_safa(){
	log_info(logger, strcat(SE_INTENTARA_CONECTAR_LA_IP_Y_PUERTO, SAFA), ip_safa, puerto_safa);
	int socket_safa=conectarseA(ip_safa, puerto_safa);
	validar_comunicacion(socket_safa, SAFA);
	realizar_handshake_con_safa(socket_safa);
	//TODO ver en que condicion se cierra la conexion con SAFA y como comunicarse entre hilos para dar aviso de eventos
	sleep(50);
	cerrar_socket_y_terminar(socket_safa);
}

void realizar_handshake_con_safa(int socket_id){
	mandar_handshake_a(socket_id, PLANIFICADOR, SAFA);
	recibir_handshake_de(socket_id, PLANIFICADOR, SAFA);
}
