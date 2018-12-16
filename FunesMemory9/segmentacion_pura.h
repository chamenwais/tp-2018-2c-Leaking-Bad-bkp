/*
 * segmentacion_pura.h
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#ifndef SEGMENTACION_PURA_H_
#define SEGMENTACION_PURA_H_

#include "contexto.h"
#include "utilidades_fm9.h"
#include "utilidades_para_listas.h"

struct entrada_tabla_segmentos{
	int base;
	int limite;
	char * archivo;
};
typedef struct entrada_tabla_segmentos t_entrada_tabla_segmentos;

struct hueco{
	int base;
	int limite;
};
typedef struct hueco t_hueco;

extern t_list * lista_de_huecos;

void inicializar_lista_de_huecos();
bool es_un_proceso_conocido(void * tabla_segmentos);
bool tiene_tabla_de_segmentos(void * tabla_segmentos, int pid);
int agregar_entrada_tabla_segmentos(tp_cargarEnMemoria nombre_archivo, t_list* entradas_segmentos, int nueva_base, int nuevo_limite);
int el_proceso_tiene_tabla_de_segmentos();
int agregar_nueva_tabla_segmentos_para_proceso(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite);
void crear_estructuras_esquema_segmentacion();
void cargar_parte_archivo_en_segmento(int DAM_fd);
void destruir_estructuras_esquema_segmentacion();
void eliminar_lista_de_entradas(void * tabla_segmentos);
t_hueco* tomar_hueco_con_limite_mas_grande();
bool archivo_mas_grande_que_hueco(int tamanio_en_lineas_archivo,const t_hueco* hueco);
bool archivo_igual_al_hueco(int tamanio_archivo_en_linas,const t_hueco* hueco_usado);
t_hueco* tomar_hueco();
void actualizar_info_tabla_de_huecos(int tamanio_archivo_en_memoria, t_hueco* hueco);
int actualizar_tabla_segmentos(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite);
void copiar_archivo_a_memoria_fisica(size_t tamanio_archivo_en_memoria, t_hueco* hueco, char* archivo_separado_en_lineas);
int crear_nueva_entrada_tabla_de_segmentos(tp_cargarEnMemoria parte_archivo, int nueva_base, int nuevo_limite);
void buscar_informacion_administrativa_esquema_segmentacion_y_mem_real(int id);
void obtener_parte_archivo_con_segmentacion(int DAM_fd);
t_archivo_devolviendose* filtrar_info_archivo_devolviendose(int id_dtb);
bool trozo_faltante_es_menor_que_transfersize(t_archivo_devolviendose* info_trozo_archivo, int transfer_size);
int obtener_tamanio_trozo_faltante(int tamanio_archivo_en_memoria, int bytes_transferidos);
void devolver_trozo_faltante_archivo(tp_obtenerArchivo pedido_obtencion, int DAM_fd,
		t_archivo_devolviendose* info_archivo_devolviendose);
void devolver_trozo_con_tamanio_transfersize(tp_obtenerArchivo pedido_obtencion,
		int DAM_fd, t_archivo_devolviendose* info_archivo_devolviendose);
void informar_archivo_no_abierto(int DAM_fd);
t_tabla_segmentos* buscar_tabla_de_segmentos(int pid);
t_archivo_devolviendose* crear_elemento_en_archivos_devolviendose(t_entrada_tabla_segmentos* segmento_archivo, int pid);
void darle_una_linea_al_cpu_segmentacion_pura(int sock);
void asignar_datos_a_linea_segmentacion_pura(int sock);
void liberar_archivo_abierto_segmentacion_pura(int sock);

#endif /* SEGMENTACION_PURA_H_ */
