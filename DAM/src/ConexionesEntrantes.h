/*
 * ConexionesEntrantes.h
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#ifndef CONEXIONESENTRANTES_H_
#define CONEXIONESENTRANTES_H_
#include <lqvg/com.h>
#include <lqvg/protocolo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#include "Contexto.h"
#include "Utilidades.h"

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_safa);
void realizar_handshake_con_cpu(int socket_id);
int escuchar_a_los_CPU();
void tratar_error_select(int socketfd_escucha);
void establecer_nuevo_fd_maximo(int * maximo_actual, int potencial_nuevo_maximo);
void clasificar_y_crear_hilo_correspondiente_a_pedido_CPU(
		enum MENSAJES mensaje_entrante
		, int socket_CPU_solicitante
		, int socket_fm9
		, int socket_safa);
int cabecera_correcta(t_cabecera* cabecera);
int cabecera_esta_vacia(t_cabecera* cabecera);
void loguear_tamanio_cabecera_recibida_de_CPU(t_cabecera* cabecera_de_CPU);
void loguear_nueva_conexion_con_CPU(int nuevo_socketfd);
void loguear_y_cerrar_comunicacion_erronea_con_CPU(
		int sockfd_CPU_a_cerrar);

#endif /* CONEXIONESENTRANTES_H_ */
