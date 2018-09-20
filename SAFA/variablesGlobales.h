/*
 * variablesGlobales.h
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#ifndef SAFA_VARIABLESGLOBALES_H_
#define SAFA_VARIABLESGLOBALES_H_

#include "SAFA.h"

typedef struct defConfiguracionSAFA {
	int puerto;
	char* algoritmo_planif;
	int quantum;
	int grado_multiprogramacion;
	int retardo;
} t_configuracionSAFA;

typedef struct defDTB {
	int id_GDT;
	char* escriptorio; // ver si esto realmente es un char*
	int program_counter;
	int iniGDT; // 0 o 1
	//tabla_dir_archivos;
} t_DTB;

extern t_configuracionSAFA configSAFA;
extern char estadoSAFA;
extern pthread_t hiloComDMA;
extern pthread_t hiloComCPU;
extern int fd_DMA;
extern int resultadoComElDiego;
extern bool safa_conectado;


#endif /* SAFA_VARIABLESGLOBALES_H_ */
