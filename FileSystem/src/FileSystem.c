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

int puerto;
char *montaje;
t_config *configuracion;

void archivoDeCofiguracion(char* argv);

int main(int argc, char *argv[]) {
	archivoDeCofiguracion(argv[1]);
	char *ipfs = "127.0.0.1";
	int socketfs =iniciar_socket_server(ipfs,puerto);
	int cliente = escuchar_conexiones(socketfs);
	char *handshake = malloc(1024);
	//while(1){
		recibir(cliente,handshake,1024);
		handshake[27] = '/0';
		printf("handshake %s \n",handshake);
	//}
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
