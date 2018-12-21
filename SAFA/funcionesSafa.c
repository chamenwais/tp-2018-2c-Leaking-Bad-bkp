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
	char* alg;

	log_info(LOG_SAFA, "Abriendo la configuracion de SAFA");

	if (!config_has_property(configuracion, "PUERTO")) {
		log_error(LOG_SAFA,
				"No esta el puerto de escucha para las conexiones");
		config_destroy(configuracion);
		exit(EXIT_FAILURE);
	}

	if (config_has_property(configuracion, "ALGORITMO_PLANIF")) {
		//log_error(LOG_SAFA, "No esta el algoritmo de planificacion");
		alg = config_get_string_value(configuracion, "ALGORITMO_PLANIF");
		//log_info(LOG_SAFA, "alg es %s", alg);
		if (string_equals_ignore_case(alg,"RR")){
						//Round Robin
						algoritmo_planificacion = ROUND_ROBIN;
						log_info(LOG_SAFA, "Se obtuvo el alg de planificacion Round Robin del archivo de config");
		}else if (string_equals_ignore_case(alg,"VRR")){
						//Virtual Round Robin
						algoritmo_planificacion = VIRTUAL_RR;
						log_info(LOG_SAFA, "Se obtuvo el alg de planificacion Virtual RR del archivo de config");
		}else if (string_equals_ignore_case(alg, "BOAF")){
						//BOAF
						algoritmo_planificacion = BOAF;
						log_info(LOG_SAFA, "Se obtuvo el algo de planificacion BOAF del archivo de config");
		}
		//config_destroy(configuracion);
		//exit(EXIT_FAILURE);
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

	//configSAFA.algoritmo_planif = string_duplicate(config_get_string_value(configuracion, "ALGORITMO_PLANIF"));

	configSAFA.quantum = config_get_int_value(configuracion, "QUANTUM");

	configSAFA.grado_multiprogramacion = config_get_int_value(configuracion, "GRADO_MULTIPROGRAMACION");

	configSAFA.retardo = config_get_int_value(configuracion,"RETARDO");

	config_destroy(configuracion);
	//free(alg);

	return EXIT_SUCCESS;
}


int inicializarVariablesSAFA(){
	estadoSAFA = CORRUPTO; //Se inicializa en estado CORRUPTO
	resultadoComElDiego = EXIT_SUCCESS;
	safa_conectado = true;
	id = 1;
	DAM_conectado = false;
	hayDummy = 0;
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
	if (pthread_mutex_init(&mutexDePausaDePlanificacion, NULL) != 0) {
		/*Para pausar la planificacion*/
		log_error(LOG_SAFA,"No se pudo inicializar el semaforo de planificacion");
		exit(1);
	}
	if (pthread_mutex_init(&mutexDePausaPCP, NULL) != 0) {
			/*Para pausar la planificacion*/
		log_error(LOG_SAFA,"No se pudo inicializar el semaforo de planificacion");
		exit(1);
	}

	pthread_mutex_init(&mutex_NUEVOS, NULL);
	pthread_mutex_init(&mutex_LISTOS, NULL);
	pthread_mutex_init(&mutex_BLOQUEADOS, NULL);
	pthread_mutex_init(&mutex_EJECUTANDO, NULL);
	pthread_mutex_init(&mutex_TERMINADOS, NULL);
	pthread_mutex_init(&mutex_AUXVRR, 0);
	pthread_mutex_init(&mutex_CPULIBRES, 0);
	pthread_mutex_init(&mutex_CPUEJEC, 0);
	pthread_mutex_init(&mutex_EQGRANDE, 0);
	pthread_mutex_init(&mutex_TABLAREC, 0);
	return EXIT_SUCCESS;
 }

int inicializarListas(){
	log_info(LOG_SAFA, "inicializando listas");
	listos = list_create();
	nuevos = list_create();
	ejecutando = list_create();
	bloqueados = list_create();
	terminados = list_create();
	auxVirtualRR = list_create();
	cpu_libres = list_create();
	cpu_ejecutando = list_create();
	dtbConEqGrandeAbierto = list_create();
	tabla_recursos = list_create();
	return EXIT_SUCCESS;
}

