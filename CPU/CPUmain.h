/*
 * CPUmain.h
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#ifndef CPUMAIN_H_
#define CPUMAIN_H_

#include "contextoCPU.h"
#include "funcionesCPU.h"

struct addrinfo* crear_addrinfo(char * ip, char * puerto);
int conectar_SAFA(char * ip, char * puerto);
int conectar_DIEGO(char * ip, char * puerto);
int conectar_MEM(char * ip, char * puerto);
void realizar_handshakes();
void cerrar_script(tp_DTB dtb);
void solicitar_a_DAM_busqueda_dummy(tp_DTB dtb);
void desalojar_dtb(tp_DTB dtb);
void iniciar_operacion_dummy(tp_DTB dtb);
void solicitar_a_FM9_la_sentencia(tp_DTB dtb);
tp_lineaParaCPU * recibir_de_FM9_linea_a_parsear(tp_DTB dtb);
void actualizar_program_counter(tp_DTB dtb);
bool tienen_el_mismo_nombre(void * arch1);
bool chequear_si_archivo_esta_abierto(tp_DTB dtb);
void abrir_nuevo_archivo_para_escriptorio(char * path, tp_DTB dtb);
void correr_una_unidad_de_tiempo_del_quantum();
void informar_a_safa_que_aborte_DTB(tp_DTB dtb);
void asignar_datos_a_linea(char * datos, char * linea, char * path, tp_DTB dtb);
void enviar_a_SAFA_solicitud_retencion_recurso(char * recurso, tp_DTB dtb);
void enviar_a_SAFA_solicitud_liberar_recurso(char * recurso, tp_DTB dtb);
void esperar_respuesta_afirmativa_de_SAFA();
void retener_recurso(char * recurso, tp_DTB dtb);
void liberar_recurso(char * recurso, tp_DTB dtb);
void guardar_contenido_de_fm9_en_mdj(char * path, tp_DTB dtb);
void enviar_FM9_solicitud_liberar_archivo(char * path, tp_DTB dtb);
void actualizar_dtb_liberar_archivo(char * path, tp_DTB dtb);
void liberar_archivo_abierto(char * path, tp_DTB dtb);
void crear_nuevo_archivo_en_mdj(char * path, int cant_lineas, tp_DTB dtb);
void eliminar_archivo_de_mdj(char * path, tp_DTB dtb);
void realizar_la_operacion_que_corresponda_segun(t_operacion resultado_del_parseado, tp_DTB dtb);
void proceder_con_lectura_escriptorio(tp_DTB dtb);
void recibir_dtb_y_delegar(tp_DTB dtb);
tp_DTB recibir_SAFA_CPU_DTB(int sock);

#endif /* CPUmain_H_ */

