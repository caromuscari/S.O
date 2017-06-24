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

extern t_bitarray * bitmap;
extern char *montaje;
extern t_dictionary * archivos;

void armar_archivo(FILE * archivo)
{
	char * tamanio = "TAMANIO=0\n";
	char * bloques =strdup("");
	int bit = 0;
	bool comprobar;
	while(comprobar == false){
		bit++;
		comprobar = bitarray_test_bit(bitmap,bit);
	}
	bitarray_set_bit(bitmap,bit);
	string_append(&bloques, "BLOQUES=[");
	string_append(&bloques, string_itoa(bit));
	string_append(&bloques, "]");

	fwrite(tamanio,sizeof(char),string_length(tamanio),archivo);
	fwrite(bloques,sizeof(char),string_length(bloques),archivo);

	free(bloques);
}

char * armar_pathBloque(char *path,int bloqueSig,t_arch *archivo)
{
	char *path2 =strdup("");
	string_append(&path2,path);
	string_append(&path2,string_itoa(*archivo->bloques[bloqueSig]));
	string_append(&path2,".bin");
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
	bool comprobar;
	while(comprobar == false)
	{
		bit++;
		comprobar = bitarray_test_bit(bitmap,bit);
	}
	bitarray_set_bit(bitmap,bit);
	return bit;
}

void modificar_archivo(char* path, int tamanio, char* bloques)
{
	char* tamanio2 = strdup("");
	char* bloques2 = strdup("");
	t_config * archconf;

	archconf = config_create(path);

	string_append(&tamanio2,string_itoa(tamanio));
	string_append(&tamanio2,"\n");

	config_set_value(archconf,"TAMANIO", tamanio2);
	config_set_value(archconf,"BLOQUES", bloques2);

	config_save(archconf);

	config_destroy(archconf);
	free(tamanio2);

}

char * crear_string_bloques(char ** bloques, char * bloques_nuevos)
{
	char * bloque = strdup("");
	int i = 0;

	while(!strcmp(bloques[i], "]"))
	{
		string_append(&bloque,bloques[i]);
		i++;
	}

	string_append(&bloque,bloques_nuevos);
	string_append(&bloque,"]");

	return bloque;
}
