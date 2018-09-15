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
	char* seIntentaraConectarLaIpSPuertoDDe = string_new();
	string_append(&seIntentaraConectarLaIpSPuertoDDe, "Se intentara conectar la ip %s , puerto %d de ");
	string_append(&seIntentaraConectarLaIpSPuertoDDe, "SAFA");
	log_info(logger, seIntentaraConectarLaIpSPuertoDDe, ip_safa, puerto_safa);
	int socket_safa=conectarseA(ip_safa, puerto_safa);
	validar_comunicacion(socket_safa, "SAFA");
	realizar_handshake_con_safa(socket_safa);
	cerrar_socket_y_terminar(socket_safa);
}

void realizar_handshake_con_safa(int socket_id){
	mandar_handshake_a(socket_id, PLANIFICADOR, "SAFA");
	recibir_handshake_de(socket_id, PLANIFICADOR, "SAFA");
}
