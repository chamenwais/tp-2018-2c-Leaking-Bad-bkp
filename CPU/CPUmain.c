/*
 * CPUmain.c
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#include "CPUmain.h"
#include <stdio.h>



int main(){

	configurar_logger();
	levantarArchConfig();
	connect_to_server(configuracion.IPSAFA, configuracion.PUERTOSAFA);
	connect_to_server(configuracion.IPELDIEGO, configuracion.PUERTOELDIEGO);
return 0;
}

















int levantarArchConfig(){


	config = config_create("CPUConfig.cfg");
	if (!config_has_property(config, "IP_SAFA")) {
		printf("No se encuentra el parametro IP_SAFA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro IP_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.IPSAFA = config_get_string_value(config, "IP_SAFA");

	if(!config_has_property(config, "PUERTO_SAFA")) {
		printf("No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.PUERTOSAFA = config_get_int_value(config, "PUERTO_SAFA");

	if (!config_has_property(config, "IP_DIEGO")) {
		printf("No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.IPELDIEGO = config_get_string_value(config, "IP_DIEGO");

	if(!config_has_property(config, "PUERTO_DIEGO")) {
		printf("No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.PUERTOELDIEGO = config_get_int_value(config, "PUERTO_DIEGO");


return 0;
}


void configurar_logger()
{
	logger= log_create("CPU.log", "CPU", 1, LOG_LEVEL_INFO);
	log_info(logger, "Creando Archivo de LOG CPU.log");
}

