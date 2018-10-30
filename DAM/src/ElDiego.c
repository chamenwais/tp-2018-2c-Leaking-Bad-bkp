/*
 * ElDiego.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ElDiego.h"

int main(int argc, char ** argv) {
	configurar_logger();
	levantar_configuracion(argc,argv);
	configurar_signals();
	//Va a hacer el handshake con MDJ solo para chequear si escucha
	int socket_file_system=comunicarse_con_file_system();
	close(socket_file_system);
	//Va a hacer el handshake con FM9
	int socket_memoria=comunicarse_con_memoria();
	//Va a comunicarse con S-AFA
	int socket_safa=comunicarse_con_safa();
	// Inicializo semaforos
	inicializar_semaforos_de_procesos();
	//Va a quedarse escuchando a los CPU
	crear_hilos_conexiones_entrantes(socket_memoria, socket_safa);
	close(socket_memoria);
	close(socket_safa);
	terminar_controladamente(EXIT_SUCCESS);
}

void mostrar_mensaje_previa_conexion_con_fm9() {
	char* mensaje_informativo_previa_conexion_con_fm9 =
			mensaje_informativo_previa_conexion_con(CONST_NAME_FM9);
	logger_DAM(escribir_loguear,l_info, mensaje_informativo_previa_conexion_con_fm9, ip_fm9,
			puerto_fm9);
	free(mensaje_informativo_previa_conexion_con_fm9);
}

int comunicarse_con_memoria(){
	mostrar_mensaje_previa_conexion_con_fm9();
	int socket_fm9=conectarseA(ip_fm9, puerto_fm9);
	validar_comunicacion(socket_fm9, CONST_NAME_FM9);
	realizar_handshake_con_fm9(socket_fm9);
	return socket_fm9;
}

void realizar_handshake_con_fm9(int socket_id){
	mandar_handshake_a(socket_id, MEMORIA, CONST_NAME_FM9);
}

void mostrar_mensaje_previa_conexion_con_safa() {
	char* mensaje_informativo_previa_conexion_con_safa =
			mensaje_informativo_previa_conexion_con(CONST_NAME_SAFA);
	logger_DAM(escribir_loguear,l_info, mensaje_informativo_previa_conexion_con_safa, ip_safa,
			puerto_safa);
	free(mensaje_informativo_previa_conexion_con_safa);
}

int comunicarse_con_safa(){
	mostrar_mensaje_previa_conexion_con_safa();
	int socket_safa=conectarseA(ip_safa, puerto_safa);
	validar_comunicacion(socket_safa, CONST_NAME_SAFA);
	realizar_handshake_con_safa(socket_safa);
	return socket_safa;
}

void realizar_handshake_con_safa(int socket_id){
	mandar_handshake_a(socket_id, PLANIFICADOR, CONST_NAME_SAFA);
}
