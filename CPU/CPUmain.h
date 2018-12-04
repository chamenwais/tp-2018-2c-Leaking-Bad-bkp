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
void abrir_script(char *path);
void parsear_escriptorio(char * linea_del_archivo);
void proceder_con_lectura_escriptorio(t_DTB * dtb);
void recibir_dtb_y_delegar(t_DTB * dtb);

#endif /* CPUmain_H_ */

