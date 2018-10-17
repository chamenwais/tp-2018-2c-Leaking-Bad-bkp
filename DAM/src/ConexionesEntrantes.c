/*
 * ConexionesEntrantes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "ConexionesEntrantes.h"

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_safa){
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
		for(iterador_conexiones_existentes=0
				; iterador_conexiones_existentes<=maximo_sockfd
				; iterador_conexiones_existentes++){
			if(FD_ISSET(iterador_conexiones_existentes, &lista_fd_temporal)){
				//Recibimos una conexion
				if (iterador_conexiones_existentes==socketfd_escucha){
					//Nueva conexion
					nuevo_socketfd=aceptarConexion(socketfd_escucha);
					validar_comunicacion(socketfd_escucha, const_name_cpu);
					FD_SET(nuevo_socketfd, &lista_fd_maestra);
					establecer_nuevo_fd_maximo(&maximo_sockfd,nuevo_socketfd);
					loguear_nueva_conexion_con_CPU(nuevo_socketfd);
					realizar_handshake_con_cpu(nuevo_socketfd);
				} else {
					//Conexion conocida
					t_cabecera cabecera=recibirCabecera(iterador_conexiones_existentes);
					if (cabecera_esta_vacia(&cabecera)) {
						//No se puede leer el mensaje, se va a cerrar el sockfd
						loguear_y_cerrar_comunicacion_erronea_con_CPU(
								iterador_conexiones_existentes);
						FD_CLR(iterador_conexiones_existentes, &lista_fd_maestra);
					}
					if(cabecera_correcta(&cabecera)){
						loguear_tamanio_cabecera_recibida_de_CPU(&cabecera);
						clasificar_y_crear_hilo_correspondiente_a_pedido_CPU(
								cabecera.tipoDeMensaje
								,iterador_conexiones_existentes
								,socket_fm9
								,socket_safa);
					}
				}
			}
		}
	}
}

void clasificar_y_crear_hilo_correspondiente_a_pedido_CPU(
		enum MENSAJES mensaje_entrante
		, int socket_CPU_solicitante
		, int socket_fm9
		, int socket_safa){
	switch(mensaje_entrante){
		case AbrirPathParaProceso:;
			//TODO pedir path y pid
			char * path="";
			int pid=0;
			logger_DAM(escribir_loguear, l_trace,"Ehhh, voy a buscar [%s] para [%d]",path,pid);
			//TODO crear hilo que maneje el abrir
			break;
		case GuardarArchivoEnDisco:
			break;
		case CrearLineasEnArchivo:
			break;
		case EliminarArchivoDeDisco:
			break;
		default:
			break;
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

void tratar_error_select(int socketfd_escucha) {
	logger_DAM(escribir_loguear, l_error, "Error en el select/multiplexor ");
	cerrar_socket_y_terminar(socketfd_escucha);
}

void loguear_nueva_conexion_con_CPU(int nuevo_socketfd) {
	logger_DAM(escribir_loguear, l_trace, "Se recibio un CPU en el socket %d",
			nuevo_socketfd);
}

void loguear_y_cerrar_comunicacion_erronea_con_CPU(
		int sockfd_CPU_a_cerrar) {
	//No se puede leer el mensaje, se va a cerrar el sockfd
	logger_DAM(escribir_loguear, l_trace,
			"No se pudo obtener info del CPU. Se cierra el sockfd %d",
			sockfd_CPU_a_cerrar);
	close(sockfd_CPU_a_cerrar);
}

int cabecera_esta_vacia(t_cabecera* cabecera) {
	return sizeof(*cabecera) == 0;
}

int cabecera_correcta(t_cabecera* cabecera){
	return cabecera->tamanio>0;
}

void loguear_tamanio_cabecera_recibida_de_CPU(t_cabecera* cabecera_de_CPU) {
	logger_DAM(escribir_loguear, l_trace,
			"Se recibio una cabecera con tamanio %d", cabecera_de_CPU->tamanio);
}
