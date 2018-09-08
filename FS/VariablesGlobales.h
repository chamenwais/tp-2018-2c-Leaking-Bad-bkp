/*
 * VariablesGlobales.h
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#include "FileSystem.h"

typedef struct defConfiguracionDelFS {
	int puerto;
	char* punto_montaje;
	int retardo;
} t_configuracionDelFS;

typedef struct defConfiguracionMetadata {
	int tamanioBloques;
	int cantidadBloques;
	char* magicNumber;
} t_configuracionMetadata;

extern t_configuracionDelFS configuracionDelFS;
extern t_configuracionMetadata configuracionDeMetadata;
extern pthread_t threadConsola;
extern t_bitarray *bitmap;

#endif /* VARIABLESGLOBALES_H_ */
