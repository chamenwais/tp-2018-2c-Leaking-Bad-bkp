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

void* list_remove_by_condition_comparing(t_list *self, bool(*condition_equal_to)(void*, int), int to_compare) {
	int index = 0;

	t_link_element* element = list_find_element_comparing(self, condition_equal_to, &index, to_compare);
	if (element != NULL) {
		return list_remove(self, index);
	}

	return NULL;
}

t_link_element* list_find_element_comparing(t_list *self, bool(*condition_equal_to)(void*, int), int* index, int to_compare) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition_equal_to(element->data, to_compare)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}
