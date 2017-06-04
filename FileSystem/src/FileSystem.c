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


int puerto;
char *ip;
char *montaje;
int flagsocket;
int socketfs;
t_log * log;

int verificarHS(char *handshake);
void archivoDeCofiguracion(char* argv);
void handshake();
void reservar_memoria();
void liberar_memoria();


int main(int argc, char *argv[])
{
	reservar_memoria();
	archivoDeCofiguracion(argv[1]);
	flagsocket=0;
	socketfs =iniciar_socket_server(ip,puerto,&flagsocket);
	handshake();
	while(1)
	{
		char *mensaje = strdup("");
		int codigo;
		mensaje = recibir(socketfs,&flagsocket);
		codigo = get_codigo(mensaje);
		switch(codigo)
		{
			case 11:
				char * mensaje2 = strdup("");
				mensaje2 = get_mensaje(mensaje);
				validar_archivo(mensaje2);
				free(mensaje2);
				break;
			case 12:
				char * mensaje2 = strdup("");
				mensaje2 = get_mensaje(mensaje);
				crear_archivo();
				free(mensaje2);
				break;
			case 13:
				char * mensaje2 = strdup("");
				mensaje2 = get_mensaje(mensaje);
				borrar_archivo();
				free(mensaje2);
				break;
			case 14:
				char *mensaje2 = strdup("");
				char **parametros;
				mensaje2 = get_mensaje(mensaje);
				parametros = string_split(mensaje2,",");
				obtener_datos(parametros[0],atoi(parametros[1]),atoi(parametros[2]));
				free(mensaje2);
				break;
			case 15:
				char *mensaje2 = strdup("");
				char **parametros;
				mensaje2 = get_mensaje(mensaje);
				parametros = string_split(mensaje2,",");
				guardar_datos(parametros[0],atoi(parametros[1]),atoi(parametros[2]),parametros[3]);
				free(mensaje2);
				break;
			default:
				escribir_log("Mensaje incorrecto");

		}
		free(mensaje);

	}
	liberar_memoria();
	return EXIT_SUCCESS;
}
					////// AUXILIARES //////
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

void handshake()
{
	char *handshake = strdup("");
	int esKernel=0;
	while(esKernel == 0)
	{
		int cliente = escuchar_conexiones(socketfs,&flagsocket);
		handshake = recibir(cliente,&flagsocket);
		if (verificarHS(handshake)== 1)
		{
			esKernel = 1;
		}else{
			cerrar_conexion(cliente);
			printf("intruso no kernel eliminado \n");
			escribir_log("Proceso no Kernel eliminado");
		}
	}
	printf("KERNEL CONECTADO \n");
	escribir_log("Se conecto el Kernel");
	free(handshake);
}

void reservar_memoria()
{
	montaje = strdup("");
	ip = strdup("");
	log =crear_archivo_log("/home/utnso/log_fs.txt");
}

void liberar_memoria()
{
	free(montaje);
	free(ip);
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

