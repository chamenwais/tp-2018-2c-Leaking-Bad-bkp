/*
 * Utilidades.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "../../COM/lqvg/com.h"

#include "Contexto.h"

void configurar_logger();
void leer_puerto_de_escucha();
void levantar_configuracion();
void terminar_controladamente(int return_nr);
void enlazar_hilos(pthread_t hilo1, pthread_t hilo2);
void leer_configuracion_safa();
void leer_configuracion_fm9();
void leer_configuracion_mdj();
void leer_transfer_size();

//Conexiones
void cerrar_socket_y_terminar(int socket_id);
void validar_comunicacion(int socket_id, const char * proceso);
void informar_handshake_erroneo_y_cerrar(int socket_id, const char * proceso);
void mandar_handshake_a(int socket_id, enum PROCESO enumProceso, const char * proceso);
void recibir_handshake_de(int socket_id, enum PROCESO enumProceso, const char * proceso);
void configurar_signals(void);
void captura_sigpipe(int signo);

#endif /* UTILIDADES_H_ */
