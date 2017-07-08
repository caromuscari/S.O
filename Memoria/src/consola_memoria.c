/*
 * consola_memoria.c
 *
 *  Created on: 8/7/2017
 *      Author: utnso
 */
#include "consola_memoria.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include "hash.h"

typedef struct{
	int pid;
	int pag;
	char *dataFrame;
}t_cache;
typedef struct{
	int estado;
	int pid;
	int pag;
} t_tablaPagina;

t_tablaPagina* tablaPaginas;
int terminar_hilo;
int retardo;
extern int cantMarcos;
extern int cantMarcosTablaPag;
t_cache* Cache;
int entradasCache;
extern int tamanioMarco;
extern char *Memoria;
extern int tamTablaPaginas;
int cantidad_de_dump_cache;
int cantidad_de_dump_estructuras;
int cantidad_de_dump_frames;
void hilo_consola_memoria(){

	cantidad_de_dump_cache = 0;
	cantidad_de_dump_estructuras = 0;
	cantidad_de_dump_frames = 0;
	terminar_hilo = 0;
	char *input = malloc(50);
	memset(input,'0',50);
	while(terminar_hilo == 0){

		scanf("%[^\n]s",input);
		int caso = que_case_es(input);
		free(input);

		switch(caso){
		case 1:;
			int nuevo_retardo = 0;
			printf("Por favor, ingrese un nuevo valor de retardo (en milisegundos)\n");
			scanf("%d",&nuevo_retardo);
			retardo = nuevo_retardo;
			printf(" （＾－＾） Muchas Gracias, en unos instantes podras ver el menu principal nuevamente\n\n");
			break;
		case 2:;
			char *que_dump = malloc(50);
			display_menu_dump();
			scanf("%[^\n]s",que_dump);
			if(strncmp(que_dump,"Cache",strlen("Cache"))==0){
				char *path = dump_cache();
				printf("El path del estado actual de la cache archivado es: %s \n",path);
				free(path);
			}else if(strncmp(que_dump,"Estructuras de Memoria",strlen("Estructuras de Memoria"))==0){
				char *path = dump_estructuras();
				printf("El path del estado actual de la Tabla de Archvos archivado es: %s \n",path);
				free(path);

			}else if(strncmp(que_dump,"Contenido en Marcos",strlen("Estructuras de Memoria"))==0){
				printf("Si quiere ver todo el contenido de los marco ingrese -1 \n");
				printf("Si quiere marcos particulares de un proceso ingrese su PID \n");
				int pid = 0;
				scanf("%d",&pid);
				char *path = dump_datos(pid);
				printf("El path del estado actual de los datos de pid :%d archivado es: %s \n",pid,path);
				free(path);
			}else{
				printf("Lo siento,pero no conozco el comando ingresado,volveremos al menu principal \n");
			}
			free(que_dump);
			printf(" （＾－＾） Muchas Gracias, en unos instantes podras ver el menu principal nuevamente\n\n");

			break;
		case 3:
			printf("Limpiando contenido de la Caché \n");
			limpiar_cache();
			printf(" （＾－＾） Muchas Gracias, en unos instantes podras ver el menu principal nuevamente\n\n");

			break;
		case 4:
			printf("Tamaño de la Memoria \n");
			printf("Cantidad de Marcos : %d \n",cantMarcos);
			int libres = marcos_libres();
			printf("Cantidad de Marcos Libres : %d \n",libres); //calcular marcos libre
			printf("Cantidad de Marcos Ocupados : %d \n",cantMarcos-libres); // restarle los marcos libres

			printf(" （＾－＾） Muchas Gracias, en unos instantes podras ver el menu principal nuevamente\n\n");
			break;
		case 5:;
			int pid;
			printf("Ingrese el PID del que quiere saber el tamaño \n");
			scanf("%d",&pid);
			int cantpagspid = calcular_paginas_de_proceso(pid);
			int size_proceso = cantpagspid * tamanioMarco;
			printf("Size del PID %d es :%d \n",pid,size_proceso);
			printf(" （＾－＾） Muchas Gracias, en unos instantes podras ver el menu principal nuevamente\n\n");
			break;
		case 6:
			printf("ಠ-ಠ ¿Qué ingresaste? ಠ-ಠ \n");
			break;
		}
		display_menu_principal();
	}
}

int que_case_es(char *input){

	int retornar;
	if(strncmp(input,"Modificar Retardo",17)==0){
		retornar = 1;
	}else if(strncmp(input,"Dump",4)==0){
		retornar = 2;
	}else if(strncmp(input,"Flush",5)==0){
		retornar = 3;
	}else if(strncmp(input,"Size Memoria",12)==0){
		retornar = 4;
	}else if(strncmp(input,"Size PID",8)==0){
		retornar = 5;
	}else{
		retornar = 6;
	}

	return retornar;
}
void display_menu_principal(){

	printf("Bienvenid@ （‐＾▽＾‐） \n");
	printf("Ingrese alguna de las siguientes opciones\n");
	printf("	Modificar Retardo\n");
	printf("	Dump\n");
	printf("	Flush\n");
	printf("	Size Memoria\n");
	printf("	Size PID\n\n");

}
void display_menu_dump(){

	printf("Bienvenid@ al menu del comando Dump（‐＾▽＾‐） \n");
	printf("¿Sobre qué quiere realizar el Dump?\n");
	printf("	Cache\n");
	printf("	Esctructuras de Memoria\n");
	printf("	Contenido en Marcos\n\n");

}

