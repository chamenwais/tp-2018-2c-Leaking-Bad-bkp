/*
 * variablesGlobales.c
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_configuracionSAFA configSAFA;
char estadoSAFA;
pthread_t hiloComDMA;
pthread_t hiloComCPU;
int fd_DMA;
int resultadoComElDiego;
int fd_CPU;
fd_set readset;
fd_set auxReadSet;
int maxfd;
bool salir;
pthread_mutex_t mutexSalir;
pthread_mutex_t mutexSelect;
