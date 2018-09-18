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



int connect_to_server(char * ip, int * puerto);
void _exit_with_error(int socket, char* error_msg, void * buffer);
void exit_gracefully(int return_nr);






#endif /* CONEXIONESCPU_H_ */
