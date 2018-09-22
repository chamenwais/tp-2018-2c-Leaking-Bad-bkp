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
			if(instruccion[1]!=NULL){
				printf("Voy a crear el archivo: %s\n",instruccion[1]);
				crearArchivoDeConsola(instruccion[1]);
			}else{
				printf("Faltan parametros para poder crear el archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"ObtenerDatos")==0){
			if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)){
				printf("Voy a obtener datos del archivo: %s\n",instruccion[1]);
				obtenerDatosDeConsola(instruccion[1],instruccion[2],instruccion[3]);
			}else{
				printf("Faltan parametros para obtener el archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"GuardarDatos")==0){
			if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)){
				printf("Voy a guardar datos de consola del archivo: %s\n",instruccion[1]);
				guardarDatosDeConsola(instruccion[1],instruccion[2],instruccion[3],instruccion[4]);
			}else{
				printf("Faltan parametros para guardar datos del archivo\n");
				}
		}else{
		if(strcmp(instruccion[0],"man")==0){
			printf("Voy a listar todas las instrucciones posibles:\n");
			man();
		}else{
		printf("Comando desconocido\n");
		}}}}}}}}}}}
		free(linea);
		//for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
		//free(instruccion);
	}//Cierre del while(1)
	resultadoDeLaFinalizacionDeLaComunicacionConElDMA=EXIT_FAILURE;
	return resultadoDeLaFinalizacionDeLaComunicacionConElDMA;
	//pthread_exit(ret);
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
	printf("7) \"CrearArchivo\" [Path] creará el archivo dentro del path solicitado\n");
	printf("   El archivo creado deberá tener la cantidad de bloques necesarios para guardar las\n");
	printf("   líneas indicadas por la operación crear con su contenido vacío\n");
	printf("8) \"ObtenetDatos\" [Path, Offset, Size]  devolverá del path enviado por parámetro,\n");
	printf("   la cantidad de bytes definidos por el Size a partir del offset solicitado.\n");
	printf("9) \"GuardarDatos\" [Path, Offset, Size, Buffer] almacenará en el path enviado por parámetro,\n");
	printf("   los bytes del buffer, definidos por el valor del Size y a partir del offset solicitado. En caso de\n");
	printf("   que se soliciten datos o se intenten guardar datos en un archivo inexistente el File System\n");
	printf("   deberá retornar un error de Archivo no encontrado\n");
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
  if(fd==NULL){
	  log_info(LOGGER,"No existe el archivo %s",directorioDelArchivo);
	  return false;
  	  }
  log_info(LOGGER,"Si existe el archivo %s",directorioDelArchivo);
  fclose(fd);
  return true;
}

