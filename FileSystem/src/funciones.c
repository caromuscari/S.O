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
extern t_bitarray * bitmap;


void validar_archivo(char *mensaje)
{
	FILE * archivo;
	char * mensaje2;
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
	}

	free(mensaje2);
	free(pathArmado);
}

void crear_archivo(char *mensaje)
{
	FILE *archivo;
	char *mensaje2;
	char *pathArmado;
	char * pathsinarchivo;

	pathsinarchivo = sacar_archivo(mensaje);

	pathArmado = armar_path(mensaje);

	char * pathmkdir= string_from_format("mkdir -p %s",pathsinarchivo);
	system(pathmkdir);
	free(pathmkdir);
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
	free(pathsinarchivo);
}

void borrar_archivo(char *mensaje)
{
	char *pathArmado;
	char *mensaje2;
	int i = 0;

	pathArmado = armar_path(mensaje);
	t_arch * archivo;

	archivo = leer_archivo(pathArmado);
	while(archivo->bloques != '\0')
	{
		bitarray_clean_bit(bitmap,atoi(archivo->bloques[i]));
		i++;
	}

	remove(pathArmado);

	mensaje2 = armar_mensaje("F03","");
	enviar(socketfs,mensaje2,&flagsocket);

	free(pathArmado);
	free(mensaje2);
	free(archivo);
}

void obtener_datos(char *path, int offset, int size)
{
	char *mensaje; // para enviar los datos
	char *path2 = "/mnt/FS_SADICA/Bloques/"; //para sacar cada path de bloques
	char *pathBloque;// para guardar los path hechos
	FILE *bloques; //para abrir cada archivo de bloques
	char *lectura=strdup(""); // para guardar lo que se lee
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
			fread(lectura,sizeof(char),restoSize,bloques);

		} // preguntar estructura de los bloques.bin
		else{

			fread(lectura,sizeof(char),64-bloque.rem,bloques);

			fclose(bloques);

			restoSize = size - string_length(lectura);
			bloqueSig ++;
			if(archivo->bloques[bloqueSig] != '\0'){
				pathBloque = armar_pathBloque(path2,bloqueSig,archivo);
				bloques =fopen(pathBloque,"r");
				bloque.rem = 0;
			}
			else{
				restoSize = size + 1;
			}
		}
	}

	if(restoSize == size){
		mensaje = armar_mensaje("F04",lectura);
		enviar(socketfs,mensaje,&flagsocket);
	}
	else {
		mensaje = armar_mensaje("F06","");
		enviar(socketfs,mensaje,&flagsocket);
	}


	fclose(bloques);
	free(mensaje);
	free(pathBloque);
	free(lectura);
	free(archivo);
	free(pathArmado);

}

void guardar_datos(char *path, int offset, int size, char *buffer)
{
	char *mensaje; // para enviar los datos
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
	int sizeAguardar = 0;
	int flag=0;
	int stop;

	pathArmado = armar_path(path);

	archivo = leer_archivo(pathArmado);
	bloque = div(offset,tBloques);
	bloqueSig = bloque.quot;

	pathBloque = armar_pathBloque(path2,bloqueSig,archivo);

	bloques = fopen(pathBloque,"w");
	fseek(bloques,bloque.rem,SEEK_SET);

	if(offset < archivo->tamanio)
	{
		while (guardado < size)
		{
			if((size - guardado) <= (64-bloque.rem))
			{
				fputs(string_substring(buffer,guardado,size - guardado),bloques);
				guardado = size;
				fclose(bloques);

			}
			else
			{
				fputs(string_substring(buffer,guardado,64-bloque.rem),bloques);
				guardado += 64-bloque.rem;

				fclose(bloques);

				if(archivo->bloques[bloqueSig+1] != '\0')
				{
					bloqueSig ++;
					pathBloque = armar_pathBloqueNuevo(path2,bloqueSig,archivo);
					bloques =fopen(pathBloque,"w");
					bloque.rem = 0;
				}
				else
				{
					bloqueSig = agregar_bloque();

					if(bloqueSig != -1)
					{
						pathBloque = armar_pathBloqueNuevo(path2,bloqueSig,archivo);
						bloques =fopen(pathBloque,"w");
						bloque.rem = 0;

						string_append(&bloques_agregados,",");
						string_append(&bloques_agregados,string_itoa(bloqueSig));

						if((size - guardado) > (64-bloque.rem))
						{
							string_append(&bloques_agregados,",");
						}

						if(flag == 0){
							flag = 1;
							stop = guardado;
						}

					 }
					 else
					 {
						 if(flag == 0){
							 guardado = size + 1;
						 }
						 else{
							 sizeAguardar = guardado - stop;
							 guardado = size + 1;
						 }
					 }
				}
			}
		}
	}

	else
	{
		while(guardado < size)
		{
			if((size - guardado) <= (64-bloque.rem))
			{
				fputs(string_substring(buffer,guardado,size - guardado),bloques);
				guardado = size;
				fclose(bloques);

			} // preguntar estructura de los bloques.bin
			else
			{
				fputs(string_substring(buffer,guardado,64-bloque.rem),bloques);
				guardado += 64-bloque.rem;

				fclose(bloques);

				bloqueSig = agregar_bloque();

				if(bloqueSig != -1)
				{
					pathBloque = armar_pathBloqueNuevo(path2,bloqueSig,archivo);
					bloques =fopen(pathBloque,"w");
					bloque.rem = 0;

					string_append(&bloques_agregados,",");
					string_append(&bloques_agregados,string_itoa(bloqueSig));

					if((size - guardado) > (64-bloque.rem))
					{
						string_append(&bloques_agregados,",");
					}

					if(flag == 0){
						flag = 1;
						stop = guardado;
					}
				}
				else{
					if(flag == 0){
						guardado = size + 1;
					}
					else {
						sizeAguardar = guardado - stop;
						guardado = size + 1;
					}
				}
			}
		}
	}

	if(flag == 1){
		archivo->tamanio += sizeAguardar;
		bloques_final = crear_string_bloques(archivo->bloques, bloques_agregados);
		modificar_archivo(pathArmado,archivo->tamanio,bloques_final);
	}

	if(guardado == size){
		mensaje = armar_mensaje("F05","ok");
		enviar(socketfs,mensaje,&flagsocket);
	}
	else{
		mensaje = armar_mensaje("F05","no");
		enviar(socketfs,mensaje,&flagsocket);
	}


	free(mensaje);
	free(pathBloque);
	free(bloques_final);
	free(archivo);
	free(bloques_agregados);
	free(pathArmado);

}
