/*
 * Contexto.c
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */
#include "Contexto.h"

/**
 * Constantes
 */
char * path_archivo_log="/home/utnso/DAM.log";
char * path_archivo_configuracion="configDAM.properties";
char * clave_puerto_de_escucha="puerto.de.escucha";

t_log * logger;
t_config * configuracion;
int puerto_de_escucha;
