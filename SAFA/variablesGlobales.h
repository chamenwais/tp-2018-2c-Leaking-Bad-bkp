/*
 * variablesGlobales.h
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#ifndef SAFA_VARIABLESGLOBALES_H_
#define SAFA_VARIABLESGLOBALES_H_

#include "SAFA.h"

typedef struct defConfiguracionSAFA {
	int puerto;
	char* algoritmo_planif;
	int quantum;
	int grado_multiprogramacion;
	int retardo;
} t_configuracionSAFA;

extern t_configuracionSAFA configSAFA;
extern char estadoSAFA;


#endif /* SAFA_VARIABLESGLOBALES_H_ */
