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
#include "sockets_server.h"
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
int puerto;
char *montaje;
t_config *configuracion;
int verificarHS(char *handshake);

void archivoDeCofiguracion(char* argv);

int main(int argc, char *argv[]) {
	archivoDeCofiguracion(argv[1]);
	char *ipfs = "127.0.0.1";
	int flagsocket=0;
	int socketfs =iniciar_socket_server(ipfs,puerto,&flagsocket);

	char *handshake = malloc(1024);
	int esKernel=0;
	while(esKernel == 0){
		int cliente = escuchar_conexiones(socketfs,&flagsocket);
		recibir(cliente,&flagsocket,handshake);
		if (verificarHS(handshake)== 1){
			esKernel = 1;
		}else{
			cerrar_conexion(cliente);
			printf("intruso no kernel eliminado \n");
		}
	}
	printf("KERNEL CONECTADO \n");
	free(handshake);
	return EXIT_SUCCESS;
}
					////// AUXILIARES //////
void archivoDeCofiguracion(char* argv) {
	montaje = malloc(256);
	printf("ruta archivo de configuacion: %s \n", argv);
	configuracion = config_create(argv);
	puerto = config_get_int_value(configuracion, "PUERTO");
	strcpy(montaje, config_get_string_value(configuracion, "PUNTO_MONTAJE"));
	printf("Valor puerto para conexion del KERNEL: %d \n", puerto);
	printf("Valor punto montaje FS: %s \n", montaje);

	free(montaje);
	config_destroy(configuracion);
}
int verificarHS(char *handshake){
	int resp=0;
	bool res = string_equals_ignore_case(handshake, "KERNEL");
	if(res == true){
		resp = 1;
	}
return resp;
}

