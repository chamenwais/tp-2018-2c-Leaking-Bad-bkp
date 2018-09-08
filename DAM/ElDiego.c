/*
 * ElDiego.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */

#include "ElDiego.h"

int main() {
	configurar_logger();
	levantar_configuracion();
	log_info(logger, "El puerto de escucha va a ser: %d",puerto_de_escucha);
	terminar_controladamente(0);
}
