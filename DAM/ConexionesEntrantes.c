/*
 * ConexionesEntrantes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "ConexionesEntrantes.h"

pthread_t *crear_hilo_conexiones_entrantes(){
	pthread_t conexiones_entrantes;
	//pthread_create(&conexiones_salientes, NULL, printDataAndWait, NULL);
	return conexiones_entrantes;
}
