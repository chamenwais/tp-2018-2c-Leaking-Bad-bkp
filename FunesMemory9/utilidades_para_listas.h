/*
 * utilidades_para_listas.h
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */

#ifndef UTILIDADES_PARA_LISTAS_H_
#define UTILIDADES_PARA_LISTAS_H_

#include <commons/collections/list.h>	// Para manejo de listas

/***Funciones***/
t_list* list_filter_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);
int list_count_satisfying_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);
bool list_any_satisfy_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare);

#endif /* UTILIDADES_PARA_LISTAS_H_ */
