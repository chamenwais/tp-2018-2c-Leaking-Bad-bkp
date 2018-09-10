/*
 * FileSystem.c
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#include "FileSystem.h"

t_log* LOGGER;


int main(int argc,char** argv) {

	inicializarLog();
	inicializarVariables();
	if(levantarArchivoDeConfiguracion(argc,argv)==EXIT_FAILURE) finalizarTodoPorError();

	if(levantarMetadataBin()==EXIT_FAILURE) finalizarTodoPorError();
	if(levantarBitMap()==EXIT_FAILURE) finalizarTodoPorError();
	iniciarEscuchaConDMA();

	if(iniciarConsola()==EXIT_FAILURE) finalizarTodoPorError();
	esperarAQueTermineLaConsola();

	liberarRecursos();
	return EXIT_SUCCESS;
}
