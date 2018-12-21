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

void cerrar_script(tp_DTB dtb){
	char * script = dtb->escriptorio;
	logger_CPU(escribir_loguear, l_warning,"Fin de archivo, se cerrara el script y terminara el programa");
	enviar_FM9_solicitud_liberar_archivo(script, dtb);
	finalizar_cpu();
}

void solicitar_a_DAM_busqueda_dummy(tp_DTB dtb){
	enviarCabecera(serverDIEGO, AbrirPathParaProceso, sizeof(AbrirPathParaProceso));
	prot_enviar_CPU_DMA_abrirPath(dtb->escriptorio, dtb->id_GDT, serverDIEGO);
	logger_CPU(escribir_loguear, l_info,"Se le envio a DAM la informacion necesaria para abrir el escriptorio");
}

void desalojar_dtb(tp_DTB dtb){
	enviarCabecera(serverSAFA, BloquearDTB, sizeof(BloquearDTB));
	prot_enviar_CPU_SAFA_bloquear_DTB(dtb->id_GDT, serverSAFA);
	logger_CPU(escribir_loguear, l_info,"Se informo a SAFA que bloquee al DTB y su respectivo ID: %d",dtb->id_GDT);
}

void iniciar_operacion_dummy(tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Voy a solicitar a DAM que busque en el MDJ el Escriptorio %s\nindicado en el DTB %d para la operacion DUMMY"
			,dtb->escriptorio,dtb->id_GDT);
	solicitar_a_DAM_busqueda_dummy(dtb);

	t_cabecera respuesta_del_diego = recibirCabecera(serverDIEGO);

	if(respuesta_del_diego.tipoDeMensaje == AbrirPathEjecutandose){
		logger_CPU(escribir_loguear, l_warning,"El DTB dummy sera desalojado");
		desalojar_dtb(dtb);
	}
}

void solicitar_a_FM9_la_sentencia(tp_DTB dtb){
	enviarCabecera(serverMEM, PedirLineaParaEjecutar, sizeof(PedirLineaParaEjecutar));
	prot_enviar_CPU_FM9_pedir_linea(dtb->escriptorio, dtb->id_GDT, dtb->program_counter, serverMEM);//pc es numero de linea
	logger_CPU(escribir_loguear, l_trace,"Le he pedido a FM9 la sentencia que debo ejecutar...");
}

tp_lineaParaCPU * recibir_de_FM9_linea_a_parsear(tp_DTB dtb){
	t_cabecera respuesta_de_FM9 = recibirCabecera(serverMEM);
	tp_lineaParaCPU linea_pedida;

	if(respuesta_de_FM9.tipoDeMensaje == NoHuboProblemaConLaLineaParaCpu){
		linea_pedida = prot_recibir_CPU_FM9_linea_pedida(serverMEM);
		logger_CPU(escribir_loguear, l_info,"Sentencia recibida");
	}else{
		logger_CPU(escribir_loguear, l_error, "Hubo un problema con FM9");
		informar_a_safa_que_aborte_DTB(dtb);
		finalizar_cpu();
	}

	return linea_pedida;
}

void actualizar_program_counter(tp_DTB dtb){
	dtb->program_counter = dtb->program_counter+1;
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
		logger_CPU(escribir_loguear, l_trace,"Voy a solicitar a DAM que busque en el MDJ el archivo %s indicado en el DTB", path);
		enviarCabecera(serverDIEGO, AbrirPathParaProceso, sizeof(AbrirPathParaProceso));
		prot_enviar_CPU_DMA_abrirPath(path, dtb->id_GDT, serverDIEGO);
		logger_CPU(escribir_loguear, l_info,"Se le envio a DAM la informacion necesaria del GDT %d", path, dtb->id_GDT);

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
		prot_enviar_CPU_FM9_asignar_datos_linea(datos, linea, path, dtb->id_GDT, dtb->program_counter, serverMEM);
		logger_CPU(escribir_loguear, l_info,"Se le envio a FM9 la informacion necesaria");

		t_cabecera respuesta_de_fm9 = recibirCabecera(serverMEM);

		if(respuesta_de_fm9.tipoDeMensaje == AsignarDatosALineaEjecutandose){
			logger_CPU(escribir_loguear, l_trace,"FM9 puso manos a la obra!");
		}else{
			if(respuesta_de_fm9.tipoDeMensaje == FalloDeSegmentoMemoria){
				logger_CPU(escribir_loguear, l_error,"Fallo de segmento/memoria en FM9");
				informar_a_safa_que_aborte_DTB(dtb);
			}

			if(respuesta_de_fm9.tipoDeMensaje == EspacionInsuficiente){
				logger_CPU(escribir_loguear, l_error,"Espacio insuficiente en FM9");
				informar_a_safa_que_aborte_DTB(dtb);
			}
		}
	}else{
		logger_CPU(escribir_loguear, l_error,"20001");
		informar_a_safa_que_aborte_DTB(dtb);
	}
}

