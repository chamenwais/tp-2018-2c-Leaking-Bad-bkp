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

int cargarDirectorioActual(){
	directorioActual=string_new();
	string_append(&directorioActual, configuracionDelFS.punto_montaje);
	string_append(&directorioActual, "/Archivos");
	log_info(LOGGER,"Directorio actual: %s",directorioActual);
	//free(directorioActual);
	return EXIT_SUCCESS;
}

int crearDirectorios(){
	/* Me asegura que todos los directorios que contienen estructuras administrativas esten creados
	 * */

	log_info(LOGGER,"Creando directorios");

	char*path=string_new();
	string_append(&path, configuracionDelFS.punto_montaje);
	mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",path);
	free(path);

	path=string_new();
	string_append(&path, configuracionDelFS.punto_montaje);
	string_append(&path, "/Archivos/");
	mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",path);
	free(path);

	path=string_new();
	string_append(&path, configuracionDelFS.punto_montaje);
	string_append(&path, "/Bloques/");
	mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",path);
	free(path);

	path=string_new();
	string_append(&path, configuracionDelFS.punto_montaje);
	string_append(&path, "/Metadata/");
	mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",path);
	free(path);

	return EXIT_SUCCESS;
}

int inicializarSemaforos(){
	if(pthread_mutex_init(&mutexUsoDelCanalDeComunicacionDelDMA, NULL) != 0) {
		log_error(LOGGER,"No se pudo inicializar el semaforo mutexUsoDelCanalDeComunicacionDelDMA");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexFinalizarPrograma, NULL) != 0) {
		log_error(LOGGER,"No se pudo inicializar el semaforo de la variable para saber si hay que finalizar el programa");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexIniciarConsola, NULL) != 0) {
		log_error(LOGGER,"No se pudo inicializar el semaforo de la consola");
		return EXIT_FAILURE;
	}else{
		pthread_mutex_lock(&mutexIniciarConsola);
		}
	if(pthread_mutex_init(&mutexSistemaDeArchivos, NULL) != 0) {
		log_error(LOGGER,"No se pudo inicializar el semaforo del sistema de archivos");
		return EXIT_FAILURE;
		}
	return EXIT_SUCCESS;
}

int inicializarVariables(){
	finalizarPrograma=false;
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
	pthread_mutex_lock(&mutexIniciarConsola);
	//Lo uso para q la consola arranque despues de que inicie todo lo demas del otro hilo
	log_info(LOGGER,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL, funcionHiloConsola, "Hilo consola");
		if(resultadoDeCrearHilo){
			log_error(LOGGER,"Error al crear el hilo, return code: %d",resultadoDeCrearHilo);
			avisoDeFinalizarPrograma();
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
	char* ubicacionDelPunteroDeLaConsola;
	log_info(LOGGER,"Consola lista");
	printf("\n");
	while(1){
		ubicacionDelPunteroDeLaConsola=string_new();
		string_append(&ubicacionDelPunteroDeLaConsola,"~/");
		string_append(&ubicacionDelPunteroDeLaConsola,directorioActual);
		string_append(&ubicacionDelPunteroDeLaConsola,"$ ");
		linea = readline(ubicacionDelPunteroDeLaConsola);
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
			avisoDeFinalizarPrograma();
			return ret;
			//pthread_exit(ret);
			}else{
		if(strcmp(instruccion[0],"ls")==0){
			if(instruccion[1]==NULL){
				listarDirectorioActual();
			}else{
				listarDirectorioConParametro(instruccion[1]);
				}
		}else{
		if(strcmp(instruccion[0],"cd")==0){
			if(instruccion[1]!=NULL){
				funcionDeConsolacd(instruccion[1]);
			}else{
				printf("Faltan parametros, falta el path del directorio, reintentar\n");
				}
		}else{
		if(strcmp(instruccion[0],"md5")==0){
			if(instruccion[1]!=NULL){
				printf("Generando MD5 del archivo: %s\n",instruccion[1]);
				generarMD5(instruccion[1]);
			}else{
				printf("Faltan parametros para generar el MD5, falta el path del archivo,reintentar\n");
				}
		}else{
		if(strcmp(instruccion[0],"cat")==0){
			if(instruccion[1]!=NULL){
				funcionDeConsolacat(instruccion[1]);
			}else{
				printf("Faltan parametros para mostrar el contenido del archivo por pantalla, falta el path del archivo,reintentar\n");
				}
		}else{
		//Los que vienen a continuacion son un agredado para poder provar el FS
		if(strcmp(instruccion[0],"ValidarArchivo")==0){
			if(instruccion[1]!=NULL){
				printf("Voy a validar el archivo: %s\n",instruccion[1]);
				validarArchivoDeConsola(instruccion[1]);
			}else{
				printf("Faltan parametros para poder validar el archivo\n");
			}
		}else{
		if(strcmp(instruccion[0],"CrearArchivo")==0){
			if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)){
				printf("Voy a crear el archivo: %s con %d bytes\n",instruccion[1],atoi(instruccion[2]));
				crearArchivoDeConsola(instruccion[1],atoi(instruccion[2]));
			}else{
				printf("Faltan parametros para poder crear el archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"BorrarArchivo")==0){
			if(instruccion[1]!=NULL){
				printf("Voy a borrar el archivo: %s\n",instruccion[1]);
				borrarArchivoDeConsola(instruccion[1]);
			}else{
				printf("Falta el path para poder borrar el archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"ObtenerDatos")==0){
			if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)){
				printf("Voy a obtener datos del archivo: %s\n",instruccion[1]);
				obtenerDatosDeConsola(instruccion[1],atoi(instruccion[2]),atoi(instruccion[3]));
			}else{
				printf("Faltan parametros para obtener el archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"GuardarDatos")==0){
			if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)){
				printf("Voy a guardar datos de consola del archivo: %s\n",instruccion[1]);
				guardarDatosDeConsola(instruccion[1],atoi(instruccion[2]),atoi(instruccion[3]),instruccion[4]);
			}else{
				printf("Faltan parametros para guardar datos del archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"man")==0){
			printf("Voy a listar todas las instrucciones posibles:\n");
			man();
		}else{
		if(strcmp(instruccion[0],"mostrarbloque")==0){
			mostrarBloque(atoi(instruccion[1]));
		}else{
		if(strcmp(instruccion[0],"pwd")==0){
			pwd();
		}else{
		if(strcmp(instruccion[0],"config")==0){
			mostrarConfiguracion();
		}else{
		if(strcmp(instruccion[0],"bitmap")==0){
			imprimirEstadoDelBitmap();
		}else{
		if(strcmp(instruccion[0],"existeledirectorio")==0){
			if(instruccion[1]!=NULL){
				existeElDirectorioDeConsola(instruccion[1]);
			}else{
				printf("Faltan parametros\n");
				}
		}else{
		printf("Comando desconocido\n");
		}}}}}}}}}}}}}}}}}
		free(linea);
		//for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
		//free(instruccion);
		free(ubicacionDelPunteroDeLaConsola);
	}//Cierre del while(1)
	resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
	return ret;
	//pthread_exit(ret);
}

int mostrarConfiguracion(){
	printf("Mostrando configuracion\n");
	printf("Puerto: %d\n", configuracionDelFS.puerto);
	printf("Punto de montaje: %s\n", configuracionDelFS.punto_montaje);
	printf("Retardo: %d\n", configuracionDelFS.retardo);
	printf("Cantidad de bloques: %d\n", configuracionDeMetadata.cantidadBloques);
	printf("Magic number: %s\n", configuracionDeMetadata.magicNumber);
	printf("Tamaño de bloques: %d\n", configuracionDeMetadata.tamanioBloques);
	return EXIT_SUCCESS;
}

