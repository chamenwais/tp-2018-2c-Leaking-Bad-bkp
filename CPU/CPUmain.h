/*
 * CPUmain.h
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#ifndef CPUMAIN_H_
#define CPUMAIN_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h> // Para malloc
#include <signal.h>
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <pthread.h>

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/error.h>

#include "funcionesCPU.h"
#include "conexionesCPU.h"


//Estructuras


typedef struct dConfiguracion{
	char *IPSAFA;
	int PUERTOSAFA;
	char *IPELDIEGO;
	int PUERTOELDIEGO;
	char *IPMEM;
	int PUERTOMEM;
}tConfiguracion;


typedef struct defDTB {
	int id_GDT;
	char* escriptorio; // ver si esto realmente es un char*
	int program_counter;
	int iniGDT; // 0 o 1
	t_list* tabla_dir_archivos; //tabla_dir_archivos;
	int quantum;
} t_DTB;


t_log * LOG_CPU;
t_config * config;
tConfiguracion configuracion;




//Funciones


#endif /* CPUmain_H_ */

