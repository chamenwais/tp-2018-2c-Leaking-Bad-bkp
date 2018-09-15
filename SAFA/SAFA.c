/*
 * SAFA.c
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#include "SAFA.h"

t_log* LOG_SAFA;

int main(int argc, char** argv){

	//Inicio el Log y variables//
	inicializarLog();
	inicializarSemaforosSAFA();
	inicializarVariablesSAFA(); // estado = CORRUPTO
	/*Levanto archivo de configuracion*/
	if(ubicarArchivoDeConfiguracion(argc,argv)==EXIT_FAILURE) finalizarTodo();
	/*Iniciar Consola
	 * iniciarConsola();
	 */
	if(iniciarEscuchaConDiego()==EXIT_FAILURE) finalizarTodo();
	if(iniciarEscuchaCPU()==EXIT_FAILURE) finalizarTodo();

	if(esperarFinEscuchaDMA()==EXIT_FAILURE) finalizarTodo();



}
