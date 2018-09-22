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
char * path_archivo_log="/home/utnso/DAM.log";
char * path_archivo_configuracion="configDAM.cfg";
char * clave_puerto_de_escucha="PUERTO_DE_ESCUCHA";
char * clave_ip_safa="IP_SAFA";
char * clave_puerto_safa="PUERTO_S-AFA";
char * clave_ip_fm9="IP_FM9";
char * clave_puerto_fm9="PUERTO_FM9";
char * clave_ip_mdj="IP_MDJ";
char * clave_puerto_mdj="PUERTO_MDJ";
char * clave_transfer_size="TRANSFER_SIZE";
char * const_name_fm9 = "FM9";
char * const_name_mdj = "MDJ";
char * const_name_safa = "SAFA";
char * const_name_cpu = "CPU";

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