void enviar_a_SAFA_solicitud_retencion_recurso(char * recurso, tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Se ejecutara la primitiva Wait para el recurso %s", recurso);
	enviarCabecera(serverSAFA, RetenerRecurso, sizeof(RetenerRecurso));
	prot_enviar_CPU_SAFA_retener_recurso(recurso, dtb->id_GDT, serverSAFA);
}

void enviar_a_SAFA_solicitud_liberar_recurso(char * recurso, tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Se ejecutara la primitiva Signal para el recurso %s", recurso);
	enviarCabecera(serverSAFA, LiberarRecurso, sizeof(LiberarRecurso));
	prot_enviar_CPU_SAFA_liberar_recurso(recurso, dtb->id_GDT, serverSAFA);
}

void esperar_respuesta_afirmativa_de_SAFA(){
	t_cabecera respuesta_de_SAFA = recibirCabecera(serverSAFA);

	if(respuesta_de_SAFA.tipoDeMensaje == RespuestaASolicitudDeRecursoAfirmativa){
		logger_CPU(escribir_loguear, l_warning,"El recurso fue asignado");
	}
	/*
	 * El recv es bloqueante, se quedará esperando a que llegue la respuesta afirmativa
	 */
}

void retener_recurso(char * recurso, tp_DTB dtb){
	enviar_a_SAFA_solicitud_retencion_recurso(recurso,dtb);
	esperar_respuesta_afirmativa_de_SAFA();
}

void liberar_recurso(char * recurso, tp_DTB dtb){
	enviar_a_SAFA_solicitud_liberar_recurso(recurso,dtb);
}

void guardar_contenido_de_fm9_en_mdj(char * path, tp_DTB dtb){
	path_archivo_para_comparar = path;
	if(chequear_si_archivo_esta_abierto(dtb)){
		logger_CPU(escribir_loguear, l_trace,"Voy a comunicarme con el diego para realizar el flush");
		enviarCabecera(serverDIEGO, FlushDeArchivoADisco, sizeof(FlushDeArchivoADisco));
		//prot_enviar_CPU_DMA_flush(path, dtb->id_GDT,memoryadress, serverDIEGO)
		logger_CPU(escribir_loguear, l_info,"Se le envio a FM9 la informacion necesaria");

		t_cabecera respuesta_del_diego = recibirCabecera(serverDIEGO);

		if(respuesta_del_diego.tipoDeMensaje == FlushDeArchivoADiscoEjecutandose){
			logger_CPU(escribir_loguear, l_trace,"DAM esta realizando el flush");
		}
	}else{
		logger_CPU(escribir_loguear, l_error,"30001");
		informar_a_safa_que_aborte_DTB(dtb);
	}
}

void enviar_FM9_solicitud_liberar_archivo(char * path, tp_DTB dtb){
	logger_CPU(escribir_loguear, l_info,"Voy a comunicarme con FM9 para liberar un archivo abierto.");
	enviarCabecera(serverMEM, LiberarArchivoAbierto, sizeof(LiberarArchivoAbierto));
	prot_enviar_CPU_FM9_liberar_archivo(path, dtb->id_GDT, serverMEM);
	logger_CPU(escribir_loguear, l_info,"Se le envio a FM9 la informacion necesaria");

	t_cabecera respuesta_de_fm9 = recibirCabecera(serverMEM);

	if(respuesta_de_fm9.tipoDeMensaje == LiberarArchivoAbiertoEjecutandose){
		logger_CPU(escribir_loguear, l_trace,"FM9 se esta encargando del archivo en cuestion...");
	}else{
		if(respuesta_de_fm9.tipoDeMensaje == LiberarArchivoAbiertoNoFinalizado){
			logger_CPU(escribir_loguear, l_error,"FM9 no pudo liberar el archivo");
			informar_a_safa_que_aborte_DTB(dtb);
		}
	}
}

void actualizar_dtb_liberar_archivo(char * path, tp_DTB dtb){
	path_archivo_para_comparar = path;
	list_remove_by_condition(dtb->tabla_dir_archivos, &tienen_el_mismo_nombre);
	///se lo tengo que enviar a safa?? lo hace safa??
}

void liberar_archivo_abierto(char * path, tp_DTB dtb){
	path_archivo_para_comparar = path;
	if(chequear_si_archivo_esta_abierto(dtb)){
		enviar_FM9_solicitud_liberar_archivo(path, dtb);
		actualizar_dtb_liberar_archivo(path, dtb);
	}else{
		logger_CPU(escribir_loguear, l_error,"40001");
		informar_a_safa_que_aborte_DTB(dtb);
	}
}

