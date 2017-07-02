/*
 * fsecundarias.c
 *
 *  Created on: 11/6/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include "estructuras.h"
#include <commons/bitarray.h>
#include <commons/config.h>
#include "fsecundarias.h"

extern t_bitarray * bitmap;
extern char *montaje;
extern int cantBloques;

void armar_archivo(FILE * archivo)
{
	char * tamanio = "TAMANIO=0\n";
	char * bloques =strdup("");
	int bit;
	bit = agregar_bloque();

	if(bit != -1){
		string_append(&bloques, "BLOQUES=[");
		string_append(&bloques, string_itoa(bit));
		string_append(&bloques, "]");

		fwrite(tamanio,sizeof(char),string_length(tamanio),archivo);
		fwrite(bloques,sizeof(char),string_length(bloques),archivo);
	}

	free(bloques);
}

char * armar_pathBloque(char *path,int bloqueSig,t_arch *archivo)
{
	char *path2 =strdup("");
	char * var=string_itoa(*archivo->bloques[bloqueSig]);
	string_append(&path2,path);
	string_append(&path2,var);
	string_append(&path2,".bin");
	free(var);
	return path2;
}

char * armar_pathBloqueNuevo(char *path,int bloqueSig,t_arch *archivo)
{
	char *path2 =strdup("");
	char *var= string_itoa(bloqueSig);
	string_append(&path2,path);
	string_append(&path2,var);
	string_append(&path2,".bin");
	free(var);
	return path2;
}

t_arch *leer_archivo(char * path)
{
	t_arch *archivo;
	t_config *config=config_create(path);
	archivo = malloc( sizeof(t_arch) );
	archivo->tamanio = config_get_int_value(config,"TAMANIO");
	archivo->bloques = config_get_array_value(config,"BLOQUES");
	config_destroy(config);
	return archivo;
}

char * armar_path(char *mensaje)
{
	char *mensaje2 = strdup("");
	string_append(&mensaje2,montaje);
	string_append(&mensaje2,mensaje);
	return mensaje2;

}

int agregar_bloque()
{
	int bit = 0;
	int prueba = -1;
	bool comprobar;
	while(bit < cantBloques && comprobar == true)
	{
		comprobar = bitarray_test_bit(bitmap,bit);

		bit++;
	}
	if(bit == cantBloques && comprobar == true){
		return prueba;
	}
	else {
		bitarray_set_bit(bitmap,bit);
		return bit;
	}

}

void modificar_archivo(char* path, int tamanio, char* bloques)
{
	char* tamanio2 = strdup("");
	t_config * archconf;
	char * var= string_itoa(tamanio);

	archconf = config_create(path);

	string_append(&tamanio2,var);
	string_append(&tamanio2,"\n");

	config_set_value(archconf,"TAMANIO", tamanio2);
	config_set_value(archconf,"BLOQUES", bloques);

	config_save(archconf);

	config_destroy(archconf);
	free(tamanio2);
	free(var);
}

char * crear_string_bloques(char ** bloques, char * bloques_nuevos)
{
	char * bloque = strdup("");
	int i = 0;

	string_append(&bloque,"[");

	while(bloques[i] != '\0')
	{
		string_append(&bloque,bloques[i]);
		string_append(&bloque,",");
		i++;
	}

	if(!strcmp(bloques_nuevos,"")){
		string_append(&bloque,"]");
	}
	else{
		string_append(&bloque,bloques_nuevos);
		string_append(&bloque,"]");
	}

	return bloque;
}

t_datos * recuperar_datos(char * codigo, char * mensaje)
{
	t_datos * estructura = malloc(sizeof(t_datos));
	int size;
	int start;
	char * var;

	var = string_substring(mensaje, 0, 4);
	size = atoi(var);
	estructura->path = string_substring(mensaje,4,size);
	free(var);

	start = 4+ size;

	var = string_substring(mensaje, start, 4);
	size = atoi(var);
	start += 4;
	free(var);
	var = string_substring(mensaje,start,size);
	estructura->offset = atoi(var);
	free(var);

	start += size;

	var = string_substring(mensaje,start,4);
	size = atoi(var);
	start +=4;
	free(var);
	var = string_substring(mensaje,start,size);
	estructura->size = atoi(var);
	free(var);

	start += size;

	if(atoi(codigo) == 15)
	{
		var = string_substring(mensaje,start,4);
		size = atoi(var);
		start +=4;
		free(var);
		estructura->buffer = string_substring(mensaje,start,size);
	}

	return estructura;
}

char * sacar_archivo(char * mensaje){
	char * path = strdup("");
	char ** split;
	int i=0;
	int j=0;
	split = string_split(mensaje, "/");

	while(split[i] != NULL/*'\0'*/) i++;
	i-=2;
	while(j <= i){
		string_append(&path,"/");
		string_append(&path,split[j]);
		j++;
	}

	return path;
}
