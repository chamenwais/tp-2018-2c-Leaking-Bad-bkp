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

/*typedef struct defDTB {
	int id_GDT;
	char* escriptorio; // ver si esto realmente es un char*
	int program_counter;
	int iniGDT; // 0 o 1
	t_list* tabla_dir_archivos; //tabla_dir_archivos;
	int quantum;
} t_DTB;*/

typedef struct recurso{
	char* nombre;
	int valor;
	t_list* gdts_en_espera;
}t_recurso;

typedef t_recurso* tp_recurso;

enum ESTADO {
	CORRUPTO,
	OPERATIVO
};

extern t_configuracionSAFA configSAFA;
extern char estadoSAFA;
extern pthread_t hiloComDMA;
extern pthread_t hiloComCPU;
extern pthread_t hiloConsola;
extern pthread_t hiloPlanif;
extern pthread_t hiloPLP;
extern int fd_DMA;
extern int resultadoComElDiego;
extern bool safa_conectado;
extern int id;
extern int hayDummy;
extern t_list* nuevos;
extern t_list* listos;
extern t_list* ejecutando;
extern t_list* bloqueados;
extern t_list* terminados;
extern t_list* auxVirtualRR;
extern t_list* cpu_libres;
extern t_list* cpu_ejecutando;
extern t_list* dtbConEqGrandeAbierto;
extern t_list* tabla_recursos;
extern t_list* lista_espera_recursos;
extern pthread_mutex_t mutexDePausaDePlanificacion;
extern bool DAM_conectado;


#endif /* SAFA_VARIABLESGLOBALES_H_ */
