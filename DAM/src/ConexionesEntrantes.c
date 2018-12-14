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
					if (!cabecera_correcta(&cabecera)) {
						//No se puede leer el mensaje, se va a cerrar el sockfd
						loguear_y_cerrar_comunicacion_erronea_con_CPU(
								iterador_conexiones_existentes);
						FD_CLR(iterador_conexiones_existentes, &lista_fd_maestra);
					}
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
			pthread_create(&hilo_correspondiente_a_pedido,&atributo_detachable,(void *)operacion_crear_archivo,
					adaptar_sockets_sin_mp_para_hilo(socket_CPU_solicitante,socket_safa,socket_filesystem));
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
	if(!cabecera_correcta(&respuesta_validacion_path)){
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

int * adaptar_sockets_sin_mp_para_hilo(int CPU_Fd, int Safa_fd, int filesystem_fd){
	int * sockets=calloc(3,sizeof(int));
	sockets[0]=CPU_Fd;
	sockets[1]=Safa_fd;
	sockets[2]=filesystem_fd;
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

bool todavia_no_se_recibio_todo_el_archivo(int tamanio_parcial_archivo, int tamanio_total_archivo) {
	return tamanio_parcial_archivo < tamanio_total_archivo;
}

int loguear_cantidad_datos_y_cargar(tp_datosObtenidosDeProtocolo parte_archivo,
		int direccion_de_memoria, int socket_fm9, tp_abrirPath info_cpu,
		int offset_Fm9) {
	logger_DAM(escribir_loguear, l_trace,
			"Se recibio un fragmento de archivo de %d bytes",
			parte_archivo->size);
	direccion_de_memoria = cargar_datos_en_Fm9(socket_fm9, info_cpu, offset_Fm9,
			parte_archivo);
	return direccion_de_memoria;
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
		int tamanio_parcial_archivo=parte_archivo->size;
		int direccion_de_memoria;
		while (todavia_no_se_recibio_todo_el_archivo(tamanio_parcial_archivo, parte_archivo->tamanio_total_archivo)) {
			direccion_de_memoria = loguear_cantidad_datos_y_cargar(
					parte_archivo, direccion_de_memoria, socket_fm9, info_cpu,
					offset_Fm9);
			if(direccion_de_memoria==-1){
				informar_carga_en_memoria_erronea(socket_safa, info_cpu);
				return;
			}
			logger_DAM(escribir_loguear, l_trace,"Se cargo el fragmento en memoria");
			offset_Mdj+=parte_archivo->size;
			free(parte_archivo->buffer);
			offset_Fm9+=parte_archivo->size;
			parte_archivo=pedir_datos_a_Mdj(info_cpu->path, offset_Mdj, socket_mdj);
			if(validar_fragmento_archivo(parte_archivo, socket_safa, info_cpu)){
				loguear_no_obtencion_de_fragmento_archivo();
				informar_carga_en_memoria_erronea(socket_safa, info_cpu);
				return;
			}
			tamanio_parcial_archivo+=parte_archivo->size;
		}
		if(parte_archivo->size>0){
			direccion_de_memoria = loguear_cantidad_datos_y_cargar(
								parte_archivo, direccion_de_memoria, socket_fm9, info_cpu,
								offset_Fm9);
			if(direccion_de_memoria==-1){
				informar_carga_en_memoria_erronea(socket_safa, info_cpu);
				return;
			}
		}
		informar_operacion_abrir_exitosa(socket_safa, info_cpu,	direccion_de_memoria);
		free(info_cpu->path);
		//TODO verificar si hace falta este free
		free(parte_archivo->buffer);
	}
}

void loguear_error_comunicacion_en_flush(char* name_Fm9) {
	logger_DAM(escribir_loguear, l_warning,
			"Hubo un error de comunicacion con %s al hacer flush el archivo",
			name_Fm9);
}

tp_datosObtenidosDeProtocolo recibir_datos_fm9(int socket_fm9) {
	pthread_mutex_lock(&MX_MEMORIA);
	tp_datosObtenidosDeProtocolo datos_obtenidos = prot_recibir_FM9_DMA_devolverDatos(socket_fm9);
	pthread_mutex_unlock(&MX_MEMORIA);
	return datos_obtenidos;
}

enum MENSAJES guardar_datos_en_disco(int socket_mdj, char * ruta, int offset_Mdj, char *  datos_obtenidos){
	t_cabecera cabecera_resultado;
	pthread_mutex_lock(&MX_FS);
	enviarCabecera(socket_mdj,GuardarDatos,sizeof(GuardarDatos));
	prot_enviar_DMA_FS_guardarDatos(ruta, offset_Mdj, transfer_size, datos_obtenidos, socket_mdj);
	cabecera_resultado = recibirCabecera(socket_mdj);
	pthread_mutex_unlock(&MX_FS);
	if(!cabecera_correcta(&cabecera_resultado)){
		loguear_error_comunicacion_en_flush(CONST_NAME_MDJ);
		return ArchivoNoEncontrado;
	}
	return cabecera_resultado.tipoDeMensaje;
}

enum MENSAJES loguear_cantidad_datos_y_guardar_en_disco(
		tp_datosObtenidosDeProtocolo datos_obtenidos,
		enum MENSAJES resultado_guardado, int socket_mdj,
		tp_datosEnMemoria pedido_flush, int offset_Mdj) {
	logger_DAM(escribir_loguear, l_trace,
			"Se recibio un fragmento de archivo de %d bytes",
			datos_obtenidos->size);
	resultado_guardado = guardar_datos_en_disco(socket_mdj, pedido_flush->path,
			offset_Mdj, datos_obtenidos->buffer);
	return resultado_guardado;
}

void tratar_archivo_inexistente_en_mdj_flush(tp_datosEnMemoria pedido_flush,
		int socket_safa) {
	logger_DAM(escribir_loguear, l_error,
			"El archivo %s no existe en MDJ. Error 30004\n",
			pedido_flush->path);
	informar_operacion_flush_erronea(socket_safa, pedido_flush);
}

void tratar_espacio_insuficiente_mdj_flush(int socket_safa,
		tp_datosEnMemoria pedido_flush) {
	logger_DAM(escribir_loguear, l_error,
			"Espacio insuficiente en disco. Error 30003\n");
	informar_operacion_flush_erronea(socket_safa, pedido_flush);
}

void tratar_existencia_archivo_en_mp(enum MENSAJES existencia, tp_datosEnMemoria pedido_flush, int socket_fm9,
		int socket_mdj, int socket_safa) {
	if (ElArchivoExiste == existencia) {
		//guarda el archivo en disco y avisa a Safa
		int offset_Mdj = 0;
		logger_DAM(escribir_loguear, l_trace,
				"El archivo %s existe en FM9, vamos a guardarlo en disco",
				pedido_flush->path);
		tp_datosObtenidosDeProtocolo datos_obtenidos=recibir_datos_fm9(socket_fm9);
		int tamanio_parcial_archivo=datos_obtenidos->size;
		enum MENSAJES resultado_guardado;
		while (todavia_no_se_recibio_todo_el_archivo(tamanio_parcial_archivo,datos_obtenidos->tamanio_total_archivo)){
			resultado_guardado = loguear_cantidad_datos_y_guardar_en_disco(
					datos_obtenidos, resultado_guardado, socket_mdj,
					pedido_flush, offset_Mdj);
			if(resultado_guardado==ArchivoNoEncontrado){
				tratar_archivo_inexistente_en_mdj_flush(pedido_flush,
						socket_safa);
				return;
			}
			if(resultado_guardado==NoHayMasBloquesLibres){
				tratar_espacio_insuficiente_mdj_flush(socket_safa,
						pedido_flush);
				return;
			}
			logger_DAM(escribir_loguear, l_trace,"Se hizo flush del fragmento en disco");
			offset_Mdj+=datos_obtenidos->size;
			free(datos_obtenidos->buffer);
			t_cabecera cabecera_existencia=validar_archivo_en_memoria(socket_fm9, socket_safa, pedido_flush);
			if(!cabecera_correcta(&cabecera_existencia)){
				loguear_informar_error_comunicacion_flush(socket_safa, pedido_flush);
				return;
			} else if(ErrorSegmentoPagina==cabecera_existencia.tipoDeMensaje){
				tratar_error_segmento_pagina(pedido_flush, socket_safa);
				return;
			}
			tamanio_parcial_archivo+=datos_obtenidos->size;
		}
		if(datos_obtenidos->size>0){
			resultado_guardado = loguear_cantidad_datos_y_guardar_en_disco(
								datos_obtenidos, resultado_guardado, socket_mdj,
								pedido_flush, offset_Mdj);
			if(resultado_guardado==ArchivoNoEncontrado){
				informar_operacion_flush_erronea(socket_safa, pedido_flush);
				return;
			}
		}
		//Informa operacion flush exitosa a safa y hace free de estructuras dinamicas
		informar_operacion_flush_exitosa(socket_safa,pedido_flush);
		free(pedido_flush->path);
		//TODO verificar si hace falta este free
		free(datos_obtenidos->buffer);
	}
}

void tratar_error_segmento_pagina(tp_datosEnMemoria pedido_flush, int socket_safa) {
	logger_DAM(escribir_loguear, l_error,
			"Hubo un error de segmento/memoria en FM9 para el path %s. Error 30002",
			pedido_flush->path);
	informar_operacion_flush_erronea(socket_safa, pedido_flush);
}

void loguear_informar_error_comunicacion_flush(int socket_safa, tp_datosEnMemoria pedido_flush) {
	loguear_error_comunicacion_en_flush(CONST_NAME_FM9);
	informar_operacion_flush_erronea(socket_safa, pedido_flush);
}

void operacion_flush_archivo(int * sockets){
	int socket_CPU=sockets[0];
	int socket_fm9=sockets[1];
	int socket_safa=sockets[2];
	int socket_mdj=sockets[3];
	free(sockets);
	t_cabecera resultado_existencia_archivo;
	tp_datosEnMemoria pedido_flush=prot_recibir_CPU_DMA_flush(socket_CPU);
	logger_DAM(escribir_loguear, l_trace,"Voy a hacer flush de [%s] para [%d]",pedido_flush->path,pedido_flush->pid);
	enviarCabecera(socket_CPU,FlushDeArchivoADiscoEjecutandose,sizeof(FlushDeArchivoADiscoEjecutandose));
	resultado_existencia_archivo=validar_archivo_en_memoria(socket_fm9, socket_safa, pedido_flush);
	if(!cabecera_correcta(&resultado_existencia_archivo)){
		loguear_informar_error_comunicacion_flush(socket_safa, pedido_flush);
	}else{
		loguear_cabecera_recibida(CONST_NAME_FM9);
		enum MENSAJES existencia=tratar_inexistencia_archivo_mp(resultado_existencia_archivo, pedido_flush, socket_safa);
		if(ElArchivoNoExiste == existencia) {
			return;
		} else if(ErrorSegmentoPagina==existencia){
			tratar_error_segmento_pagina(pedido_flush, socket_safa);
			return;
		}
		tratar_existencia_archivo_en_mp(existencia, pedido_flush, socket_fm9, socket_mdj, socket_safa);
	}
}

t_cabecera validar_archivo_en_memoria(int fm9_sock, int safa_sock, tp_datosEnMemoria datos_flush){
	pthread_mutex_lock(&MX_MEMORIA);
	enviarCabecera(fm9_sock, ObtenerDatos, sizeof(ObtenerDatos));
	prot_enviar_DMA_FM9_obtenerArchivo(datos_flush->path,datos_flush->pid,datos_flush->memory_address,
			transfer_size, fm9_sock);
	t_cabecera respuesta_validacion = recibirCabecera(fm9_sock);
	pthread_mutex_unlock(&MX_MEMORIA);
	return respuesta_validacion;
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

void operacion_crear_archivo(int *sockets){
	int socket_CPU=sockets[0];
	int socket_safa=sockets[1];
	int socket_mdj=sockets[2];
	free(sockets);
	tp_crearLineasArch pedido_creacion=prot_recibir_CPU_DMA_crear_lineas_arch(socket_CPU);
	logger_DAM(escribir_loguear, l_trace,"Ehhh, voy a crear [%s] con [%d] lineas",
			pedido_creacion->path, pedido_creacion->cant_lineas);
	enviarCabecera(socket_CPU,CrearLineasEnArchivoEjecutandose,sizeof(CrearLineasEnArchivoEjecutandose));
	t_cabecera respuesta_creacion_path = crear_archivo(socket_mdj, pedido_creacion);
	if(!cabecera_correcta(&respuesta_creacion_path)){
		//informa el error de creacion a safa
		loguear_y_avisar_a_safa_creacion_erronea(socket_safa,pedido_creacion->id_GDT);
	} else {
		loguear_cabecera_recibida(CONST_NAME_MDJ);
		//valida el enum respuesta TODO falta diferenciar falta de espacio o archivo ya existe
		if(ArchivoNoCreado==respuesta_creacion_path.tipoDeMensaje){
			//informar el error creacion a safa
			logger_DAM(escribir_loguear, l_warning,"Hubo un error de espacio insuficiente en MDJ al crear el archivo");
			informar_operacion_crear_erronea(socket_safa, pedido_creacion->id_GDT);
			return;
		}
		//informar el exito de creacion a safa
		logger_DAM(escribir_loguear, l_trace,"El archivo %s se creo en disco",pedido_creacion->path);
		informar_operacion_crear_exitosa(socket_safa,pedido_creacion->id_GDT);
		free(pedido_creacion->path);
	}
}

t_cabecera crear_archivo(int socket_mdj, tp_crearLineasArch mensaje_cpu) {
	pthread_mutex_lock(&MX_FS);
	enviarCabecera(socket_mdj, CrearArchivo, sizeof(CrearArchivo));
	prot_enviar_DMA_FS_CrearArchivo(mensaje_cpu->path, mensaje_cpu->cant_lineas,socket_mdj);
	t_cabecera respuesta_creacion = recibirCabecera(socket_mdj);
	pthread_mutex_unlock(&MX_FS);
	return respuesta_creacion;
}

void loguear_y_avisar_a_safa_creacion_erronea(int sockfd_safa, int pid){
	logger_DAM(escribir_loguear, l_warning,"Hubo un error de comunicacion con MDJ al crear el archivo");
	informar_operacion_crear_erronea(sockfd_safa, pid);
}

void informar_operacion_crear_erronea(int socket_safa, int pid) {
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, CrearLineasEnArchivoNoFinalizado, sizeof(CrearLineasEnArchivoNoFinalizado));
	prot_enviar_DMA_SAFA_crearArchivo(pid, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
}

void informar_operacion_crear_exitosa(int socket_safa, int pid){
	pthread_mutex_lock(&MX_SAFA);
	enviarCabecera(socket_safa, CrearLineasEnArchivoFinalizadoOk, sizeof(CrearLineasEnArchivoFinalizadoOk));
	prot_enviar_DMA_SAFA_crearArchivo(pid, socket_safa);
	pthread_mutex_unlock(&MX_SAFA);
	logger_DAM(escribir_loguear, l_trace,
				"Se informo a safa la creacion del archivo para el pid [%d]", pid);
}
