/*
 * Contexto.h
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#ifndef CONTEXTO_H_
#define CONTEXTO_H_
#include <commons/log.h>
#include <commons/config.h>

extern t_log * logger;
extern t_config * configuracion;
extern char * path_archivo_configuracion;
extern int puerto_de_escucha;
extern char * clave_puerto_de_escucha;
extern char * path_archivo_log;

#endif /* CONTEXTO_H_ */
