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
	inicializarListas(); //inicializo listas planificacion
	/*Levanto archivo de configuracion*/
	char* ubicacionDelArchivoDeConfiguracion;
		if(argc > 1){
			ubicacionDelArchivoDeConfiguracion = argv[1];
		}
		else{
			ubicacionDelArchivoDeConfiguracion = "configuracionSAFA.cfg";
			}
			if(levantarConfiguracionSAFA(ubicacionDelArchivoDeConfiguracion)==EXIT_SUCCESS){
				log_info(LOG_SAFA,"Configuracion de SAFA recuperada exitosamente");
			}
			else{
				log_error(LOG_SAFA,"No se pudo levantar la configuracion de SAFA, abortando");
				finalizarTodo();
			}
	//Iniciar Consola//
	 iniciarConsola();

	//Iniciar escucha//
	if(escuchar()==EXIT_FAILURE) finalizarTodo();
	/*if(iniciarEscuchaCPU()==EXIT_FAILURE) finalizarTodo();
	if(esperarFinEscuchaDMA()==EXIT_FAILURE) finalizarTodo();*/


}
