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
#include <commons/collections/list.h>

enum PROCESO {
	FS,
	MEMORIA,
	DMA,
	CPU,
	PLANIFICADOR
};

enum MENSAJES {
	GuardarDatos,
	ValidarArchivo,
	CrearArchivo,
	ObtenerDatos,
	BorrarArchivo,
	ElArchivoNoExiste,
	ElArchivoExiste,
	ArchivoCreado,
	ArchivoNoCreado,
	DatosGuardados,
	ArchivoNoEncontrado,
	DatosObtenidos,
	AbrirPathParaProceso,
	AbrirPathEjecutandose,
	AbrirPathNoFinalizado,
	AbrirPathFinalizadoOk,
	FlushDeArchivoADisco,
	FlushDeArchivoADiscoEjecutandose,
	FlushDeArchivoADiscoNoFinalizado,
	FlushDeArchivoADiscoFinalizadoOk,
	CrearLineasEnArchivo,
	CrearLineasEnArchivoEjecutandose,
	CrearLineasEnArchivoNoFinalizado,
	CrearLineasEnArchivoFinalizadoOk,
	EliminarArchivoDeDisco,
	EliminarArchivoDeDiscoEjecutandose,
	EliminarArchivoDeDiscoNoFinalizado,
	EliminarArchivoDeDiscoFinalizadoOk,
	CargarParteEnMemoria,
	ArchivoBorrado,
	ArchivoNoBorrado,
	FinDeQuantum,
	BloquearDTB,
	NoHayMasBloquesLibres,
	FinalizarTrabajoConElFS,
	PedirLineaParaEjecutar,
	NoHuboProblemaConLaLineaParaCpu,
	ErrorSegmentoPagina,
	AsignarDatosALinea,
	AsignarDatosALineaEjecutandose,
	ArchivoNoAbierto,
	AbortarDTB,
	RetenerRecurso,
	LiberarRecurso,
	RespuestaASolicitudDeRecursoAfirmativa,
	RespuestaASolicitudDeRecursoDenegada,
	LiberarArchivoAbierto,
	LiberarArchivoAbiertoEjecutandose,
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
/* Funcionamiento de enviar y recibir handshake:
 * El cliente hace un enviar handshake al server y el server del otro lado
 * hace un recibir handshake, no hace falta hacer nada mas, por parametro le
 * pasamos los procesos que intervienen y las mismas funciones chequean que sean
 * los procesos que corresponden los que se estan comunicando
 * */
int enviarHandshake(enum PROCESO emisor,enum PROCESO procesoEsperado,int sock);
int recibirHandshake(enum PROCESO receptor, enum PROCESO procesoEsperado, int sock);
int enviarCabecera(int sock, enum MENSAJES tipoDeMensaje,int tamanio);
t_cabecera recibirCabecera(int sock);

#endif /* COM_H_ */
