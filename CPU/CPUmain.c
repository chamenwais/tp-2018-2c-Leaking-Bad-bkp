/*
 * CPUmain.c
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#include "CPUmain.h"
#include <stdio.h>



int main(){

	configurar_LOG_CPU();
	levantarArchConfig();
	connect_to_server(configuracion.IPSAFA, configuracion.PUERTOSAFA);
	connect_to_server(configuracion.IPELDIEGO, configuracion.PUERTOELDIEGO);

return 0;
}









