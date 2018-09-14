/*
 * ElDiego.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef ELDIEGO_H_
#define ELDIEGO_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <commons/log.h>

#include "ConexionesEntrantes.h"
#include "ConexionesSalientes.h"
#include "Contexto.h"
#include "Utilidades.h"

int comunicarse_con_memoria_y_reportar_a_fs(int socket_fs);
int comunicarse_con_file_system();
void realizar_handshake_con_mdj(int socket_id);
void realizar_handshake_con_fm9(int socket_id);
void reportar_a_fs(int socket_fs, int socket_fm9);

#endif /* ELDIEGO_H_ */
