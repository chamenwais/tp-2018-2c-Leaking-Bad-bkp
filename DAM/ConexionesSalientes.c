/*
 * ConexionesSalientes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ConexionesSalientes.h"

pthread_t *crear_hilo_conexiones_salientes(){
	pthread_t conexiones_salientes;
	//pthread_create(&conexiones_salientes, NULL, printDataAndWait, NULL);
	return conexiones_salientes;
}
