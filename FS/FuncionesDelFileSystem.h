/*
 * FuncionesDelFileSystem.h
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESDELFILESYSTEM_H_
#define FUNCIONESDELFILESYSTEM_H_

#include "FileSystem.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
//#include <lqvg/com.h>
//#include "lqvg/com.h"
//#include "../../../../utnso/rep/tp-2018-2c-Leaking-Bad/COM/lqvg/com.h"
//#include "../COM/lqvg/com.h"
#include "//usr/include/lqvg/com.h"

int inicializarLog();
int inicializarVariables();
int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion);
int levantarArchivoDeConfiguracion(int argc,char** argv);
int iniciarConsola();
char** parser_instruccion(char* linea);
void *funcionHiloConsola(void *arg);
int esperarAQueTermineLaEscuchaConElDMA();
int esperarAQueTermineLaConsola();
int finalizarTodoPorError();
void liberarRecursos();
int listarDirectorioActual();
int listarDirectorio(char* directorio);
int levantarMetadataBin();
bool existeElArchivo(char *directorioDelArchivo);
int levantarBitMap();
int bajarADiscoBitmap();
int iniciarEscuchaConDMA();
void *funcionHiloComunicacionConElDMA(void *arg);
int iniciarTrabajoConElDMA();
int validarArchivoDeConsola(char *ubicacionDelArchivo);
int validarArchivoDeDMA();
int validarArchivo(char *path);
int crearArchivoDeConsola(char *path);
int crearArchivoDeDMA();
int crearCarpetas(char *carpetasACrear);
int crearArchivo(char *ubicacionDelArchivo, char *path);
int obtenerDatosDeDMA();
int obtenerDatos(char *path, int offset, int Size);
int guardarDatosDeDMA();
int guardarDatos(char *path, int offset, int size, char *Buffer);

#endif /* FUNCIONESDELFILESYSTEM_H_ */
