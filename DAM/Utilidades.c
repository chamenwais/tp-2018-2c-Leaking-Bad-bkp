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
}

void leer_puerto_de_escucha(){
	puerto_de_escucha = config_get_int_value(configuracion, clave_puerto_de_escucha);
}

void terminar_controladamente(int return_nr){
	config_destroy(configuracion);
	log_destroy(logger);
	exit(return_nr);
}
