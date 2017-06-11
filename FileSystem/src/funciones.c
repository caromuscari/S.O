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

extern t_bitarray * bitmap;
extern char *montaje;
extern int tBloques;
extern int cantBloques;
extern int socketfs;
extern int flagsocket;
//extern t_dictionary * archivos;


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
		//dictionary_put(archivos,mensaje,archivo);

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
			//dictionary_put(archivos,mensaje,archivo);
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
	//dictionary_remove(archivos,mensaje);
	free(pathArmado);
	free(mensaje2);
}

void obtener_datos(char *path, int offset, int size)
{
	char *mensaje =strdup(""); // para enviar los datos
	//FILE *archivo = dictionary_get(archivos,path);
	char *path2 = "/mnt/FS_SADICA/Bloques/"; //para sacar cada path de bloques
	char *pathBloque;// para guardar los path hechos
	FILE *bloques; //para abrir cada archivo de bloques
	char *lectura=strdup(""); // para guardar lo que se lee
	char *lectura2 = strdup(""); // no se si es necesario para seguir guardando lo que se lee
	int restoSize; // lo que falta leer
	t_arch *archivo; //guarda la info del archivo en gral
	div_t bloque; //guarda los datos de la division para sacar los bloques y el offset
	int bloqueSig; // guarda el bloque al que hay que ir

	archivo = leer_archivo(path);
	bloque = div(offset,tBloques);
	bloqueSig = bloque.quot;

	pathBloque = armar_pathBloque(path2,bloqueSig,archivo);
	//string_append(&pathBloque,string_itoa(archivo.bloques[bloqueSig]));
	//string_append(&pathBloque,".bin");

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
			while(!feof(archivo))
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

	free(mensaje);
	free(pathBloque);
	free(lectura);
	free(lectura2);
	free(archivo); //pregunta a ceci

}

void guardar_datos(char *path, int offset, int size, char *buffer)
{

}
