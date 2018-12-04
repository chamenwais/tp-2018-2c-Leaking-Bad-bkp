/*
 * CPUmain.c
 *
 *  Created on: 4 sep. 2018
 *      Author: utnso
 */

#include "CPUmain.h"

struct addrinfo* crear_addrinfo(char * ip, char * puerto){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(ip, puerto, &hints, &serverInfo);

	return serverInfo;
}

int conectar_SAFA(char * ip, char * puerto){

	struct addrinfo *serverInfoSAFA = crear_addrinfo(IP_SAFA, PUERTO_SAFA);

	int serverSAFA = socket(serverInfoSAFA->ai_family, serverInfoSAFA->ai_socktype, serverInfoSAFA->ai_protocol);

	if (serverSAFA < 0){
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al SAFA\n");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverSAFA, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_SAFA = connect(serverSAFA, serverInfoSAFA->ai_addr, serverInfoSAFA->ai_addrlen);

	if (resultado_conexion_SAFA < 0){
		freeaddrinfo(serverInfoSAFA);
		close(serverSAFA);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al SAFA\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoSAFA);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor SAFA\n");

	return serverSAFA;

}

int conectar_DIEGO(char * ip, char * puerto){

	struct addrinfo *serverInfoDIEGO = crear_addrinfo(IP_DIEGO, PUERTO_DIEGO);

	int serverDIEGO = socket(serverInfoDIEGO->ai_family, serverInfoDIEGO->ai_socktype, serverInfoDIEGO->ai_protocol);

	if (serverDIEGO < 0){
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al DIEGO\n");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverDIEGO, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_DIEGO = connect(serverDIEGO, serverInfoDIEGO->ai_addr, serverInfoDIEGO->ai_addrlen);

	if (resultado_conexion_DIEGO < 0){
		freeaddrinfo(serverInfoDIEGO);
		close(serverDIEGO);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al DIEGO\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoDIEGO);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor DIEGO\n");

	return serverDIEGO;

}

int conectar_MEM(char * ip, char * puerto){

	struct addrinfo *serverInfoMEM = crear_addrinfo(IP_MEM, PUERTO_MEM);

	int serverMEM = socket(serverInfoMEM->ai_family, serverInfoMEM->ai_socktype, serverInfoMEM->ai_protocol);

	if (serverMEM < 0){
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al FM9\n");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverMEM, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_MEM = connect(serverMEM, serverInfoMEM->ai_addr, serverInfoMEM->ai_addrlen);

	if (resultado_conexion_MEM < 0){
		freeaddrinfo(serverInfoMEM);
		close(serverMEM);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al FM9\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoMEM);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor FM9\n");

	return serverMEM;

}

void realizar_handshakes(){

	if(enviarHandshake(CPU, PLANIFICADOR, serverSAFA)){
		logger_CPU(escribir_loguear, l_info,"Handshake exitoso con SAFA\n");
	}else{
		logger_CPU(escribir_loguear, l_error,"No pudo realizarse el handshake con SAFA\n");
		exit(EXIT_FAILURE);
	}

	if(enviarHandshake(CPU, DMA, serverDIEGO)){
		logger_CPU(escribir_loguear, l_info,"Handshake exitoso con DAM\n");
	}else{
		logger_CPU(escribir_loguear, l_error,"No pudo realizarse el handshake con DAM\n");
		exit(EXIT_FAILURE);
	}
}

void solicitar_a_DAM_busqueda_dummy(t_DTB * dtb){
	enviarCabecera(serverDIEGO, AbrirPathParaProceso, sizeof(AbrirPathParaProceso));
	prot_enviar_CPU_DMA_abrirPath(dtb->escriptorio, dtb->id_GDT, serverDIEGO);
	logger_CPU(escribir_loguear, l_info,"Se le envio a DAM la informacion necesaria\n");
}

void desalojar_dtb(t_DTB * dtb){
	enviarCabecera(serverSAFA, BloquearDTB, sizeof(BloquearDTB));

	int id = dtb->id_GDT;
	enviar(serverSAFA, id, sizeof(id));
	logger_CPU(escribir_loguear, l_info,"Se informo a SAFA que bloquee al DTB y su respectivo ID\n");
}

void iniciar_operacion_dummy(t_DTB * dtb){
	logger_CPU(escribir_loguear, l_info,"Voy a solicitar a DAM que busque en el MDJ el Escriptorio indicado en el DTB\n");
	solicitar_a_DAM_busqueda_dummy(dtb);

	t_cabecera respuesta_del_diego = recibirCabecera(serverDIEGO);

	if(respuesta_del_diego.tipoDeMensaje == AbrirPathEjecutandose){
		logger_CPU(escribir_loguear, l_info,"El DTB dummy sera desalojado.\n");
		desalojar_dtb(dtb);
	}
}

void abrir_script(char *path){
	if(path != NULL){
		archivo_a_leer_por_el_CPU = fopen(path, "r");
	}

	if(archivo_a_leer_por_el_CPU == NULL){
		logger_CPU(escribir_loguear, l_error,"Error al intentar abrir el archivo a leer.\n");
		exit(EXIT_FAILURE);
	}
}

void parsear_escriptorio(char * linea_del_archivo){


}

void proceder_con_lectura_escriptorio(t_DTB * dtb){
	int unidad_de_tiempo = dtb->quantum;
	char * path = dtb->escriptorio;

	abrir_script(path);

	char * linea_a_parsear = NULL;
	size_t direccion_de_la_linea_a_parsear = 0;
	ssize_t read = 0;

	while(!feof(archivo_a_leer_por_el_CPU)){
		read = getline(&linea_a_parsear, &direccion_de_la_linea_a_parsear, archivo_a_leer_por_el_CPU);

		if(read == -1){
			//Se llegó al fin del archivo,
			break;
		}

		/*
		 * Lo que está a continuación se me hace que está super MAL,
		 * pero no se me ocurre otra manera de hacerlo, no entendí
		 * bien lo del retardo. HELP WANTED HERE
		 */

		for(int i = 0; i++; i<unidad_de_tiempo){
			parsear_escriptorio(linea_a_parsear);
			sleep(RETARDO);
		}
	}
}

void recibir_dtb_y_delegar(t_DTB * dtb) {

	dtb = malloc(sizeof(t_DTB));
	recibir(serverSAFA, &dtb, sizeof(t_DTB));

	logger_CPU(escribir_loguear, l_info,"DTB recibido de SAFA\n");

	if(dtb->iniGDT == 0){
		logger_CPU(escribir_loguear, l_info,"Se procedera a realizar la operacion DUMMY \n");
		iniciar_operacion_dummy(dtb);
	}else{
		logger_CPU(escribir_loguear, l_info,"Se procedera a leer el escriptorio\n");
		proceder_con_lectura_escriptorio(dtb);
	}

	free(dtb);
}

int main(int argc, char **argv){
	t_DTB * dtb;

	inicializar_logger();
	//Obtengo los datos del archivo de configuracion
	cargar_archivo_de_config(argv[1]);
	configurar_signals();

	logger_CPU(escribir_loguear, l_info,"Iniciando conexiones con servidores...\n");
	serverSAFA = conectar_SAFA(IP_SAFA, PUERTO_SAFA);
	serverDIEGO = conectar_DIEGO(IP_DIEGO, PUERTO_DIEGO);

	logger_CPU(escribir_loguear, l_info,"Realizando handshakes...\n");
	realizar_handshakes();

	recibir_dtb_y_delegar(dtb);

	/*
	 * Hacer las funciones para salir sin memory  leaks
	 */


return 0;

}
