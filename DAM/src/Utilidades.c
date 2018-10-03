/*
 * Utilidades.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "Utilidades.h"

void configurar_logger() {
	logger = log_create(path_archivo_log, "DAM", false, LOG_LEVEL_TRACE);
}

void validar_apertura_archivo_configuracion() {
	if (configuracion == NULL) {
		logger_DAM(escribir_loguear,l_error, "No encontre el arch de config");
		destruir_logger();
		exit(EXIT_FAILURE);
	}
}

void levantar_configuracion(){
	configuracion = config_create(path_archivo_configuracion);
	validar_apertura_archivo_configuracion();
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

void destruir_logger() {
	logger_DAM(escribir_loguear,l_info, "Se destruira el logger");
	log_destroy(logger);
}

void terminar_controladamente(int return_nr){
	logger_DAM(escribir_loguear,l_info, "Se destruira estructura de archivo de configuracion");
	config_destroy(configuracion);
	destruir_logger();
	exit(return_nr);
}

void enlazar_hilos(pthread_t hilo1, pthread_t hilo2){
	pthread_join(hilo1, NULL);
	pthread_join(hilo2, NULL);
	return;
}

void validar_comunicacion(int socket_id, char * proceso){
	if(socket_id<0){
		logger_DAM(escribir_loguear,l_error, "No se pudo conectar con el proceso %s ", proceso);
		cerrar_socket_y_terminar(socket_id);
	}
}

void informar_handshake_erroneo_y_cerrar(int socket_id, char * proceso){
	logger_DAM(escribir_loguear,l_error, "No se pudo concluir handshake con el proceso %s ",proceso);
	cerrar_socket_y_terminar(socket_id);
}

char* mensaje_informativo_envio_handshake(char* proceso) {
	char* seIntentaraMandarHandshakeA = string_new();
	string_append(&seIntentaraMandarHandshakeA,
			"Se intentara mandar handshake a ");
	string_append(&seIntentaraMandarHandshakeA, proceso);
	return seIntentaraMandarHandshakeA;
}

char* mensaje_handshake_exitoso(char* proceso, int socket_id) {
	char* seRealizoHandshakeCon = string_new();
	string_append(&seRealizoHandshakeCon,
			"Se realizo el handshake con ");
	string_append(&seRealizoHandshakeCon, proceso);
	string_append(&seRealizoHandshakeCon, " en el sockfd: ");
	string_append(&seRealizoHandshakeCon, string_itoa(socket_id));
	return seRealizoHandshakeCon;
}

void informar_envio_handshake(char* proceso) {
	char* mensajeInformativoEnvioHandshake =
			mensaje_informativo_envio_handshake(proceso);
	logger_DAM(escribir_loguear, l_info, mensajeInformativoEnvioHandshake);
	free(mensajeInformativoEnvioHandshake);
}

void informar_handksake_exitoso(int socket_id, char* proceso) {
	char* mensajeHandshakeExitoso = mensaje_handshake_exitoso(proceso,
			socket_id);
	logger_DAM(escribir_loguear, l_trace, mensajeHandshakeExitoso);
	free(mensajeHandshakeExitoso);
}

void mandar_handshake_a(int socket_id, enum PROCESO enumProceso, char * proceso){
	informar_envio_handshake(proceso);
	if (enviarHandshake(DMA, enumProceso, socket_id) == 0) {
		informar_handshake_erroneo_y_cerrar(socket_id, proceso);
	}
	informar_handksake_exitoso(socket_id, proceso);
}

char* mensaje_informativo_recibir_handshake(char* proceso) {
	char* seIntentaraRecibirHandshakeDe = string_new();
	string_append(&seIntentaraRecibirHandshakeDe,
			"Se intentara recibir handshake de ");
	string_append(&seIntentaraRecibirHandshakeDe, proceso);
	return seIntentaraRecibirHandshakeDe;
}

void informar_recibimiento_handshake(char* proceso) {
	char* mensajeInformativoRecibirHandshake =
			mensaje_informativo_recibir_handshake(proceso);
	logger_DAM(escribir_loguear, l_info, mensajeInformativoRecibirHandshake);
	free(mensajeInformativoRecibirHandshake);
}

void recibir_handshake_de(int socket_id, enum PROCESO enumProceso, char * proceso){
	informar_recibimiento_handshake(proceso);
	if (recibirHandshake(enumProceso, DMA, socket_id) == 0) {
		informar_handshake_erroneo_y_cerrar(socket_id, proceso);
	}
	informar_handksake_exitoso(socket_id, proceso);
}

void cerrar_socket_y_terminar(int socket_id){
	close(socket_id);
	terminar_controladamente(EXIT_FAILURE);
}

void captura_sigpipe(int signo){
	if (signo == SIGINT) {
		terminar_controladamente(EXIT_FAILURE);
	} else if (signo == SIGPIPE) {
		logger_DAM(escribir_loguear,l_error,"Hubo otro problema");
	}
}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		logger_DAM(escribir_loguear,l_error,"\n SIGACTION error \n");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		logger_DAM(escribir_loguear,l_error,"\n SIGACTION error \n");
	}
}

char* mensaje_informativo_previa_conexion_con(char* proceso) {
	char* se_intentara_conectar_con_ip_y_puerto = string_new();
	string_append(&se_intentara_conectar_con_ip_y_puerto,
			"Se intentara conectar a la ip %s , puerto %d de ");
	string_append(&se_intentara_conectar_con_ip_y_puerto, proceso);
	return se_intentara_conectar_con_ip_y_puerto;
}

void logger_DAM(int tipo_esc, int tipo_log, const char* mensaje, ...){

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