int liberarMemoria(){
	log_info(LOG_SAFA, "Liberando memoria");
	list_destroy_and_destroy_elements(listos, free);
	list_destroy_and_destroy_elements(nuevos, free);
	list_destroy_and_destroy_elements(ejecutando, free);
	list_destroy_and_destroy_elements(bloqueados, free);
	list_destroy_and_destroy_elements(terminados, free);
	list_destroy_and_destroy_elements(auxVirtualRR, free);
	list_destroy_and_destroy_elements(cpu_libres, free);
	list_destroy_and_destroy_elements(cpu_ejecutando, free);
	list_destroy_and_destroy_elements(dtbConEqGrandeAbierto, free);
	list_destroy_and_destroy_elements(tabla_recursos, free);

	log_info(LOG_SAFA, "Habemus memoria liberada");
	return EXIT_SUCCESS;
}

 void *funcionHiloComDMA(void *arg){//
		//Trabajar con el Diegote eeeehhhhhh
	DAM_conectado = true;
	while(DAM_conectado){
	t_cabecera cabecera = recibirCabecera(fd_DMA);
	log_info(LOG_SAFA,"Recibi cabecera del DMA");
	tp_datosEnMemoria datos_recibidos;
	tp_DTB DTB_exit;
	tp_pathPid pathPid;
	tp_DTB DTB_Listo;
	switch(cabecera.tipoDeMensaje){
		case AbrirPathNoFinalizado:
			log_info(LOG_SAFA,"Recibi cabecera: AbrirPathNoFinalizado");
			datos_recibidos = prot_recibir_DMA_SAFA_datosEnMemoria(fd_DMA);
				log_info(LOG_SAFA, "El archivo no existe, el DTB %i pasa a EXIT", datos_recibidos->pid);
				bool coincideIDExit(void* node) {
					 return ((((tp_DTB) node)->id_GDT)==datos_recibidos->pid);
					 }
				pthread_mutex_lock(&mutex_BLOQUEADOS);
				DTB_exit = list_remove_by_condition(bloqueados, coincideIDExit);
				pthread_mutex_unlock(&mutex_BLOQUEADOS);
				free(datos_recibidos);
				pthread_mutex_lock(&mutex_TERMINADOS);
				list_add(terminados, DTB_exit);
				pthread_mutex_unlock(&mutex_TERMINADOS);
			break;
		case AbrirPathFinalizadoOk: //TODO ver aca cuando abre arch si es de equipo grande
			log_info(LOG_SAFA,"Recibi cabecera: AbrirPathFinalizadoOk");
			datos_recibidos = prot_recibir_DMA_SAFA_datosEnMemoria(fd_DMA);
			bool coincideIDListo(void* node) {
				 return ((((tp_DTB) node)->id_GDT)==datos_recibidos->pid);
				 }
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_Listo = list_remove_by_condition(bloqueados, coincideIDListo);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			list_add(DTB_Listo->tabla_dir_archivos, datos_recibidos->path);
			log_info(LOG_SAFA, "Se agrego el path %s a la tabla de dir de archivos", datos_recibidos->path);
			pthread_mutex_lock(&mutex_LISTOS);
			list_add(listos, DTB_Listo);
			pthread_mutex_unlock(&mutex_LISTOS);
			free(datos_recibidos);
			log_info(LOG_SAFA, "El DTB %i ahora pasa a LISTOS", datos_recibidos->pid);
			pthread_mutex_unlock(&mutexDePausaPCP);
			break;
		case FlushDeArchivoADiscoNoFinalizado: //asumo que si falla va a exit
			log_info(LOG_SAFA, "Recibi cabecera: FlushDeArchivoADiscoNoFinalizado");
			pathPid = prot_recibir_DMA_SAFA_finFlush(fd_DMA);
			log_info(LOG_SAFA, "El Flush fallo, el DTB %i pasa a EXIT", pathPid->pid);
					bool coincideIDExit2(void* node) {
						return ((((tp_DTB) node)->id_GDT)==pathPid->pid);
					}
			free(pathPid);
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_exit = list_remove_by_condition(bloqueados, coincideIDExit2);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			pthread_mutex_lock(&mutex_TERMINADOS);
			list_add(terminados, DTB_exit);
			pthread_mutex_unlock(&mutex_TERMINADOS);
		break;
		case FlushDeArchivoADiscoFinalizadoOk: //asumo que si no falla el dtb se desbloquea
			log_info(LOG_SAFA, "Recibi cabecera: FlushDeArchivoADiscoFinalizadoOk");
			pathPid = prot_recibir_DMA_SAFA_finFlush(fd_DMA);
			log_info(LOG_SAFA, "El Flush fue un exito, el DTB %i pasa a LISTOS", pathPid->pid);
					bool coincideIDReady(void* node) {
						return ((((tp_DTB) node)->id_GDT)==pathPid->pid);
					}
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_Listo = list_remove_by_condition(bloqueados, coincideIDReady);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			free(pathPid);
			pthread_mutex_lock(&mutex_LISTOS);
			list_add(listos, DTB_Listo);
			pthread_mutex_unlock(&mutex_LISTOS);
		break;
		case CrearLineasEnArchivoNoFinalizado:
			log_info(LOG_SAFA, "Recibi cabecera: CrearLineasEnArchivoNoFinalizado");
			int id = prot_recibir_DMA_SAFA_crearArchivo(fd_DMA);
			log_info(LOG_SAFA, "El archivo no pudo ser creado, se aborta el DTB %i", id);
			bool coincideIDExit3(void* node) {
					return ((((tp_DTB) node)->id_GDT)==id);
			     }
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_exit = list_remove_by_condition(bloqueados, coincideIDExit3);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			free(id);
			pthread_mutex_lock(&mutex_TERMINADOS);
			list_add(terminados, DTB_exit);
			pthread_mutex_unlock(&mutex_TERMINADOS);
		break;
		case CrearLineasEnArchivoFinalizadoOk:
			log_info(LOG_SAFA, "Recibi cabecera: CrearLineasEnArchivoFinalizadoOk");
			int i = prot_recibir_DMA_SAFA_crearArchivo(fd_DMA);
			log_info(LOG_SAFA, "El archivo pudo crearse en el DTB %i", i);
			bool coincideIDReady2(void* node) {
					return ((((tp_DTB) node)->id_GDT)==i);
			    	}
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_Listo = list_remove_by_condition(bloqueados, coincideIDReady2);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			free(i);
			pthread_mutex_lock(&mutex_LISTOS);
			list_add(listos, DTB_exit);
			pthread_mutex_unlock(&mutex_LISTOS);
		break;
		case EliminarArchivoDeDiscoNoFinalizado:
			log_info(LOG_SAFA, "Recibi cabecera: EliminarArchivoDeDiscoNoFinalizado");
			int idtb = prot_recibir_DMA_SAFA_eliminarArchivo(fd_DMA);
			log_info(LOG_SAFA, "El archivo no pudo ser elminado, se aborta el DTB %i", idtb);
			bool coincideIDExit4(void* node) {
					return ((((tp_DTB) node)->id_GDT)==idtb);
			     }
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_exit = list_remove_by_condition(bloqueados, coincideIDExit4);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			free(idtb);
			pthread_mutex_lock(&mutex_TERMINADOS);
			list_add(terminados, DTB_exit);
			pthread_mutex_unlock(&mutex_TERMINADOS);
		break;
		case EliminarArchivoDeDiscoFinalizadoOk:
			log_info(LOG_SAFA, "Recibi cabecera: EliminarArchivoDeDiscoFinalizadoOk");
			int iddtb = prot_recibir_DMA_SAFA_eliminarArchivo(fd_DMA);
			log_info(LOG_SAFA, "El archivo fue eliminado, el DTB %i pasa a LISTOS", iddtb);
			bool coincideIDReady3(void* node) {
					return ((((tp_DTB) node)->id_GDT)==iddtb);
			    	}
			pthread_mutex_lock(&mutex_BLOQUEADOS);
			DTB_Listo = list_remove_by_condition(bloqueados, coincideIDReady3);
			pthread_mutex_unlock(&mutex_BLOQUEADOS);
			free(iddtb);
			pthread_mutex_lock(&mutex_LISTOS);
			list_add(listos, DTB_Listo);
			pthread_mutex_unlock(&mutex_LISTOS);
		break;
	}
	}
	resultadoComElDiego=EXIT_SUCCESS;
	return resultadoComElDiego;
}
 /*int esperarFinEscuchaDMA(){
	pthread_join(hiloComDMA, NULL);
	log_info(LOG_SAFA,"Finalizando el hilo de comunicacion con DMA");
	if(resultadoComElDiego==EXIT_FAILURE){
		log_info(LOG_SAFA,"Hilo de comunicacion con el DMA finalizado exitosamente");
	}else{
		log_error(LOG_SAFA,"Hilo de comunicacion con el DMA finalizado por error");
	}
	return resultadoComElDiego;
}
 int iniciarEscuchaCPU(){ //
	int resultadoHiloCPU = pthread_create(
			&hiloComCPU, NULL, funcionHiloComCPU, &configSAFA);
		if(resultadoHiloCPU){
			log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con la CPUr: %d\n",resultadoHiloCPU);
			exit(EXIT_FAILURE);
			}
		else{
			log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con la CPU");
			list_add(cpu_libres, hiloComCPU);
			return EXIT_SUCCESS;
			}
		return EXIT_SUCCESS;
} */
 void *funcionHiloComCPU(void *sockCPU){
	 pthread_mutex_lock(&mutex_CPULIBRES);
	 list_add(cpu_libres, sockCPU);
	 pthread_mutex_unlock(&mutex_CPULIBRES);
	 //si es la primera cpu que se conecta se pasa a operativo
	 if((list_size(cpu_libres)==1)&&(list_size(cpu_ejecutando)==0)){
	 pasarSafaOperativo();
	 }
	 while(safa_conectado){
	 t_cabecera cabecera = recibirCabecera(sockCPU);
	 log_info(LOG_SAFA,"Recibi cabecera de la CPU");
	 tp_DTB id_DTB; //tengo que hacer malloc? TODO
	 tp_tipoRecurso recurso;
	 int idGDT;
	 switch (cabecera.tipoDeMensaje){
	 	 case BloquearDTB:
	 		 idGDT = prot_recibir_CPU_SAFA_bloquear_DTB(sockCPU);
	 		 printf("el id a a bloquear es %d",idGDT);
	 		 log_info(LOG_SAFA, "CPU me pide bloquear el DTB %i", idGDT);
	 		 bool coincideID(void* node) {
	 		 return ((((tp_DTB) node)->id_GDT)==idGDT);
	 		 }
	 		 pthread_mutex_lock(&mutex_EJECUTANDO);
	 		 id_DTB=list_remove_by_condition(ejecutando, coincideID);
	 		 pthread_mutex_unlock(&mutex_EJECUTANDO);
	 		 if(id_DTB->iniGDT == 0){//se cambia el flag del dummy al bloquearlo
	 			 log_info(LOG_SAFA, "el DTB Dummy se bloquea, pasa flag a 1");
	 			 id_DTB->iniGDT = 1;
	 			 --hayDummy;
	 			//pthread_mutex_unlock(&mutexDePausaDePlanificacion);//ahora puedo aceptar otro dummy
	 		 }
	 		 pthread_mutex_lock(&mutex_BLOQUEADOS);
	 		 list_add(bloqueados, id_DTB);
	 		 pthread_mutex_unlock(&mutex_BLOQUEADOS);
	 		 pthread_mutex_lock(&mutex_CPULIBRES);
	 		 list_add(cpu_libres, sockCPU);
	 		 pthread_mutex_unlock(&mutex_CPULIBRES);
	 		 log_info(LOG_SAFA, "Se bloqueo el DTB %i", id_DTB->id_GDT);
	 	 break;
	 	 case AbortarDTB:
	 		 idGDT = prot_recibir_CPU_SAFA_abortar_DTB(sockCPU);
	 		 log_info(LOG_SAFA, "CPU me pide abortar el DTB %i", idGDT);
	 		 bool coincideID_Abortar(void* node){
	 		 return((((tp_DTB) node)->id_GDT)==idGDT);
	 		 }
	 		 pthread_mutex_lock(&mutex_EJECUTANDO);
	 		 id_DTB=list_remove_by_condition(ejecutando, coincideID_Abortar);
	 		 pthread_mutex_unlock(&mutex_EJECUTANDO);
	 		 pthread_mutex_lock(&mutex_TERMINADOS);
	 		 list_add(terminados, id_DTB);
	 		 pthread_mutex_unlock(&mutex_TERMINADOS);
	 		 pthread_mutex_lock(&mutex_CPULIBRES);
	 		 list_add(cpu_libres, sockCPU);
	 		 pthread_mutex_unlock(&mutex_CPULIBRES);
	 		 log_info(LOG_SAFA, "Se aborto el DTB %i", id_DTB->id_GDT);
	 	 break;
	 	 case RetenerRecurso:
	 		 recurso = prot_recibir_CPU_SAFA_retener_recurso(sockCPU);
	 		 log_info(LOG_SAFA, "CPU pide el recurso %s");
	 		 if(recursoEstaEnTabla(recurso->recurso)){
	 			 if(recursoEstaAsignado(recurso->recurso)){
	 				 log_info(LOG_SAFA, "El recurso %s esta tomado", recurso->recurso);
	 				 enviarCabecera(sockCPU, RespuestaASolicitudDeRecursoDenegada, sizeof(RespuestaASolicitudDeRecursoDenegada));
	 				 bool coincideIDBlq(void* node) {
	 					 return ((((tp_DTB) node)->id_GDT)==recurso->id_GDT);
	 				 }
	 				 pthread_mutex_lock(&mutex_EJECUTANDO);
	 				 id_DTB=list_remove_by_condition(ejecutando, coincideIDBlq);
	 				 pthread_mutex_unlock(&mutex_EJECUTANDO);
	 				 pthread_mutex_lock(&mutex_BLOQUEADOS);
	 				 list_add(bloqueados, id_DTB);
	 				 pthread_mutex_unlock(&mutex_BLOQUEADOS);
	 				 log_info(LOG_SAFA, "Se bloqueo el DTB %i", id_DTB->id_GDT);
	 				 agregarGdtAColaRecurso(recurso);
	 				 pthread_mutex_lock(&mutex_CPULIBRES);
	 				 list_add(cpu_libres, sockCPU);
	 				 pthread_mutex_unlock(&mutex_CPULIBRES);
	 				 log_info(LOG_SAFA, "Se agrego el GDT a la cola de espera");
	 			 }else{
	 				 log_info(LOG_SAFA, "El recurso %s esta libre", recurso->recurso);
	 				 enviarCabecera(sockCPU, RespuestaASolicitudDeRecursoAfirmativa, sizeof(RespuestaASolicitudDeRecursoAfirmativa));
	 				 log_info(LOG_SAFA, "Asigno el recurso %s al DTB %s", recurso->recurso, recurso->id_GDT);
	 				 bool coincideIDRec(void* node) {
	   					 return strcmp((((tp_tipoRecurso) node)->recurso),recurso->recurso);
	  				 }
	 				 pthread_mutex_lock(&mutex_TABLAREC);
	 				 tp_recurso rec = list_remove(tabla_recursos, coincideIDRec);
	 				 rec->valor = rec->valor - 1;
	 				 list_add(tabla_recursos, rec);
	 				 pthread_mutex_unlock(&mutex_TABLAREC);
	 			 }
	 		 }else{
	 			 log_info(LOG_SAFA, "El Recurso %s no existe, paso a crearlo", recurso->recurso);
	 			 tp_recurso recurso_creado = calloc(1, sizeof(t_recurso));
	 			 strcpy(recurso_creado->nombre, recurso->recurso);
	 			 recurso_creado->valor = 1;
	 			 recurso_creado->gdts_en_espera = list_create();
	 			 pthread_mutex_lock(&mutex_TABLAREC);
	 			 list_add(tabla_recursos, recurso_creado);
	 			 pthread_mutex_unlock(&mutex_TABLAREC);
	 			 log_info(LOG_SAFA, "Se agrego el recurso %s a la tabla", recurso_creado->nombre);
	 			 enviarCabecera(sockCPU, RespuestaASolicitudDeRecursoAfirmativa, sizeof(RespuestaASolicitudDeRecursoAfirmativa));
	 			 log_info(LOG_SAFA, "Recurso %s asignado a la CPU", recurso_creado->nombre);
	 		 }
	 	 break;
	 	 case LiberarRecurso:
	 		 recurso = prot_recibir_CPU_SAFA_liberar_recurso(sockCPU);
	 		 log_info(LOG_SAFA, "CPU pide liberar el recurso %s", recurso->recurso);
	 		 if(recursoEstaEnTabla(recurso->recurso)){
	 			 tp_recurso recurso_tabla;
	 			bool coincideNombre3(void* node){
	 				return strcmp((((tp_recurso) node)->nombre), recurso->recurso);
	 			}
	 			 pthread_mutex_lock(&mutex_TABLAREC);
	 			 recurso_tabla = list_remove_by_condition(tabla_recursos, coincideNombre3);
	 			 recurso_tabla->valor = recurso_tabla->valor + 1;
	 			 //obtengo el primer gdt que esta bloqueado
	 			 int idGDT;
	 			 idGDT = list_remove(recurso_tabla->gdts_en_espera, 0);
	 			 list_add(tabla_recursos, recurso_tabla);
	 			 pthread_mutex_unlock(&mutex_TABLAREC);
	 			 log_info(LOG_SAFA, "Recurso %s liberado", recurso_tabla->nombre);
	 			 //Desbloqueo el GDT
	 			 tp_DTB bloqDTB;
	 				bool coincideId(void* node) {
	 					return ((((tp_DTB) node)->id_GDT)==idGDT);
	 					}
	 			 pthread_mutex_lock(&mutex_BLOQUEADOS);
	 			 bloqDTB = list_remove_by_condition(bloqueados, coincideId);
	 			 pthread_mutex_unlock(&mutex_BLOQUEADOS);
	 			 pthread_mutex_lock(&mutex_LISTOS);
	 			 list_add(listos, bloqDTB);
	 			 pthread_mutex_unlock(&mutex_LISTOS);
	 			 //TODO deberia arrancar PCP?
	 			 log_info(LOG_SAFA, "El GDT %i ahora esta en READY", bloqDTB->id_GDT);
	 		 }else{
	 			log_info(LOG_SAFA, "El Recurso %s no existe, paso a crearlo", recurso->recurso);
	 				tp_recurso recurso_creado = calloc(1, sizeof(t_recurso));
	 				strcpy(recurso_creado->nombre, recurso->recurso);
	 				recurso_creado->valor = 1;
	 				recurso_creado->gdts_en_espera = list_create();
	 				pthread_mutex_lock(&mutex_TABLAREC);
	 				list_add(tabla_recursos, recurso_creado);
	 				pthread_mutex_unlock(&mutex_TABLAREC);
	 				log_info(LOG_SAFA, "Se agrego el recurso %s a la tabla", recurso_creado->nombre);
	 				//enviarCabecera(sockCPU, RespuestaASolicitudDeRecursoAfirmativa, sizeof(RespuestaASolicitudDeRecursoAfirmativa));
	 				log_info(LOG_SAFA, "Recurso %s asignado a la CPU", recurso_creado->nombre);
	 		 }
	 	break;

	 }
	 }
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
		enum PROCESO procesoConectado = recibir_Handshake_SAFA(PLANIFICADOR, client_fd);
		if(procesoConectado == DMA){
			log_info(LOG_SAFA, "Conexion realizada con El Diego");
			fd_DMA = client_fd;
			log_info(LOG_SAFA,"Handshake exitoso con el Diego en socket: %i", fd_DMA);
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
				continue;
				}
		}else if(procesoConectado == CPU){
				log_info(LOG_SAFA, "Conexion realizada con CPU");
				int fd_CPU = client_fd;
				log_info(LOG_SAFA,"Handshake exitoso con CPU en socket: %i", fd_CPU);
				log_info(LOG_SAFA, "Iniciando hilo para la comunicacion con el CPU");
				int resultadoHiloCPU = pthread_create(&hiloComCPU, NULL, funcionHiloComCPU, fd_CPU);
				if(resultadoHiloCPU){
					log_error(LOG_SAFA,"Error no se pudo crear el hilo para la comunicacion con la CPUr: %d\n",resultadoHiloCPU);
					exit(EXIT_FAILURE);
					}
				else{
					log_info(LOG_SAFA,"Se creo el hilo para la comunicacion con la CPU");
					pthread_detach(resultadoHiloCPU);
					continue;
					}

		} else {
					log_error(LOG_SAFA, "Se conecto algo distinto a DAM o CPU");
					//client_fd = -1;
				}
				continue;
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
			if(estadoSAFA==CORRUPTO){
				log_error(LOG_SAFA, "SAFA se encuentra en estado Corrupto, no se puede continuar");

			}else if(instruccion[1]!=NULL){
					printf("Creando DTB para escriptorio: %s/n", instruccion[1]);
					tp_DTB nuevo_DTB = crear_DTB(instruccion[1]);//se crea con 1. el PLP lo pasa a 0 cuando es dummy
					pthread_mutex_lock(&mutex_NUEVOS);
					list_add(nuevos, nuevo_DTB);//agregar DTB a cola de NEW
					pthread_mutex_unlock(&mutex_NUEVOS);
					pthread_mutex_unlock(&mutexDePausaDePlanificacion);//habilito PLP
					log_info(LOG_SAFA, "Se agrega el nuevo DTB a la lista de Nuevos");
					}else{
					printf("Falta el path del escriptorio, pediselo a Donofrio y reintenta/n");
					}

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
		if(strlen(linea)>0){
			free(instruccion);
		}
		free(linea);
		//for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
		//free(instruccion);
	}//Cierre del while(1)
	pthread_exit(ret);
}