int imprimirEstadoDelBitmap(){
	int i;
	int bloquesLibres=0;
	int bloquesOcupados=0;
	printf("Cantidad total de bloques: %d\n",configuracionDeMetadata.cantidadBloques);
	printf("Imprimiendo estado de los bloques\n");
	printf("Bloques libres:\n");
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	for(i=0;i<configuracionDeMetadata.cantidadBloques;i++){
		if(!bitarray_test_bit(bitmap,i)){
			printf("%d; ",i);
			bloquesLibres++;
			}
		}
	printf("\nBloques ocupados:\n");
	for(i=0;i<configuracionDeMetadata.cantidadBloques;i++){
		if(bitarray_test_bit(bitmap,i)){
			printf("%d; ",i);
			bloquesOcupados++;
			}
		}
	printf("\n");
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return EXIT_SUCCESS;
}

int man(){
	printf("1) \"exit\" cierra la consola\n");
	printf("2) \"ls\" [path del directorio(opcional)] Lista los directorios y archivos dentro del directorio pasado por parametro\n");
	printf("   Si no se pasa ningún parámetro, se usa el directorio actual\n");
	printf("3) \"cd\" [path del directorio] Se cambia el directorio actual al pasado por parámetro\n");
	printf("   El directorio “.” indica el directorio actual y “..” indica el directorio anterior\n");
	printf("4) \"md5\" [path del archivo] Genera el md5 del archivo y lo muestra por pantalla\n");
	printf("5) \"cat\" [path del archivo] Muestra el contenido de un archivo por pantalla, como una cadena\n");
	printf("Comandos propios:\n");
	printf("6) \"ValidarArchivo\" [Path] deberá validar que el archivo exista\n");
	printf("7) \"CrearArchivo\" [Path][N cantidad de bytes] creará el archivo dentro del path solicitado\n");
	printf("   El archivo creado deberá tener la cantidad de bloques necesarios para guardar las\n");
	printf("   líneas indicadas por la operación crear con su contenido vacío\n");
	printf("8) \"ObtenetDatos\" [Path, Offset, Size]  devolverá del path enviado por parámetro,\n");
	printf("   la cantidad de bytes definidos por el Size a partir del offset solicitado.\n");
	printf("9) \"GuardarDatos\" [Path, Offset, Size, Buffer] almacenará en el path enviado por parámetro,\n");
	printf("   los bytes del buffer, definidos por el valor del Size y a partir del offset solicitado. En caso de\n");
	printf("   que se soliciten datos o se intenten guardar datos en un archivo inexistente el File System\n");
	printf("   deberá retornar un error de Archivo no encontrado\n");
	printf("10) \"config\", me muestra la data que levante de la configracion\n");
	printf("11) \"bitmap\", muestra el estado de todos los bloques del bitmap\n");
	printf("12) \"BorrarArchivo\" [Path], borra el archivo solicitado\n");
	printf("13) \"pwd\", print working directory\n");
	printf("14) \"mostrarbloque\" [NumeroDeBloque]");
	printf("15) \"existeledirectorio\" [path], me dice si el directorio que le paso existe o no");
	return EXIT_SUCCESS;
}

char* directorioDeUsuario(){
	log_info(LOGGER,"directorioActual=%s",directorioActual);
	char** pathPartido = string_split(directorioActual, "/");
	char* directorio=string_new();
	bool encontreArchivo=false;
	int i;
	for(i=0;pathPartido[i]!=NULL;i++){
		if(encontreArchivo){
			string_append(&directorio,pathPartido[i]);
			if(pathPartido[i+1]!=NULL){
				string_append(&directorio,"/");
				}
			}
		if(strcmp(pathPartido[i],"Archivos")==0)
			encontreArchivo=true;
		}
	for(i=0;pathPartido[i]!=NULL;i++){
		free(pathPartido[i]);
		}
	return directorio;
}

int pwd(){
	char* directorio=directorioDeUsuario();
	printf("El working directory es: \"%s\"\n",directorio);
	free(directorio);
	return EXIT_SUCCESS;
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
	log_info(LOGGER,"Desmapeando el bitmap");
	msync(bitmap, tamanioBitmap, MS_SYNC);
	munmap(bitmap,tamanioBitmap);
	log_info(LOGGER,"Destruyendo el bitarray");
	bitarray_destroy(bitmap);
	log_info(LOGGER,"Cerrando log");
	log_destroy(LOGGER);
	pthread_mutex_unlock(&mutexIniciarConsola);
	pthread_mutex_destroy(&mutexIniciarConsola);
	pthread_mutex_destroy(&mutexFinalizarPrograma);
	pthread_mutex_destroy(&mutexPath);
	pthread_mutex_destroy(&mutexSistemaDeArchivos);
	free(configuracionDelFS.punto_montaje);
	free(configuracionDeMetadata.magicNumber),
	printf("PROGRAMA FINALIZADO\n");
}


int obtenerLongigutDelArchivo(char* path){
	int longitudDelArchivo=0;
	char* ubicacionDelArchivo;
	ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	log_info(LOGGER,"Buscando el archivo %s",ubicacionDelArchivo);

	pthread_mutex_lock(&mutexSistemaDeArchivos);
	t_config* configuracion = config_create(ubicacionDelArchivo);

	if(configuracion!=NULL){
		log_info(LOGGER,"El archivo %s existe",ubicacionDelArchivo);
		log_info(LOGGER,"Abriendo el archivo de configuracion del FS, su ubicacion es: %s",ubicacionDelArchivo);
		if(!config_has_property(configuracion,"TAMANIO")) {
			log_error(LOGGER,"No esta el valor TAMANIO en el archivo");
			config_destroy(configuracion);
			pthread_mutex_unlock(&mutexSistemaDeArchivos);
			return longitudDelArchivo;
			}
		longitudDelArchivo = config_get_int_value(configuracion,"TAMANIO");
		log_info(LOGGER,"El tamaño del archivo fue recuperado: %d",longitudDelArchivo);
	}else{
		log_error(LOGGER,"No existe el archivo %s",ubicacionDelArchivo);
		pthread_mutex_unlock(&mutexSistemaDeArchivos);
		return longitudDelArchivo;
		}
	log_info(LOGGER,"Cerrando el archivo %s, info recuperada",ubicacionDelArchivo);
	config_destroy(configuracion);
	free(ubicacionDelArchivo);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return longitudDelArchivo;
}

static void MDPrint (digest)
unsigned char digest[16];{
	unsigned int i;
	for (i = 0; i < 16; i++)
		printf ("%02x", digest[i]);
}

int generarMD5(char* pathDelArchivo){
	pthread_mutex_lock(&mutexUsoDelCanalDeComunicacionDelDMA);
	int longitudDelArchivo=obtenerLongigutDelArchivo(pathDelArchivo);
	t_datosObtenidos datosObtenidos = obtenerDatos(pathDelArchivo,0,longitudDelArchivo);
	pthread_mutex_unlock(&mutexUsoDelCanalDeComunicacionDelDMA);
	char*content=datosObtenidos.datos;
	if(datosObtenidos.resultado==DatosObtenidos){
		printf("Los datos del archivo %s son:\n",pathDelArchivo);
		for(int i=0;i<longitudDelArchivo;i++) printf("%c",datosObtenidos.datos[i]);
		printf("\n");
	}else{
		printf("No se pudieron recuperar los datos del archivo:%s\n",pathDelArchivo);
		return EXIT_FAILURE;
		}
	//unsigned char digest[16];
	void * digest = malloc(MD5_DIGEST_LENGTH);
	//caddr_t digest = (caddr_t)malloc(16);
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, content, longitudDelArchivo);
	MD5_Final(digest, &context);
	free(content);
	printf("Resultado del MD5:\n");
	MDPrint (digest);
	printf("\n");
	free(digest);
	free(datosObtenidos.datos);
	return EXIT_SUCCESS;
}

