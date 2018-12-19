/*
 * protocolo.h
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */

#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"

typedef struct stru_obtenerDatos{
	char* path;
	long int offset;
	long int size;
	char* buffer;
} __attribute__((packed))
t_obtenerDatos;
typedef t_obtenerDatos* tp_obtenerDatos;

typedef struct stru_crearArchivo{
	char* path;
	int size;
} __attribute__((packed))
t_crearArchivo;
typedef t_crearArchivo* tp_crearArchivo;

typedef struct datos_Obtenidos{
	int tamanio_total_archivo;
	char* buffer;
	long int size;
} __attribute__((packed))
t_datosObtenidosDeProtocolo;
typedef t_datosObtenidosDeProtocolo* tp_datosObtenidosDeProtocolo;

typedef struct defDatosObtenidos {
	enum MENSAJES resultado;
	char * datos;
	long int size;
	long int tamanio_total_archivo;
} __attribute__((packed))
t_datosObtenidos;
typedef t_datosObtenidos* tp_datosObtenidos;

typedef struct stru_abrirPath{
	char* path;
	int pid;
} t_abrirPath;
typedef t_abrirPath* tp_abrirPath;

typedef struct stru_cargarEnMemoria{
	int pid;
	char* path;
	char* buffer;
	int offset;
	int transfer_size;
	int file_size;
} t_cargarEnMemoria;
typedef t_cargarEnMemoria* tp_cargarEnMemoria;

typedef struct stru_datosEnMemoria{
	char* path;
	int pid;
	int memory_address;
} t_datosEnMemoria;
typedef t_datosEnMemoria* tp_datosEnMemoria;

typedef struct stru_obtenerArchivo{
	char* path;
	int pid;
	int memory_address;
	int size;
} t_obtenerArchivo;
typedef t_obtenerArchivo* tp_obtenerArchivo;

typedef struct stru_pathPid{
	char* path;
	int pid;
} t_pathPid;
typedef t_pathPid* tp_pathPid;

typedef struct stru_lineaCPU{
	char* linea;
	int id_GTD;
	int pc;
} t_lineaCPU;
typedef t_lineaCPU* tp_lineaCPU;

typedef struct stru_lineaParaCPU{
	char* linea;
} t_lineaParaCPU;
typedef t_lineaParaCPU* tp_lineaParaCPU;

typedef struct stru_asigarDatos{
	int id_GDT;
	int pc;
	char * datos;
	char * linea;
	char * path;
} t_asignarDatosLinea;
typedef t_asignarDatosLinea* tp_asignarDatosLinea;

typedef struct defDTB {
	int id_GDT;
	char* escriptorio; // ver si esto realmente es un char*
	int program_counter;
	int iniGDT; // 0 o 1
	t_list* tabla_dir_archivos; //tabla_dir_archivos;
	int quantum;
} t_DTB;
typedef t_DTB* tp_DTB;

typedef struct stru_tipoRec{
	char * recurso;
	int id_GDT;
} t_tipoRecurso;
typedef t_tipoRecurso * tp_tipoRecurso;

typedef struct stru_liberarArchivo{
	char * path;
	int id_GDT;
} t_liberarArchivo;
typedef t_liberarArchivo * tp_liberarArchivo;

typedef struct stru_crearLineasArch{
	char * path;
	int cant_lineas;
	int id_GDT;
} t_crearLineasArch;
typedef t_crearLineasArch * tp_crearLineasArch;

typedef struct stru_eliminarArch{
	char * path;
	int id_GDT;
} t_eliminarArch;
typedef t_eliminarArch * tp_eliminarArch;


