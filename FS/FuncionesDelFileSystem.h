/*
 * FuncionesDelFileSystem.h
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESDELFILESYSTEM_H_
#define FUNCIONESDELFILESYSTEM_H_

#include "VariablesGlobales.h"
#include "FileSystem.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
//#include <lqvg/com.h>
//#include "lqvg/com.h"
//#include "../../../../utnso/rep/tp-2018-2c-Leaking-Bad/COM/lqvg/com.h"
//#include "../COM/lqvg/com.h"
#include "//usr/include/lqvg/com.h"
#include "//usr/include/lqvg/protocolo.h"

typedef struct defMetadata {
	int tamanio;
	t_list * bloques;
} t_metadata;
typedef t_metadata* tp_metadata;

typedef struct defHiloDMA {
	pthread_attr_t attr;
	pthread_t thread;
	int fd;
} t_hiloDMA;
typedef t_hiloDMA* tp_hiloDMA;


int inicializarLog();
int inicializarVariables();
int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion);
int levantarArchivoDeConfiguracion(int argc,char** argv);
int iniciarConsola();
char** parser_instruccion(char* linea);
void *funcionHiloConsola(void *arg);
int mostrarConfiguracion();
int man();
int esperarAQueTermineLaEscuchaConElDMA();
int esperarAQueTermineLaConsola();
int finalizarTodoPorError();
void liberarRecursos();
int listarDirectorioActual();
int listarDirectorio(char* directorio);
int levantarMetadataBin();
bool existeElArchivo(char *directorioDelArchivo);
int reservarBloqueYCrearEstructuras(int numeroDeBloqueLibre);
int obtenerBloqueLibreDelBitMap();
int levantarBitMap();
int bajarADiscoBitmap();
int iniciarEscuchaConDMA();
void *funcionHiloComunicacionConElDMA(void *arg);
int iniciarTrabajoConElDMA();
int validarArchivoDeConsola(char *ubicacionDelArchivo);
int validarArchivoDeDMA(int FDDMA);
int validarArchivo(char *path);
int crearArchivoDeConsola(char *path);
int crearArchivoDeDMA(int FDDMA);
int crearCarpetas(char *carpetasACrear);
int crearArchivo(char *ubicacionDelArchivo, char *path);
int obtenerDatosDeConsola(char *path, int offset, int Size);
int obtenerDatosDeDMA(int FDDMA);
int obtenerDatos(char *path, int offset, int Size);
int guardarDatosDeConsola(char *path, int offset, int size, char *Buffer);
int guardarDatosDeDMA(int fileDescriptorActual);
int guardarDatos(char *path, int offset, int size, char *Buffer);
int actualizarMetaData(char* ubicacionDelArchivoDeMetadata,tp_metadata metadata);
tp_metadata recuperarMetaData(char *ubicacionDelArchivoDeMetadata);

#endif /* FUNCIONESDELFILESYSTEM_H_ */
