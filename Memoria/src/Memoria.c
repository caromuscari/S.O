/*
 ============================================================================
 Name        : Memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "estructuras.h"
#include "log.h"
#include "socket.h"

t_memoria *memoria_data;

void leer_archivo_metadata(char *ruta_archivo_par);
void liberar_memoria_y_cerrar();

int main(void)
{
	crear_archivo_log("/home/utnso/Archivos/log");
	escribir_log("Memoria - Iniciando modulo memoria");
	leer_archivo_metadata("/home/utnso/Archivos/configuracion");

	//Crear socket server
	*memoria_data->SOCKET = iniciar_socket_server(memoria_data->IP, *memoria_data->PUERTO);

	//Imprimo datos al azar
	escribir_log_con_numero("Socket server creado: ", *memoria_data->SOCKET);
	escribir_log_con_numero("Numero de marcos: ", *memoria_data->MARCOS);

	liberar_memoria_y_cerrar();
	liberar_log();
	return EXIT_SUCCESS;
}

void leer_archivo_metadata(char *ruta_archivo_par)
{
	t_config *config_meta;
	config_meta = config_create(ruta_archivo_par);

	memoria_data = malloc(sizeof(t_memoria));
	memoria_data->IP = config_get_string_value(config_meta, "IP");
	*memoria_data->CACHE_X_PROC = config_get_int_value(config_meta, "CACHE_X_PROC");
	*memoria_data->ENTRADAS_CACHE = config_get_int_value(config_meta, "ENTRADAS_CACHE");
	*memoria_data->MARCOS = config_get_int_value(config_meta, "MARCOS");
	*memoria_data->MARCO_SIZE = config_get_int_value(config_meta, "MARCO_SIZE");
	*memoria_data->PUERTO = config_get_int_value(config_meta, "PUERTO");
	*memoria_data->REEMPLAZO_CACHE = config_get_int_value(config_meta, "REEMPLAZO_CACHE");
	*memoria_data->RETARDO_MEMORIA = config_get_int_value(config_meta, "RETARDO_MEMORIA");

	config_destroy(config_meta);
}

void liberar_memoria_y_cerrar()
{
	cerrar_conexion(*memoria_data->SOCKET);
	free(memoria_data);
}
