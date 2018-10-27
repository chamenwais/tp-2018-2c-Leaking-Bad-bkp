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
	crearDirectorios();
	cargarDirectorioActual();
	if(levantarMetadataBin()==EXIT_FAILURE) finalizarTodoPorError();
	if(levantarBitMap()==EXIT_FAILURE) finalizarTodoPorError();

	if(iniciarEscuchaConDMA()==EXIT_FAILURE) finalizarTodoPorError();

	if(iniciarConsola()==EXIT_FAILURE) finalizarTodoPorError();

	if(esperarAQueTermineLaConsola()==EXIT_FAILURE) finalizarTodoPorError();

	pthread_cancel(threadComunicacionConElDMA);

	//if(esperarAQueTermineLaEscuchaConElDMA()==EXIT_FAILURE) finalizarTodoPorError();

	liberarRecursos();

	return EXIT_SUCCESS;
}
