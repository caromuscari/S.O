#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "estructuras.h"
#include <commons/string.h>
#include "log.h"
#include <commons/log.h>

//Ceci dice que hay que cambiar todo!!!
//modificar controlador y agregar indice de errores para el controlador

extern t_log *log;

int iniciar_socket_cliente(char *ip, int puerto_conexion, t_program *program)
{
	int connected_socket;
	struct sockaddr_in dest;
	program->control = 0;

	//Creating socket
	if ((connected_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		//perror("error creando socket\n");
		escribir_log_con_numero("Kernel - Error creando socket para PID: ", *program->PID);
		*program->control = 1;
	}
	escribir_log_con_numero("Kernel - Socket creado para PID: ", *program->PID);
	//printf("socket creado \n");

	dest.sin_family = AF_INET;
	dest.sin_port = htons( puerto_conexion );
	dest.sin_addr.s_addr = inet_addr( ip );

	//Connecting socket
	if (connect(connected_socket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
		//printf("error conectando socket\n");
		escribir_log_con_numero("Kernel - Error conectando socket para PID: ", *program->PID);
		*program->control = 1;
	//	error_conectando = 1;
	}else
	//printf("conectado a servidor %s:%d\n", ip, puerto_conexion);}
	{
		char *aux = strdup("Kernel - Socket conectado a IP: ");
		string_append(&aux, ip);
		string_append(&aux, ", Puerto: ");
		string_append(&aux, (char *)puerto_conexion);
		string_append(&aux, " para PID: ");
		escribir_log_con_numero(aux, *program->PID);
		free(aux);
	}
	return connected_socket;
}

int iniciar_socket_server(char *ip, int puerto_conexion, int *controlador)
{
	int socketServidor;
	struct sockaddr_in my_addr;
	int yes = 1;
	controlador = 0;

	//Creating socket
	if ((socketServidor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		//perror("error creating socket\n");
		escribir_log("Kernel - Error creando socket server");
		*controlador = 1;
	}
	//printf("created socket\n");

	setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( puerto_conexion );
	my_addr.sin_addr.s_addr = inet_addr( ip );

	//Binding socket
	if (bind(socketServidor, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) != 0) {
		//perror("error binding socket\n");
		escribir_log("Kernel - Error bindeando socket server");
		*controlador = 1;
	}
	else
	{
		escribir_log("Kernel - Socket server creado");
	}
	//printf("binded socket\n");

	return socketServidor;
}

int escuchar_conexiones(int socketServidor, int *controlador)
{
	int client_sock_accepted;
	int c;
	struct sockaddr_in client;
	int BACKLOG = 20; //Cantidad de conexiones maximas
	controlador = 0;

	//Listening socket
	if (listen(socketServidor, BACKLOG) != 0) {
		//perror("error listening\n");
		escribir_log("Kernel - Socket server, error escuchando");
		*controlador = 1;
	}
	//printf("listening at port: %d\n", ntohs(my_addr.sin_port));

	//puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock_accepted = accept(socketServidor, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock_accepted < 0)
	{
		//perror("error accepting failed\n");
		escribir_log("Kernel - Error aceptando conexion");
		*controlador = 1;
	}
	//printf("Connection accepted\n");

	escribir_log_con_numero("Kernel - Nueva conexion aceptada para socket: ", client_sock_accepted);
	return client_sock_accepted;
}

int enviar_srv(int socket_emisor, char *mensaje_a_enviar, t_program *prog)
{
	int ret;
	signal(SIGPIPE, SIG_IGN);
	size_t sbuffer = sizeof(char)*1024;
	*prog->control = 0;
	//char* buffer = (char*)malloc(sbuffer);

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);
	//memcpy(buffer, mensaje_a_enviar, sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, MSG_NOSIGNAL)) < 0)
	{
		//close(socket_emisor);
		*prog->control = 1;
		escribir_log_con_numero("Kernel - Error al enviar mensaje a PID: ", *prog->PID);
		//perror("error en el envio del mensaje");
	} else
	{
		escribir_log_con_numero("Kernel - Exito al enviar mensaje a PID: ", *prog->PID);
	}

	free(buffer);
	return ret;
}

int enviar_clt(int socket_emisor, char *mensaje_a_enviar, int *controlador)
{
	int ret;
	signal(SIGPIPE, SIG_IGN);
	size_t sbuffer = sizeof(char)*1024;
	*controlador = 0;
	//char* buffer = (char*)malloc(sbuffer);

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);
	//memcpy(buffer, mensaje_a_enviar, sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, MSG_NOSIGNAL)) < 0)
	{
		//close(socket_emisor);
		*controlador = 1;
		//perror("error en el envio del mensaje");
	}


	free(buffer);
	return ret;
}

char *recibir(int socket_receptor, int *controlador)
{
	int ret;

	char *buffer = malloc(1024);

	*controlador = 0;

	if ((ret = recv(socket_receptor, buffer, 1024, 0)) <= 0)
	{
		//printf("error receiving or connection lost \n");
		if (ret == 0)
		{
			//printf("socket %d se desconectó \n", socket_receptor);
		} else {
			//printf("error recibiendo el mensaje \n");
				}
		*controlador = 1;
		//close(socket_receptor);
	}


	//buffer[ret]='\0';
	char *buffer_aux= strdup(buffer);
	free(buffer);
	return buffer_aux;
}

void cerrar_conexion(int socket_)
{
	close(socket_);
}
