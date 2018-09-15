/*
 * com.h
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#ifndef COM_H_
#define COM_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


enum PROCESO {
	FS,
	MEMORIA,
	DMA,
	CPU,
	PLANIFICADOR
};

enum MENSAJES {
	ValidarArchivo,
	CrearArchivo,
	ObtenerDatos,
	GuardarDatos,
	FinalizarTrabajoConElFS,
	MemoriaDown,
	MemoriaUp
};

typedef struct {
	enum MENSAJES tipoDeMensaje;
	int tamanio;
}__attribute__((packed)) t_cabecera;

int escucharEn(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int escucha);
int enviar(int sock, void *mensaje, int tamanio);
int recibir(int sock, void *mensaje, int tamanio);
void cerrarConexion(int sock);
int enviarHandshake(enum PROCESO emisor,enum PROCESO procesoEsperado,int sock);
int recibirHandshake(enum PROCESO receptor, enum PROCESO procesoEsperado, int sock);
int enviarCabecera(int sock, enum MENSAJES tipoDeMensaje,int tamanio);
t_cabecera recibirCabecera(int sock);

#endif /* COM_H_ */