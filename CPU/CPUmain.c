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
	configurar_signals();
	conexionHiloSAFA();
	//conexionHiloDAM();


return 0;
}









