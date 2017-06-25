/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include "funciones.h"
#include "archivos.h"
#include "log.h"
#include <commons/log.h>
#include "mensaje.h"
#include <commons/bitarray.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <commons/collections/dictionary.h>

int puerto;
char *ip;
char *montaje;
int flagsocket;
int socketfs;
t_log * log;
int tBloques;
int cantBloques;
char *magic_number;
t_bitarray * bitmap;
t_dictionary *archivos;

struct stat mystat;

int verificarHS(char *handshake);
void handshake1();
void reservar_memoria();
void liberar_memoria();


int main(int argc, char *argv[])
{
	int metadata;
	int bitmap;
	int flag = 0;

	reservar_memoria();
	archivoDeCofiguracion(argv[1]);

	metadata = leer_metadata();
	if (metadata == -1) goto finalizar;

	bitmap = abrir_bitmap();
	if(bitmap == -1) goto finalizar;

	flagsocket=0;
	socketfs =iniciar_socket_server(ip,puerto,&flagsocket);

	handshake1();

	while(flag == 0)
	{
		char *mensaje; //saque el strdup
		int codigo;
		char * mensaje2 = strdup("");
		char **parametros;

		mensaje = recibir(socketfs,&flagsocket);
		codigo = get_codigo(mensaje);

		switch(codigo)
		{
			case 11:
				mensaje2 = get_mensaje(mensaje);
				validar_archivo(mensaje2);
				free(mensaje2);
				break;
			case 12:
				mensaje2 = get_mensaje(mensaje);
				crear_archivo(mensaje2);
				free(mensaje2);
				break;
			case 13:
				mensaje2 = get_mensaje(mensaje);
				borrar_archivo(mensaje2);
				free(mensaje2);
				break;
			case 14:
				mensaje2 = get_mensaje(mensaje);
				parametros = string_split(mensaje2,",");
				obtener_datos(parametros[0],atoi(parametros[1]),atoi(parametros[2]));
				free(mensaje2);
				break;
			case 15:
				mensaje2 = get_mensaje(mensaje);
				parametros = string_split(mensaje2,",");
				guardar_datos(parametros[0],atoi(parametros[1]),atoi(parametros[2]),parametros[3]);
				free(mensaje2);
				break;
			default:
				escribir_log("Mensaje incorrecto");
				flag =1;
				break;

		}
		free(mensaje);

	}

	finalizar: escribir_log("Error leyendo archivos iniciales");

	liberar_memoria();
	return EXIT_SUCCESS;
}
					////// AUXILIARES //////

void handshake1()
{
	char *handshake;//saque el strdup
	int esKernel=0;
	char *mensaje; //saque el strdup
	while(esKernel == 0)
	{
		int cliente = escuchar_conexiones(socketfs,&flagsocket);
		handshake = recibir(cliente,&flagsocket);
		if (verificarHS(handshake)== 1)
		{
			esKernel = 1;
			mensaje = armar_mensaje("F00","");
			enviar(socketfs,mensaje,&flagsocket);
		}else{
			cerrar_conexion(cliente);
			printf("intruso no kernel eliminado \n");
			escribir_log("Proceso no Kernel eliminado");
		}
		free(handshake);
	}
	printf("KERNEL CONECTADO \n");
	escribir_log("Se conecto el Kernel");
}

void reservar_memoria()
{
	montaje = strdup("");
	ip = strdup("");
	magic_number = strdup("");
	crear_archivo_log("/home/utnso/log_fs.txt");
	//archivos = dictionary_create();
}

void liberar_memoria()
{
	free(montaje);
	free(ip);
	free(magic_number);
	munmap(&mystat,mystat.st_size);
	//dictionary_clean(archivos);
	//dictionary_destroy(archivos);
	liberar_log();
}

int verificarHS(char *handshake)
{
	char *header = strdup("");
	header = get_header(handshake);
	if(!strcmp(header,"K"))
	{
		free(header);
		return 1;
	}
	else{
		free(header);
		return 0;
	}

}

