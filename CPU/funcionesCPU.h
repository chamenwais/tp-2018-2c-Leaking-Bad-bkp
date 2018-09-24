/*
 * funcionesCPU.h
 *
 *  Created on: Sep 14, 2018
 *      Author: utnso
 */

#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_


#include "CPUmain.h"
#include "conexionesCPU.h"






void funciona();
int configurar_LOG_CPU();
void levantarArchConfig();
int finalizarTodo();
void captura_sigpipe(int signo);
void configurar_signals(void);






#endif /* FUNCIONESCPU_H_ */
