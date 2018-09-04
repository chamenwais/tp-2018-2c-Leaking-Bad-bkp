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
	if(levantarArchivoDeConfiguracion(argc,argv)==EXIT_FAILURE) finalizarTodo();
	if(iniciarConsola()==EXIT_FAILURE) finalizarTodo();
	levantarMetadataBin();
	esperarAQueTermineLaConsola();
	return EXIT_SUCCESS;
}
