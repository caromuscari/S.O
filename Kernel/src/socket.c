#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/string.h>

//extern int error_recepcion;
//extern int error_conectando;

int iniciar_socket_cliente(char *ip, int puerto/*char *puerto*/)
{
	int connected_socket, puerto_conexion;
	struct sockaddr_in dest;

	//Creating socket
	if ((connected_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("error creando socket\n");
	}
	printf("socket creado \n");

	puerto_conexion = /*atoi*/(puerto);

	dest.sin_family = AF_INET;
	dest.sin_port = htons( puerto_conexion );
	dest.sin_addr.s_addr = inet_addr( ip );

	//Connecting socket
	if (connect(connected_socket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
		printf("error conectando socket\n");
	//	error_conectando = 1;
	}else
	printf("conectado a servidor %s:%d\n", ip, puerto_conexion);

	return connected_socket;
}

int enviar_cl(int socket_emisor, char *mensaje_a_enviar)
{
	int ret;

	size_t sbuffer = sizeof(char)*1024;
	//char* buffer = (char*)malloc(sbuffer);

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);
	//memcpy(buffer, mensaje_a_enviar, sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, 0)) < 0) {
		perror("error en el envio del mensaje");
	}

	free(buffer);
	return ret;
}

char *recibir_cl(int socket_receptor)
{
	int ret;

	char *buffer = malloc(1024);

	if ((ret = recv(socket_receptor, buffer, 1024, 0)) <= 0) {
		printf("error receiving or connection lost \n");
			if (ret == 0) {
				printf("socket %d se desconectó \n", socket_receptor);

			} else {
				printf("error recibiendo el mensaje \n");

			}
//			error_recepcion = 1;
			//close(socket_receptor);
	}

	//buffer[ret]='\0';
//	char *buffer_aux= strdup(buffer);
	//free(buffer);
	return buffer;
	//free(sbuffer);

}

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


int enviar(int socket_emisor, char *mensaje_a_enviar, int *control)
{
	int ret;
	signal(SIGPIPE, SIG_IGN);
	size_t sbuffer = sizeof(char)*1024;
	//char* buffer = (char*)malloc(sbuffer);

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);
	//memcpy(buffer, mensaje_a_enviar, sbuffer);
	*control = 0;

	if ((ret = send(socket_emisor, buffer, sbuffer, MSG_NOSIGNAL)) < 0)
	{
		//close(socket_emisor);
		//escribir_log("Se desconectó un entrenador");
		*control = 1;
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
