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

	t_config* configuracion = config_create(ubicacionDelArchivoConfiguracion);

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

	configSAFA.puerto =	config_get_int_value(configuracion, "PUERTO");

	configSAFA.algoritmo_planif = string_duplicate(config_get_string_value(configuracion, "ALGORITMO_PLANIF"));

	configSAFA.quantum = config_get_int_value(configuracion, "QUANTUM");

	configSAFA.grado_multiprogramacion = config_get_int_value(configuracion, "GRADO_MULTIPROGRAMACION");

	configSAFA.retardo = config_get_int_value(configuracion,"RETARDO");

	config_destroy(configuracion);

	return EXIT_SUCCESS;
}

int inicializarVariablesSAFA(){
	estadoSAFA = 'C'; //Se inicializa en estado CORRUPTO
	resultadoComElDiego = EXIT_SUCCESS;
	safa_conectado = true;
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

int inicializarSemaforosSAFA(){
	//log_info(LOG_SAFA,"Inicializando semaforos");
		return EXIT_SUCCESS;
 }

 void *funcionHiloComDMA(void *arg){//TODO: ver que hace
		//Trabajar con el Diegote eeeehhhhhh
		log_info(LOG_SAFA,"Handshake exitoso con el Diego");
		//t_cabecera cabecera = recibirCabecera(fd_DMA);
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
 int iniciarEscuchaCPU(){ //TODO ver si uso hilos o select
	int resultadoHiloCPU = pthread_create(
			&hiloComCPU, NULL, funcionHiloComCPU, &configSAFA);
		if(resultadoHiloCPU){
			log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con la CPUr: %d\n",resultadoHiloCPU);
			exit(EXIT_FAILURE);
			}
		else{
			log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con la CPU");
			return EXIT_SUCCESS;
			}
		return EXIT_SUCCESS;
}
 void *funcionHiloComCPU(void *arg){//TODO: ver que hace
	return EXIT_SUCCESS;
}

int escuchar(){
	log_info(LOG_SAFA,"Esperando conexiones entrantes en puerto: %d", configSAFA.puerto);
	int port = configSAFA.puerto;
	int sockServer = escucharEn(port); //creo el servidor
	while(safa_conectado){
		//Acepto la conexion con el cliente
		int client_fd = aceptarConexion(sockServer);
		log_info(LOG_SAFA, "Conexion recibida");
		if(recibirHandshake(PLANIFICADOR, DMA, client_fd) > 0){
			log_info(LOG_SAFA, "Conexion realizada con El Diego");
			fd_DMA = client_fd;
			log_info(LOG_SAFA,"Iniciando hilo para la comunicacion con el Diegote");
			int resultadoHiloDMA = pthread_create(&hiloComDMA, NULL, funcionHiloComDMA,
					&configSAFA);
			if(resultadoHiloDMA){
				log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con el Diegote: %d",resultadoHiloDMA);
				return EXIT_FAILURE;
				}
			else{
				log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con el Diegote");
				pthread_detach(resultadoHiloDMA);
				return EXIT_SUCCESS;
				}
		}else if(recibirHandshake(PLANIFICADOR, CPU, client_fd) > 0){
				log_info(LOG_SAFA, "Conexion realizada con CPU");
				int fd_CPU = client_fd;
				log_info(LOG_SAFA, "Iniciando hilo para la comuniacion con el CPU");
				int resultadoHiloCPU = pthread_create(
						&hiloComCPU, NULL, funcionHiloComCPU, &configSAFA);
				if(resultadoHiloCPU){
					log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con la CPUr: %d\n",resultadoHiloCPU);
					exit(EXIT_FAILURE);
					}
				else{
					log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con la CPU");
					pthread_detach(resultadoHiloCPU);
					return EXIT_SUCCESS;
					}
				return EXIT_SUCCESS;
		}
	}
	return EXIT_SUCCESS;
}

int iniciarConsola(){

	int resultadoDeCrearHilo = pthread_create( &hiloConsola, NULL, funcionHiloConsola, "Hilo consola");

	if(resultadoDeCrearHilo){
		log_error(LOG_SAFA,"Error al crear el hilo, return code: %d",resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
		}
	else{
		log_info(LOG_SAFA,"La consola se creo exitosamente");
		return EXIT_SUCCESS;
		}

}

void *funcionHiloConsola(void *arg){
	char * linea;
	char *ret="Cerrando hilo";
	char** instruccion;

	printf("> Consola lista\n");

	while(1){
		linea = readline(">");
		if(strlen(linea)>0){
			add_history(linea);

		instruccion = parser_instruccion(linea);
		if(instruccion[0] == NULL) continue;
		if(strcmp(instruccion[0],"exit")==0){//Mata al hilo de la consola
			//salir_true();
			for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
			free(instruccion);
			free(linea);
			log_info(LOG_SAFA,"Cerrando consola");
			//pthread_mutex_unlock(&mutexDePausaDePlanificacion);
			pthread_exit(ret);
			}else{
		if(strcmp(instruccion[0],"ejecutar")==0){//TODO
			/* Permitirá el ingreso de un nuevo programa G.DT al sistema.
			 * Pasada la ruta del script Escriptorio, el planificador creará su DTB asociado
			 * y colocará el proceso en la cola de NEW para que el PLP lo administre cuando lo permita
			 * el grado de multiprogramación. Cuando suceda esto, el PLP se comunicará con este submódulo
			 * para comenzar la ejecución del DTB dummy de Iniciar Escriptorio definida en el Proceso CPU
			 * El DTB dummy a enviar contendrá el archivo Escriptorio que se desea ejecutar y
			 * el flag de inicialización en 0, siendo el único DTB que contenga este flag con dicho valor
			 * (cualquier otro DTB lo tendrá en 1). */
			//ejecutarConsola();
			}else{
		if(strcmp(instruccion[0],"status")==0){
			/*Se deberá mostrar el estado de cada cola, así como la información complementaria a las mismas.
			 *En caso de tener un parámetro, deberá informar todos los datos almacenados en el DT Block
			 *En (tanto mínimos como agregados por el grupo).
			 */
			//statusConsola();
			}else{
		if(strcmp(instruccion[0],"finalizar")==0){
			/* Obligará a un DTB a pasar a la cola de EXIT para poder destrabar la ejecución y
			 * dar lugar a otro G.DT a operar sobre dicho equipo. Si el G.DT se encuentra en la cola
			 * EXEC se deberá esperar a terminar la operación actual, para luego moverlo a la cola EXIT*/

			}else{
		if(strcmp(instruccion[0],"metricas")==0){
			/* Detalla las siguientes métricas:
			 *	Cant. de sentencias ejecutadas que esperó un DTB en la cola NEW
			 * Cant.de sentencias ejecutadas prom. del sistema que usaron a “El Diego”
			 * Cant. de sentencias ejecutadas prom. del sistema para que un DTB termine en la cola EXIT
			 * Porcentaje de las sentencias ejecutadas promedio que fueron a “El Diego”
			 * Tiempo de Respuesta promedio del Sistema
			 */
			}else{
		printf("Comando desconocido\n");
		}}}}}}
		free(linea);
		for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
		free(instruccion);
	}//Cierre del while(1)
	pthread_exit(ret);
}

char** parser_instruccion(char* linea) {
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

