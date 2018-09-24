/*
 * ElDiego.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ElDiego.h"

int main() {
	configurar_logger();
	levantar_configuracion();
	configurar_signals();
	//Va a hacer el handshake con MDJ
	int socket_file_system=comunicarse_con_file_system();
	//Va a hacer el handshake con FM9
	int socket_memoria=comunicarse_con_memoria();
	//Va a comunicarse con S-AFA
	int socket_safa=comunicarse_con_safa();
	//Va a quedarse escuchando al CPU
	pthread_t hilo_conexiones_entrantes=crear_hilo_conexiones_entrantes(socket_memoria, socket_file_system, socket_safa);
	close(socket_file_system);
	close(socket_memoria);
	close(socket_safa);
	terminar_controladamente(EXIT_SUCCESS);
}

void mostrar_mensaje_previa_conexion_con_mdj() {
	char* mensaje_informativo_previa_conexion_con_mdj =
			mensaje_informativo_previa_conexion_con(const_name_mdj);
	log_info(logger, mensaje_informativo_previa_conexion_con_mdj, ip_mdj,
			puerto_mdj);
	free(mensaje_informativo_previa_conexion_con_mdj);
}

int comunicarse_con_file_system(){
	mostrar_mensaje_previa_conexion_con_mdj();
	int socket_mdj=conectarseA(ip_mdj, puerto_mdj);
	validar_comunicacion(socket_mdj, const_name_mdj);
	realizar_handshake_con_mdj(socket_mdj);
	return socket_mdj;
}

void realizar_handshake_con_mdj(int socket_id){
	mandar_handshake_a(socket_id, FS, const_name_mdj);
}

void mostrar_mensaje_previa_conexion_con_fm9() {
	char* mensaje_informativo_previa_conexion_con_fm9 =
			mensaje_informativo_previa_conexion_con(const_name_fm9);
	log_info(logger, mensaje_informativo_previa_conexion_con_fm9, ip_fm9,
			puerto_fm9);
	free(mensaje_informativo_previa_conexion_con_fm9);
}

int comunicarse_con_memoria(){
	mostrar_mensaje_previa_conexion_con_fm9();
	int socket_fm9=conectarseA(ip_fm9, puerto_fm9);
	validar_comunicacion(socket_fm9, const_name_fm9);
	realizar_handshake_con_fm9(socket_fm9);
	return socket_fm9;
}

void realizar_handshake_con_fm9(int socket_id){
	mandar_handshake_a(socket_id, MEMORIA, const_name_fm9);
}

void mostrar_mensaje_previa_conexion_con_safa() {
	char* mensaje_informativo_previa_conexion_con_safa =
			mensaje_informativo_previa_conexion_con(const_name_safa);
	log_info(logger, mensaje_informativo_previa_conexion_con_safa, ip_safa,
			puerto_safa);
	free(mensaje_informativo_previa_conexion_con_safa);
}

int comunicarse_con_safa(){
	mostrar_mensaje_previa_conexion_con_safa();
	int socket_safa=conectarseA(ip_safa, puerto_safa);
	validar_comunicacion(socket_safa, const_name_safa);
	realizar_handshake_con_safa(socket_safa);
	return socket_safa;
}

void realizar_handshake_con_safa(int socket_id){
	mandar_handshake_a(socket_id, PLANIFICADOR, const_name_safa);
}
