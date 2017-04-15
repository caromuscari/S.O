#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/string.h>
#include <commons/log.h>

//Ceci dice que hay que cambiar todo!!!
//modificar controlador y agregar indice de errores para el controlador

//extern t_log *log;

int iniciar_socket_server(char *ip, int puerto_conexion, int *controlador)
{
	int socketServidor;
	struct sockaddr_in my_addr;
	int yes = 1;
	controlador = 0;

	//Creating socket
	if ((socketServidor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		*controlador = 3;
	}
	//printf("created socket\n");

	setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( puerto_conexion );
	my_addr.sin_addr.s_addr = inet_addr( ip );

	//Binding socket
	if (bind(socketServidor, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) != 0)
	{
		*controlador = 4;
	}
	else
	{
		//escribir_log("Kernel - Socket server creado");
		//printf("FS - Socket server creado");
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
	if (listen(socketServidor, BACKLOG) != 0)
	{
		*controlador = 5;
	}

	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock_accepted = accept(socketServidor, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock_accepted < 0)
	{
		*controlador = 6;
	}//else
	//escribir_log_con_numero("Kernel - Nueva conexion aceptada para socket: ", client_sock_accepted);
	//printf("FS - Nueva conexion aceptada para socket:%d", client_sock_accepted);
	return client_sock_accepted;
}

int enviar(int socket_emisor, char *mensaje_a_enviar, int *controlador)
{
	int ret;
	signal(SIGPIPE, SIG_IGN);
	size_t sbuffer = sizeof(char)*1024;
	*controlador = 0;

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, MSG_NOSIGNAL)) < 0)
	{
		//close(socket_emisor);
		*controlador = 7;

	} else
	{
		//Este mensaje debera esta en la funcion que invoque esta
		//escribir_log_con_numero("Kernel - Exito al enviar mensaje a PID: ", *prog->PID);
	}
	free(buffer);
	return ret;
}

void recibir(int socket_receptor, int *controlador, char *buffer)
{
	int ret;

	*controlador = 0;

	if ((ret = recv(socket_receptor, buffer, 1024, 0)) <= 0)
	{
		//printf("error receiving or connection lost \n");
		if (ret == 0)
		{
			*controlador = 8;
		} else {
			//printf("error recibiendo el mensaje \n");
				}
		*controlador = 1;
		//close(socket_receptor);
	}

}

void cerrar_conexion(int socket_)
{
	close(socket_);
}
