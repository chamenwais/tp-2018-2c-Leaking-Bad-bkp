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
#include <readline/history.h>
#include <curses.h>
#include <term.h>

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
t_DTB * crear_DTB(char* path);
int inicializarListas();

#endif /* SAFA_FUNCIONESSAFA_H_ */
