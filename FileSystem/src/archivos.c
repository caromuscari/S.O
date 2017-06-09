/*
 * archivos.c
 *
 *  Created on: 6/6/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <commons/bitarray.h>

extern char *montaje;
extern int puerto;
extern char *ip;
extern int tBloques;
extern int cantBloques;
extern char *magic_number;
extern t_bitarray * bitmap;

void archivoDeCofiguracion(char* argv)
{
	t_config *configuracion;
	printf("ruta archivo de configuacion: %s \n", argv);
	configuracion = config_create(argv);
	puerto = config_get_int_value(configuracion, "PUERTO");
	string_append(montaje, config_get_string_value(configuracion, "PUNTO_MONTAJE"));
	string_append(ip, config_get_string_value(configuracion, "IP"));
	printf("Valor Ip para conexion del KERNEL: %d \n", ip);
	printf("Valor puerto para conexion del KERNEL: %d \n", puerto);
	printf("Valor punto montaje FS: %s \n", montaje);
	escribir_log_compuesto("Valor IP para conexion con Kernel: ", ip);
	escribir_log_con_numero("Valor puerto para conexion del Kernel: ", puerto);
	escribir_log_compuesto("Valor punto montaje FS: ",montaje);

	config_destroy(configuracion);
}

void leer_metadata()
{
	char *ruta = strdup("");
	char *mensaje = strdup("");
	FILE *metadata;
	long int final;
	char **split;
	string_append(ruta,montaje);
	string_append(ruta,"Metadata/Metadata.bin");
	metadata = fopen(ruta, "r");
	if(!strcmp(metadata,NULL)) escribir_log("No se pudo abrir el archivo de metadata");
	fseek( metadata, 0L, SEEK_END );
	final = ftell( metadata );
	fseek(metadata,0,0);

	fgets(mensaje,final,metadata);
	split = string_split(mensaje,"=");
	tBloques = atoi(split[1]);

	fgets(mensaje,final,metadata);
	split = string_split(mensaje,"=");
	cantBloques = atoi(split[1]);

	fgets(mensaje,final,metadata);
	split = string_split(mensaje,"=");
	magic_number = split[1];
	//verificar que diga SADICA

	escribir_log_con_numero("Tamanio de los bloques: ", tBloques);
	escribir_log_con_numero("Cantidad de Bloques: ", cantBloques);
	escribir_log_compuesto("Magic number: ", magic_number);

	fclose(metadata);
	free(mensaje);
	free(ruta);

}

void abrir_bitmap()
{
	char *ruta = strdup("");
	string_append(ruta,montaje);
	string_append(ruta,"Metadata/Bitmap.bin");
	bitmap = bitarray_create_with_mode(ruta,cantBloques,LSB_FIRST);
	//mmap();
	free(ruta);

}
