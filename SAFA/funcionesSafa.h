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
int ubicarArchivoDeConfiguracion(int argc,char** argv);
int levantarConfiguracionSAFA(char* ubicacionDelArchivoConfiguracion);
int inicializarVariablesSAFA();
int inicializarSemaforosSAFA();
int finalizarTodo();
void *funcionHiloComDMA(void *arg);
int esperarFinEscuchaDMA();
int iniciarEscuchaCPU();
void *funcionHiloComCPU(void *arg);
int escuchar();
int iniciarConsola();
void *funcionHiloConsola(void *arg);
char** parser_instruccion(char* linea);

#endif /* SAFA_FUNCIONESSAFA_H_ */