char** parser_instruccion(char* linea) {
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

tp_DTB crear_DTB(char* path){
	tp_DTB new_DTB = calloc(1,sizeof(t_DTB));
	log_info(LOG_SAFA, "Creando nuevo DTB");
	new_DTB->id_GDT = id;
	log_info(LOG_SAFA, "DTB %i creado", id);
	id++;
	new_DTB->escriptorio = malloc(strlen(path)+1);
	strcpy(new_DTB->escriptorio, path);
	new_DTB->iniGDT = 1;
	new_DTB->program_counter = 1;
	new_DTB->tabla_dir_archivos = list_create();
	new_DTB->quantum = configSAFA.quantum;
	return new_DTB;
}


int iniciarPLP(){
	printf("entra a iniciarPLP");
	int resultadoDeCrearHilo = pthread_create(
		&hiloPLP, NULL, funcionHiloPLP, &configSAFA);
	if(resultadoDeCrearHilo){
		log_error(LOG_SAFA,"Error: no se pudo crear el hilo para la planificacion a largo plazo",resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
		}
	else{
		log_info(LOG_SAFA,"Se creo el hilo para la planificacion a largo plazo");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void *funcionHiloPLP(void *arg){
	char *ret="Cerrando hilo PLP";

	while(safa_conectado){
		//log_info(LOGGER,"Lockeando semaforo de pausa de planificacion");
		pthread_mutex_lock(&mutexDePausaDePlanificacion);

		log_info(LOG_SAFA,"Corre PLP");
		if((list_size(nuevos) + list_size(ejecutando) + list_size (bloqueados) - hayDummy)<configSAFA.grado_multiprogramacion){
			planificar_PLP();
		}
		//log_info(LOGGER,"Deslockeando semaforo de pausa de planificacion");
		//pthread_mutex_unlock(&mutexDePausaDePlanificacion);
		}

	pthread_exit(ret);
	return EXIT_SUCCESS;
}

int planificar_PLP(){
	printf("entra a planificar_PLP");
	tp_DTB idDTB;
	//lockearListas();
	log_info(LOG_SAFA,"PLP en accion");
	log_info(LOG_SAFA, "hay %i nuevos y hayDummy es %i", list_size(nuevos), hayDummy);
	if(list_size(nuevos)>0 && hayDummy == 0){
		pthread_mutex_lock(&mutex_NUEVOS);
		idDTB=list_remove(nuevos, 0);
		pthread_mutex_unlock(&mutex_NUEVOS);
		idDTB->iniGDT = 0; //lo desbloqueo como Dummy
		++hayDummy;
		pthread_mutex_lock(&mutex_LISTOS);
		list_add(listos, idDTB);
		pthread_mutex_unlock(&mutex_LISTOS);
		//pthread_mutex_unlock(&mutexDePausaPCP);
		log_info(LOG_SAFA,"DTB listo para planificar %d",idDTB->id_GDT);
		}else{
			log_error(LOG_SAFA, "AWANTIIIAAAA, EMOCIONADO: Ya existe un DTB Dummy");
		}
	//deslockearListas();
	pthread_mutex_unlock(&mutexDePausaPCP);
	return EXIT_SUCCESS;
}

int iniciarPCP(){
	int resultadoDeCrearHilo = pthread_create(
		&hiloPlanif, NULL, funcionHiloPlanif, &configSAFA);
	if(resultadoDeCrearHilo){
		log_error(LOG_SAFA,"Error: no se pudo crear el hilo para la planificacion a corto plazo",resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
		}
	else{
		log_info(LOG_SAFA,"Se creo el hilo para la planificacion a corto plazo");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void* funcionHiloPlanif(void *arg){
	char *ret="Cerrando hilo PCP";
	log_info(LOG_SAFA,"PCP inicia bloqueado");
	//log_info(LOG_SAFA, "Alg es %i", algoritmo_planificacion);
	while(safa_conectado){
		//log_info(LOG_SAFA,"Lockeando semaforo de pausa de planificacion");
		pthread_mutex_lock(&mutexDePausaPCP);
		log_info(LOG_SAFA,"Lockeando semaforo de pausa de planificacion");

		//log_info(LOGGER,"Planifico");
		//if(safa_conectado)
			planificar();

		//log_info(LOGGER,"Deslockeando semaforo de pausa de planificacion");
		//pthread_mutex_unlock(&mutexDePausaPCP);
		}

	pthread_exit(ret);
	return EXIT_SUCCESS;
}

int planificar(){

	log_info(LOG_SAFA,"Vamo a planificarno");
	if(list_size(listos)>0){
		tp_DTB DTB;
		int idDTB;
		//lockearListas();
		idDTB = proximoDTBAPlanificar();
		log_info(LOG_SAFA,"Proximo DTB a planificar %d",idDTB);
		DTB = buscarDTBPorId(idDTB);
		//ahora ya tengo el DTB entero que necesito enviar a CPU //
		pthread_mutex_lock(&mutex_CPULIBRES);
		int proxCPUaUsar = list_remove(cpu_libres, 0);
		pthread_mutex_unlock(&mutex_CPULIBRES);
		pthread_mutex_lock(&mutex_CPUEJEC);
		list_add(cpu_ejecutando, proxCPUaUsar);
		pthread_mutex_unlock(&mutex_CPUEJEC);
		//log_info(LOG_SAFA, "id_GDT: %i program_counter %i iniGDT %i escriptorio %s quantum %i cpu %i", DTB->id_GDT, DTB->program_counter, DTB->iniGDT, DTB->escriptorio, DTB->quantum, proxCPUaUsar);
		prot_enviar_SAFA_CPU_DTB(DTB->id_GDT, DTB->program_counter, DTB->iniGDT, DTB->escriptorio, DTB->tabla_dir_archivos, DTB->quantum, proxCPUaUsar);
		log_info(LOG_SAFA, "Se envio el DTB %i a la CPU %i", DTB->id_GDT, proxCPUaUsar);
		pthread_mutex_lock(&mutex_LISTOS);
		list_remove(listos, 0);
		pthread_mutex_unlock(&mutex_LISTOS);
		pthread_mutex_lock(&mutex_EJECUTANDO);
		list_add(ejecutando, DTB);
		pthread_mutex_unlock(&mutex_EJECUTANDO);

	}
	//deslockearListas();
	return EXIT_SUCCESS;
}

int proximoDTBAPlanificar(){
	int idDTBAPlanificar = 0;
	/*if(list_size(listos) < configSAFA.grado_multiprogramacion){
			log_info(LOG_SAFA,"Hay mas procesos para ejecutar");*/
	switch(algoritmo_planificacion){
		case ROUND_ROBIN:
			log_info(LOG_SAFA, "El algoritmo de planif es ROUND ROBIN");
			idDTBAPlanificar=calcularDTBAPlanificarConRR();
		break;
		case VIRTUAL_RR:
			log_info(LOG_SAFA, "El algoritmo de planif es VIRTUAL ROUND ROBIN");
			idDTBAPlanificar=calcularDTBAPlanificarConVRR();
		break;
		case BOAF:
			log_info(LOG_SAFA, "El algoritmo de planif es BOAF");
			idDTBAPlanificar=calcularDTBAPlanificarConBOAF();
	}
			/*if(strcmp(configSAFA.algoritmo_planif,"RR") == 0){
				printf("Entre por RR");
				log_info(LOG_SAFA, "El algoritmo de planif es ROUND ROBIN");
				idDTBAPlanificar=calcularDTBAPlanificarConRR();
				return idDTBAPlanificar;
			}
			if(strcmp(configSAFA.algoritmo_planif,"VRR") == 0){
				printf("Entre por VRR");
				log_info(LOG_SAFA, "El algoritmo de planif es VIRTUAL ROUND ROBIN");
				idDTBAPlanificar=calcularDTBAPlanificarConVRR();
				return idDTBAPlanificar;
			}
			if(strcmp(configSAFA.algoritmo_planif,"BOAF") == 0){
				printf("Entre por BOAF");
				log_info(LOG_SAFA, "El algoritmo de planif es BIG ONE ALWAYS FIRST");
				idDTBAPlanificar=calcularDTBAPlanificarConBOAF();
				return idDTBAPlanificar;
			}
		//log_info(LOG_SAFA,"Proximo DTB a planificar: %d ",idDTBAPlanificar);*/
		return idDTBAPlanificar;

	//}
	//log_error(LOG_SAFA, "No hay mas DTB para ser ejecutados");
	//return EXIT_FAILURE;
}

int calcularDTBAPlanificarConRR(){
	int id;
	id = obtenerPrimerId(listos);

	return id;
}

int calcularDTBAPlanificarConVRR(){
	int id;
	if(list_size(auxVirtualRR)>0){
		id = obtenerPrimerId(auxVirtualRR);
	}else{
		id = obtenerPrimerId(listos);
	}
	return id;
}

int calcularDTBAPlanificarConBOAF(){ /*Priorización de aquellos DTB que tengan abiertos archivos de "equipos grandes".
	La prioridad comienza una vez que lo abren y no antes.
	Queda a criterio de cada grupo definir qué equipos se consideran "grandes" TODO ver cuando abre archivo si es de equipo grande*/
	int id;
	if(list_size(dtbConEqGrandeAbierto)>0){
		id = obtenerPrimerId(dtbConEqGrandeAbierto);
	}else{
		id = obtenerPrimerId(listos);
	}
	return id;
}

int obtenerPrimerId(t_list* lista){
	tp_DTB primerDTB = list_get(lista, 0);
	return primerDTB->id_GDT;
}

tp_DTB buscarDTBPorId(int idDTB){
	tp_DTB el_DTB;
	bool coincideID(void* node) {
			return ((((tp_DTB) node)->id_GDT)==idDTB);
			}
	if(list_size(auxVirtualRR)>0){
		el_DTB = list_find(auxVirtualRR, coincideID);
	}else{
		el_DTB = list_find(listos, coincideID);
	}
	return el_DTB;
}

void pasarSafaOperativo(){
	if(DAM_conectado){
		estadoSAFA = OPERATIVO;
		log_info(LOG_SAFA, "SAFA ahora esta OPERATIVO");
	}
}

bool recursoEstaEnTabla(char* rec){
	tp_recurso recurso;
	bool coincideNombre(void* node){
		return strcmp((((tp_recurso) node)->nombre), rec);
	}
	bool ret = list_any_satisfy(tabla_recursos, coincideNombre);
	return ret;
}

bool recursoEstaAsignado(char* rec){
	tp_recurso recurso;
	bool coincideNombre2(void* node){
		return strcmp((((tp_recurso) node)->nombre), rec);
	}
	recurso = list_find(tabla_recursos, coincideNombre2);
	if(recurso->valor <= 0){
		return true;
	}else{
		return false;
	}
}

void agregarGdtAColaRecurso(tp_tipoRecurso recurso){
	tp_recurso recu;
	bool coincideNombreCola(void* node){
		return strcmp((((tp_recurso) node)->nombre), recurso->recurso);
		}
	pthread_mutex_lock(&mutex_TABLAREC);
	recu = list_remove_by_condition(tabla_recursos, coincideNombreCola);
	list_add(recu->gdts_en_espera, recurso->id_GDT);
	recu->valor = recu->valor - 1;
	list_add(tabla_recursos, recu);
	pthread_mutex_unlock(&mutex_TABLAREC);
}

/*void captura_sigpipe(int signo){

    if(signo == SIGINT)
    {
    	log_info(LOG_SAFA,"Finalizando proceso... Gracias vuelva prontos.");
    	//GLOBAL_SEGUIR = 0;
    	liberarMemoria();
    	finalizarTodo();
    }
    else if(signo == SIGPIPE)
    {
    	log_error(LOG_SAFA,"Tecla no reconocida");
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
    	log_error(LOG_SAFA," SIGACTION error ");
    }

    sigaddset(&signal_struct.sa_mask, SIGINT);
    if (sigaction(SIGINT, &signal_struct, NULL) < 0)
    {
    	log_error(LOG_SAFA," SIGACTION error ");
    }

}*/

