/*
 * CPUmain.h
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#ifndef CPUMAIN_H_
#define CPUMAIN_H_


#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>


#define IP-SAFA "127.0.0.1"
#define PUERTO-SAFA "8080"
#define IP-ELDIEGO "127.0.0.1"
#define PUERTO-ELDIEGO "8080"



//Estructuras


typedef struct Configuracion{
	char *IPSAFA;
	int PUERTOSAFA;
	char * IPELDIEGO;
	int PUERTOELDIEGO;
}__attribute__((packed)) Configuracion;

t_log * logger;
t_config config;
Configuracion configuracion;

//Funciones
void configurar_logger();
Configuracion levantarArchConfig(struct Configuracion);

























#endif /* CPUMAIN_H_ */
