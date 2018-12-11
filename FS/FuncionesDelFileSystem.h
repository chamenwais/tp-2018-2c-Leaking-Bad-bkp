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
#include <sys/mman.h>
#include <errno.h>
#include <dirent.h>
#include <openssl/md5.h>

typedef struct defMetadata {
	int tamanio;
	t_list * bloques;
} t_metadata;
typedef t_metadata* tp_metadata;

typedef struct defDatosObtenidos {
	enum MENSAJES resultado;
	char * datos;
} t_datosObtenidos;

typedef t_metadata* tp_metadata;
typedef struct defHiloDMA {
	pthread_attr_t attr;
	pthread_t thread;
	int fd;
} t_hiloDMA;
typedef t_hiloDMA* tp_hiloDMA;


int inicializarLog();
int cargarDirectorioActual();
int crearDirectorios();
int inicializarVariables();
int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion);
int levantarArchivoDeConfiguracion(int argc,char** argv);
int iniciarConsola();
char** parser_instruccion(char* linea);
void *funcionHiloConsola(void *arg);
int mostrarConfiguracion();
int imprimirEstadoDelBitmap();
int man();
char* directorioDeUsuario();
int pwd();
int esperarAQueTermineLaEscuchaConElDMA();
int esperarAQueTermineLaConsola();
int finalizarTodoPorError();
void liberarRecursos();
int obtenerLongigutDelArchivo(char* path);
int generarMD5(char* pathDelArchivo);
int mostrarBloque(int numeroDeBloque);
int funcionDeConsolacat(char* path);
int funcionDeConsolacd(char* path);
int listarDirectorioConParametro(char* path);
int listarDirectorioActual();
void volverUnaCarpetaParaAtras();
void agregarCarpetaAlDirectorioActual(char* carpeta);
int listarDirectorio(char* directorio);
int levantarMetadataBin();
bool existeElDirectorioDeConsola(char* path);
bool existeElDirectorio(char* path);
bool existeElArchivo(char *directorioDelArchivo);
int reservarBloqueYCrearEstructuras(int numeroDeBloqueLibre);
int obtenerBloqueLibreDelBitMap();
int levantarBitMap();
int bajarADiscoBitmap();
int iniciarEscuchaConDMA();
void *funcionHiloComunicacionConElDMA(void *arg);
bool hayQueFinalizarElPrograma();
int iniciarTrabajoConElDMA(int cabecera);
int avisoDeFinalizarPrograma();
int validarArchivoDeConsola(char *ubicacionDelArchivo);
int validarArchivoDeDMA(int FDDMA);
int validarArchivo(char *path);
int crearArchivoDeConsola(char *path, int cantidadDeBytes);
int crearArchivoDeDMA(int FDDMA);
int crearCarpetas(char *carpetasACrear);
int crearArchivo(char *ubicacionDelArchivo, int cantidadDeBytes, char *path);
int borrarArchivoDeConsola(char *path);
int borrarArchivoDeDMA(int fileDescriptorActual);
int borrarArchivo(char *path);
int obtenerDatosDeConsola(char *path, long int offset, long int Size);
int obtenerDatosDeDMA(int FDDMA);
t_datosObtenidos obtenerDatos(char *path, long int offset, long int size);
int guardarDatosDeConsola(char *path, long int offset, long int size, char *Buffer);
int guardarDatosDeDMA(int fileDescriptorActual);
int guardarDatos(char *path, long int offset, long int size, char *Buffer);
int actualizarMetaData(char* ubicacionDelArchivoDeMetadata,tp_metadata metadata);
tp_metadata recuperarMetaData(char *ubicacionDelArchivoDeMetadata);

#endif /* FUNCIONESDELFILESYSTEM_H_ */