int mostrarBloque(int numeroDeBloque){
	char *archivoDeBloque=string_new();
	string_append(&archivoDeBloque,configuracionDelFS.punto_montaje);
	string_append(&archivoDeBloque, "/Bloques/");
	string_append(&archivoDeBloque, string_itoa(numeroDeBloque));
	string_append(&archivoDeBloque, ".bin");
	log_info(LOGGER,"Mostrando el bloque: %d que esta en el archivo: %s",numeroDeBloque,archivoDeBloque);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	FILE * archivo = fopen(archivoDeBloque,"rb+");
	char caracter;
	if(archivo!=NULL){
		while((caracter = fgetc(archivo)) != EOF) printf("%c",caracter);
		}
	printf("\n");
	free(archivoDeBloque);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return EXIT_SUCCESS;
}

int funcionDeConsolacat(char* path){
	pthread_mutex_lock(&mutexUsoDelCanalDeComunicacionDelDMA);
	printf("Mostrando el contenido del archivo %s por pantalla\n",path);
	int longitudDelArchivo=obtenerLongigutDelArchivo(path);
	t_datosObtenidos datosObtenidos = obtenerDatos(path,0,longitudDelArchivo);
	pthread_mutex_unlock(&mutexUsoDelCanalDeComunicacionDelDMA);
	if(datosObtenidos.resultado==DatosObtenidos){
		printf("Los datos del archivo %s son:\n",path);
		for(int i=0;i<longitudDelArchivo;i++) printf("%c",datosObtenidos.datos[i]);
		printf("\n");
	}else{
		printf("No se pudieron recuperar los datos del archivo:%s\n",path);
		}
	free(datosObtenidos.datos);
	return EXIT_SUCCESS;
}

int funcionDeConsolacd(char* path){
	log_info(LOGGER,"Pedido por consola del comando \"cd\", con el parametro: %s",path);
	char** pathPartido = string_split(path, "/");
	char* copiaDelDirectorioActual=string_new();
	pthread_mutex_lock(&mutexPath);
	string_append(&copiaDelDirectorioActual,directorioActual);
	int i;
	for(i=0;pathPartido[i]!=NULL;i++){
		if(strcmp(path, "..")==0){
			volverUnaCarpetaParaAtras();
		}else{
			if(strcmp(path, ".")==0){
				//nada
			}else{
				agregarCarpetaAlDirectorioActual(pathPartido[i]);
				}
			}
		}

	if(existeElDirectorio(directorioActual)){
		printf("Cambiando al directorio %s\n",directorioActual);
		free(copiaDelDirectorioActual);
		pthread_mutex_unlock(&mutexPath);
		return EXIT_SUCCESS;
	}else{
		printf("Error no existe el directorio %s\n",directorioActual);
		directorioActual=string_new();
		string_append(&directorioActual,copiaDelDirectorioActual);
		printf("Volviendo al directorio %s\n",directorioActual);
		free(copiaDelDirectorioActual);
		pthread_mutex_unlock(&mutexPath);
		return EXIT_FAILURE;
		}
}

void volverUnaCarpetaParaAtras(){
	log_info(LOGGER,"Volviendo una carpeta para atras");
	char** pathPartido = string_split(directorioActual, "/");
	free(directorioActual);
	directorioActual=string_new();
	for(int i=0;pathPartido[i+1]!=NULL;i++){
		string_append(&directorioActual,pathPartido[i]);
		if(pathPartido[i+2]!=NULL){
			string_append(&directorioActual,"/");
			}
		//ver q pasa con el primero
		}
	//free(directorioActual);
	}

void agregarCarpetaAlDirectorioActual(char* carpeta){
	log_info(LOGGER,"Agregando la capeta %s al directorio actual",carpeta);
	string_append(&directorioActual,"/");
	string_append(&directorioActual,carpeta);
}

int listarDirectorio(char* directorio){
	printf("Voy a listar el directorio: %s\n",directorio);
	struct dirent **namelist;

	int n;
	int elementosEnElDirectorio=scandir(directorio, &namelist, NULL, alphasort);
	n=elementosEnElDirectorio;
	if(n==-1){
		perror("scandir");
		return EXIT_FAILURE;
		}
	log_info(LOGGER,"Cantidad de elementos a listar: %d",n);
	while((n--)>2){
		if(namelist[n]->d_type==DT_DIR){
			//A directory.
			printf("%-40.40s | dir\n", namelist[n]->d_name);
			}
		if(namelist[n]->d_type==DT_REG){
			//A regular file.
			printf("%-40.40s | file\n", namelist[n]->d_name);
			}

		free(namelist[n]);
		}
	free(namelist);
	log_info(LOGGER,"Se listo el directorio actual: %s",directorioActual);
	return EXIT_SUCCESS;
	/*
	Tipos de valores para d_type:
	DT_UNKNOWN:	The type is unknown. Only some filesystems have full support to return the type of the file, others might always return this value.
	DT_REG:	A regular file.
	DT_DIR:	A directory.
	DT_FIFO: A named pipe, or FIFO. See FIFO Special Files.
	DT_SOCK: A local-domain socket.
	DT_CHR: A character device.
	DT_BLK: A block device.
	DT_LNK: A symbolic link.
	*/
}

int listarDirectorioConParametro(char* path){
	char* directorio=string_new();
	string_append(&directorio,configuracionDelFS.punto_montaje);
	string_append(&directorio, "/Archivos/");
	string_append(&directorio, path);
	printf("Listando directorio pasado por parametro, %s\n",directorio);
	pthread_mutex_lock(&mutexPath);
	int resultado = listarDirectorio(directorio);
	free(directorio);
	pthread_mutex_lock(&mutexPath);
	return resultado;
}

int listarDirectorioActual(){
	printf("Listando directorio actual, %s\n",directorioActual);
	pthread_mutex_lock(&mutexPath);
	int resultado=listarDirectorio(directorioActual);
	pthread_mutex_unlock(&mutexPath);
	return resultado;
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
			free(ubicacionDelArchivo);
			return EXIT_FAILURE;
			}
		configuracionDeMetadata.tamanioBloques = config_get_int_value(configuracion,"TAMANIO_BLOQUES");
		log_info(LOGGER,"Tamaño de los bloques del archivo \"Metadata.bin\" recuperado: %d",
				configuracionDeMetadata.tamanioBloques);

		//Recupero la CANTIDAD_BLOQUES
		if(!config_has_property(configuracion,"CANTIDAD_BLOQUES")) {
			log_error(LOGGER,"No esta la cantidad de bloques en el archivo \"Metadata.bin\"");
			config_destroy(configuracion);
			free(ubicacionDelArchivo);
			return EXIT_FAILURE;
			}
		configuracionDeMetadata.cantidadBloques = config_get_int_value(configuracion,"CANTIDAD_BLOQUES");
		log_info(LOGGER,"Cantidad de bloques del archivo \"Metadata.bin\" recuperado: %d",
				configuracionDeMetadata.cantidadBloques);

		//Recupero el MAGIC_NUMBER
		if(!config_has_property(configuracion,"MAGIC_NUMBER")) {
			log_error(LOGGER,"No esta el \"Magic number\" en el archivo \"Metadata.bin\"");
			config_destroy(configuracion);
			free(ubicacionDelArchivo);
			return EXIT_FAILURE;
			}
		char* magicNumber = config_get_string_value(configuracion,"MAGIC_NUMBER");
		configuracionDeMetadata.magicNumber = malloc(strlen(magicNumber)+1);
		strcpy(configuracionDeMetadata.magicNumber,magicNumber);
		log_info(LOGGER,"\"Magic number\" del archivo \"Metadata.bin\" recuperado: %s",
				configuracionDeMetadata.magicNumber);

	}else{
		log_error(LOGGER,"No existe el archivo \"Metadata.bin\"");
		free(ubicacionDelArchivo);
		return EXIT_FAILURE;
		}
	log_info(LOGGER,"Cerrando \"Metadata.bin\", info recuperada");
	config_destroy(configuracion);
	free(ubicacionDelArchivo);
	return EXIT_SUCCESS;
}

