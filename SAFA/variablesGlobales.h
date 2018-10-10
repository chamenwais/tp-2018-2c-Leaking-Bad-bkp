/*
 * variablesGlobales.h
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#ifndef SAFA_VARIABLESGLOBALES_H_
#define SAFA_VARIABLESGLOBALES_H_

//#include "SAFA.h"
#include <commons/collections/list.h>
#include "pthread.h"

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
	t_list* tabla_dir_archivos; //tabla_dir_archivos;
} t_DTB;

extern t_configuracionSAFA configSAFA;
extern char estadoSAFA;
extern pthread_t hiloComDMA;
extern pthread_t hiloComCPU;
extern pthread_t hiloConsola;
extern int fd_DMA;
extern int resultadoComElDiego;
extern bool safa_conectado;
extern int id;
extern t_list* nuevos;
extern t_list* listos;
extern t_list* ejecutando;
extern t_list* bloqueados;
extern t_list* exit_status;


#endif /* SAFA_VARIABLESGLOBALES_H_ */