void prot_enviar_FS_DMA_datosObtenidos(char* datos, int resultado, int tamanioTotalDelArchivo, int sock);
tp_datosObtenidosDeProtocolo prot_recibir_FS_DMA_datosObtenidos(int sock);
void prot_enviar_DMA_FS_path(char* path,int sock);
char* prot_recibir_DMA_FS_path(int sock);
void prot_enviar_DMA_FS_guardarDatos(char *path, long int offset, long int size, char *buffer, int sock);
tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos(int sock);
void prot_enviar_DMA_FS_obtenerDatos(char *path, long int offset, long int size, int sock);
tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos(int sock);
void prot_enviar_CPU_DMA_abrirPath(char* path, int pid, int sock);
tp_abrirPath prot_recibir_CPU_DMA_abrirPath(int sock);
void prot_enviar_DMA_FM9_cargarEnMemoria(int pid, char* path, char* buffer, int offset, int transfer_size, int file_size, int sock);;
tp_cargarEnMemoria prot_recibir_DMA_FM9_cargarEnMemoria(int sock);
void prot_enviar_FM9_DMA_cargaEnMemoria(int memory_address, int sock);
int prot_recibir_FM9_DMA_cargaEnMemoria(int sock);
void prot_enviar_DMA_SAFA_datosEnMemoria(char* path, int pid, int memory_address, int sock);
tp_datosEnMemoria prot_recibir_DMA_SAFA_datosEnMemoria(int sock);
void prot_enviar_DMA_FS_CrearArchivo(char* path,int longitud,int sock);
tp_crearArchivo prot_recibir_DMA_FS_CrearArchivo(int sock);
void prot_enviar_CPU_DMA_flush(char* path, int pid, int memory_address, int sock);
tp_datosEnMemoria prot_recibir_CPU_DMA_flush(int sock);
void prot_enviar_DMA_FM9_obtenerArchivo(char* path, int pid, int memory_address, int size, int sock);
tp_obtenerArchivo prot_recibir_DMA_FM9_obtenerArchivo(int sock);
void prot_enviar_FM9_DMA_devolverDatos(char* datos, int tamanio_trozo, int tamanio_total_archivo, int sock);
tp_datosObtenidosDeProtocolo prot_recibir_FM9_DMA_devolverDatos(int sock);
void prot_enviar_DMA_SAFA_finFlush(char* path, int pid, int sock);
tp_pathPid prot_recibir_DMA_SAFA_finFlush(int sock);
void prot_enviar_CPU_FM9_pedir_linea(char * path, int id, int pc, int sock);
void prot_enviar_CPU_FM9_linea_pedida(char * linea, int sock);
tp_lineaCPU prot_recibir_CPU_FM9_pedir_linea(int sock);
void prot_enviar_SAFA_CPU_DTB(int id_GDT, int program_counter, int iniGDT, char* escriptorio, t_list* lista, int quantum, int sock);
tp_DTB prot_recibir_SAFA_CPU_DTB(int sock);
void prot_enviar_CPU_FM9_asignar_datos_linea(char * datos, char * linea, char * path, int id_GDT, int pc, int sock);
void prot_enviar_CPU_SAFA_abortar_DTB(int id_GDT, int sock);
tp_asignarDatosLinea prot_recibir_CPU_FM9_asignar_datos_linea(int sock);
void prot_enviar_CPU_SAFA_bloquear_DTB(int id_GDT, int sock);
tp_tipoRecurso prot_recibir_CPU_SAFA_retener_recurso(int sock);
void prot_enviar_CPU_SAFA_retener_recurso(char * recurso, int id_GDT, int sock);
void prot_enviar_CPU_SAFA_liberar_recurso(char * recurso, int id_GDT, int sock);
tp_tipoRecurso prot_recibir_CPU_SAFA_liberar_recurso(int sock);
void prot_enviar_CPU_FM9_liberar_archivo(char * path, int id_GDT, int sock);
tp_liberarArchivo prot_recibir_CPU_FM9_liberar_archivo(int sock);
void prot_enviar_CPU_DMA_crear_lineas_arch(char * path, int cant_lineas, int id_GDT, int sock);
tp_crearLineasArch prot_recibir_CPU_DMA_crear_lineas_arch(int sock);
void prot_enviar_CPU_DAM_eliminar_arch_de_disco(char * path, int id_GDT, int sock);
tp_eliminarArch prot_recibir_CPU_DAM_eliminar_arch_de_disco(int sock);
int prot_recibir_CPU_SAFA_bloquear_DTB(int sock);
int prot_recibir_CPU_SAFA_abortar_DTB(int sock);
void prot_enviar_DMA_SAFA_crearArchivo(int id_GDT, int sock);
int prot_recibir_DMA_SAFA_crearArchivo(int sock);
void prot_enviar_DMA_SAFA_eliminarArchivo(int id_GDT, int sock);
int prot_recibir_DMA_SAFA_eliminarArchivo(int sock);
void prot_enviar_FS_DMA_datosObtenidos_serializado(t_datosObtenidos datosObtenidos, int sock);
tp_datosObtenidos prot_recibir_FS_DMA_datosObtenidos_serializado(int sock);
void prot_enviar_DMA_FS_obtenerDatos_serializado(t_obtenerDatos datos, int sock);
tp_obtenerDatos prot_recibir_DMA_FS_obtenerDatos_serializado(int sock);
void prot_enviar_DMA_FS_guardarDatos_serializado(t_obtenerDatos datos, int sock);
tp_obtenerDatos prot_recibir_FS_DMA_guardarDatos_serializado(int sock);
void prot_enviar_DMA_FS_CrearArchivo_serializado(t_crearArchivo dataDelArchivo,int sock);
tp_crearArchivo prot_recibir_DMA_FS_CrearArchivo_serializado(int sock);
tp_lineaParaCPU prot_recibir_CPU_FM9_linea_pedida(int sock);

#endif /* LQVG_PROTOCOLO_H_ */
