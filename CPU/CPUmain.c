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
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al SAFA");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverSAFA, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_SAFA = connect(serverSAFA, serverInfoSAFA->ai_addr, serverInfoSAFA->ai_addrlen);

	if (resultado_conexion_SAFA < 0){
		freeaddrinfo(serverInfoSAFA);
		close(serverSAFA);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al SAFA");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoSAFA);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor SAFA");

	return serverSAFA;

}

int conectar_DIEGO(char * ip, char * puerto){

	struct addrinfo *serverInfoDIEGO = crear_addrinfo(IP_DIEGO, PUERTO_DIEGO);

	int serverDIEGO = socket(serverInfoDIEGO->ai_family, serverInfoDIEGO->ai_socktype, serverInfoDIEGO->ai_protocol);

	if (serverDIEGO < 0){
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al DIEGO");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverDIEGO, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_DIEGO = connect(serverDIEGO, serverInfoDIEGO->ai_addr, serverInfoDIEGO->ai_addrlen);

	if (resultado_conexion_DIEGO < 0){
		freeaddrinfo(serverInfoDIEGO);
		close(serverDIEGO);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al DIEGO");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoDIEGO);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor DIEGO");

	return serverDIEGO;

}

int conectar_MEM(char * ip, char * puerto){

	struct addrinfo *serverInfoMEM = crear_addrinfo(IP_MEM, PUERTO_MEM);

	int serverMEM = socket(serverInfoMEM->ai_family, serverInfoMEM->ai_socktype, serverInfoMEM->ai_protocol);

	if (serverMEM < 0){
		logger_CPU(escribir_loguear, l_error,"Error al intertar conectarse al FM9");
		exit(EXIT_FAILURE);
	}

	int activado = 1;
	setsockopt(serverMEM, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	int resultado_conexion_MEM = connect(serverMEM, serverInfoMEM->ai_addr, serverInfoMEM->ai_addrlen);

	if (resultado_conexion_MEM < 0){
		freeaddrinfo(serverInfoMEM);
		close(serverMEM);
		logger_CPU(escribir_loguear, l_error,"Error al intentar conectar al FM9");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfoMEM);

	logger_CPU(escribir_loguear, l_info,"Conectado al servidor FM9");

	return serverMEM;

}

void realizar_handshakes(){

	if(enviarHandshake(CPU, PLANIFICADOR, serverSAFA)){
		logger_CPU(escribir_loguear, l_info,"Handshake exitoso con SAFA");
	}else{
		logger_CPU(escribir_loguear, l_error,"No pudo realizarse el handshake con SAFA");
		exit(EXIT_FAILURE);
	}

	if(enviarHandshake(CPU, DMA, serverDIEGO)){
		logger_CPU(escribir_loguear, l_info,"Handshake exitoso con DAM");
	}else{
		logger_CPU(escribir_loguear, l_error,"No pudo realizarse el handshake con DAM");
		exit(EXIT_FAILURE);
	}
}

void solicitar_a_DAM_busqueda_dummy(tp_DTB dtb){
	enviarCabecera(serverDIEGO, AbrirPathParaProceso, sizeof(AbrirPathParaProceso));
	prot_enviar_CPU_DMA_abrirPath(dtb->escriptorio, dtb->id_GDT, serverDIEGO);
	logger_CPU(escribir_loguear, l_info,"Se le envio a DAM la informacion necesaria");
}

void desalojar_dtb(tp_DTB dtb){
	enviarCabecera(serverSAFA, BloquearDTB, sizeof(BloquearDTB));
	prot_enviar_CPU_SAFA_bloquear_DTB(dtb->id_GDT, serverSAFA);
	logger_CPU(escribir_loguear, l_info,"Se informo a SAFA que bloquee al DTB y su respectivo ID");
}

void iniciar_operacion_dummy(tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Voy a solicitar a DAM que busque en el MDJ el Escriptorio indicado en el DTB para la operacion DUMMY");
	solicitar_a_DAM_busqueda_dummy(dtb);

	t_cabecera respuesta_del_diego = recibirCabecera(serverDIEGO);

	if(respuesta_del_diego.tipoDeMensaje == AbrirPathEjecutandose){
		logger_CPU(escribir_loguear, l_warning,"El DTB dummy sera desalojado");
		desalojar_dtb(dtb);
	}
}

void solicitar_a_FM9_la_sentencia(tp_DTB dtb){
	enviarCabecera(serverMEM, PedirLineaParaEjecutar, sizeof(PedirLineaParaEjecutar));
	prot_enviar_CPU_FM9_pedir_linea(dtb->escriptorio, dtb->id_GDT, dtb->program_counter, serverMEM);
	logger_CPU(escribir_loguear, l_trace,"Le he pedido a FM9 la sentencia que debo ejecutar...");
}

tp_lineaCPU * recibir_de_FM9_linea_a_parsear(tp_DTB dtb){
	t_cabecera respuesta_de_FM9 = recibirCabecera(serverMEM);
	tp_lineaCPU linea_pedida;

	if(respuesta_de_FM9.tipoDeMensaje == NoHuboProblemaConLaLineaParaCpu){
		linea_pedida = prot_recibir_CPU_FM9_pedir_linea(serverMEM);
		logger_CPU(escribir_loguear, l_info,"Sentencia recibida");
	}else{
		logger_CPU(escribir_loguear, l_error, "Hubo un problema con FM9");
		informar_a_safa_que_aborte_DTB(dtb);
		finalizar_cpu();
	}

	return linea_pedida;
}

void actualizar_program_counter(tp_DTB dtb, int pc){
	pc++;
	dtb->program_counter = pc;
	logger_CPU(escribir_loguear, l_info,"Actualizado el program counter del dtb");
}

bool tienen_el_mismo_nombre(void * arch1){
	return string_equals_ignore_case(path_archivo_para_comparar, (char*)arch1);
}

bool chequear_si_archivo_esta_abierto(tp_DTB dtb){
	return list_any_satisfy(dtb->tabla_dir_archivos, &tienen_el_mismo_nombre);
}

void abrir_nuevo_archivo_para_escriptorio(char * path, tp_DTB dtb){
	path_archivo_para_comparar = path;
	if(chequear_si_archivo_esta_abierto(dtb)){
		logger_CPU(escribir_loguear, l_warning,"El archivo ya se encuentra abierto.");
	}else{
		logger_CPU(escribir_loguear, l_trace,"Voy a solicitar a DAM que busque en el MDJ el Escriptorio indicado en el DTB");
		enviarCabecera(serverDIEGO, AbrirPathParaProceso, sizeof(AbrirPathParaProceso));
		prot_enviar_CPU_DMA_abrirPath(path, dtb->id_GDT, serverDIEGO);
		logger_CPU(escribir_loguear, l_info,"Se le envio a DAM la informacion necesaria");

		t_cabecera respuesta_del_diego = recibirCabecera(serverDIEGO);

		if(respuesta_del_diego.tipoDeMensaje == AbrirPathEjecutandose){
			logger_CPU(escribir_loguear, l_warning,"El DTB dummy sera desalojado");
			desalojar_dtb(dtb);
		}
	}
}

void correr_una_unidad_de_tiempo_del_quantum(){
	sleep((int)RETARDO);
}

void informar_a_safa_que_aborte_DTB(tp_DTB dtb){
	logger_CPU(escribir_loguear, l_warning,"Se le informara a SAFA que aborte el DTB con id: %d", dtb->id_GDT);
	enviarCabecera(serverSAFA, AbortarDTB, sizeof(AbortarDTB));
	prot_enviar_CPU_SAFA_abortar_DTB(dtb->id_GDT, serverSAFA);
}

void asignar_datos_a_linea(char * datos, char * linea, char * path, tp_DTB dtb){
	path_archivo_para_comparar = path;
	if(chequear_si_archivo_esta_abierto(dtb)){
		logger_CPU(escribir_loguear, l_info,"Voy a comunicarme con FM9 para asignar datos a linea.");
		enviarCabecera(serverMEM, AsignarDatosALinea, sizeof(AsignarDatosALinea));
		prot_enviar_CPU_FM9_asignar_datos_linea(datos, linea, path, dtb->id_GDT, serverMEM);
		logger_CPU(escribir_loguear, l_info,"Se le envio a FM9 la informacion necesaria");

		t_cabecera respuesta_de_fm9 = recibirCabecera(serverMEM);

		if(respuesta_de_fm9.tipoDeMensaje == AsignarDatosALineaEjecutandose){
			logger_CPU(escribir_loguear, l_trace,"FM9 puso manos a la obra!");
		}
	}else{
		logger_CPU(escribir_loguear, l_error,"20001\n");
		informar_a_safa_que_aborte_DTB(dtb);
	}
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

void realizar_la_operacion_que_corresponda_segun(t_operacion resultado_del_parseado, tp_DTB dtb){
	switch(resultado_del_parseado.tipo_de_operacion){
		case ABRIR:
			abrir_nuevo_archivo_para_escriptorio(resultado_del_parseado.parametros.abrir.path, dtb);
			break;
		case CONCENTRAR:
			correr_una_unidad_de_tiempo_del_quantum();
			break;
		case ASIGNAR:
			asignar_datos_a_linea(resultado_del_parseado.parametros.asignar.datos, resultado_del_parseado.parametros.asignar.linea,
					resultado_del_parseado.parametros.asignar.path, dtb);
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


void proceder_con_lectura_escriptorio(tp_DTB dtb){
	int unidad_de_tiempo = dtb->quantum;

	for(int i=0;i++;i<unidad_de_tiempo){

		solicitar_a_FM9_la_sentencia(dtb);
		tp_lineaCPU paquete_linea = recibir_de_FM9_linea_a_parsear(dtb);
		actualizar_program_counter(dtb, paquete_linea->pc);
		t_operacion resultado_del_parseado = parsear(paquete_linea->linea);
		realizar_la_operacion_que_corresponda_segun(resultado_del_parseado, dtb);
		sleep((int)RETARDO);

	}

}

void recibir_dtb_y_delegar(tp_DTB dtb) {

	dtb = prot_recibir_SAFA_CPU_DTB(serverSAFA);

	logger_CPU(escribir_loguear, l_info,"DTB recibido de SAFA");

	if(dtb->iniGDT == 0){
		logger_CPU(escribir_loguear, l_trace,"Se procedera a realizar la operacion DUMMY");
		iniciar_operacion_dummy(dtb);
	}else{
		logger_CPU(escribir_loguear, l_trace,"Se procedera con la ejecucion de sentencias");
		proceder_con_lectura_escriptorio(dtb);
	}

	free(dtb);
}

int main(int argc, char **argv){
	tp_DTB * dtb;

	inicializar_logger();
	//Obtengo los datos del archivo de configuracion
	cargar_archivo_de_config(argv[1]);
	configurar_signals();

	logger_CPU(escribir_loguear, l_trace,"Iniciando conexiones con servidores...");
	serverSAFA = conectar_SAFA(IP_SAFA, PUERTO_SAFA);
	serverDIEGO = conectar_DIEGO(IP_DIEGO, PUERTO_DIEGO);
	serverMEM = conectar_MEM(IP_MEM, PUERTO_MEM);

	logger_CPU(escribir_loguear, l_trace,"Realizando handshakes...");
	realizar_handshakes();

	recibir_dtb_y_delegar(dtb);

	/*
	 * Hacer las funciones para salir sin memory  leaks
	 */


return 0;

}
