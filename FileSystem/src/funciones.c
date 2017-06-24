/*
 * funciones.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <commons/bitarray.h>
#include "log.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "mensaje.h"
#include "socket.h"
#include "estructuras.h"
#include "fsecundarias.h"


extern int tBloques;
extern int socketfs;
extern int flagsocket;
extern t_dictionary * archivos;


void validar_archivo(char *mensaje)
{
	FILE * archivo;
	char * mensaje2 = strdup("");
	char *pathArmado;

	pathArmado = armar_path(mensaje);
	archivo = fopen(pathArmado,"r+");

	if(archivo == NULL){
		mensaje2 = armar_mensaje("F01","no");
		enviar(socketfs,mensaje2,&flagsocket);
	}
	else {
		mensaje2 = armar_mensaje("F01","ok");
		enviar(socketfs,mensaje2,&flagsocket);
		dictionary_put(archivos,pathArmado,archivo);
	}

	free(mensaje2);
	free(pathArmado);
}

void crear_archivo(char *mensaje)
{
	FILE *archivo;
	char *mensaje2 = strdup("");
	char *pathArmado;

	pathArmado = armar_path(mensaje);
	archivo = fopen(pathArmado,"a+");

	if(archivo == NULL){
			mensaje2 = armar_mensaje("F02","no");
			enviar(socketfs,mensaje2,&flagsocket);
		}
		else {
			mensaje2 = armar_mensaje("F02","ok");
			enviar(socketfs,mensaje2,&flagsocket);
			armar_archivo(archivo);

		}

	free(mensaje2);
	free(pathArmado);
}

void borrar_archivo(char *mensaje)
{
	char *pathArmado;
	char *mensaje2 =strdup("");

	pathArmado = armar_path(mensaje);

	remove(pathArmado);

	mensaje2 = armar_mensaje("F03","");
	enviar(socketfs,mensaje2,&flagsocket);

	free(pathArmado);
	free(mensaje2);
}

void obtener_datos(char *path, int offset, int size)
{
	char *mensaje =strdup(""); // para enviar los datos
	char *path2 = "/mnt/FS_SADICA/Bloques/"; //para sacar cada path de bloques
	char *pathBloque;// para guardar los path hechos
	FILE *bloques; //para abrir cada archivo de bloques
	char *lectura=strdup(""); // para guardar lo que se lee
	char *lectura2 = strdup(""); // no se si es necesario para seguir guardando lo que se lee
	int restoSize; // lo que falta leer
	t_arch *archivo; //guarda la info del archivo en gral
	div_t bloque ; //guarda los datos de la division para sacar los bloques y el offset
	int bloqueSig; // guarda el bloque al que hay que ir
	char* pathArmado;

	pathArmado = armar_path(path);

	archivo = leer_archivo(pathArmado);
	bloque = div(offset,tBloques);
	bloqueSig = bloque.quot;

	pathBloque = armar_pathBloque(path2,bloqueSig,archivo);

	bloques = fopen(pathBloque,"r");
	fseek(bloques,bloque.rem,SEEK_SET);
	restoSize = size;

	while(string_length(lectura)<size)
	{
		if(restoSize <= 64-bloque.rem)
		{
			while(!feof(bloques))
			{
				fgets(lectura2,restoSize,bloques);
				string_append(&lectura,lectura2);
			}

		} // preguntar estructura de los bloques.bin
		else{
			while(!feof(bloques))
			{
				fgets(lectura2,64-bloque.rem,bloques);
				string_append(&lectura,lectura2);
			}
			fclose(bloques);

			restoSize = size - string_length(lectura);
			bloqueSig ++;
			pathBloque = armar_pathBloque(path2,bloqueSig,archivo);
			bloques =fopen(pathBloque,"r");
			bloque.rem = 0;

		}
	}

	mensaje = armar_mensaje("F04",lectura);
	enviar(socketfs,mensaje,&flagsocket);

	fclose(bloques);
	free(mensaje);
	free(pathBloque);
	free(lectura);
	free(lectura2);
	free(archivo);
	free(pathArmado);

}

void guardar_datos(char *path, int offset, int size, char *buffer)
{
	char *mensaje =strdup(""); // para enviar los datos
	char *path2 = "/mnt/FS_SADICA/Bloques/"; //para sacar cada path de bloques
	char *pathBloque;// para guardar los path hechos
	FILE *bloques; //para abrir cada archivo de bloques
	t_arch *archivo; //guarda la info del archivo en gral
	div_t bloque ; //guarda los datos de la division para sacar los bloques y el offset
	int bloqueSig; // guarda el bloque al que hay que ir
	int guardado = 0;
	char * bloques_agregados = strdup("");
	char * pathArmado;
	char * bloques_final;

	pathArmado = armar_path(path);

	archivo = leer_archivo(pathArmado);
	bloque = div(offset,tBloques);
	bloqueSig = bloque.quot;

	pathBloque = armar_pathBloque(path2,bloqueSig,archivo);

	bloques = fopen(pathBloque,"w");
	fseek(bloques,bloque.rem,SEEK_SET);

	if(offset < archivo->tamanio)
	{

	}

	else
	{
		while(guardado < size)
		{
			if((size - guardado) <= (64-bloque.rem))
			{
				fputs(string_substring(buffer,guardado,64-bloque.rem),bloques);
				guardado = size - guardado;

			} // preguntar estructura de los bloques.bin
			else
			{
				fputs(string_substring(buffer,guardado,64-bloque.rem),bloques);
				guardado += 64-bloque.rem;

				fclose(bloques);

				bloqueSig = agregar_bloque();
				pathBloque = armar_pathBloque(path2,bloqueSig,archivo);
				bloques =fopen(pathBloque,"w");
				bloque.rem = 0;
				string_append(&bloques_agregados,",");
				string_append(&bloques_agregados,string_itoa(bloqueSig));

			}

		}

	}

	archivo->tamanio += size;
	bloques_final = crear_string_bloques(archivo->bloques, bloques_agregados);
	modificar_archivo(pathArmado,archivo->tamanio,bloques_final);


	mensaje = armar_mensaje("F05","");
	enviar(socketfs,mensaje,&flagsocket);

	fclose(bloques);
	free(mensaje);
	free(pathBloque);
	free(archivo);
	free(bloques_agregados);
	free(pathArmado);

}
