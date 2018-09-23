/*
 * conexionesCPU.c
 *
 *  Created on: Sep 15, 2018
 *      Author: utnso
 */


#include "conexionesCPU.h"





void * comunicarse_con_DAM()
{
	log_info(LOG_CPU,"Conectando con ELDIEGO/DAM por el puerto %d", configuracion.PUERTOELDIEGO);
	int socket_conecta_DAM = conectarseA(configuracion.IPELDIEGO, configuracion.PUERTOELDIEGO);
	if(validar_comunicacion(socket_conecta_DAM, "DAM"))
	{
		if(enviarHandshake(CPU, DMA , socket_conecta_DAM))
			log_info(LOG_CPU,"Handshake exitoso con DAM");
		else
			log_error(LOG_CPU,"Handshake incorrecto, no era el DAM");
	}
}

void * comunicarse_con_SAFA()
{
	log_info(LOG_CPU,"Conectando con SAFA por el puerto %d", configuracion.PUERTOSAFA);
	int socket_conecta_SAFA = conectarseA(configuracion.IPSAFA, configuracion.PUERTOSAFA);
	if(validar_comunicacion(socket_conecta_SAFA, "SAFA"))
		if(enviarHandshake(CPU, DMA , socket_conecta_SAFA))
			log_info(LOG_CPU,"Handshake exitoso con SAFA");
		else
			log_error(LOG_CPU,"Handshake incorrecto, no era el SAFA");
}


int validar_comunicacion(int socket_id, char * proceso)
{
	if(socket_id<0)
	{
		log_error(LOG_CPU, "No se pudo conectar con el proceso %s ", proceso);
		cerrar_socket_y_terminar(socket_id);
	}
	else
	{
		log_info(LOG_CPU, "Conexion realizada con %s", proceso);
		return EXIT_SUCCESS;
	}

}


int cerrar_socket_y_terminar(int socket_id)
{
	close(socket_id);
	terminar_controladamente(EXIT_FAILURE);
}

int terminar_controladamente(int return_nr){
	log_info(LOG_CPU, "Se destruira estructura de archivo de configuracion");
	config_destroy(config);
	log_info(LOG_CPU, "Se destruira el logger");
	log_destroy(LOG_CPU);
	exit(return_nr);
}




/*
 void _exit_with_error(int socket, char* error_msg, void * buffer) {
 if (buffer != NULL) {
 free(buffer);
 }
 log_error(LOG_CPU, error_msg);
  close(socket);
  exit_gracefully(1);
}


void exit_gracefully(int return_nr) {

    20.   Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion

 	log_destroy(LOG_CPU);
 	exit(return_nr);
}
*/