bool existeElDirectorioDeConsola(char* path){
	bool resultado = existeElDirectorio(path);
	if(resultado){
		printf("El directorio \"%s\" existe",path);
	}else{
		printf("El directorio \"%s\" no existe",path);
		}
	return resultado;
}

bool existeElDirectorio(char* path){
	DIR * directorio = opendir(path);
	if(directorio==NULL){
		log_info(LOGGER,"El directorio \"%s\" no existe",path);
		return false;
	}else{
		log_info(LOGGER,"El directorio \"%s\" existe",path);
		return true;
	}
}

bool existeElArchivo(char *directorioDelArchivo){
	/* Tengo q hacer un open con write si o si xq:
	 * The fopen() function will fail if:
	 * [EISDIR] The named file is a directory and mode requires write access.
	 * sino lo hago asi cuando abro un directorio me dice q esta todo bien
	 * "r+": Opens a file to update both reading and writing. The file must exist.
	 * */

	FILE *fd=fopen(directorioDelArchivo, "r+");
	if(fd==NULL){
		log_info(LOGGER,"No existe el archivo %s",directorioDelArchivo);
		return false;
	}else{
		log_info(LOGGER,"Si existe el archivo %s",directorioDelArchivo);
		fclose(fd);
		return true;
  		}
}

int obtenerBloqueLibreDelBitMap(){
	/* Retorna un numero de bloque libre en el bitmap
	 * Si no hay mas bloques libre retorna -1 */
	int i;
	log_info(LOGGER,"Obteniendo bloque libre");
	for(i=0;i<configuracionDeMetadata.cantidadBloques;i++){
		if(!bitarray_test_bit(bitmap,i)){
			log_info(LOGGER,"El bloque %d esta libre", i);
			return i;
			}
		log_info(LOGGER,"El bloque: %d esta ocupado",i);
		}
	log_info(LOGGER,"No hay mas bloques libres");
	return -1;
}

int reservarBloqueYCrearEstructuras(int numeroDeBloqueLibre){
	return EXIT_SUCCESS;
}

int levantarBitMap(){
	FILE *archivoBitmap;
	char* ubicacionDelArchivo;
	bool estaCreadoElBitmap;
	bool estaVacioElBitmap=false;
	ubicacionDelArchivo=string_new();
	tamanioBitmap=configuracionDeMetadata.cantidadBloques * sizeof(char);
	string_append(&ubicacionDelArchivo,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Metadata/Bitmap.bin");
	log_info(LOGGER,"Buscando el archivo \"Bitmap.bin\" en el directorio: %s",ubicacionDelArchivo);

	int FDbitmap = open(ubicacionDelArchivo, O_RDWR);

	if(FDbitmap==-1){
		log_error(LOGGER,"No se pudo abrir el file descriptor del archivo de bitmap %s",ubicacionDelArchivo);
		archivoBitmap=fopen(ubicacionDelArchivo, "wb");
		for(int i=0;i<tamanioBitmap;i++){
			fprintf(archivoBitmap,"0");
			}
		fclose(archivoBitmap);
		FDbitmap = open(ubicacionDelArchivo, O_RDWR);
		estaCreadoElBitmap=false;
	}else{
		log_info(LOGGER,"Se abrio el file descriptor del archivo de bitmap%s",ubicacionDelArchivo);
		estaCreadoElBitmap=true;
		}

	struct stat mystat;

	if(fstat(FDbitmap, &mystat) < 0) {
	    log_error(LOGGER,"Error al establecer fstat");
	    close(FDbitmap);
		}

	if(mystat.st_size==0){
		estaVacioElBitmap=true;
		log_error(LOGGER,"El archivo esta vacio y no tiene nada para mapearlo a memeoria");
		close(FDbitmap);
		archivoBitmap=fopen(ubicacionDelArchivo, "wb");
		for(int i=0;i<tamanioBitmap;i++){
			fprintf(archivoBitmap,"0");
			}
		fclose(archivoBitmap);
		FDbitmap = open(ubicacionDelArchivo, O_RDWR);
	}else{
		estaVacioElBitmap=false;
		}

	char * src = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, FDbitmap, 0);

	if(src == MAP_FAILED){
		log_error(LOGGER,"Error al mapear a memoria: %s",strerror(errno));
		log_info(LOGGER,"Es probable que no este creado el archivo o este vacio, paso a crearlo y llenarlo con basura");
	}else{
		log_info(LOGGER,"MAP exitoso");
		}

	char * bufferArchivo = malloc(tamanioBitmap);

	memcpy(bufferArchivo, src, tamanioBitmap);

	if(!estaCreadoElBitmap||estaVacioElBitmap){
		log_info(LOGGER,"El archivo de bitmap no existia");
		log_info(LOGGER,"Generando el BITMAP con los datos de \"Bitmap.bin\" en: %s",ubicacionDelArchivo);
		bzero(bufferArchivo,configuracionDeMetadata.cantidadBloques);
	}else{
		log_info(LOGGER,"El archivo de bitmap existia");
		}

	bitmap = bitarray_create_with_mode(bufferArchivo,tamanioBitmap,MSB_FIRST);

	if(!estaCreadoElBitmap||estaVacioElBitmap){
		for(int i=0;i<configuracionDeMetadata.cantidadBloques;i++){
			bitarray_clean_bit(bitmap,i);
			}

		}
	close(FDbitmap);
	free(ubicacionDelArchivo);
	//free(bufferArchivo);
	return EXIT_SUCCESS;
}

int bajarADiscoBitmap(){
	msync(bitmap, tamanioBitmap, MS_SYNC);
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

void *hiloDePedidoDeDMA(void* arg){
	int cabecera = (int)arg;
	iniciarTrabajoConElDMA(cabecera);
	log_info(LOGGER,"Finalizando pedido del DMA");
	free(arg);
	pthread_mutex_unlock(&mutexUsoDelCanalDeComunicacionDelDMA);
	return EXIT_SUCCESS;
}

void *funcionHiloComunicacionConElDMA(void *arg){
	pthread_attr_t attr;
	pthread_t thread;
	t_cabecera cabecera;
	int * valorDeLaCabecera;
	log_info(LOGGER,"Esperando conexion entrante del DMA por el puerto: %d", configuracionDelFS.puerto);
	int port = configuracionDelFS.puerto;
	int sockDelServer = escucharEn(port); //crea servidor
	if (pthread_attr_init(&attr)!=0){
		log_error(LOGGER, "Fallo la creacion de attr del hilo");
		exit(EXIT_FAILURE);
		}
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=0){
		log_error(LOGGER, "No se pudo setear dettached state");
		exit(EXIT_FAILURE);
		}
	pthread_mutex_unlock(&mutexIniciarConsola);
	FDDMA = aceptarConexion(sockDelServer);
	if(recibirHandshake(FS,DMA,FDDMA) > 0){
		//hago el handshake
		log_info(LOGGER,"Handshake exitoso con el DMA :), por el FD %d",FDDMA);
	}else{
		log_error(LOGGER,"El proceso no es el esperado por el FD %d",FDDMA);
		return EXIT_FAILURE;
		}

	log_info(LOGGER,"Me pongo a esperar pedidos");

	while(!hayQueFinalizarElPrograma()){
		pthread_mutex_lock(&mutexUsoDelCanalDeComunicacionDelDMA);
		cabecera = recibirCabecera(FDDMA);
		if(cabecera.tamanio>0){
			log_info(LOGGER,"Recibi un dato");
			log_info(LOGGER,"Cabecera recibida: %d, cantidad de bytes: %d",
				cabecera.tipoDeMensaje, cabecera.tamanio);
			valorDeLaCabecera=malloc(sizeof(int));
			valorDeLaCabecera=(int)cabecera.tipoDeMensaje;
			pthread_create(&thread, &attr,&hiloDePedidoDeDMA, valorDeLaCabecera);
			log_info(LOGGER,"Voy a atender una conexion por el FD: %d", FDDMA);
			}
		}
	log_info(LOGGER,"Cierro la conexion");
	cerrarConexion(FDDMA);
	log_info(LOGGER,"Destruyo el hilo");
	pthread_attr_destroy(&attr);
	return EXIT_SUCCESS;
}

