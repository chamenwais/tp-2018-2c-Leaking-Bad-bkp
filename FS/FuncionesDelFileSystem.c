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

	return EXIT_SUCCESS;
}

int levantarLineasDelArchivoDeConfiguracion(char* ubicacionDelArchivoConfiguracion){
	t_config* configuracion = config_create(ubicacionDelArchivoConfiguracion);

	if(configuracion!=NULL){
		log_info(LOGGER,"El archivo de configuracion existe");
	}else{
		log_error(LOGGER,"No existe el archivo de configuracion");
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
	log_info(LOGGER,"Iniciando consola");
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
			pthread_exit(ret);
			}else{
		if(strcmp(instruccion[0],"ls")==0){
			if(instruccion[1]==NULL){
				printf("Listando directorio actual\n",instruccion[1]);
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
	pthread_exit(ret);
}

int esperarAQueTermineLaConsola(){
	pthread_join( threadConsola, NULL);
	log_info(LOGGER,"Hilo de consola finalizado");
	return EXIT_SUCCESS;
}

void finalizarTodo(){
	exit(1);
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
	return EXIT_SUCCESS;
}
