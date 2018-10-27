/*
 * VariablesGlobales.c
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#include "VariablesGlobales.h"

t_configuracionDelFS configuracionDelFS;
t_configuracionMetadata configuracionDeMetadata;
pthread_t threadConsola;
t_bitarray *bitmap;
pthread_t threadComunicacionConElDMA;
int resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
int tamanioBitmap;
bool finalizarPrograma;
pthread_mutex_t mutexFinalizarPrograma;
char* directorioActual;