bool hayQueFinalizarElPrograma(){
	bool resultado;
	pthread_mutex_lock(&mutexFinalizarPrograma);
	resultado=finalizarPrograma;
	pthread_mutex_unlock(&mutexFinalizarPrograma);
	return resultado;
}

int avisoDeFinalizarPrograma(){
	pthread_mutex_lock(&mutexFinalizarPrograma);
	finalizarPrograma=true;
	pthread_mutex_unlock(&mutexFinalizarPrograma);
	log_warning(LOGGER,"Hay que finalizar el programa");
	return EXIT_SUCCESS;
}

int iniciarTrabajoConElDMA(int cabecera){
	log_info(LOGGER,"La cabecera que recibi es: %d", cabecera);
	switch(cabecera){
		case ValidarArchivo:
			log_warning(LOGGER,"Pedido del DMA de \"ValidarArchivo\"");
			validarArchivoDeDMA(FDDMA);
			break;
		case CrearArchivo:
			log_warning(LOGGER,"Pedido del DMA de \"CrearArchivo\"");
			crearArchivoDeDMA(FDDMA);
			break;
		case ObtenerDatos:
			log_warning(LOGGER,"Pedido del DMA de \"ObtenerDatos\"");
			obtenerDatosDeDMA(FDDMA);
			break;
		case GuardarDatos:
			log_warning(LOGGER,"Pedido del DMA de \"GuardarDatos\"");
			guardarDatosDeDMA(FDDMA);
			break;
		case BorrarArchivo:
			log_warning(LOGGER,"Pedido del DMA de \"BorrarArchivo\"");
			borrarArchivoDeDMA(FDDMA);
			break;
		default:
			log_error(LOGGER,"Error, me llego un tipo de mensaje del DMA desconocido, %d",cabecera);
			pthread_mutex_unlock(&mutexUsoDelCanalDeComunicacionDelDMA);
			return EXIT_FAILURE;
			break;
		}//end swith
	log_warning(LOGGER,"Pedido finalizado");
	return EXIT_SUCCESS;
}

int validarArchivoDeConsola(char *path){
	log_info(LOGGER,"Recibiendo el path: %s, para validar el archivo",path);
	if(validarArchivo(path)==ElArchivoExiste){
		printf("El archivo %s existe\n",path);
	}else{
		printf("El archivo %s no existe\n",path);
	}
	return EXIT_SUCCESS;
}

int validarArchivoDeDMA(int FDDMA){
	char*path=prot_recibir_DMA_FS_path(FDDMA);
	log_info(LOGGER,"Recibiendo el path: \"%s\", para validar el archivo",path);
	enviarCabecera(FDDMA, validarArchivo(path), 1);
	log_info(LOGGER,"Enviando respuesta de validar archivo al DMA");
	return EXIT_SUCCESS;
}

int validarArchivo(char *ubicacionDelArchivo){
	/* Parámetros​: [Path]
	 * Descripción​: Cuando el El Diego reciba la operación de abrir un archivo deberá validar
	 * que el archivo exista.
	 */
	int resultado;
	char*path=string_new();
	string_append(&path, configuracionDelFS.punto_montaje);
	string_append(&path, "/Archivos/");
	string_append(&path, ubicacionDelArchivo);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	log_info(LOGGER,"Voy a ver si existe el archivo: %s",path);
	if(existeElArchivo(path)){
		resultado = ElArchivoExiste;
	}else{
		resultado = ElArchivoNoExiste;
		}
	free(path);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return resultado;
}

int crearArchivoDeConsola(char *path, int cantidadDeBytes){
	log_info(LOGGER,"Recibiendo el path: %s, para crear el archivo, con %d bytes",path,cantidadDeBytes);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	if(crearArchivo(ubicacionDelArchivo, cantidadDeBytes, path)==ArchivoCreado){
		free(ubicacionDelArchivo);
		return EXIT_SUCCESS;
	}else{
		free(ubicacionDelArchivo);
		return EXIT_FAILURE;
		}
}

int crearArchivoDeDMA(int FDDMA){
	/* Recibe del DMA los valores: path
	 */
	tp_crearArchivo dataParaCrearElArchivo=prot_recibir_DMA_FS_CrearArchivo(FDDMA);
	int cantidadDeBytes=0; //modificar
	log_info(LOGGER,"Recibiendo el path: \"%s\", para crear el archivo",dataParaCrearElArchivo->path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, dataParaCrearElArchivo->path);
	int resultadoDeCrearElArchivo=crearArchivo(ubicacionDelArchivo, dataParaCrearElArchivo->size,
			dataParaCrearElArchivo->path);
	enviarCabecera(FDDMA, resultadoDeCrearElArchivo, 1);
	log_info(LOGGER,"Enviando respuesta de crear archivo al DMA (%d)",resultadoDeCrearElArchivo);
	free(ubicacionDelArchivo);
	return resultadoDeCrearElArchivo;
}

