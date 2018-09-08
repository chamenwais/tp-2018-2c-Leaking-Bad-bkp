/*
 * Utilidades.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "Contexto.h"

void configurar_logger();
void leer_puerto_de_escucha();
void levantar_configuracion();
void terminar_controladamente(int return_nr);


#endif /* UTILIDADES_H_ */
