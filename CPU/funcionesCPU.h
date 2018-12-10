/*
 * funcionesCPU.h
 *
 *  Created on: Sep 14, 2018
 *      Author: utnso
 */

#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_


#include "contextoCPU.h"

t_operacion parsear(char* linea);
void finalizar_cpu();
void cargar_archivo_de_config(char *path);
void logger_CPU(int tipo_esc, int tipo_log, const char* mensaje, ...);
void captura_sigpipe(int signo);
void configurar_signals(void);
void inicializar_logger();

#endif /* FUNCIONESCPU_H_ */
