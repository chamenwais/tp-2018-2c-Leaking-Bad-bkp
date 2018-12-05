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

void solicitar_a_FM9_la_sentencia(t_DTB * dtb){
	enviarCabecera(serverMEM, PedirLineaParaEjecutar, sizeof(PedirLineaParaEjecutar));
	prot_enviar_CPU_FM9_pedir_linea(dtb->escriptorio, dtb->id_GDT, dtb->program_counter, serverMEM);
	logger_CPU(escribir_loguear, l_info,"Le he pedido a FM9 la sentencia que debo ejecutar...\n");
}

tp_lineaCPU * recibir_de_FM9_linea_a_parsear(t_DTB * dtb){
	t_cabecera respuesta_de_FM9 = recibirCabecera(serverMEM);
	tp_lineaCPU linea_pedida;

	if(respuesta_de_FM9.tipoDeMensaje == NoHuboProblemaConLaLineaParaCpu){
		linea_pedida = prot_recibir_CPU_FM9_pedir_linea(serverMEM);
		logger_CPU(escribir_loguear, l_info,"Sentencia recibida.\n");
	}else{
		logger_CPU(escribir_loguear, l_error, "Hubo un problema con FM9\n");
		abortar(dtb);
		finalizar_cpu();
	}

	return linea_pedida;
}

void abortar(t_DTB * dtb){

}

void actualizar_program_counter(t_DTB * dtb, int pc){
	pc++;
	dtb->program_counter = pc;
	logger_CPU(escribir_loguear, l_info,"Actualizado el program counter del dtb.\n");
}

void abrir_nuevo_archivo_para_escriptorio(){
//ya se puede hacer
}

void correr_una_unidad_de_tiempo_del_quantum(){
	sleep(RETARDO);
}

void asignar_datos_a_linea(){

}

void retener_recurso(){

}

void liberar_recurso(){

}

void guardar_contenido_de_fm9_en_mdj(){

}

void liberar_archivo_abierto(){

}

void crear_nuevo_archivo_en_mdj(){

}

void eliminar_archivo_de_mdj(){

}

void realizar_la_operacion_que_corresponda_segun(t_operacion resultado_del_parseado){
	switch(resultado_del_parseado.tipo_de_operacion){
		case ABRIR:
			abrir_nuevo_archivo_para_escriptorio();
			break;
		case CONCENTRAR:
			correr_una_unidad_de_tiempo_del_quantum();
			break;
		case ASIGNAR:
			asignar_datos_a_linea();
			break;
		case WAIT:
			retener_recurso();
			break;
		case SIGNAL:
			liberar_recurso();
			break;
		case FLUSH:
			guardar_contenido_de_fm9_en_mdj();
			break;
		case CLOSE:
			liberar_archivo_abierto();
			break;
		case CREAR:
			crear_nuevo_archivo_en_mdj();
			break;
		case BORRAR:
			eliminar_archivo_de_mdj();
			break;
	}
}


void proceder_con_lectura_escriptorio(t_DTB * dtb){
	int unidad_de_tiempo = dtb->quantum;

	for(int i=0;i++;i<unidad_de_tiempo){

		solicitar_a_FM9_la_sentencia(dtb);
		tp_lineaCPU paquete_linea = recibir_de_FM9_linea_a_parsear(dtb);
		actualizar_program_counter(dtb, paquete_linea->pc);
		t_operacion resultado_del_parseado = parsear(paquete_linea->linea);
		realizar_la_operacion_que_corresponda_segun(resultado_del_parseado);
		sleep(RETARDO);

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
