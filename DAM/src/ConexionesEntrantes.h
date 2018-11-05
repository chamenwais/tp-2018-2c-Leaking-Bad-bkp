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

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_safa, int socket_filesystem);
void realizar_handshake_con_cpu(int socket_id);
int escuchar_a_los_CPU();
void tratar_error_select(int socketfd_escucha);
void establecer_nuevo_fd_maximo(int * maximo_actual, int potencial_nuevo_maximo);
void clasificar_y_crear_hilo_correspondiente_a_pedido_CPU(
		enum MENSAJES mensaje_entrante
		, int socket_CPU_solicitante
		, int socket_fm9
		, int socket_safa
		, int socket_filesystem);
int cabecera_correcta(t_cabecera* cabecera);
int cabecera_esta_vacia(t_cabecera* cabecera);
void loguear_nueva_conexion_con_CPU(int nuevo_socketfd);
void loguear_y_cerrar_comunicacion_erronea_con_CPU(int sockfd_CPU_a_cerrar);
void loguear_y_avisar_a_safa_apertura_erronea(int sockfd_safa, char * proceso, tp_abrirPath path_y_pid);
void informar_operacion_abrir_erronea(int socket_safa, tp_abrirPath path_y_pid);
void informar_operacion_abrir_exitosa(int socket_safa, tp_abrirPath path_y_pid, int direccion_de_memoria);
void operacion_abrir_path(int * sockets);
int * adaptar_sockets_para_hilo(int CPU_Fd, int fm9_Fd, int Safa_fd, int filesystem_fd);
t_cabecera validar_archivo(int socket_mdj, tp_abrirPath mensaje_cpu);
bool validar_fragmento_archivo(tp_datosObtenidosDeProtocolo fragmento_archivo, int socket_safa, tp_abrirPath mensaje_cpu);
void informar_carga_en_memoria_erronea(int socket_safa, tp_abrirPath mensaje_cpu);
int cargar_datos_en_Fm9(int socket_fm9, tp_abrirPath info_cpu, int offset_Fm9, char* parte_archivo);
void loguear_no_obtencion_de_fragmento_archivo();
tp_datosObtenidosDeProtocolo pedir_datos_a_Mdj(char * ruta, int offset_Mdj, int socket_mdj);
void tratar_invalidez_archivo(t_cabecera respuesta_validez_archivo, tp_abrirPath path_y_pid, int socket_safa);
void tratar_validez_archivo(t_cabecera respuesta_validez_archivo, tp_abrirPath path_y_pid, int socket_mdj, int socket_fm9, int socket_safa);
#endif /* CONEXIONESENTRANTES_H_ */