void limpiar_cache(){
	int i;
	for(i=0;i<entradasCache;i++){
		if(Cache[i].pid != -1 && Cache[i].pag != -1){
			Cache[i].pid = -1;
			Cache[i].pag = -1;
			free(Cache[i].dataFrame);
		}
	}
}
char *dump_cache(){
	cantidad_de_dump_cache ++;
	char *path = string_from_format("/home/utnso/dump_cache_%d",cantidad_de_dump_cache);
	FILE * fd = fopen(path,"ar+");
	int i;
	int desplazamiento=0;
	fseek(fd,desplazamiento,SEEK_SET);
	for(i=0; i<entradasCache ; i++){
		char *linea = string_from_format("Pid:%s-Pag:%s-Data:%s \n",Cache[i].pid,Cache[i].pag,Cache[i].dataFrame);
		printf("Info posicion %d de cache",i);
		printf("%s",linea);
		fwrite(linea,sizeof(char),strlen(linea),fd);
		free(linea);
		desplazamiento += strlen(linea);
	}
	fclose(fd);
return path;
}
char *dump_estructuras(){

	cantidad_de_dump_estructuras ++;
	char *path = string_from_format("/home/utnso/dump_estructuras_%d",cantidad_de_dump_estructuras);
	t_log* logi = log_create(path,"DUMP_ESTRUCTURAS",true,LOG_LEVEL_INFO);
	int a;
	log_info(logi,"Tabla De Paginas");
	char *linea;
	char *aux;

	for(a=0;a<cantMarcos;a++){
		aux = string_from_format("Entrada correspondiente al marco %d",a);
		log_info(logi,aux);
		free(aux);

		linea = string_from_format("PID:%d-PAG:%d-ESTADO:%d",tablaPaginas[a].pid,tablaPaginas[a].pag,tablaPaginas[a].estado);
		log_info(logi,linea);
		free(linea);
	}

	log_destroy(logi);
	return path;
}
char *dump_datos(int pid){
	cantidad_de_dump_frames ++;
	char *path = string_from_format("/home/utnso/dump_frames_%d",cantidad_de_dump_estructuras);
	t_log* logi = log_create(path,"DUMP_FRAMES",true,LOG_LEVEL_INFO);
	if(pid == -1){
		log_info(logi,"Imprimir todos los frames de memoria");
		int a;
		for (a = cantMarcosTablaPag+1; a<cantMarcos; a ++ ) {
		char *dataFrame = malloc (tamanioMarco); memset(dataFrame,'\0',tamanioMarco);
		int pos= (a* tamanioMarco)-1;
		memcpy(dataFrame,Memoria+pos,tamanioMarco);

		char *imp = string_from_format("|%s|\n",dataFrame);
		log_info(logi,imp);
		free(imp);
		free(dataFrame);
		}

	}else{
		char *aux= string_from_format("Imprimir todos los frames del PID: %d",pid);
		log_info(logi,aux);
		free(aux);

		char *dats = info_marco_proceso(pid);
		log_info(logi,dats);
		free(dats);

	}
	log_destroy(logi);
	return path;
}
int marcos_libres(){
	int ret = 0;
	int a;
	for(a=0;a<cantMarcos;a++){
		if(tablaPaginas[a].estado == 0){
			ret++;
		}
	}
	return ret;
}
int calcular_paginas_de_proceso(int pid){
	int ret = 0;
	int a;
	for(a=tamTablaPaginas;a<cantMarcos;a++){
		if(tablaPaginas[a].pid == pid){
			ret++;
		}
	}
	return ret;
}

char *info_marco_proceso(int pid){


	int cant_pagina = calcular_paginas_de_proceso(pid);
	char *datos = malloc(cant_pagina * tamanioMarco);
	int i;
	int desplazamiento = 0;
	for(i=0;i<cant_pagina;i++){
		int pos_hash = hash(pid,i);
		if(es_marco_correcto(pid,i,pos_hash) == 0){
			int pos_rehsh = buscar_marco_colision(pid,i,pos_hash);
			memcpy(datos,Memoria+((pos_rehsh)*tamanioMarco),tamanioMarco);
			desplazamiento += tamanioMarco;
		}else{
			memcpy(datos,Memoria+((pos_hash)*tamanioMarco),tamanioMarco);
			desplazamiento += tamanioMarco;
		}
	}
	return datos;

}
