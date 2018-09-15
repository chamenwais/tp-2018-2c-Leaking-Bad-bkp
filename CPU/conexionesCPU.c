/*
 * conexionesCPU.c
 *
 *  Created on: Sep 15, 2018
 *      Author: utnso
 */


#include "conexionesCPU.h"






int connect_to_server(char * ip, int * puerto) {
	  struct addrinfo hints;
	  struct addrinfo *server_info;

	  memset(&hints, 0, sizeof(hints));
	  hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

	  getaddrinfo(ip, puerto, &hints, &server_info);  // Carga en server_info los datos de la conexion

	  // 2. Creemos el socket con el nombre "server_socket" usando la "server_info" que creamos anteriormente
	  int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	  // 3. Conectemosnos al server a traves del socket!
	  int res = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

	  freeaddrinfo(server_info);  // No lo necesitamos mas

	  /*
	    3.1 Recuerden chequear por si no se pudo contectar.
	        Si hubo un error, lo loggeamos y podemos terminar el programa con la funcioncita
	        exit_gracefully pasandole 1 como parametro para indicar error ;).
	        Pss, revisen los niveles de log de las commons.
	  */
	  if (res < 0) {
	    _exit_with_error(server_socket, "No me pude conectar al servidor", NULL);
	  }
	  // 4 Logeamos que pudimos conectar y retornamos el socket
	  log_info(logger, "Conectado!");

return server_socket;
}


void _exit_with_error(int socket, char* error_msg, void * buffer) {
  if (buffer != NULL) {
    free(buffer);
  }
  log_error(logger, error_msg);
  close(socket);
  exit_gracefully(1);
}


void exit_gracefully(int return_nr) {
  /*
    20.   Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion
  */
  log_destroy(logger);
  exit(return_nr);
}
