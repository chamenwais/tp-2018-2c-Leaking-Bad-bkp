/*
 * funcionesSafa.h
 *
 *  Created on: 7 sep. 2018
 *      Author: utnso
 */

#ifndef SAFA_FUNCIONESSAFA_H_
#define SAFA_FUNCIONESSAFA_H_

#include "SAFA.h"
#include <readline/readline.h>

int inicializarLog();
int levantarConfiguracionSAFA(char* ubicacionDelArchivoConfiguracion);
int inicializarVariablesSAFA();
int finalizarTodo();
int iniciarEscuchaConDiego();
void *funcionHiloComDMA(void *arg);
int esperarFinEscuchaDMA();

#endif /* SAFA_FUNCIONESSAFA_H_ */
