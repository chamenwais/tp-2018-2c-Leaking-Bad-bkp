/*
 * funcionesSafa.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "funcionesSafa.h"

int inicializarLog(){
	LOG_SAFA = log_create("SAFA.log","SAFA",true,LOG_LEVEL_INFO);
	log_info(LOG_SAFA,"Se inicializo el log de SAFA");
	return EXIT_SUCCESS;
}

int levantarConfiguracionSAFA(char* ubicacionDelArchivoConfiguracion) {

	t_configuracionSAFA* configuracion = config_create(ubicacionDelArchivoConfiguracion);

	log_info(LOG_SAFA, "Abriendo la configuracion de SAFA");

	if (!config_has_property(configuracion, "PUERTO")) {
		log_error(LOG_SAFA,
				"No esta el puerto de escucha para las conexiones");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	if (!config_has_property(configuracion, "ALGORITMO_PLANIF")) {
		log_error(LOG_SAFA, "No esta el algoritmo de planificacion");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	if (!config_has_property(configuracion, "QUANTUM")) {
		log_error(LOG_SAFA, "No se especifica el quantum");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	if (!config_has_property(configuracion, "GRADO_MULTIPROGRAMACION")) {
		log_error(LOG_SAFA, "No se especifica el grado de multiprogramacion");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	if (!config_has_property(configuracion, "RETARDO")) {
		log_error(LOG_SAFA, "No se especifica el retardo");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	configSAFA.puerto =
			config_get_int_value(configuracion, "PUERTO");

	configSAFA.algoritmo_planif = config_get_string_value(configuracion, "ALGORITMO_PLANIF");

	configSAFA.quantum = config_get_int_value(
			configuracion, "QUANTUM");

	configSAFA.grado_multiprogramacion = config_get_int_value(
			configuracion, "GRADO_MULTIPROGRAMACION");

	configSAFA.retardo = config_get_int_value(configuracion,
			"RETARDO");

	config_destroy(configuracion);

	return EXIT_SUCCESS;
}

int inicializarVariablesSAFA(){
	estadoSAFA = 'C'; //Se inicializa en estado CORRUPTO
	resultadoComElDiego = EXIT_SUCCESS;

	return EXIT_SUCCESS;
}

int finalizarTodo(){
	log_info(LOG_SAFA, "Finalizando SAFA");
	log_info(LOG_SAFA, "Liberando estructuras");
	log_info(LOG_SAFA, "Chau Log");
	log_destroy(LOG_SAFA);

	exit(1);
	return EXIT_SUCCESS;
}

int iniciarEscuchaConDiego(){
	log_info(LOG_SAFA,"Iniciando hilo para la comunicacion con el Diegote");
	int resultadoDeCrearHilo = pthread_create(
			&hiloComDMA, NULL, funcionHiloComDMA,
			&configSAFA);
	if(resultadoDeCrearHilo){
		log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con el Diegote: %d",resultadoDeCrearHilo);
		return EXIT_FAILURE;
		}
	else{
		log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con el Diegote");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void *funcionHiloComDMA(void *arg){
	log_info(LOG_SAFA,"Esperando conexion entrante del Diego en puerto: %d", configSAFA.puerto);
	int port = configSAFA.puerto;
	int sockServer = escucharEn(port); //creo el servidor
	fd_DMA = aceptarConexion(sockServer);
	if(recibirHandshake(PLANIFICADOR,DMA,fd_DMA) > 0){
		//Trabajar con el Diegote eeeehhhhhh
		log_info(LOG_SAFA,"Handshake exitoso con el Diego");
		t_cabecera cabecera = recibirCabecera(fd_DMA);

	}
	resultadoComElDiego=EXIT_SUCCESS;
	return resultadoComElDiego;
}

int esperarFinEscuchaDMA(){
	pthread_join(hiloComDMA, NULL);
	log_info(LOG_SAFA,"Finalizando el hilo de comunicacion con DMA");
	if(resultadoComElDiego==EXIT_FAILURE){
		log_info(LOG_SAFA,"Hilo de comunicacion con el DMA finalizado exitosamente");
	}else{
		log_error(LOG_SAFA,"Hilo de comunicacion con el DMA finalizado por error");
	}
	return resultadoComElDiego;
}
