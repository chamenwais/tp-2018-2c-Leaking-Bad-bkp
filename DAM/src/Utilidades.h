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

enum tipo_logueo { escribir, loguear, escribir_loguear, l_trace, l_debug, l_info, l_warning, l_error};

void configurar_logger();
void logger_DAM(int tipo_esc, int tipo_log, const char* mensaje, ...);
void leer_puerto_de_escucha();
void levantar_configuracion(int cantidad_parametros, char ** parametros);
void terminar_controladamente(int return_nr);
void enlazar_hilos(pthread_t hilo1, pthread_t hilo2);
void leer_configuracion_safa();
void leer_configuracion_fm9();
void leer_configuracion_mdj();
void leer_transfer_size();
char* mensaje_informativo_recibir_handshake(char* proceso);
char* mensaje_informativo_previa_conexion_con(char* proceso);
char* mensaje_handshake_exitoso(char* proceso, int socket_id);
void informar_envio_handshake(char* proceso);
void informar_handksake_exitoso(int socket_id, char* proceso);
void informar_recibimiento_handshake(char* proceso);
void destruir_logger();
void validar_apertura_archivo_configuracion();
void configurar_signals(void);
void captura_sigpipe(int signo);
void loguear_cabecera_recibida(char * proceso);

//Conexiones
void cerrar_socket_y_terminar(int socket_id);
void validar_comunicacion(int socket_id, char * proceso);
void informar_handshake_erroneo_y_cerrar(int socket_id, char * proceso);
void mandar_handshake_a(int socket_id, enum PROCESO enumProceso, char * proceso);
void recibir_handshake_de(int socket_id, enum PROCESO enumProceso, char * proceso);
int comunicarse_con_file_system();
void realizar_handshake_con_mdj(int socket_id);
void mostrar_mensaje_previa_conexion_con_mdj();
void inicializar_semaforos_de_procesos();
void apagar_semaforos_de_procesos();

#endif /* UTILIDADES_H_ */