void crear_nuevo_archivo_en_mdj(char * path, int cant_lineas, tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Se procedera a crear un nuevo archivo en MDJ");
	enviarCabecera(serverDIEGO,CrearLineasEnArchivo, sizeof(CrearLineasEnArchivo));
	prot_enviar_CPU_DMA_crear_lineas_arch(path, cant_lineas, dtb->id_GDT, serverDIEGO);
	logger_CPU(escribir_loguear, l_info,"Se le envio al DAM la informacion necesaria");

	t_cabecera respuesta_de_fm9 = recibirCabecera(serverDIEGO);

	if(respuesta_de_fm9.tipoDeMensaje == CrearLineasEnArchivoEjecutandose){
		logger_CPU(escribir_loguear, l_info,"DAM recibio la informacion correctamente");
		desalojar_dtb(dtb);
	}
}

void eliminar_archivo_de_mdj(char * path, tp_DTB dtb){
	logger_CPU(escribir_loguear, l_trace,"Se procedera a eliminar un archivo de MDJ");
	enviarCabecera(serverDIEGO,EliminarArchivoDeDisco, sizeof(EliminarArchivoDeDisco));
	prot_enviar_CPU_DAM_eliminar_arch_de_disco(path, dtb->id_GDT, serverDIEGO);
	logger_CPU(escribir_loguear, l_info,"Se le envio al DAM la informacion necesaria");

	t_cabecera respuesta_de_fm9 = recibirCabecera(serverDIEGO);

	if(respuesta_de_fm9.tipoDeMensaje == EliminarArchivoDeDiscoEjecutandose){
		logger_CPU(escribir_loguear, l_info,"DAM recibio la informacion correctamente");
		desalojar_dtb(dtb);
	}

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
			retener_recurso(resultado_del_parseado.parametros.wait.recurso,dtb);
			break;
		case SIGNAL:
			liberar_recurso(resultado_del_parseado.parametros.signal.recurso,dtb);
			break;
		case FLUSH:
			guardar_contenido_de_fm9_en_mdj(resultado_del_parseado.parametros.flush.path, dtb);
			break;
		case CLOSE:
			liberar_archivo_abierto(resultado_del_parseado.parametros.close.path, dtb);
			break;
		case CREAR:
			crear_nuevo_archivo_en_mdj(resultado_del_parseado.parametros.crear.path, resultado_del_parseado.parametros.crear.cantidad_de_lineas, dtb);
			break;
		case BORRAR:
			eliminar_archivo_de_mdj(resultado_del_parseado.parametros.borrar.path, dtb);
			break;
		case FIN:
			cerrar_script(dtb);
			break;
	}
}


void proceder_con_lectura_escriptorio(tp_DTB dtb){
	int unidad_de_tiempo = dtb->quantum;
	for(int i=0;i++;i<unidad_de_tiempo){
		solicitar_a_FM9_la_sentencia(dtb);
		tp_lineaParaCPU paquete_linea = recibir_de_FM9_linea_a_parsear(dtb);
		actualizar_program_counter(dtb);
		t_operacion resultado_del_parseado = parsear(paquete_linea->linea);
		realizar_la_operacion_que_corresponda_segun(resultado_del_parseado, dtb);
		//free(resultado_del_parseado);
		sleep((int)RETARDO);
	}
}

void liberar_dtb(tp_DTB dtb){
	list_destroy(dtb->tabla_dir_archivos);
	free(dtb->escriptorio);
	free(dtb);
}

void validar_dtb(int id_dtb){
	if(abs(id_dtb)>99999||id_dtb==0){
		finalizar_cpu();
	}
}

void recibir_dtb_y_delegar(){
	tp_DTB dtb;

	while(true){

		dtb = prot_recibir_SAFA_CPU_DTB(serverSAFA);

		validar_dtb(dtb->id_GDT);

		logger_CPU(escribir_loguear, l_info,"DTB recibido de SAFA");

		if(dtb->iniGDT == 0){
			logger_CPU(escribir_loguear, l_trace,"Se procedera a realizar la operacion DUMMY");
			iniciar_operacion_dummy(dtb);
		}else{
			logger_CPU(escribir_loguear, l_trace,"Se procedera con la ejecucion de sentencias");
			proceder_con_lectura_escriptorio(dtb);
		}

		liberar_dtb(dtb);
	}
}

int main(int argc, char **argv){

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

	recibir_dtb_y_delegar();

	finalizar_cpu();

return 0;

}
