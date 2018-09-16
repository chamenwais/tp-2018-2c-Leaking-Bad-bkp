/*
 * ConexionesEntrantes.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef CONEXIONESENTRANTES_H_
#define CONEXIONESENTRANTES_H_
#include "../../COM/lqvg/com.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#include "Contexto.h"
#include "Utilidades.h"

pthread_t crear_hilo_conexiones_entrantes(int socket_fm9, int socket_mdj);
void * comunicarse_con_cpu();
void realizar_handshake_con_cpu(int socket_id);

#endif /* CONEXIONESENTRANTES_H_ */
