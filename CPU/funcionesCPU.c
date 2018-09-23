/*
 * funcionesCPU.c
 *
 *  Created on: Sep 14, 2018
 *      Author: utnso
 */

#include "funcionesCPU.h"







void funciona()
{
	printf("Anda hasta aca\n");
}


int levantarArchConfig()
{
	funciona();
	config = config_create("/CPUConfig.cfg");
	if (!config_has_property(config, "IP_SAFA")) {
		printf("No se encuentra el parametro IP_SAFA en el archivo de configuracion\n");
		log_error(LOG_CPU,"No se encuentra el parametro IP_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.IPSAFA = config_get_string_value(config, "IP_SAFA");

	if(!config_has_property(config, "PUERTO_SAFA")) {
		printf("No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion\n");
		log_error(LOG_CPU,"No se encuentra el parametro PUERTO_SAFA en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.PUERTOSAFA = config_get_int_value(config, "PUERTO_SAFA");

	if (!config_has_property(config, "IP_DIEGO")) {
		printf("No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion\n");
		log_error(LOG_CPU,"No se encuentra el parametro IP_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.IPELDIEGO = config_get_string_value(config, "IP_DIEGO");

	if(!config_has_property(config, "PUERTO_DIEGO")) {
		printf("No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion\n");
		log_error(LOG_CPU,"No se encuentra el parametro PUERTO_ELDIEGO en el archivo de configuracion");
		config_destroy(config);
		//liberarMemoria();
		exit(-1);
	}
	else
		configuracion.PUERTOELDIEGO = config_get_int_value(config, "PUERTO_DIEGO");


return 0;
}


int configurar_LOG_CPU()
{
	LOG_CPU = log_create("CPU.log", "CPU", 1, LOG_LEVEL_INFO);
	log_info(LOG_CPU, "Creando Archivo de LOG CPU.log");
	return EXIT_SUCCESS;
}

int finalizarTodo()
{
	log_info(LOG_CPU, "Finalizando CPU");
	log_info(LOG_CPU, "Liberando estructuras");
	log_info(LOG_CPU, "Chau CPULog");
	log_destroy(LOG_CPU);

	exit(1);
	return EXIT_SUCCESS;
}

void captura_sigpipe(int signo)
{
    if(signo == SIGINT)
    {
    	log_trace(LOG_CPU,"\nFinalizando proceso... Gracias vuelva prontos.\n");
    	terminar_controladamente(EXIT_FAILURE);
    }
    else if(signo == SIGPIPE)
    {
    	log_error(LOG_CPU,"\n Se desconectó un proceso al que se quizo enviar.\n");
    }

}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
	signal_struct.sa_flags   = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
    if (sigaction(SIGPIPE, &signal_struct, NULL) < 0)
    	log_error(LOG_CPU,"\n SIGACTION error \n");

    sigaddset(&signal_struct.sa_mask, SIGINT);
    if (sigaction(SIGINT, &signal_struct, NULL) < 0)
    	log_error(LOG_CPU,"\n SIGACTION error \n");

}
