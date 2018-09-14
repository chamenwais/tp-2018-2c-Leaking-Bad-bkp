/*
 * protocolo.h
 *
 *  Created on: 13 sep. 2018
 *      Author: utnso
 */

#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"

void prot_enviar_DMA_FS_path(char* path,int sock);
char* prot_recibir_DMA_FS_path(int sock);

#endif /* LQVG_PROTOCOLO_H_ */
