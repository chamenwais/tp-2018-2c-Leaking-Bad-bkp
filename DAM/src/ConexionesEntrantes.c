/*
 * ConexionesEntrantes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "ConexionesEntrantes.h"

void tratar_error_select(int socketfd_escucha) {
	logger_DAM(escribir_loguear, l_error, "Error en el select/multiplexor ");
	cerrar_socket_y_terminar(socketfd_escucha);
}

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_mdj, int socket_safa){
	fd_set lista_fd_maestra;
	fd_set lista_fd_temporal;
	int maximo_sockfd;
	int socketfd_escucha;
	int nuevo_socketfd;
	int iterador_conexiones_existentes;

	// Limpiamos la lista de file descriptors maestra y temporal
	FD_ZERO(&lista_fd_maestra);
	FD_ZERO(&lista_fd_temporal);

	socketfd_escucha=escuchar_a_los_CPU();

	//Agregamos el socket de escucha a la lista maestra
	FD_SET(socketfd_escucha, &lista_fd_maestra);

	//Se establece el numero de socket maximo
	maximo_sockfd=socketfd_escucha;

	while(true){
		//Backup de la lista de file descriptors maestra
		lista_fd_temporal=lista_fd_maestra;

		if(select(maximo_sockfd+1,&lista_fd_temporal, NULL, NULL, NULL)==-1){
			tratar_error_select(socketfd_escucha);
		}

		//Chequeamos en las conexiones abiertas si hay algo para leer
		for(iterador_conexiones_existentes=0; iterador_conexiones_existentes<=maximo_sockfd; iterador_conexiones_existentes++){
			if(FD_ISSET(iterador_conexiones_existentes, &lista_fd_temporal)){
				//Recibimos una conexion
				if (iterador_conexiones_existentes==socketfd_escucha){
					//Nueva conexion
					nuevo_socketfd=aceptarConexion(socketfd_escucha);
					validar_comunicacion(socketfd_escucha, const_name_cpu);
					FD_SET(nuevo_socketfd, &lista_fd_maestra);
					establecer_nuevo_fd_maximo(&maximo_sockfd,nuevo_socketfd);
					logger_DAM(escribir_loguear, l_trace,
								"Se recibio un CPU en el socket %d", nuevo_socketfd);
					realizar_handshake_con_cpu(nuevo_socketfd);
				} else {
					//Conexion preexistente
					t_cabecera cabecera=recibirCabecera(iterador_conexiones_existentes);
					if(sizeof(cabecera)==0){
						//No se puede leer el mensaje, se va a cerrar el sockfd
						logger_DAM(escribir_loguear, l_trace,
									"No se pudo obtener info del CPU. Se cierra el sockfd %d", iterador_conexiones_existentes);
						close(iterador_conexiones_existentes);
						FD_CLR(iterador_conexiones_existentes, &lista_fd_maestra);
					}
					//TODO tratar el caso en que un CPU nos pidio algo (cabecera.tipoDeMensaje==algo)
					logger_DAM(escribir_loguear, l_trace,
														"¡Tratar el caso en que un CPU nos pidio algo!");
				}
			}
		}
	}
}

void establecer_nuevo_fd_maximo(int * maximo_actual, int potencial_nuevo_maximo){
	if (potencial_nuevo_maximo>*maximo_actual){
		*maximo_actual=potencial_nuevo_maximo;
	}
}

int escuchar_a_los_CPU() {
	logger_DAM(escribir_loguear, l_info,
			"Se va a escuchar a los CPU en el puerto %d", puerto_de_escucha);
	int socket_escucha = escucharEn(puerto_de_escucha);
	return socket_escucha;
}

void realizar_handshake_con_cpu(int socket_id){
	recibir_handshake_de(socket_id, CPU, const_name_cpu);
}
