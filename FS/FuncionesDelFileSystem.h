/*
 * FuncionesDelFileSystem.h
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESDELFILESYSTEM_H_
#define FUNCIONESDELFILESYSTEM_H_

#include "FileSystem.h"
#include <readline/readline.h>

int inicializarLog();
int inicializarVariables();
int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion);
int levantarArchivoDeConfiguracion(int argc,char** argv);
int iniciarConsola();
char** parser_instruccion(char* linea);
void *funcionHiloConsola(void *arg);
int esperarAQueTermineLaConsola();
void finalizarTodo();
int listarDirectorioActual();
int listarDirectorio(char* directorio);
int levantarMetadataBin();

#endif /* FUNCIONESDELFILESYSTEM_H_ */
