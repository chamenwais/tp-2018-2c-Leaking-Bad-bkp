/*
 * CPUmain.h
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#ifndef CPUMAIN_H_
#define CPUMAIN_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "funcionesCPU.h"


//Estructuras


typedef struct dConfiguracion{
	char *IPSAFA;
	int PUERTOSAFA;
	char * IPELDIEGO;
	int PUERTOELDIEGO;
}tConfiguracion;

 t_log * logger;
 t_config * config;
 tConfiguracion configuracion;


//Funciones
void configurar_logger();
int levantarArchConfig();


#endif /* CPUmain_H_ */

