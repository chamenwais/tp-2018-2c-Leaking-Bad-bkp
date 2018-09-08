/*
 * ElDiego.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ElDiego.h"

int main() {
	configurar_logger();
	levantar_configuracion();
	pthread_t * hilo_conexiones_entrantes=crear_hilo_conexiones_entrantes();
	pthread_t * hilo_conexiones_salientes=crear_hilo_conexiones_salientes();
	enlazar_hilos(hilo_conexiones_entrantes, hilo_conexiones_salientes);
	log_info(logger, "El puerto de escucha va a ser: %d",puerto_de_escucha);
	terminar_controladamente(0);
}
