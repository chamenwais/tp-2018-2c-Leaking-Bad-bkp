/*
 * ConexionesEntrantes.c
 *
 *  Created on: 8 sep. 2018
 *      Author: faromero
 */
#include "ConexionesEntrantes.h"

void crear_hilos_conexiones_entrantes(int socket_fm9, int socket_safa, int socket_filesystem){
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
					//Nueva conexion, la agregamos a la lista de conocidas (master)
					nuevo_socketfd=aceptarConexion(socketfd_escucha);
					validar_comunicacion(socketfd_escucha, CONST_NAME_CPU);
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
						loguear_cabecera_recibida(CONST_NAME_CPU);
						clasificar_y_crear_hilo_correspondiente_a_pedido_CPU(
								cabecera.tipoDeMensaje
								,iterador_conexiones_existentes
								,socket_fm9
								,socket_safa
								,socket_filesystem);
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
		, int socket_safa
		, int socket_filesystem){
	pthread_attr_t atributo_detachable;
	pthread_t hilo_correspondiente_a_pedido;
	pthread_attr_init(&atributo_detachable);
	pthread_attr_setdetachstate(&atributo_detachable, PTHREAD_CREATE_DETACHED);
	switch(mensaje_entrante){
		case AbrirPathParaProceso:
			pthread_create(&hilo_correspondiente_a_pedido,&atributo_detachable,(void *)operacion_abrir_path,
					adaptar_sockets_para_hilo(socket_CPU_solicitante,socket_fm9,socket_safa,socket_filesystem));
			break;
		case FlushDeArchivoADisco:
			pthread_create(&hilo_correspondiente_a_pedido,&atributo_detachable,(void *)operacion_flush_archivo,
					adaptar_sockets_para_hilo(socket_CPU_solicitante,socket_fm9,socket_safa,socket_filesystem));
			break;
		case CrearLineasEnArchivo:
			break;
		case EliminarArchivoDeDisco:
			break;
		default:
			break;
	}
	pthread_attr_destroy(&atributo_detachable);
	pthread_detach(hilo_correspondiente_a_pedido);
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
	recibir_handshake_de(socket_id, CPU, CONST_NAME_CPU);
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
	logger_DAM(escribir_loguear, l_error,
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

t_cabecera validar_archivo(int socket_mdj, tp_abrirPath mensaje_cpu) {
	pthread_mutex_lock(&MX_FS);
	enviarCabecera(socket_mdj, ValidarArchivo, sizeof(ValidarArchivo));
	prot_enviar_DMA_FS_path(mensaje_cpu->path, socket_mdj);
	t_cabecera respuesta_validacion = recibirCabecera(socket_mdj);
	pthread_mutex_unlock(&MX_FS);
	return respuesta_validacion;
}

void operacion_abrir_path(int * sockets){
	int socket_CPU=sockets[0];
	int socket_fm9=sockets[1];
	int socket_safa=sockets[2];
	int socket_mdj=sockets[3];
	free(sockets);
	tp_abrirPath mensaje_cpu=prot_recibir_CPU_DMA_abrirPath(socket_CPU);
	logger_DAM(escribir_loguear, l_trace,"Ehhh, voy a buscar [%s] para [%d]",mensaje_cpu->path,mensaje_cpu->pid);
	enviarCabecera(socket_CPU,AbrirPathEjecutandose,sizeof(AbrirPathEjecutandose));
	t_cabecera respuesta_validacion_path = validar_archivo(socket_mdj, mensaje_cpu);
	if(cabecera_esta_vacia(&respuesta_validacion_path)||!cabecera_correcta(&respuesta_validacion_path)){
		loguear_y_avisar_a_safa_apertura_erronea(socket_safa,CONST_NAME_MDJ,mensaje_cpu);
	} else {
		loguear_cabecera_recibida(CONST_NAME_MDJ);
		//Tratamos la respuesta de la validez o invalidez del path del archivo por abrir
		if(ElArchivoNoExiste==tratar_invalidez_archivo(respuesta_validacion_path, mensaje_cpu, socket_safa)){
			return;
		}
		tratar_validez_archivo(respuesta_validacion_path, mensaje_cpu, socket_mdj, socket_fm9, socket_safa);
	}
}

int * adaptar_sockets_para_hilo(int CPU_Fd, int fm9_Fd, int Safa_fd, int filesystem_fd){
	int * sockets=calloc(4,sizeof(int));
	sockets[0]=CPU_Fd;
	sockets[1]=fm9_Fd;
	sockets[2]=Safa_fd;
	sockets[3]=filesystem_fd;
	return sockets;
}

void loguear_y_avisar_a_safa_apertura_erronea(int sockfd_safa, char * proceso, tp_abrirPath path_y_pid){
	logger_DAM(escribir_loguear, l_warning,"Hubo un error de comunicacion con %s al abrir el archivo",proceso);
	informar_operacion_abrir_erronea(sockfd_safa,path_y_pid);
}

void informar_operacion_abrir_erronea(int socket_safa, tp_abrirPath path_y_pid) {
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, AbrirPathNoFinalizado, sizeof(AbrirPathNoFinalizado));
	prot_enviar_DMA_SAFA_datosEnMemoria(path_y_pid->path, path_y_pid->pid, -1, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
}

void informar_operacion_abrir_exitosa(int socket_safa, tp_abrirPath path_y_pid, int direccion_de_memoria) {
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, AbrirPathFinalizadoOk, sizeof(AbrirPathFinalizadoOk));
	prot_enviar_DMA_SAFA_datosEnMemoria(path_y_pid->path, path_y_pid->pid, direccion_de_memoria, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
	logger_DAM(escribir_loguear, l_trace,
			"Se mando a safa la direccion de memoria %d del archivo que se abrio",
			direccion_de_memoria);
	free(path_y_pid->path);
}

enum MENSAJES tratar_invalidez_archivo(t_cabecera respuesta_validez_archivo, tp_abrirPath info_cpu, int socket_safa){
	enum MENSAJES tipo_de_mensaje = respuesta_validez_archivo.tipoDeMensaje;
	if (ElArchivoNoExiste == (tipo_de_mensaje)) {
		logger_DAM(escribir_loguear, l_error,"Dijo Mdj que el path %s no existe. Error 10001",info_cpu->path);
		informar_operacion_abrir_erronea(socket_safa, info_cpu);
	}
	return tipo_de_mensaje;
}

tp_datosObtenidosDeProtocolo pedir_datos_a_Mdj(char* ruta, int offset_Mdj, int socket_mdj) {
	pthread_mutex_lock(&MX_FS);
	enviarCabecera(socket_mdj, ObtenerDatos, sizeof(ObtenerDatos));
	prot_enviar_DMA_FS_obtenerDatos(ruta, offset_Mdj, transfer_size, socket_mdj);
	tp_datosObtenidosDeProtocolo datos = prot_recibir_FS_DMA_datosObtenidos(socket_mdj);
	pthread_mutex_unlock(&MX_FS);
	return datos;
}

int cargar_datos_en_Fm9(int socket_fm9, tp_abrirPath info_cpu, int offset_Fm9, tp_datosObtenidosDeProtocolo parte_archivo) {
	pthread_mutex_lock(&MX_MEMORIA);
	enviarCabecera(socket_fm9, CargarParteEnMemoria, sizeof(CargarParteEnMemoria));
	prot_enviar_DMA_FM9_cargarEnMemoria(info_cpu->pid, info_cpu->path, parte_archivo->buffer
			, offset_Fm9, transfer_size, parte_archivo->tamanio_total_archivo, socket_fm9);
	int direccion_logica = prot_recibir_FM9_DMA_cargaEnMemoria(socket_fm9);
	pthread_mutex_unlock(&MX_MEMORIA);
	free(parte_archivo);
	return direccion_logica;
}

bool validar_fragmento_archivo(tp_datosObtenidosDeProtocolo fragmento_archivo, int socket_safa, tp_abrirPath mensaje_cpu){
	if(fragmento_archivo==NULL){
		loguear_y_avisar_a_safa_apertura_erronea(socket_safa,CONST_NAME_MDJ,mensaje_cpu);
		return false;
	}
	return true;
}

void informar_carga_en_memoria_erronea(int socket_safa, tp_abrirPath mensaje_cpu){
	logger_DAM(escribir_loguear, l_error,"No se cargo el path por espacio insuficiente en FM9");
	loguear_y_avisar_a_safa_apertura_erronea(socket_safa,CONST_NAME_MDJ,mensaje_cpu);
}

void loguear_no_obtencion_de_fragmento_archivo() {
	logger_DAM(escribir_loguear, l_error, "No se obtuvo el fragmento de archivo desde el FS");
}

bool todavia_no_se_recibio_todo_el_archivo(int tamanio_parcial_archivo, tp_datosObtenidosDeProtocolo parte_archivo) {
	return tamanio_parcial_archivo <= parte_archivo->tamanio_total_archivo;
}

void tratar_validez_archivo(t_cabecera respuesta_validez_archivo, tp_abrirPath info_cpu, int socket_mdj, int socket_fm9, int socket_safa){
	if(ElArchivoExiste==(respuesta_validez_archivo.tipoDeMensaje)){
		int offset_Mdj=0;
		int offset_Fm9=0;
		logger_DAM(escribir_loguear, l_trace,"El archivo %s es valido, vamos a cargarlo en memoria",info_cpu->path);
		tp_datosObtenidosDeProtocolo parte_archivo = pedir_datos_a_Mdj(info_cpu->path, offset_Mdj, socket_mdj);
		if(validar_fragmento_archivo(parte_archivo, socket_safa, info_cpu)==false){
			loguear_no_obtencion_de_fragmento_archivo();
			return;
		}
		int tamanio_parcial_archivo=0;
		int direccion_de_memoria;
		while (todavia_no_se_recibio_todo_el_archivo(tamanio_parcial_archivo, parte_archivo)) {
			logger_DAM(escribir_loguear, l_trace,"Se recibio un fragmento de archivo de %d bytes",parte_archivo->size);
			offset_Mdj+=parte_archivo->size;
			direccion_de_memoria = cargar_datos_en_Fm9(socket_fm9, info_cpu, offset_Fm9, parte_archivo);
			if(direccion_de_memoria==-1){
				informar_carga_en_memoria_erronea(socket_safa, info_cpu);
				return;
			}
			logger_DAM(escribir_loguear, l_trace,"Se cargo el fragmento en memoria");
			offset_Fm9+=parte_archivo->size;
			parte_archivo=pedir_datos_a_Mdj(info_cpu->path, offset_Mdj, socket_mdj);
			if(validar_fragmento_archivo(parte_archivo, socket_safa, info_cpu)){
				loguear_no_obtencion_de_fragmento_archivo();
				return;
			}
		}
		informar_operacion_abrir_exitosa(socket_safa, info_cpu,	direccion_de_memoria);
		free(parte_archivo->buffer);
		free(parte_archivo);
	}
}

void operacion_flush_archivo(int * sockets){
	int socket_CPU=sockets[0];
	int socket_fm9=sockets[1];
	int socket_safa=sockets[2];
	int socket_mdj=sockets[3];
	free(sockets);
	tp_datosEnMemoria pedido_flush=prot_recibir_CPU_DMA_flush(socket_CPU);
	logger_DAM(escribir_loguear, l_trace,"Voy a hacer flush de [%s] para [%d]",pedido_flush->path,pedido_flush->pid);
	enviarCabecera(socket_CPU,FlushDeArchivoADiscoEjecutandose,sizeof(FlushDeArchivoADiscoEjecutandose));
	tp_datosObtenidosDeProtocolo datos_obtenidos=obtener_archivo(socket_fm9,socket_safa,pedido_flush,0);
	//TODO terminar
}

tp_datosObtenidosDeProtocolo obtener_archivo(int fm9_sock, int safa_sock, tp_datosEnMemoria datos_flush, int offset){
	pthread_mutex_lock(&MX_MEMORIA);
	enviarCabecera(fm9_sock, ObtenerDatos, sizeof(ObtenerDatos));
	prot_enviar_DMA_FM9_obtenerArchivo(datos_flush->path,datos_flush->pid,datos_flush->memory_address,
			offset, transfer_size, fm9_sock);
	t_cabecera respuesta_validacion = recibirCabecera(fm9_sock);
	pthread_mutex_unlock(&MX_MEMORIA);
	if(ElArchivoNoExiste==tratar_inexistencia_archivo_mp(respuesta_validacion, datos_flush, safa_sock)){
		return NULL;
	}
	pthread_mutex_lock(&MX_MEMORIA);
	tp_datosObtenidosDeProtocolo datos_obtenidos=prot_recibir_FM9_DMA_devolverDatos(fm9_sock);
	pthread_mutex_unlock(&MX_MEMORIA);
	return datos_obtenidos;
}

void informar_operacion_flush_erronea(int socket_safa, tp_datosEnMemoria path_y_pid) {
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, FlushDeArchivoADiscoNoFinalizado, sizeof(FlushDeArchivoADiscoNoFinalizado));
	prot_enviar_DMA_SAFA_finFlush(path_y_pid->path, path_y_pid->pid, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
}

void informar_operacion_flush_exitosa(int socket_safa, tp_datosEnMemoria path_y_pid) {
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, FlushDeArchivoADiscoFinalizadoOk, sizeof(FlushDeArchivoADiscoFinalizadoOk));
	prot_enviar_DMA_SAFA_finFlush(path_y_pid->path, path_y_pid->pid, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
	logger_DAM(escribir_loguear, l_trace,
			"Se informo a safa que se hizo flush del archivo");
	free(path_y_pid->path);
}

enum MENSAJES tratar_inexistencia_archivo_mp(t_cabecera respuesta_validez_archivo, tp_datosEnMemoria info_cpu, int socket_safa){
	enum MENSAJES tipo_de_mensaje = respuesta_validez_archivo.tipoDeMensaje;
	if (ElArchivoNoExiste == (tipo_de_mensaje)) {
		logger_DAM(escribir_loguear, l_error,"Dijo FM9 que el path %s no existe. Error 30001",info_cpu->path);
		informar_operacion_flush_erronea(socket_safa, info_cpu);
	}
	return tipo_de_mensaje;
}
