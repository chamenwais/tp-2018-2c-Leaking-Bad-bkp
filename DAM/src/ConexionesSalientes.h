/*
 * ConexionesSalientes.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef CONEXIONESSALIENTES_H_
#define CONEXIONESSALIENTES_H_
#include "../../COM/lqvg/com.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#include "Contexto.h"

pthread_t crear_hilo_conexiones_salientes(int socket_fm9, int socket_mdj);
void * comunicarse_con_safa();
void realizar_handshake_con_safa(int socket_id);

#endif /* CONEXIONESSALIENTES_H_ */
