/*
 * funcionesCPU.c
 *
 *  Created on: Sep 14, 2018
 *      Author: utnso
 */

#include "funcionesCPU.h"

void cargar_archivo_de_config(char *path){
	if (path != NULL){

		t_config * config_file = config_create(path);

		if (config_has_property(config_file,ARCH_CONFIG_PUERTO_SAFA)){
			PUERTO_SAFA = strdup(config_get_string_value(config_file, ARCH_CONFIG_PUERTO_SAFA));
		}

		if (config_has_property(config_file,ARCH_CONFIG_PUERTO_DIEGO)){
			PUERTO_DIEGO = strdup(config_get_string_value(config_file, ARCH_CONFIG_PUERTO_DIEGO));
		}

		if (config_has_property(config_file,ARCH_CONFIG_PUERTO_MEM )){
			PUERTO_MEM = strdup(config_get_string_value(config_file, ARCH_CONFIG_PUERTO_MEM));
		}

		if (config_has_property(config_file,ARCH_CONFIG_IP_SAFA)){
			IP_SAFA = strdup(config_get_string_value(config_file, ARCH_CONFIG_IP_SAFA));
		}

		if (config_has_property(config_file,ARCH_CONFIG_IP_DIEGO)){
			IP_DIEGO = strdup(config_get_string_value(config_file, ARCH_CONFIG_IP_DIEGO));
		}

		if (config_has_property(config_file,ARCH_CONFIG_IP_MEM)){
			IP_MEM = strdup(config_get_string_value(config_file, ARCH_CONFIG_IP_MEM));
		}

		if (config_has_property(config_file,ARCH_CONFIG_RETARDO)){
			RETARDO = strdup(config_get_string_value(config_file, ARCH_CONFIG_RETARDO));
		}

		config_destroy(config_file);
	}
	else {
		logger_CPU(escribir_loguear, l_error,"Error al intertar abrir el archivo de configuracion \n");
		exit(1);
	}
}

void logger_CPU(int tipo_esc, int tipo_log, const char* mensaje, ...){

	//Colores (reset,vacio,vacio,cian,verde,vacio,amarillo,rojo)
	static char *log_colors[8] = {"\x1b[0m","","","\x1b[36m", "\x1b[32m", "", "\x1b[33m", "\x1b[31m" };
	char *console_buffer=NULL;

	char *msj_salida = malloc(sizeof(char) * 256);

	//Captura los argumentos en una lista
	va_list args;
	va_start(args, mensaje);

	//Arma el mensaje formateado con sus argumentos en msj_salida.
	vsprintf(msj_salida, mensaje, args);

	//ESCRIBE POR PANTALLA
	if((tipo_esc == escribir) || (tipo_esc == escribir_loguear)){
		//printf("%s",msj_salida);
		//printf("\n");

		console_buffer = string_from_format("%s%s%s",log_colors[tipo_log],msj_salida, log_colors[0]);
		printf("%s",console_buffer);
		printf("\n");
		fflush(stdout);
		free(console_buffer);
	}

	//LOGUEA
	if((tipo_esc == loguear) || (tipo_esc == escribir_loguear)){

		if(tipo_log == l_info){
			log_info(logger, msj_salida);
		}
		else if(tipo_log == l_warning){
			log_warning(logger, msj_salida);
		}
		else if(tipo_log == l_error){
			log_error(logger, msj_salida);
		}
		else if(tipo_log == l_debug){
			log_debug(logger, msj_salida);
		}
		else if(tipo_log == l_trace){
			log_trace(logger, msj_salida);
		}
	}

	va_end(args);
	free(msj_salida);

	return;
}

void captura_sigpipe(int signo){

    if(signo == SIGINT)
    {
    	logger_CPU(escribir_loguear, l_warning,"\nApretaste Ctrl+C, por que? No hay polque.\n");
    	//finalizar_funesMemory9();
    	exit(EXIT_FAILURE);
    }
    else if(signo == SIGPIPE)
    {
    	logger_CPU(escribir_loguear, l_error,"\n Se desconectó un proceso al que se quizo enviar.\n");
    }

}

void configurar_signals(void){
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
	signal_struct.sa_flags   = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
    if (sigaction(SIGPIPE, &signal_struct, NULL) < 0)
    {
    	logger_CPU(escribir_loguear, l_error,"\n SIGACTION error \n");
    }

    sigaddset(&signal_struct.sa_mask, SIGINT);
    if (sigaction(SIGINT, &signal_struct, NULL) < 0)
    {
    	logger_CPU(escribir_loguear, l_error,"\n SIGACTION error \n");
    }

}

void inicializar_logger(){
	logger = log_create("Log_CPU.txt", "CPU", false, LOG_LEVEL_TRACE);
}
