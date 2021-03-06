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
#include <signal.h>


int inicializarLog();
int ubicarArchivoDeConfiguracion(int argc,char** argv);
int levantarConfiguracionSAFA(char* ubicacionDelArchivoConfiguracion);
int inicializarVariablesSAFA();
int inicializarSemaforosSAFA();
int finalizarTodo();
void *funcionHiloComDMA(void *arg);
int esperarFinEscuchaDMA();
int iniciarEscuchaCPU();
void *funcionHiloComCPU(void *sockCPU);
int escuchar();
int iniciarConsola();
void *funcionHiloConsola(void *arg);
char** parser_instruccion(char* linea);
tp_DTB crear_DTB(char* path);
int inicializarListas();
int iniciarPCP();
int iniciarPLP();
int liberarMemoria();
void *funcionHiloPLP(void *arg);
void *funcionHiloPlanif(void *arg);
int planificar_PLP();
int planificar();
int proximoDTBAPlanificar();
int calcularDTBAPlanificarConRR();
int calcularDTBAPlanificarConVRR();
int obtenerPrimerId(t_list* lista);
tp_DTB buscarDTBPorId(int idDTB);
//int enviarDTBaCPU(t_DTB * dtb, int sockCPU);
int calcularDTBAPlanificarConBOAF();
void pasarSafaOperativo();
bool recursoEstaEnTabla(char* rec);
bool recursoEstaAsignado(char* rec);
void agregarGdtAColaRecurso(tp_tipoRecurso recurso);
//void captura_sigpipe(int signo);
//void configurar_signals(void);
int mostrarLista(t_list *lista);
tp_DTB buscar_DTB_Por_ID_en_Lista(t_list *lista, int idDTB);
tp_DTB buscar_DTB_Por_ID(int idDTB, int* colaDondeEsta);
void mostrarDTB(tp_DTB mostrado);
void mostrarTablaArchivosAbiertos(t_list* lista);
void finalizarDTB(t_list* lista, int idDTB);
bool tieneEquipoGrande(tp_DTB miDTB);
int obtenerPrimerIdConEG(t_list* lista);
bool noEstaEnLista(t_list* lista, tp_DTB unDTB);

#endif /* SAFA_FUNCIONESSAFA_H_ */
