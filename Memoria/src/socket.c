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

int iniciar_socket_server(char *ip, char *puerto)
{
	int socketServidor;
	struct sockaddr_in my_addr;
	int yes = 1;
	int puerto_conexion;

	//Creating socket
	if ((socketServidor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("error creating socket\n");
	}
	//printf("created socket\n");

	puerto_conexion = atoi(puerto);

	setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( puerto_conexion );
	my_addr.sin_addr.s_addr = inet_addr( ip );

	//Binding socket
	if (bind(socketServidor, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) != 0) {
		perror("error binding socket\n");
	}
	//printf("binded socket\n");

	return socketServidor;
}

int escuchar_conexiones(int socketServidor)
{
	int client_sock_accepted;
	int c;
	struct sockaddr_in client;
	int BACKLOG = 20; //Cantidad de conexiones maximas

	//Listening socket
	if (listen(socketServidor, BACKLOG) != 0) {
		perror("error listening\n");
	}
	//printf("listening at port: %d\n", ntohs(my_addr.sin_port));

	//puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock_accepted = accept(socketServidor, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock_accepted < 0)
	{
		perror("error accepting failed\n");
	    return 1;
	}
	//printf("Connection accepted\n");

	return client_sock_accepted;
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

char *recibir(int socket_receptor)
{
	int ret;

	size_t sbuffer = sizeof(char)*1024;
	char* buffer = (char*)malloc(1024);

	if ((ret = recv(socket_receptor, buffer, sbuffer,MSG_WAITALL)) <=0) {
		//printf("error receiving or connection lost\n");
			if (ret == 0) {
				//printf("socket %d hung up\n", socket_receptor);
				memcpy(buffer,"ERROR",5);
				return buffer;
				free(buffer);
			} else {
				memcpy(buffer,"ERROR",5);
				return buffer;
				free(buffer);

			}
			close(socket_receptor);

	}

	buffer[ret]='\0';

	return buffer;
	free(buffer);
}