int crearCarpetas(char *carpetasACrear){
	//Recibe una lista de carpetas y si no existen las crea, el ultimo de la lista
	//es la ubicacion del archivo por lo q con eso no hace nada
	int i;
	char** carpetas = string_split(carpetasACrear, "/");
	for(i=0;carpetas[i]!=NULL;i++);
	char*directorio=string_new();
	string_append(&directorio,configuracionDelFS.punto_montaje);
	mkdir(configuracionDelFS.punto_montaje,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	string_append(&directorio,"/Archivos");
	mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	string_append(&directorio,"/");
	for(int j=0;(j<(i-1))&&(carpetas[j]!=NULL);j++){
		string_append(&directorio,carpetas[j]);
		mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		log_info(LOGGER,"Creando directorio: %s",directorio);
		if(j<i-2)
			string_append(&directorio,"/");
	}
	mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",directorio);
	free(directorio);
	return EXIT_SUCCESS;
}

int crearArchivo(char *ubicacionDelArchivo, int cantidadDeBytes, char *path){
	/* Parámetros: [Path, N cantidad de bytes]
	 * Descripción: Cuando el El Diego reciba la operación de crear un archivo deberá crear
	 * el archivo dentro del path solicitado con la cantidad de bloques necesarios para guardar
	 * la N cantidad de bytes pasados por parámetro. Además, el archivo nuevo deberá estar
	 * creado con "N" cantidad de caracteres "\n". Cabe aclarar que los archivos pueden crecer
	 * en bytes pero no en líneas.
	 * Ejemplo del contenido del archivo [Punto_Montaje]/Archivos/[PathDelArchivo] :
	 * TAMANIO=250
	 * BLOQUES=[40,21,82,3]
	 * ----------------------------------
	 * Ejemplo de contenido:
	 * path: carpeta/asd.txt
	 * ubicacionDelArchivo: montaje/Archivos/carpeta/asd.txt
	 * */

	if(cantidadDeBytes<0){
		log_error(LOGGER,"Error, la cantidad de bytes es menor que 0, es: %d",cantidadDeBytes);
		return ArchivoNoCreado;
		}
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	log_info(LOGGER,"Voy a crear el archivo en: %s, path:%s",ubicacionDelArchivo,path);
	if(!existeElArchivo(ubicacionDelArchivo)){
		//(cantidadDeBytes/configuracionDeMetadata.tamanioBloques)
		crearCarpetas(path);
		FILE *archivo=fopen(ubicacionDelArchivo, "w");
		if(archivo!=NULL){
			fprintf(archivo,"TAMANIO=0\n");
			fprintf(archivo,"BLOQUES=[]\n");
			fclose(archivo);
			log_info(LOGGER,"Archivo %s creado",ubicacionDelArchivo);
			if(cantidadDeBytes>0){
				log_info(LOGGER,"Ahora tengo que agregar los saltos de linea");
				char *buffer = string_new();
				for(int p=0;p<cantidadDeBytes;p++)
					string_append(&buffer,"\n");
				guardarDatos(path, 0, cantidadDeBytes, buffer);
				free(buffer);
				}
			pthread_mutex_unlock(&mutexSistemaDeArchivos);
			return ArchivoCreado;
		}else{
			pthread_mutex_unlock(&mutexSistemaDeArchivos);
			log_info(LOGGER,"No se pudo crear el archivo %s",ubicacionDelArchivo);
			return ArchivoNoCreado;
			}
	}else{
		pthread_mutex_unlock(&mutexSistemaDeArchivos);
		log_info(LOGGER,"El archivo ya existe");
		return ArchivoNoCreado;
	}
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
}

int borrarArchivoDeConsola(char *path){
	return borrarArchivo(path);
}

int borrarArchivoDeDMA(int fileDescriptorActual){
	char*path=prot_recibir_DMA_FS_path(fileDescriptorActual);
	int resultadoDelBorrado=borrarArchivo(path);
	enviarCabecera(fileDescriptorActual, resultadoDelBorrado, 1);
	log_info(LOGGER,"Resultado de borrar al archivo enviado al DMA (%d)",resultadoDelBorrado);
	return EXIT_SUCCESS;
}

int borrarArchivo(char *path){
	char *ubicacionDelArchivoDeMetadata=string_new();
	int numeroDeBloque,i;
	string_append(&ubicacionDelArchivoDeMetadata,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivoDeMetadata, "/Archivos/");
	string_append(&ubicacionDelArchivoDeMetadata,path);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	if(existeElArchivo(ubicacionDelArchivoDeMetadata)){
		tp_metadata metadata = recuperarMetaData(ubicacionDelArchivoDeMetadata);
		log_info(LOGGER,"Tamaño del archivo a borrar:",metadata->tamanio);
		for(i=0;i<list_size(metadata->bloques);i++){
			numeroDeBloque =(int)list_get(metadata->bloques,i);
			log_info(LOGGER,"Liberando el bloque: %d",numeroDeBloque);
			bitarray_clean_bit(bitmap,i);
			msync(bitmap, tamanioBitmap, MS_SYNC);
			}
		free(metadata);
		if(remove(ubicacionDelArchivoDeMetadata)==0){
			log_info(LOGGER,"Se elimino el archivo de metadata: %s",ubicacionDelArchivoDeMetadata);
			free(ubicacionDelArchivoDeMetadata);
			pthread_mutex_unlock(&mutexSistemaDeArchivos);
			return ArchivoBorrado;
		}else{
			log_error(LOGGER,"No se pudo eliminar el archivo de metadata: %s",ubicacionDelArchivoDeMetadata);
			free(ubicacionDelArchivoDeMetadata);
			pthread_mutex_unlock(&mutexSistemaDeArchivos);
			return ArchivoNoBorrado;
			}
	}else{
		log_error(LOGGER,"El archivo %s de metadata no existe, no lo puedo borrar",ubicacionDelArchivoDeMetadata);
		free(ubicacionDelArchivoDeMetadata);
		pthread_mutex_unlock(&mutexSistemaDeArchivos);
		return ArchivoNoBorrado;
		}
	free(ubicacionDelArchivoDeMetadata);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
}

int obtenerDatosDeConsola(char *path, int offset, int Size){
	t_datosObtenidos datosObtenidos = obtenerDatos(path,offset,Size);
	if(datosObtenidos.resultado==DatosObtenidos){
		printf("Los datos del archivo %s son:\n",path);
		for(int i=0;i<Size;i++) printf("%c",datosObtenidos.datos[i]);
		printf("\n");
	}else{
		printf("No se pudieron recuperar los datos del archivo:%s\n",path);
		}
	free(datosObtenidos.datos);
	return EXIT_SUCCESS;
	}

int obtenerDatosDeDMA(int fileDescriptorActual){
	log_info(LOGGER,"Voy a recibir los datos a guardar por el FD: %d",fileDescriptorActual);
	tp_obtenerDatos parametrosDeObtenerDatos = prot_recibir_DMA_FS_obtenerDatos(fileDescriptorActual);
	log_info(LOGGER,"Path:%s | Offset:%d | Size:%d",
		parametrosDeObtenerDatos->path,parametrosDeObtenerDatos->offset,parametrosDeObtenerDatos->size);
	log_info(LOGGER,"Llamo a la funcion para obtener los datos");
	t_datosObtenidos datosObtenidos = obtenerDatos(parametrosDeObtenerDatos->path,
		parametrosDeObtenerDatos->offset,parametrosDeObtenerDatos->size);
	int tamanioTotalDelArchivo=obtenerLongigutDelArchivo(parametrosDeObtenerDatos->path);
	prot_enviar_FS_DMA_datosObtenidos(datosObtenidos.datos, tamanioTotalDelArchivo,
			datosObtenidos.resultado, fileDescriptorActual);
	log_info(LOGGER,"Enviando respuesta de datos obtenidos al DMA");
	free(datosObtenidos.datos);
	return EXIT_SUCCESS;
}

t_datosObtenidos obtenerDatos(char *path, int offset, int size){
	/* Parámetros​: [Path, Offset, Size]
	 * Descripción​: Ante un pedido de datos File System devolverá del path enviado por parámetro,
	 * la cantidad de bytes definidos por el Size a partir del offset solicitado.
	 */
	log_info(LOGGER,"Obteniendo datos con los parametros: path \"%s\" | offset \"%d\" | size \"%d\" ",
			path, offset, size);
	t_datosObtenidos datosObtendios;
	int bytesLeidos=0;
	datosObtendios.datos=malloc(sizeof(char)*size);
	char *ubicacionDelArchivoDeMetadata=string_new();
	string_append(&ubicacionDelArchivoDeMetadata,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivoDeMetadata, "/Archivos/");
	string_append(&ubicacionDelArchivoDeMetadata,path);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	if(size>0){
		if(offset>=0){
			if(existeElArchivo(ubicacionDelArchivoDeMetadata)){
				log_info(LOGGER,"El archivo \"%s\" existe",ubicacionDelArchivoDeMetadata);
				tp_metadata metadata = recuperarMetaData(ubicacionDelArchivoDeMetadata);
				log_info(LOGGER,"Offset: %d, tamaño de bloques %d, size %d",
						offset, configuracionDeMetadata.tamanioBloques,size);
				int numeroDeBloqueDeInicioDeLectura=offset/configuracionDeMetadata.tamanioBloques;
				int numeroDeBloqueDeFinDeLectura=(offset+size)/configuracionDeMetadata.tamanioBloques;
				int leerEnPrimerArchivoDesde=offset%configuracionDeMetadata.tamanioBloques;
				int bytesALeer;
				int cantidadTotalDeBloquesCreados=list_size(metadata->bloques);
				int bloqueActual=numeroDeBloqueDeInicioDeLectura;
				log_info(LOGGER,"Voy a leer en el primer bloque desde: %d",leerEnPrimerArchivoDesde);
				log_info(LOGGER,"El numero de bloque de inicio de lectura es: %d",numeroDeBloqueDeInicioDeLectura);
				log_info(LOGGER,"El numero de bloque de fin de lectura es: %d",numeroDeBloqueDeFinDeLectura);

				for(int i=numeroDeBloqueDeInicioDeLectura;i<=numeroDeBloqueDeFinDeLectura;i++){
					int numeroDeBloque;
					char *archivoDeBloque=string_new();
					string_append(&archivoDeBloque,configuracionDelFS.punto_montaje);
					string_append(&archivoDeBloque, "/Bloques/");
					if(bloqueActual<cantidadTotalDeBloquesCreados){
						numeroDeBloque =(int)list_get(metadata->bloques,i);
						string_append(&archivoDeBloque, string_itoa(numeroDeBloque));
						string_append(&archivoDeBloque, ".bin");
						log_info(LOGGER,"Voy a leer en el bloque %s",archivoDeBloque);
					}else{
						log_error(LOGGER,"No me coincide el numero de bloque qeu quiero leer con la cantidad total de bloques que tiene el archivo");
						datosObtendios.resultado=ArchivoNoEncontrado;
						free(datosObtendios.datos);
						free(ubicacionDelArchivoDeMetadata);
						pthread_mutex_unlock(&mutexSistemaDeArchivos);
						return datosObtendios;
						}
					log_info(LOGGER,"Abriendo el bloque %s para leer",archivoDeBloque);
					FILE * archivo = fopen(archivoDeBloque,"rb+");
					//fwrite recibe: puntero a los datos, el tamaño de los registros, numero de registros, archivo
					if(archivo!=NULL){
						if((i!=numeroDeBloqueDeFinDeLectura)&&(i!=0)){
							bytesALeer=configuracionDeMetadata.tamanioBloques;
						}else{
							if(i==0){
								//Primer bloque
								bytesALeer=configuracionDeMetadata.tamanioBloques-leerEnPrimerArchivoDesde;
								fseek(archivo, leerEnPrimerArchivoDesde, SEEK_SET);
							}else{
								//Ultimo bloque
								bytesALeer=size-bytesLeidos;
								}
							}
						log_info(LOGGER,"Leyendo del archivo %s, %d bytes",archivoDeBloque,bytesALeer);
						fread(&datosObtendios.datos[bytesLeidos],sizeof(char),bytesALeer,archivo);
						bytesLeidos=bytesALeer+bytesLeidos;
						bloqueActual++;
						fclose(archivo);
					}else{
						log_error(LOGGER,"No se pudo abrir el archivo %s para leer",archivoDeBloque);
						datosObtendios.resultado=ArchivoNoEncontrado;
						free(datosObtendios.datos);
						free(ubicacionDelArchivoDeMetadata);
						free(archivoDeBloque);
						pthread_mutex_unlock(&mutexSistemaDeArchivos);
						return datosObtendios;
						}
					free(archivoDeBloque);
					}//fin del for

				log_info(LOGGER,"Se pudo recuperar todo el archivo %s devolviendo datos",ubicacionDelArchivoDeMetadata);
				datosObtendios.resultado=DatosObtenidos;
				free(ubicacionDelArchivoDeMetadata);
				pthread_mutex_unlock(&mutexSistemaDeArchivos);
				return datosObtendios;
			}else{
				log_error(LOGGER,"No se pudo encontrar el archivo: %s",path);
				}
		}else{
			log_error(LOGGER,"El offset es menor que 0, es:%d",offset);
			}
	}else{
		log_error(LOGGER,"El size es menor o igual que 0, es:%d",size);
		}
	datosObtendios.resultado=ArchivoNoEncontrado;
	free(datosObtendios.datos);
	free(ubicacionDelArchivoDeMetadata);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return datosObtendios;
}


int guardarDatosDeConsola(char *path, int offset, int size, char *Buffer){
	log_info(LOGGER,"Buffer: %s",Buffer);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	guardarDatos(path, offset, size, Buffer);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	return EXIT_SUCCESS;
}

int guardarDatosDeDMA(int fileDescriptorActual){
	log_info(LOGGER,"Voy a recibir los datos a guardar por el FD: %d",fileDescriptorActual);
	tp_obtenerDatos datos = prot_recibir_FS_DMA_guardarDatos(fileDescriptorActual);
	log_info(LOGGER,"Recibi los datos");
	log_info(LOGGER,"Path:%s | Offset:%d | Size:%d | Buffer:%s",
			datos->path,datos->offset,datos->size,datos->buffer);
	pthread_mutex_lock(&mutexSistemaDeArchivos);
	int resultadoDeGuardarDatos=guardarDatos(datos->path,datos->offset,datos->size,datos->buffer);
	pthread_mutex_unlock(&mutexSistemaDeArchivos);
	enviarCabecera(fileDescriptorActual, resultadoDeGuardarDatos, 1);
	log_info(LOGGER,"Enviando respuesta de guardar datos al DMA");
	return EXIT_SUCCESS;
}

int guardarDatos(char *path, int offset, int size, char *Buffer){
	/* Parámetros​: [Path, Offset, Size, Buffer]
	 * Descripción​: Ante un pedido de escritura MDJ almacenará en el path enviado por parámetro,
 	 * los bytes del buffer, definidos por el valor del Size y a partir del offset solicitado. En caso de
	 * que se soliciten datos o se intenten guardar datos en un archivo inexistente el File System
	 * deberá retornar un error de Archivo no encontrado.
	 */
	int escribiHasta=0;
	char *ubicacionDelArchivoDeMetadata=string_new();
	bool hayQueActualziarMetadataDelArchivo=false;
	string_append(&ubicacionDelArchivoDeMetadata,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivoDeMetadata, "/Archivos/");
	string_append(&ubicacionDelArchivoDeMetadata,path);
	if(size>0){
		if(offset>=0){
			if(existeElArchivo(ubicacionDelArchivoDeMetadata)){
				tp_metadata metadata = recuperarMetaData(ubicacionDelArchivoDeMetadata);
				log_info(LOGGER,"Offset: %d, tamaño de bloques %d, size %d",
						offset, configuracionDeMetadata.tamanioBloques,size);
				int numeroDeBloqueDeInicioDeEscritura=offset/configuracionDeMetadata.tamanioBloques;
				int numeroDeBloqueDeFinDeEscritura=(offset+size)/configuracionDeMetadata.tamanioBloques;
				int escribirEnPrimerArchivoDesde=offset%configuracionDeMetadata.tamanioBloques;
				int bytesAEscribir;
				int bytesEscritos=offset;
				int cantidadTotalDeBloquesCreados=list_size(metadata->bloques);
				int bloqueActual=numeroDeBloqueDeInicioDeEscritura;
				log_info(LOGGER,"Voy a escribir en el primer bloque desde: %d",escribirEnPrimerArchivoDesde);
				log_info(LOGGER,"El numero de bloque de inicio de escritura es: %d",numeroDeBloqueDeInicioDeEscritura);
				log_info(LOGGER,"El numero de bloque de fin de escritura es: %d",numeroDeBloqueDeFinDeEscritura);
				for(int i=numeroDeBloqueDeInicioDeEscritura;i<=numeroDeBloqueDeFinDeEscritura;i++){
					int numeroDeBloque;
					char *archivoDeBloque=string_new();
					string_append(&archivoDeBloque,configuracionDelFS.punto_montaje);
					string_append(&archivoDeBloque, "/Bloques/");
					if(bloqueActual<cantidadTotalDeBloquesCreados){
						numeroDeBloque =(int)list_get(metadata->bloques,i);
						string_append(&archivoDeBloque, string_itoa(numeroDeBloque));
						string_append(&archivoDeBloque, ".bin");
						log_info(LOGGER,"Voy a escribir en el bloque %s que ya esta creado",archivoDeBloque);
					}else{
						log_info(LOGGER,"El bloque para escribir no existe, lo tengo que crear");
						//El bloque no existe tengo que tomar uno vacio, crearlo y ademas actualizar la metadata
						int numeroDeBloqueLibre=obtenerBloqueLibreDelBitMap();
						if(numeroDeBloqueLibre!=-1){
							log_info(LOGGER,"Voy a escribir en el bloque %d",numeroDeBloqueLibre);
							//actualizo el bitarray
							bitarray_set_bit(bitmap,numeroDeBloqueLibre);
							msync(bitmap, tamanioBitmap, MS_SYNC);
							string_append(&archivoDeBloque, string_itoa(numeroDeBloqueLibre));
							//actualizo la lista y activo la bandera para actualizar mi archivo de metadata de ese archivo
							list_add(metadata->bloques,numeroDeBloqueLibre);
							log_info(LOGGER,"Creando el archivo de bloque%s",archivoDeBloque);
							FILE * archivoTemp = fopen(archivoDeBloque,"wb");
							fclose(archivoTemp);
							hayQueActualziarMetadataDelArchivo=true;
						}else{
							log_error(LOGGER,"No hay mas bloques libres");
							free(archivoDeBloque);
							free(ubicacionDelArchivoDeMetadata);
							return NoHayMasBloquesLibres;///no hay mas bloques libres
							}
						reservarBloqueYCrearEstructuras(numeroDeBloqueLibre);
						}
					log_info(LOGGER,"Abriendo el bloque %s para escribir",archivoDeBloque);
					FILE * archivo = fopen(archivoDeBloque,"rb+");
					//fwrite recibe: puntero a los datos, el tamaño de los registros, numero de registros, archivo
					if(archivo!=NULL){
						if((i!=numeroDeBloqueDeFinDeEscritura)&&(i!=numeroDeBloqueDeInicioDeEscritura)){
							bytesAEscribir=configuracionDeMetadata.tamanioBloques;
						}else{
							if(i==numeroDeBloqueDeInicioDeEscritura){
								//Primer bloque
								if(size>=configuracionDeMetadata.tamanioBloques){
									bytesAEscribir=configuracionDeMetadata.tamanioBloques;
								}else{
									bytesAEscribir=(size%configuracionDeMetadata.tamanioBloques);
									}
								log_info(LOGGER,"Bytes a escribir del primer bloque: %d",bytesAEscribir);
								fseek(archivo, escribirEnPrimerArchivoDesde, SEEK_SET);
								log_info(LOGGER,"Posicionando el puntero del archivo en %d",escribirEnPrimerArchivoDesde);
							}else{
								//Ultimo bloque
								bytesAEscribir=size-bytesEscritos;
								}
							}
						log_info(LOGGER,"Escribiendo en el archivo %s, desde %d, %d bytes",
								archivoDeBloque,bytesEscritos,bytesAEscribir);
						fwrite(&Buffer[bytesEscritos],sizeof(char),bytesAEscribir,archivo);
						char * bufferParaLog = string_new();
						for(int o=bytesEscritos;o<(bytesAEscribir+bytesEscritos);o++)
							string_append_with_format(&bufferParaLog, "%c", Buffer[o]);
						log_info(LOGGER,"Escribiendo en el FS: %s",bufferParaLog);
						free(bufferParaLog);
						bytesEscritos=bytesAEscribir+bytesEscritos;
						bloqueActual++;
						log_info(LOGGER,"Flusheando");
						fflush(archivo);
						fclose(archivo);
					}else{
						log_error(LOGGER,"No se pudo abrir el archivo %s para modificar",archivoDeBloque);
						}
					free(archivoDeBloque);
					}
				escribiHasta=offset+size;
				if(escribiHasta>metadata->tamanio){
					//tengo que actualizar la cantidad de datos q tiene el archivo
					metadata->tamanio=escribiHasta;
					hayQueActualziarMetadataDelArchivo=true;
					}
				if(hayQueActualziarMetadataDelArchivo){
					actualizarMetaData(ubicacionDelArchivoDeMetadata,metadata);
					}
				free(ubicacionDelArchivoDeMetadata);
				return DatosGuardados;
			}else{
				log_error(LOGGER,"No se pudo encontrar el archivo: %s",path);
				}
		}else{
			log_error(LOGGER,"El offset es menor que 0, es:%d",offset);
			}
	}else{
		log_error(LOGGER,"El size que quiero escribir es menor o igual a 0: %d",size);
		}
	free(ubicacionDelArchivoDeMetadata);
	return ArchivoNoEncontrado;
}

int actualizarMetaData(char* ubicacionDelArchivoDeMetadata,tp_metadata metadata){
	int i;
	log_info(LOGGER,"Actualizando metadata del archivo: %s",ubicacionDelArchivoDeMetadata);
	FILE * archivoTemp = fopen(ubicacionDelArchivoDeMetadata,"w");
	fprintf(archivoTemp,"TAMANIO=%d\n",metadata->tamanio);
	log_info(LOGGER,"	Tamaño actualizado: %d",metadata->tamanio);
	fprintf(archivoTemp,"BLOQUES=[");
	for(i=0;i<(list_size(metadata->bloques)-1);i++){
		log_info(LOGGER,"	Guardando el bloque: %d",(int)list_get(metadata->bloques,i));
		fprintf(archivoTemp,"%d,",(int)list_get(metadata->bloques,i));
		}
	log_info(LOGGER,"	Guardando el bloque: %d",(int)list_get(metadata->bloques,i));
	fprintf(archivoTemp,"%d",(int)list_get(metadata->bloques,i));
	fprintf(archivoTemp,"]");
	log_info(LOGGER,"	Bloques actualizados: %d",metadata->tamanio);
	fclose(archivoTemp);
	return EXIT_SUCCESS;
}

tp_metadata recuperarMetaData(char *ubicacionDelArchivoDeMetadata){
	tp_metadata metadata=malloc(sizeof(t_metadata));;
	metadata->tamanio=-1;
	metadata->bloques=NULL;
	t_config* configuracion = config_create(ubicacionDelArchivoDeMetadata);
	if(configuracion!=NULL){
		log_info(LOGGER,"Abriendo el archivo de metadata %s",ubicacionDelArchivoDeMetadata);
	}else{
		log_error(LOGGER,"No existe el archivo de metadata en: %s",ubicacionDelArchivoDeMetadata);
		return metadata;
		}
	//Recupero el tamanio
	if(!config_has_property(configuracion,"TAMANIO")) {
		log_error(LOGGER,"No esta el TAMANIO en el archivo de metadata");
		config_destroy(configuracion);
		return metadata;
		}
	metadata->tamanio = config_get_int_value(configuracion,"TAMANIO");
	log_info(LOGGER,"TAMANIO del archivo de metadata recuperado: %d", metadata->tamanio);

	//Recupero los BLOQUES
	if(!config_has_property(configuracion,"BLOQUES")) {
		log_error(LOGGER,"No estan los bloques en el archivo de metadata");
		config_destroy(configuracion);
		metadata->tamanio=-1;
		return metadata;
		}
	char* bloques = config_get_string_value(configuracion,"BLOQUES");
	//tengo que volarle a la mierda al string q levante los []
	char *aux= string_substring_from(bloques,1);
	bloques= string_substring_until(aux,strlen(aux)-1);
	log_info(LOGGER,"Bloques recuperados: %s", bloques);
	char** instruccion = string_split(bloques, ",");
	metadata->bloques=list_create();
	for(int i=0;instruccion[i]!=NULL;i++){
		int valor=atoi(instruccion[i]);
		list_add(metadata->bloques,valor);
		log_info(LOGGER,"Agregando el valor %d a la lista",atoi(instruccion[i]));
		}
	config_destroy(configuracion);
	return metadata;
}

