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
pthread_t hiloConsola;
pthread_t hiloPlanif;
pthread_t hiloPLP;
int fd_DMA;
int resultadoComElDiego;
bool safa_conectado;
int id;
t_list* nuevos;
t_list* listos;
t_list* ejecutando;
t_list* bloqueados;
t_list* terminados;
t_list* auxVirtualRR;
t_list* cpu_ejecutando;
t_list* cpu_libres;
t_list* dtbConEqGrandeAbierto;
pthread_mutex_t mutexDePausaDePlanificacion;
bool DAM_conectado;

