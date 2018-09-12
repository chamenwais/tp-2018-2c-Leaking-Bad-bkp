/*
 * Utilidades.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "Utilidades.h"

void configurar_logger() {
	logger = log_create(path_archivo_log, "DAM", 1, LOG_LEVEL_INFO);
}

void levantar_configuracion(){
	configuracion = config_create(path_archivo_configuracion);
	leer_puerto_de_escucha();
	leer_configuracion_safa();
	leer_configuracion_fm9();
	leer_configuracion_mdj();
	leer_transfer_size();
}

void leer_puerto_de_escucha(){
	puerto_de_escucha = config_get_int_value(configuracion, clave_puerto_de_escucha);
}

void leer_configuracion_safa(){
	ip_safa = config_get_string_value(configuracion, clave_ip_safa);
	puerto_safa = config_get_int_value(configuracion, clave_puerto_safa);
}

void leer_configuracion_fm9(){
	ip_fm9 = config_get_string_value(configuracion, clave_ip_fm9);
	puerto_fm9 = config_get_int_value(configuracion, clave_puerto_fm9);
}

void leer_configuracion_mdj(){
	ip_mdj = config_get_string_value(configuracion, clave_ip_mdj);
	puerto_mdj = config_get_int_value(configuracion, clave_puerto_mdj);
}

void leer_transfer_size(){
	transfer_size = config_get_int_value(configuracion, clave_transfer_size);
}

void terminar_controladamente(int return_nr){
	config_destroy(configuracion);
	log_destroy(logger);
	exit(return_nr);
}

void enlazar_hilos(pthread_t hilo1, pthread_t hilo2){
	pthread_join(hilo1, NULL);
	pthread_join(hilo2, NULL);
	return;
}

void validar_comunicacion(int socket_id, const char * proceso){
	if(socket_id<0){
		log_error(logger, "No se pudo conectar con el proceso %s ", proceso);
		cerrar_socket_y_terminar(socket_id);
	}
}

void informar_handshake_erroneo_y_cerrar(int socket_id, const char * proceso){
	log_error(logger, "No se pudo concluir handshake con el proceso %s ",proceso);
	cerrar_socket_y_terminar(socket_id);
}

void mandar_handshake_a(int socket_id, enum PROCESO enumProceso, const char * proceso){
	log_info(logger, strcat("Se intentara mandar handshake a ",proceso));
	if (enviarHandshake(DMA, enumProceso, socket_id) == 0) {
		informar_handshake_erroneo_y_cerrar(socket_id, proceso);
	}
}

void recibir_handshake_de(int socket_id, enum PROCESO enumProceso, const char * proceso){
	log_info(logger, strcat("Se intentara recibir handshake de ", proceso));
	if (recibirHandshake(enumProceso, DMA, socket_id) == 0) {
		informar_handshake_erroneo_y_cerrar(socket_id, proceso);
	}
}

void cerrar_socket_y_terminar(int socket_id){
	close(socket_id);
	terminar_controladamente(EXIT_FAILURE);
}

void captura_sigpipe(int signo){
	int i;
	if (signo == SIGINT) {
		terminar_controladamente(EXIT_FAILURE);
	} else if (signo == SIGPIPE) {
		printf("Hubo otro problema");
	}
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		printf("\n SIGACTION error \n");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		printf("\n SIGACTION error \n");
	}
}