int obtenerBloqueLibreDelBitMap(){
	/* Retorna un numero de bloque libre en el bitmap
	 * Si no hay mas bloques libre retorna -1 */
	int i;
	for(i=0;i<configuracionDeMetadata.cantidadBloques;i++){
		if(bitarray_test_bit(bitmap,i)){
			log_info(LOGGER,"El bloque %d esta libre", i);
			return i;
			}
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

void *hiloDePedidoDeDMA(void* arg){
	int FD = (int)arg;
	int resultadoDelTrabajoConElDMA;
	if(recibirHandshake(FS,DMA,FD) > 0){
		//Inicio trabajo con el DMA
		log_info(LOGGER,"Handshake exitoso con el DMA :), por el FD %d",FD);
		iniciarTrabajoConElDMA(FD);
		resultadoDelTrabajoConElDMA=EXIT_SUCCESS;
	}else{
		log_error(LOGGER,"El proceso no es el esperado por el FD %d",FD);
		resultadoDelTrabajoConElDMA=EXIT_FAILURE;

		}
	log_info(LOGGER,"Finalizando pedido del DMA");
	return resultadoDelTrabajoConElDMA;
}

void *funcionHiloComunicacionConElDMA(void *arg){
	int FDDMA;
	pthread_attr_t attr;
	pthread_t thread;
	log_info(LOGGER,"Esperando conexion entrante del DMA por el puerto: %d", configuracionDelFS.puerto);
	int port = configuracionDelFS.puerto;
	int sockDelServer = escucharEn(port); //crea servidor
	while(1){
		//FDDMA=malloc(sizeof(int));
		FDDMA = aceptarConexion(sockDelServer);
		log_info(LOGGER,"Voy a atender una conexion por el FD: %d", FDDMA);
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr,&hiloDePedidoDeDMA, FDDMA);
		pthread_attr_destroy(&attr);
	}
	return EXIT_SUCCESS;
}

int iniciarTrabajoConElDMA(int fileDescriptorActual){
	t_cabecera cabecera;
	cabecera = recibirCabecera(fileDescriptorActual);
	if(cabecera.tamanio>0){
		log_info(LOGGER,"Cabecera recibida: %d, cantidad de bytes: %d",
				cabecera.tipoDeMensaje, cabecera.tamanio);
		switch(cabecera.tipoDeMensaje){
			case ValidarArchivo:
				log_info(LOGGER,"Pedido del DMA de \"ValidarArchivo\"");
				validarArchivoDeDMA(fileDescriptorActual);
				break;
			case CrearArchivo:
				log_info(LOGGER,"Pedido del DMA de \"CrearArchivo\"");
				crearArchivoDeDMA(fileDescriptorActual);
				break;
			case ObtenerDatos:
				log_info(LOGGER,"Pedido del DMA de \"ObtenerDatos\"");
				obtenerDatosDeDMA(fileDescriptorActual);
				break;
			case GuardarDatos:
				log_info(LOGGER,"Pedido del DMA de \"GuardarDatos\"");
				guardarDatosDeDMA();
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
	}
	return EXIT_SUCCESS;
}

int validarArchivoDeConsola(char *path){
	log_info(LOGGER,"Recibiendo el path: %s, para validar el archivo",path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	validarArchivo(ubicacionDelArchivo);
	return EXIT_SUCCESS;
}

int validarArchivoDeDMA(int FDDMA){
	char*path=prot_recibir_DMA_FS_path(FDDMA);
	log_info(LOGGER,"Recibiendo el path: %s, para validar el archivo",path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	enviarCabecera(FDDMA, validarArchivo(ubicacionDelArchivo), 1);
	return EXIT_SUCCESS;
}

int validarArchivo(char *path){
	/* Parámetros​: [Path]
	 * Descripción​: Cuando el El Diego reciba la operación de abrir un archivo deberá validar
	 * que el archivo exista.
	 */
	log_info(LOGGER,"Voy a ver si existe el archivo: %s",path);
	if(existeElArchivo(path)){
		return ElArchivoExiste;
	}else{
		return ElArchivoNoExiste;
		}
}

int crearArchivoDeConsola(char *path){
	log_info(LOGGER,"Recibiendo el path: %s, para crear el archivo",path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	if(crearArchivo(ubicacionDelArchivo,path)==ArchivoCreado)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

int crearArchivoDeDMA(int FDDMA){
	/* Recibe del DMA los valores: path
	 */
	char*path=prot_recibir_DMA_FS_path(FDDMA);
	log_info(LOGGER,"Recibiendo el path: %s, para crear el archivo",path);
	char*ubicacionDelArchivo=string_new();
	string_append(&ubicacionDelArchivo, configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivo, "/Archivos/");
	string_append(&ubicacionDelArchivo, path);
	int resultadoDeCrearElArchivo=crearArchivo(ubicacionDelArchivo,path);
	enviarCabecera(FDDMA, resultadoDeCrearElArchivo, 1);
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
	string_append(&directorio,"/Archivos/");
	for(int j=0;(j<(i-1))&&(carpetas[j]!=NULL);j++){
		string_append(&directorio,carpetas[j]);
		mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(j<i-2)
			string_append(&directorio,"/");
	}
	mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGER,"Directorio %s creado",directorio);
	return EXIT_SUCCESS;
}

int crearArchivo(char *ubicacionDelArchivo, char *path){
	/* Parámetros​: [Path]
	 * Descripción​: Cuando el El Diego reciba la operación de crear un archivo deberá llamar a esta
	 * operación que creará el archivo dentro del path solicitado. El archivo creado deberá tener la
	 * cantidad de bloques necesarios para guardar las líneas indicadas por la operación crear con su
	 * contenido vacío.
	 * Ejemplo del contenido del archivo [Punto_Montaje]/Archivos/[PathDelArchivo] :
	 * TAMANIO=250
	 * BLOQUES=[40,21,82,3]
	 * */
	log_info(LOGGER,"Voy a crear el archivo en: %s",ubicacionDelArchivo);
	crearCarpetas(path);
	FILE *archivo=fopen(ubicacionDelArchivo, "w");
	if(archivo!=NULL){
		fprintf(archivo,"%s","TAMANIO=0\n");
		fprintf(archivo,"%s","BLOQUES=[]\n");
		fclose(archivo);
		log_info(LOGGER,"Archivo %s creado",ubicacionDelArchivo);
		return 1;//ArchivoCreado;
	}else{
		log_info(LOGGER,"No se pudo crear el archivo %s",ubicacionDelArchivo);
		return ArchivoNoCreado;
		}
}

int obtenerDatosDeConsola(char *path, int offset, int Size){

	return EXIT_SUCCESS;
}

int obtenerDatosDeDMA(int FDDMA){
	tp_obtenerDatos parametrosDeObtenerDatos = prot_recibir_DMA_FS_obtenerDatos(FDDMA);

	return EXIT_SUCCESS;
}

int obtenerDatos(char *path, int offset, int Size){
	/* Parámetros​: [Path, Offset, Size]
	 * Descripción​: Ante un pedido de datos File System devolverá del path enviado por parámetro,
	 * la cantidad de bytes definidos por el Size a partir del offset solicitado.
	 */
	if(existeElArchivo(path)){

		return DatosObtenidos;
	}else{
		return ArchivoNoEncontrado;
	}
	return EXIT_SUCCESS;
}


int guardarDatosDeConsola(char *path, int offset, int size, char *Buffer){
	guardarDatos(path, offset, size, Buffer);
	return EXIT_SUCCESS;
}

int guardarDatosDeDMA(){

	return EXIT_SUCCESS;
}

int guardarDatos(char *path, int offset, int size, char *Buffer){
	/* Parámetros​: [Path, Offset, Size, Buffer]
	 * Descripción​: Ante un pedido de escritura MDJ almacenará en el path enviado por parámetro,
 	 * los bytes del buffer, definidos por el valor del Size y a partir del offset solicitado. En caso de
	 * que se soliciten datos o se intenten guardar datos en un archivo inexistente el File System
	 * deberá retornar un error de Archivo no encontrado.
	 */
	char *ubicacionDelArchivoDeMetadata=string_new();
	string_append(&ubicacionDelArchivoDeMetadata,configuracionDelFS.punto_montaje);
	string_append(&ubicacionDelArchivoDeMetadata, "/Archivos/");
	string_append(&ubicacionDelArchivoDeMetadata,path);
	if(existeElArchivo(ubicacionDelArchivoDeMetadata)){
		tp_metadata metadata = recuperarMetaData(ubicacionDelArchivoDeMetadata);
		int numeroDeBloqueDeInicioDeEscritura=offset/configuracionDeMetadata.tamanioBloques;
		int numeroDeBloqueDeFinDeEscritura=(offset+size)/configuracionDeMetadata.tamanioBloques;
		int escribirEnPrimerArchivoDesde=offset%configuracionDeMetadata.tamanioBloques;
		int bytesAEscribir;
		int bytesEscritos=0;
		int cantidadTotalDeBloquesCreados=list_size(metadata->bloques);
		int bloqueActual=numeroDeBloqueDeInicioDeEscritura;
		for(int i=numeroDeBloqueDeInicioDeEscritura;i<=numeroDeBloqueDeFinDeEscritura;i++){
			int numeroDeBloque;
			char *archivoDeBloque=string_new();
			string_append(&archivoDeBloque,configuracionDelFS.punto_montaje);
			string_append(&archivoDeBloque, "/Bloques/");
			if(bloqueActual<cantidadTotalDeBloquesCreados){
				numeroDeBloque =list_get(metadata->bloques,i);
				string_append(&archivoDeBloque, string_itoa(numeroDeBloque));
			}else{

				//El bloque no existe tengo que tomar uno vacio, crearlo y ademas actualizar la metadata
				int numeroDeBloqueLibre=obtenerBloqueLibreDelBitMap();
				if(numeroDeBloqueLibre!=-1){
					log_info(LOGGER,"El bloque %d esta libre",numeroDeBloque);
				}else{
					log_error(LOGGER,"No hay mas bloques libres");
					return EXIT_FAILURE;///no hay mas bloques libres
					}
				reservarBloqueYCrearEstructuras(numeroDeBloqueLibre);

			}
			log_info(LOGGER,"Abriendo el bloque %s para escribir",archivoDeBloque);
			FILE * archivo = fopen(archivoDeBloque,"wb");
			//fwrite recibe: puntero a los datos, el tamaño de los registros, numero de registros, archivo

			if(i!=numeroDeBloqueDeFinDeEscritura){
				bytesAEscribir=configuracionDeMetadata.tamanioBloques;
			}else{
				bytesAEscribir=configuracionDeMetadata.tamanioBloques-escribirEnPrimerArchivoDesde;
				fseek(archivo, escribirEnPrimerArchivoDesde, SEEK_SET);
				}
			log_info(LOGGER,"Escribiendo");
			fwrite(Buffer[bytesEscritos],sizeof(char),bytesAEscribir,archivo);
			bytesEscritos=bytesAEscribir+bytesEscritos;
			bloqueActual++;
			log_info(LOGGER,"Flusheando");
			fflush(archivo);
			fclose(archivo);
			}
		return DatosGuardados;
	}else{
		return ArchivoNoEncontrado;
	}
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
		return EXIT_FAILURE;
		}
	//Recupero el tamanio
	if(!config_has_property(configuracion,"TAMANIO")) {
		log_error(LOGGER,"No esta el TAMANIO en el archivo de metadata");
		config_destroy(configuracion);
		return EXIT_FAILURE;
		}
	metadata->tamanio = config_get_int_value(configuracion,"TAMANIO");
	log_info(LOGGER,"TAMANIO del archivo de metadata recuperado: %d", metadata->tamanio);

	//Recupero los BLOQUES
	if(!config_has_property(configuracion,"BLOQUES")) {
		log_error(LOGGER,"No estan los bloques en el archivo de metadata");
		config_destroy(configuracion);
		return EXIT_FAILURE;
		}
	char* bloques = config_get_string_value(configuracion,"BLOQUES");
	//tengo que volarle a la mierda al string q levante los []
	char *aux= string_substring_from(bloques,1);
	bloques= string_substring_until(aux,strlen(aux)-1);
	log_info(LOGGER,"Bloques recuperados: %s", bloques);
	char** instruccion = string_split(bloques, ",");
	metadata->bloques=list_create();
	for(int i=0;instruccion[i]!=NULL;i++){
		list_add(metadata->bloques,atoi(instruccion[i]));
		log_info(LOGGER,"Agregando el valor %d a la lista",atoi(instruccion[i]));
		}
	return metadata;
}

