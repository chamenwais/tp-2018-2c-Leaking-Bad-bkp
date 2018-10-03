/*
 * ConexionesEntrantes.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef CONEXIONESENTRANTES_H_
#define CONEXIONESENTRANTES_H_
#include <lqvg/com.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#include "Contexto.h"
#include "Utilidades.h"

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_mdj, int socket_safa);
void realizar_handshake_con_cpu(int socket_id);
int escuchar_a_los_CPU();
void tratar_error_select(int socketfd_escucha);
void establecer_nuevo_fd_maximo(int * maximo_actual, int potencial_nuevo_maximo);

#endif /* CONEXIONESENTRANTES_H_ */
