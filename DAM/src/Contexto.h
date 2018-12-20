/*
 * Contexto.h
 *
 *  Created on: 8 sep. 2018
 *      Author: utnso
 */

#ifndef CONTEXTO_H_
#define CONTEXTO_H_
#include <commons/log.h>
#include <commons/config.h>

#define NO_SOCKET -1

extern t_log * logger;
extern char * PATH_ARCHIVO_LOG;
extern t_config * configuracion;
extern char * NOMBRE_ARCHIVO_CONFIGURACION;
extern int puerto_de_escucha;
extern char * CLAVE_PUERTO_DE_ESCUCHA;
extern char * ip_safa;
extern char * CLAVE_IP_SAFA;
extern int puerto_safa;
extern char * CLAVE_PUERTO_SAFA;
extern char * ip_fm9;
extern char * CLAVE_IP_FM9;
extern int puerto_fm9;
extern char * CLAVE_PUERTO_FM9;
extern char * ip_mdj;
extern char * CLAVE_IP_MDJ;
extern int puerto_mdj;
extern char * CLAVE_PUERTO_MDJ;
extern int transfer_size;
extern char * CLAVE_TRANSFER_SIZE;
extern char * CONST_NAME_FM9;
extern char * CONST_NAME_MDJ;
extern char * CONST_NAME_SAFA;
extern char * CONST_NAME_CPU;
extern pthread_mutex_t MX_MEMORIA;
extern pthread_mutex_t MX_FS;
extern pthread_mutex_t MX_SAFA;
extern pthread_mutex_t MX_CPU;
extern int FM9_fd;
extern int SAFA_fd;
extern int FS_fd;

#endif /* CONTEXTO_H_ */
