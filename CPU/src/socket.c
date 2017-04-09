/*
 * socket.c
 *
 *  Created on: 10/10/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>
#include "socket.h"

//#define BACKLOG 3			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
//#define PACKAGESIZE 1024
//#define IP_LOCAL "127.0.0.1"

int iniciar_socket_cliente(char *ip, char *puerto)
{
	int connected_socket, puerto_conexion;
	struct sockaddr_in dest;

	//Creating socket
	if ((connected_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("error creating socket\n");
	}
	printf("created socket\n");

	puerto_conexion = atoi(puerto);

	dest.sin_family = AF_INET;
	dest.sin_port = htons( puerto_conexion );
	dest.sin_addr.s_addr = inet_addr( ip );

	//Connecting socket
	if (connect(connected_socket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
		perror("error connecting socket\n");
	}
	printf("connected socket to server %s:%d\n", ip, puerto_conexion);

	return connected_socket;
}
char *recibir(int socket_receptor)
{
	int ret;

	size_t sbuffer = sizeof(char)*1024;
	char* buffer = (char*)malloc(1024);

	if ((ret = recv(socket_receptor, buffer, sbuffer,MSG_WAITALL)) <= 0) {
		printf("error receiving or connection lost\n");
			if (ret == 0) {
				printf("socket %d hung up\n", socket_receptor);
			} else {
				perror("error receiving massage");
			}
			close(socket_receptor);
	}

	buffer[ret]='\0';

	return buffer;
	free(buffer);
}

int enviarDATA(int socket_emisor, t_DATA *DATA_a_enviar)
{
	int ret;

	t_DATA* buffer = malloc(sizeof(t_DATA));

	memcpy(buffer, DATA_a_enviar, sizeof(t_DATA));

	if ((ret = send(socket_emisor, (void*)buffer, sizeof(t_DATA), 0)) < 0) {
		perror("Error en envio de DATA");
	}

	free(buffer);
	return ret;
}

t_DATA *recibirDATA(int socket_receptor)
{
	int ret;

	t_DATA* buffer = malloc(sizeof(t_DATA));

	if ((ret = recv(socket_receptor, buffer, sizeof(t_DATA), 0)) <= 0) {
		printf("error receiving or connection lost\n");
			if (ret == 0) {
				printf("socket %d hung up\n", socket_receptor);
			} else {
				perror("error receiving massage");
			}
			close(socket_receptor);
	}

	return buffer;
	free(buffer);
}
int enviar(int socket_emisor, char *mensaje_a_enviar)
{
	int ret;

	size_t sbuffer = sizeof(char)*1024;
	char* buffer = (char*)malloc(sbuffer);

	memcpy(buffer, mensaje_a_enviar, sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, 0)) < 0) {
		perror("error en el envio del mensaje");
	}

	free(buffer);
	return ret;
}
