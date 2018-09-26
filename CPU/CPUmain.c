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
	conexionHiloDAM();


	/*int sock;
	struct sockaddr_in DireccionServidor;
	DireccionServidor.sin_family = AF_INET;
	DireccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	DireccionServidor.sin_port = htons(2009);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, (void*) &DireccionServidor, sizeof(DireccionServidor)) != 0) {
		perror("No se pudo conectar");
		return -1;
	}
*/


//	comunicarse_con_SAFA();

	/*	conectarseA(configuracion.IPSAFA, configuracion.PUERTOSAFA);
	conectarseA(configuracion.IPELDIEGO, configuracion.PUERTOELDIEGO);*/



return 0;
}









