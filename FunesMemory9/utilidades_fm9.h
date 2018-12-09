/*
 * utilidades_fm9.h
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#ifndef UTILIDADES_FM9_H_
#define UTILIDADES_FM9_H_

#include "contexto.h"
#include "utilidades_para_listas.h"

int iniciar_servidor(char * port);
int crear_listen_socket(char * puerto, int max_conexiones);
void cargar_archivo_de_configuracion();
void logger_funesMemory9(int tipo_esc, int tipo_log, const char* mensaje, ...);
void configurar_signals(void);
void inicializar_logger();
void finalizar_funesMemory9();
struct addrinfo* crear_addrinfo();
void captura_sigpipe(int signo);
void cerrar_sockets(int server_FM9, int socket_cpu, int cliente_DAM);
bool el_proceso_tiene_archivo_cargandose(void * archivo_cargandose, int pid);
bool el_archivo_ya_se_estaba_cargando(tp_cargarEnMemoria process_id);
void informar_espacio_insuficiente(int DAM_fd);
void obtener_archivo_en_curso_de_carga(tp_cargarEnMemoria parte_archivo,
		t_archivo_cargandose** archivo_de_proceso_cargandose);
void borrar_info_archivo_cargandose(int pid);

#endif /* UTILIDADES_FM9_H_ */
