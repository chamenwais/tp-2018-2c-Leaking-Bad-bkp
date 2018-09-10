/*
 * cliente_dummy.c
 *
 *  Created on: 10 sep. 2018
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

struct addrinfo* crear_addrinfo(char * ip, char * puerto){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(ip, puerto, &hints, &serverInfo);

	return serverInfo;
}

int conectar_fm9(char * ip, char * puerto){

	struct addrinfo *serverInfo = crear_addrinfo(ip, puerto);

	int serverfm9 = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	if (serverfm9 < 0){
		printf("Error al intentar conectar al fm9\n");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverfm9, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion = connect(serverfm9, serverInfo->ai_addr, serverInfo->ai_addrlen);

	if (resultado_conexion < 0){
		freeaddrinfo(serverInfo);
		close(serverfm9);
		printf("Error al intentar conectar al fm9\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfo);

	printf("Conectado al servidor fm9: %d \n",resultado_conexion);

	return serverfm9;
}

int main(){
	int server = conectar_fm9("0.0.0.0", "8003");
	sleep(6);
	printf("\nChau, ya viste que me puedo conectar \n");
	return(0);
}
