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

typedef struct defDTB {
	int id_GDT;
	char* escriptorio; // ver si esto realmente es un char*
	int program_counter;
	int iniGDT; // 0 o 1
	t_list* tabla_dir_archivos; //tabla_dir_archivos;
	int quantum;
} t_DTB;

struct addrinfo* crear_addrinfo(char * ip, char * puerto);
int conectar_MEM(char * ip, char * puerto);
int conectar_DIEGO(char * ip, char * puerto);
int conectar_SAFA(char * ip, char * puerto);
void realizar_handshakes();
void solicitar_a_DAM_busqueda_dummy(t_DTB * dtb);
void desalojar_dtb(t_DTB * dtb);
void iniciar_operacion_dummy(t_DTB * dtb);
void parsea(char *);
void proceder_con_lectura_escriptorio(t_DTB * dtb);
void recibir_dtb_y_delegar(t_DTB * dtb);
void solicitar_a_FM9_la_sentencia(t_DTB * dtb);
tp_lineaCPU * recibir_de_FM9_linea_a_parsear(t_DTB * dtb);
void actualizar_program_counter(t_DTB * dtb, int pc);
void realizar_la_operacion_que_corresponda_segun(t_operacion resultado_del_parseado);
void abrir_nuevo_archivo_para_escriptorio();
void correr_una_unidad_de_tiempo_del_quantum();
void asignar_datos_a_linea();
void retener_recurso();
void liberar_recurso();
void guardar_contenido_de_fm9_en_mdj();
void liberar_archivo_abierto();
void crear_nuevo_archivo_en_mdj();
void eliminar_archivo_de_mdj();
void abortar(t_DTB * dtb);

#endif /* CPUmain_H_ */

