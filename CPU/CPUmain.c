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

	conectarseA(configuracion.IPSAFA, configuracion.PUERTOSAFA);
	conectarseA(configuracion.IPELDIEGO, configuracion.PUERTOELDIEGO);



return 0;
}









