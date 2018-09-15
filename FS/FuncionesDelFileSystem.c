/*
 * FuncionesDelFileSystem.c
 *
 *  Created on: 2 sep. 2018
 *      Author: utnso
 */

#include "FuncionesDelFileSystem.h"

int inicializarLog(){
	LOGGER = log_create("FileSystem.log","FileSystem",true,LOG_LEVEL_INFO);
	log_info(LOGGER,"Log del FileSystem iniciado");
	return EXIT_SUCCESS;
}

int inicializarVariables(){
	FDDMA=0;
	resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_SUCCESS;
	return EXIT_SUCCESS;
}

int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion){
	t_config* configuracion = config_create(ubicacionDelArchivoConfiguracion);

	if(configuracion!=NULL){
		log_info(LOGGER,"El archivo de configuracion existe");
	}else{
		log_error(LOGGER,"No existe el archivo de configuracion en: %s",ubicacionDelArchivoConfiguracion);
		return EXIT_FAILURE;
		}
	log_info(LOGGER,"Abriendo el archivo de configuracion del FS, su ubicacion es: %s",ubicacionDelArchivoConfiguracion);

	//Recupero el puerto
	if(!config_has_property(configuracion,"PUERTO")) {
		log_error(LOGGER,"No esta el PUERTO en el archivo de configuracion");
		config_destroy(configuracion);
		return EXIT_FAILURE;
		}
	configuracionDelFS.puerto = config_get_int_value(configuracion,"PUERTO");
	log_info(LOGGER,"Puerto del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.puerto);

	//Recupero el punto de montaje
	if(!config_has_property(configuracion,"PUNTO_MONTAJE")) {
		log_error(LOGGER,"No esta el PUNTO_MONTAJE en el archivo de configuracion");
		config_destroy(configuracion);
		return EXIT_FAILURE;
		}
	char* puntoDeMontaje = config_get_string_value(configuracion,"PUNTO_MONTAJE");
	configuracionDelFS.punto_montaje = malloc(strlen(puntoDeMontaje)+1);
	strcpy(configuracionDelFS.punto_montaje,puntoDeMontaje);
	log_info(LOGGER,"Punto de montaje del archivo de configuracion del FS recuperado: \"%s\"",
			configuracionDelFS.punto_montaje);

	//Recupero el retardo
	if(!config_has_property(configuracion,"RETARDO")) {
		log_error(LOGGER,"No esta el RETARDO en el archivo de configuracion");
		config_destroy(configuracion);
		return EXIT_FAILURE;
		}
	configuracionDelFS.retardo = config_get_int_value(configuracion,"RETARDO");
	log_info(LOGGER,"Retardo del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.retardo);
	config_destroy(configuracion);
	return EXIT_SUCCESS;
}

int levantarArchivoDeConfiguracion(int argc,char** argv){
	char* ubicacionDelArchivoDeConfiguracion;
	if(argc>1){
		log_info(LOGGER,"Ubicacion del archivo de configuracion recibida por parametro");
		ubicacionDelArchivoDeConfiguracion = argv[1];
	}else{
		log_info(LOGGER,"Ubicacion del archivo de configuracion por default");
		ubicacionDelArchivoDeConfiguracion = "configuracionFS.cfg";
		}

	if(levantarLineasDelArchivoDeConfiguracion(ubicacionDelArchivoDeConfiguracion)==EXIT_SUCCESS){
		log_info(LOGGER,"Configuracion del FS recuperada exitosamente");
		return EXIT_SUCCESS;
		}
	else{
		log_error(LOGGER,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
}

int iniciarConsola(){
	log_info(LOGGER,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL, funcionHiloConsola, "Hilo consola");
		if(resultadoDeCrearHilo){
			log_error(LOGGER,"Error al crear el hilo, return code: %d",resultadoDeCrearHilo);
			exit(EXIT_FAILURE);
			}
		else{
			log_info(LOGGER,"La consola se creo exitosamente");
			return EXIT_SUCCESS;
			}
	return EXIT_SUCCESS;
}

char** parser_instruccion(char* linea){
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

void *funcionHiloConsola(void *arg){
	char * linea;
	char *ret="Cerrando hilo";
	char** instruccion;

	printf("> Consola lista\n");

	while(1){
		linea = readline(">");
		if(strlen(linea)>0){
			add_history(linea);

		instruccion = parser_instruccion(linea);
		if(instruccion[0] == NULL) continue;
		if(strcmp(instruccion[0],"exit")==0){//Mata al hilo de la consola
			for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
			free(instruccion);
			free(linea);
			log_info(LOGGER,"Cerrando consola");
			resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
			return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
			//pthread_exit(ret);
			}else{
		if(strcmp(instruccion[0],"ls")==0){
			if(instruccion[1]==NULL){
				printf("Listando directorio actual, %s\n",instruccion[1]);
				listarDirectorioActual();
			}else{
				printf("Listando directorio: %s\n",instruccion[1]);
				listarDirectorio(instruccion[1]);
				}
		}else{
		if(strcmp(instruccion[0],"cd")==0){
			if(instruccion[1]!=NULL){
				printf("Cambiando al directorio: %s\n",instruccion[1]);
			}else{
				printf("Faltan parametros, falta el path del directorio, reintentar\n");
				}
		}else{
		if(strcmp(instruccion[0],"md5")==0){
			if(instruccion[1]!=NULL){
				printf("Generando MD5 del archivo: %s\n",instruccion[1]);
			}else{
				printf("Faltan parametros para generar el MD5, falta el path del archivo,reintentar\n");
				}
		}else{
		if(strcmp(instruccion[0],"cat")==0){
			if(instruccion[1]!=NULL){
				printf("Mostrando el contenido del archivo %s por pantalla\n",instruccion[1]);
			}else{
				printf("Faltan parametros para mostrar el contenido del archivo por pantalla, falta el path del archivo,reintentar\n");
				}
		}else{
		printf("Comando desconocido\n");
		}}}}}}
		free(linea);
		for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
		free(instruccion);
	}//Cierre del while(1)
	resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
	return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
	//pthread_exit(ret);
}

int esperarAQueTermineLaEscuchaConElDMA(){
	pthread_join(threadComunicacionConElDMA, NULL);
	log_info(LOGGER,"Hilo de la comunicacion con el DMA finalizado");
	if(resultadoDeLaFinalizacionDeLaComunicacionConElDMA==EXIT_FAILURE){
		log_info(LOGGER,"Hilo de la comunicacion con el DMA finalizado exitosamente");
	}else{
		log_error(LOGGER,"Hilo de la comunicacion con el DMA finalizado por error");
	}
	return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
}

int esperarAQueTermineLaConsola(){
	pthread_join( threadConsola, NULL);
	log_info(LOGGER,"Hilo de consola finalizado");
	return EXIT_SUCCESS;
}

int finalizarTodoPorError(){
	log_info(LOGGER,"Hubo un error insalvable, finalizando el programa");
	log_info(LOGGER,"Cerrando esctructuras");
	log_info(LOGGER,"Cerrando log");

	log_destroy(LOGGER);
	exit(1);
	return EXIT_FAILURE;
}

void liberarRecursos(){
	log_info(LOGGER,"Cerrando esctructuras");
	log_info(LOGGER,"Cerrando log");
	log_destroy(LOGGER);
}

int listarDirectorioActual(){

	return EXIT_SUCCESS;
}

int listarDirectorio(char* directorio){

	return EXIT_SUCCESS;
}

int levantarMetadataBin(){
	char* ubicacionDelArchivo;
	ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Metadata/Metadata.bin");
	log_info(LOGGER,"Buscando el archivo \"Metadata.bin\" en el directorio: %s",ubicacionDelArchivo);

	t_config* configuracion = config_create(ubicacionDelArchivo);

	if(configuracion!=NULL){
		log_info(LOGGER,"El archivo de \"Metadata.bin\" existe");
		log_info(LOGGER,"Abriendo el archivo de configuracion del FS, su ubicacion es: %s",ubicacionDelArchivo);

		//Recupero el TAMANIO_BLOQUES
		if(!config_has_property(configuracion,"TAMANIO_BLOQUES")) {
			log_error(LOGGER,"No esta el tamaño de los bloques en el archivo \"Metadata.bin\"");
			config_destroy(configuracion);
			return EXIT_FAILURE;
			}
		configuracionDeMetadata.tamanioBloques = config_get_int_value(configuracion,"TAMANIO_BLOQUES");
		log_info(LOGGER,"Tamaño de los bloques del archivo \"Metadata.bin\" recuperado: %d",
				configuracionDeMetadata.tamanioBloques);

		//Recupero la CANTIDAD_BLOQUES
		if(!config_has_property(configuracion,"CANTIDAD_BLOQUES")) {
			log_error(LOGGER,"No esta la cantidad de bloques en el archivo \"Metadata.bin\"");
			config_destroy(configuracion);
			return EXIT_FAILURE;
			}
		configuracionDeMetadata.cantidadBloques = config_get_int_value(configuracion,"CANTIDAD_BLOQUES");
		log_info(LOGGER,"Cantidad de bloques del archivo \"Metadata.bin\" recuperado: %d",
				configuracionDeMetadata.cantidadBloques);

		//Recupero el MAGIC_NUMBER
		if(!config_has_property(configuracion,"MAGIC_NUMBER")) {
			log_error(LOGGER,"No esta el \"Magic number\" en el archivo \"Metadata.bin\"");
			config_destroy(configuracion);
			return EXIT_FAILURE;
			}
		char* magicNumber = config_get_string_value(configuracion,"MAGIC_NUMBER");
		configuracionDeMetadata.magicNumber = malloc(strlen(magicNumber)+1);
		strcpy(configuracionDeMetadata.magicNumber,magicNumber);
		log_info(LOGGER,"\"Magic number\" del archivo \"Metadata.bin\" recuperado: %s",
				configuracionDeMetadata.magicNumber);

	}else{
		log_error(LOGGER,"No existe el archivo \"Metadata.bin\"");
		return EXIT_FAILURE;
		}
	log_info(LOGGER,"Cerrando \"Metadata.bin\", info recuperada");
	config_destroy(configuracion);
	return EXIT_SUCCESS;
}

bool existeElArchivo(char *directorioDelArchivo){
  FILE *fd=fopen(directorioDelArchivo, "r");
  if(fd<0){
	  log_info(LOGGER,"No existe el archivo %s",directorioDelArchivo);
	  return false;
  	  }
  log_info(LOGGER,"Si existe el archivo %s",directorioDelArchivo);
  fclose(fd);
  return true;
}

int levantarBitMap(){
	FILE *archivoBitmap;
	char* ubicacionDelArchivo;
	ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Metadata/Bitmap.bin");
	log_info(LOGGER,"Buscando el archivo \"Bitmap.bin\" en el directorio: %s",ubicacionDelArchivo);
	if(existeElArchivo(ubicacionDelArchivo)){
		log_info(LOGGER,"Levantando el BITMAP de disco");
		archivoBitmap=fopen(ubicacionDelArchivo, "wb");
		fread(bitmap,configuracionDeMetadata.cantidadBloques,1,archivoBitmap);
		fclose(archivoBitmap);
	}else{
		log_info(LOGGER,"Generando el BITMAP con los datos de \"Bitmap.bin\"");
		bitmap=bitarray_create(ubicacionDelArchivo,configuracionDeMetadata.cantidadBloques);
		for(int i=0;i<configuracionDeMetadata.cantidadBloques;i++){
			bitarray_clean_bit(bitmap,i);
			}
		bajarADiscoBitmap();
	}
	return EXIT_SUCCESS;

}

int bajarADiscoBitmap(){
	FILE *archivoBitMap;
	char *ubicacionDelArchivo;
	ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Metadata/Bitmap.bin");
	archivoBitMap = fopen(ubicacionDelArchivo, "wb");

	if(archivoBitMap==NULL){
		log_error(LOGGER,"No se pudo abrir el archivo de bitmap en %s para bajar a disco",ubicacionDelArchivo);
		return EXIT_FAILURE;
	}else{
		log_info(LOGGER,"Se pudo abrir el archivo de bitmap en %s para bajar a disco",ubicacionDelArchivo);
		fwrite(bitmap,1,bitarray_get_max_bit(bitmap),archivoBitMap);
		fclose(archivoBitMap);
		log_info(LOGGER,"Bitmap bajado a disco");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

int iniciarEscuchaConDMA(){
	log_info(LOGGER,"Iniciando hilo para la comunicacion con el DMA");
	int resultadoDeCrearHilo = pthread_create(
			&threadComunicacionConElDMA, NULL, funcionHiloComunicacionConElDMA,
			&configuracionDelFS);
	if(resultadoDeCrearHilo){
		log_error(LOGGER,"Error no se pudo crear el hilo para la comunicacion con el DMA: %d",resultadoDeCrearHilo);
		return EXIT_FAILURE;
		}
	else{
		log_info(LOGGER,"Se creo el hilo para la comunicacion con el DMA");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void *funcionHiloComunicacionConElDMA(void *arg){
	log_info(LOGGER,"Esperando conexion entrante del DMA por el puerto: %d", configuracionDelFS.puerto);
	int port = configuracionDelFS.puerto;
	int sockDelServer = escucharEn(port); //crea servidor
	log_info(LOGGER,"Esperando conexion entrante del DMA por el puerto: %d", configuracionDelFS.puerto);
	FDDMA = aceptarConexion(sockDelServer);
	if(recibirHandshake(FS,DMA,FDDMA) > 0){
		//Inicio trabajo con el DMA
		log_info(LOGGER,"Handshake exitoso con el DMA :)");
		t_cabecera cabecera = recibirCabecera(FDDMA);
		if(cabecera.tipoDeMensaje==MemoriaUp){
			log_info(LOGGER,"El proceso memoria esta levantado, me aviso el DMA");
			iniciarTrabajoConElDMA();
		}else{
			if(cabecera.tipoDeMensaje==MemoriaDown){
				log_error(LOGGER,"El proceso memoria esta caido, me aviso el DMA");
				resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
				return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
			}else{
				log_error(LOGGER,"Error en el tipo de mensaje, no se si la memoria esta andando o no");
				resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
				return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
			}
		}
	}else{
		log_error(LOGGER,"El proceso no es el esperado");
		resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
		return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
		}
	resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_SUCCESS;
	return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
}

int iniciarTrabajoConElDMA(){
	t_cabecera cabecera;
	while(1){
		cabecera = recibirCabecera(FDDMA);
		if(cabecera.tamanio>0){
			log_info(LOGGER,"Cabecera recibida: %d, cantidad de bytes: %d",
					cabecera.tipoDeMensaje, cabecera.tamanio);
			switch(cabecera.tipoDeMensaje){
				case ValidarArchivo:
					log_info(LOGGER,"Pedido del DMA de \"ValidarArchivo\"");
					validarArchivo();
					break;
				case CrearArchivo:
					log_info(LOGGER,"Pedido del DMA de \"CrearArchivo\"");
					crearArchivo();
					break;
				case ObtenerDatos:
					log_info(LOGGER,"Pedido del DMA de \"ObtenerDatos\"");
					obtenerDatos();
					break;
				case GuardarDatos:
					log_info(LOGGER,"Pedido del DMA de \"GuardarDatos\"");
					guardarDatos();
					break;
				case FinalizarTrabajoConElFS:
					log_info(LOGGER,"Pedido del DMA de \"Finalizar el trabajo\"");
					return EXIT_SUCCESS;
					break;
				default:
					log_error(LOGGER,"Error, me llego un tipo de mensaje del DMA desconocido, %d",cabecera.tipoDeMensaje);
					return EXIT_FAILURE;
					break;
			}//end swith
		}else{
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}

int validarArchivo(){
	/* Parámetros​: [Path]
	 * Descripción​: Cuando el El Diego reciba la operación de abrir un archivo deberá validar
	 * que el archivo exista.
	 */
	char*path=prot_recibir_DMA_FS_path(FDDMA);
	log_info(LOGGER,"Recibiendo el path: %s, para validar el archivo",path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	log_info(LOGGER,"Voy a ver si existe el archivo",ubicacionDelArchivo);
	if(existeElArchivo(ubicacionDelArchivo)){
		enviarCabecera(FDDMA, ElArchivoExiste, 1);
	}else{
		enviarCabecera(FDDMA, ElArchivoNoExiste, 1);
		}
	return EXIT_SUCCESS;
}

int crearArchivo(){
	/* Recibe del DMA los valores: path
	 */
	char*path=prot_recibir_DMA_FS_path(FDDMA);
	log_info(LOGGER,"Recibiendo el path: %s, para crear el archivo",path);
	return EXIT_SUCCESS;
}

int obtenerDatos(){
	/* Recibe del DMA los valores: path,offset,size
	 */
	return EXIT_SUCCESS;
}

int guardarDatos(){
	/* Recibe del DMA los valores: path,offset,size,buffer
	 */
	return EXIT_SUCCESS;
}
