/*
 * Contexto.c
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */
#include "Contexto.h"

/**
 * Constantes
 */
char * PATH_ARCHIVO_LOG="/home/utnso/DAM.log";
char * NOMBRE_ARCHIVO_CONFIGURACION="configDAM.cfg";
char * CLAVE_PUERTO_DE_ESCUCHA="PUERTO_DE_ESCUCHA";
char * CLAVE_IP_SAFA="IP_SAFA";
char * CLAVE_PUERTO_SAFA="PUERTO_S-AFA";
char * CLAVE_IP_FM9="IP_FM9";
char * CLAVE_PUERTO_FM9="PUERTO_FM9";
char * CLAVE_IP_MDJ="IP_MDJ";
char * CLAVE_PUERTO_MDJ="PUERTO_MDJ";
char * CLAVE_TRANSFER_SIZE="TRANSFER_SIZE";
char * CONST_NAME_FM9 = "FM9";
char * CONST_NAME_MDJ = "MDJ";
char * CONST_NAME_SAFA = "SAFA";
char * CONST_NAME_CPU = "CPU";

//Semaforos mutex
pthread_mutex_t MX_MEMORIA;
pthread_mutex_t MX_FS;
pthread_mutex_t MX_SAFA;

t_log * logger;
t_config * configuracion;

int puerto_de_escucha;
//S-AFA
char * ip_safa;
int puerto_safa;
//FM9
char * ip_fm9;
int puerto_fm9;
//MDJ
char * ip_mdj;
int puerto_mdj;

int transfer_size;
