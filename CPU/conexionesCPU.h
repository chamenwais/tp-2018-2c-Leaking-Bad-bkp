/*
 * conexionesCPU.h
 *
 *  Created on: Sep 15, 2018
 *      Author: utnso
 */

#ifndef CONEXIONESCPU_H_
#define CONEXIONESCPU_H_


#include "CPUmain.h"
#include "lqvg/com.h"



void * comunicarse_con_DAM();
void * comunicarse_con_SAFA();
int validar_comunicacion(int socket_id, char * proceso);
int cerrar_socket_y_terminar(int socket_id);
int terminar_controladamente(int return_nr);







/*void _exit_with_error(int socket, char* error_msg, void * buffer);
void exit_gracefully(int return_nr);*/






#endif /* CONEXIONESCPU_H_ */
