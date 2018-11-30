/*
 * utilidades_para_listas.c
 *
 *  Created on: 28 nov. 2018
 *      Author: utnso
 */
#include "utilidades_para_listas.h"

t_list* list_filter_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	t_list* filtered = list_create();

	void _add_if_apply(void* element) {
		if (condition_equal_to(element, to_compare)) {
			list_add(filtered, element);
		}
	}

	list_iterate(self, _add_if_apply);
	return filtered;
}

int list_count_satisfying_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	t_list *satisfying = list_filter_comparing(self, condition_equal_to, to_compare);
	int result = satisfying->elements_count;
	list_destroy(satisfying);
	return result;
}

bool list_any_satisfy_comparing(t_list* self, bool(*condition_equal_to)(void*, int), int to_compare){
	return list_count_satisfying_comparing(self, condition_equal_to, to_compare) > 0;
}

