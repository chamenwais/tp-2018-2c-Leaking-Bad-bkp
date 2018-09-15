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

int ubicarArchivoDeConfiguracion(int argc,char** argv){
	char* ubicacionDelArchivoDeConfiguracion;
	if(argc>1){
		log_info(LOG_SAFA,"Ubicacion del archivo de configuracion recibida por parametro");
		ubicacionDelArchivoDeConfiguracion = argv[1];
	}else{
		log_info(LOG_SAFA,"Ubicacion del archivo de configuracion por default");
		ubicacionDelArchivoDeConfiguracion = "configuracionSAFA.cfg";
		}

	if(levantarConfiguracionSAFA(ubicacionDelArchivoDeConfiguracion)==EXIT_SUCCESS){
		log_info(LOG_SAFA,"Configuracion de SAFA recuperada exitosamente");
		return EXIT_SUCCESS;
		}
	else{
		log_error(LOG_SAFA,"No se pudo levantar la configuracion de SAFA, abortando");
		return EXIT_FAILURE;
		}
}

int levantarConfiguracionSAFA(char* ubicacionDelArchivoConfiguracion) {

	t_config* configuracion = config_create(ubicacionDelArchivoConfiguracion);

	if(configuracion!=NULL){
		log_info(LOG_SAFA,"El archivo de configuracion existe");
	}else{
		log_error(LOG_SAFA,"No existe el archivo de configuracion en: %s",ubicacionDelArchivoConfiguracion);
		return EXIT_FAILURE;
	}

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
	//log_info(LOG_SAFA, "obtengo el puerto");

	configSAFA.algoritmo_planif = string_duplicate(config_get_string_value(configuracion, "ALGORITMO_PLANIF"));

	configSAFA.quantum = config_get_int_value(
			configuracion, "QUANTUM");

	configSAFA.grado_multiprogramacion = config_get_int_value(
			configuracion, "GRADO_MULTIPROGRAMACION");

	configSAFA.retardo = config_get_int_value(configuracion,
			"RETARDO");

	config_destroy(configuracion);

	return EXIT_SUCCESS;
}

int inicializarSemaforosSAFA(){
	log_info(LOG_SAFA,"Inicializando semaforos");
		if (pthread_mutex_init(&mutexSalir, NULL) != 0) {
			log_error(LOG_SAFA,"No se pudo inicializar el semaforo para salir");
			exit(1);
		}
		if (pthread_mutex_init(&mutexSelect, NULL) != 0) {
			/*Para poder meter o sacar FD del select*/
			log_error(LOG_SAFA,"No se pudo inicializar el semaforo del select");
			exit(1);
			}
		return EXIT_SUCCESS;

}

int inicializarVariablesSAFA(){
	estadoSAFA = 'C'; //Se inicializa en estado CORRUPTO
	resultadoComElDiego = EXIT_SUCCESS;
	maxfd = 0;
	salir=false;

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
	int resultadoHiloDMA = pthread_create(
			&hiloComDMA, NULL, funcionHiloComDMA,
			&configSAFA);
	if(resultadoHiloDMA){
		log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con el Diegote: %d",resultadoHiloDMA);
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
		//t_cabecera cabecera = recibirCabecera(fd_DMA);

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

void *funcionHiloComCPU(void *arg){
	char *ret="Cerrando hilo";
		int IDCPU;
		int client_fd;
		struct timeval timeout;
		timeout.tv_sec=3;
		timeout.tv_usec=0;
		int resultadoDelSelect,i;
		t_cabecera header;


		int port = configSAFA.puerto;
		int srvsock = escucharEn(port); //crea servidor
		log_info(LOG_SAFA,"Esperando conexiones en el puerto: %d", port);


		FD_ZERO(&readset);
		FD_SET(srvsock, &readset);
		if (srvsock > maxfd) maxfd = srvsock;

		log_info(LOG_SAFA,"Se añadio el socket servidor al set del select");

		while(!tieneQueSalir()){
			lockearSelect();
			memcpy(&auxReadSet, &readset, sizeof(readset));
			resultadoDelSelect = select(maxfd+1, &auxReadSet, NULL, NULL, &timeout);
			log_info(LOG_SAFA, "Comienza el select");
			if(resultadoDelSelect==0){
				printf("timeout del select\n");
			}else{
				if(resultadoDelSelect<0){
					log_error(LOG_SAFA, "Error en el select");
					return EXIT_FAILURE;
				}else{//si entra por aca es porque encontró algo
					for (i = 0; i <= (obtenerMaxFD()); i++){
						if(FD_ISSET(i, &auxReadSet)){
							log_info(LOG_SAFA,"El FD %i tiene datos", i);
							if(i==srvsock){//se me conecto un nuevo CPU
								log_info(LOG_SAFA,"Recibi una nueva CPU");
								client_fd = aceptarConexion(srvsock);
								if(recibirHandshake(PLANIFICADOR,CPU,client_fd) > 0){
									log_info(LOG_SAFA,"Handshake exitoso con una CPU, FD:%d",client_fd);
									//TODO: ver que tiene que hacer la CPU
									}
								//TODO: crearDTBlock?
								log_info(LOG_SAFA,"Ya termine de crear la nueva conexion");
							}else{
								log_info(LOG_SAFA,"Recibiendo mensaje de CPU existente");
								header = recibirCabecera(i);
								log_info(LOG_SAFA,"El ID del mensaje es:%d",header);
								switch(header.tipoDeMensaje) { // TODO: Ver case dependiendo tipo mensaje
									/*case
										break;
									default:
										//log_error(LOG_SAFA,"Header invalido, tipo: %s", header.message_type);
										break;*/
										}//fin del switch
								}
							}
						}
					}
				}
			deslockearSelect();
			}//fin del while(1)
		pthread_exit(ret);
		return EXIT_SUCCESS;
}

bool tieneQueSalir(){
	bool estadoActual;
	lockearSalir();
	estadoActual=salir;
	deslockearSalir();
	return estadoActual;
}

int deslockearSalir(){
	pthread_mutex_unlock(&mutexSalir);
	return EXIT_SUCCESS;
}

int lockearSalir(){
	pthread_mutex_lock(&mutexSalir);
	return EXIT_SUCCESS;
}

int deslockearSelect(){
	//log_info(LOG_SAFA,"Deslockeando semaforo del select");
	pthread_mutex_unlock(&mutexSelect);
	return EXIT_SUCCESS;
}

int lockearSelect(){
	//log_info(LOG_SAFA,"Lockeando semaforo del select");
	pthread_mutex_lock(&mutexSelect);
	return EXIT_SUCCESS;
}

int obtenerMaxFD() {
    int m;
    m = maxfd;;
    return m;
}
