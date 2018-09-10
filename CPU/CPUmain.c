/*
 * CPUmain.c
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#include "CPUmain.h"



int main(){
	configurar_logger();
	levantarArchConfig(configuracion);




}




Configuracion levantarArchConfig(struct Configuracion c){

	config = config_create("/CPUConfig.cfg");

	if (!config_has_property(config, "IP_SAFA")) {
		printf("No se encuentra el parametro IP_SAFA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro IP_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		c.IPSAFA = config_get_string_value(config, "IP_SAFA");

	if(!!config_has_property(config, "PUERTOSAFA")) {
		printf("No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		c.PUERTOSAFA = config_get_int_value(config, "PUERTO_SAFA");

	if (!config_has_property(config, "IP_ELDIEGO")) {
		printf("No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		c.IPELDIEGO = config_get_string_value(config, "IP_ELDIEGO");

	if(!!config_has_property(config, "PUERTOELDIEGO")) {
		printf("No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
		c.PUERTOELDIEGO = config_get_int_value(config, "PUERTO_ELDIEGO");

}

void configurar_logger()
{
	logger= log_create("CPU.log", "CPU", true, LOG_LEVEL_INFO);
}

