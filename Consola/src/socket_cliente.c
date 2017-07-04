#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
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

	puerto_conexion = puerto;

	dest.sin_family = AF_INET;
	dest.sin_port = htons( puerto_conexion );
	dest.sin_addr.s_addr = inet_addr( ip );

	//Connecting socket
	if (connect(connected_socket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
		printf("error conectando socket\n");
		//error_conectando = 1;
	}else
	printf("conectado a servidor %s:%d\n", ip, puerto_conexion);

	return connected_socket;
}

int enviar(int socket_emisor, char *mensaje_a_enviar, int tamanio)
{
	int ret;

	size_t sbuffer = sizeof(char)* tamanio;
	//char* buffer = (char*)malloc(sbuffer);

	char *buffer = string_substring(mensaje_a_enviar,0,sbuffer);
	//memcpy(buffer, mensaje_a_enviar, sbuffer);

	if ((ret = send(socket_emisor, buffer, sbuffer, 0)) < 0) {
		perror("error en el envio del mensaje");
	}

	free(buffer);
	return ret;
}

char *recibir(int socket_receptor,int tamanio)
{
	int ret;

	char buffer2[tamanio];
	char *buffer;

	if ((ret = recv(socket_receptor,(void *) buffer2, tamanio, 0)) <= 0) {
		printf("error receiving or connection lost \n");
			if (ret == 0) {
				printf("socket %d se desconectó \n", socket_receptor);

			} else {
				printf("error recibiendo el mensaje \n");

			}
			//error_recepcion = 1;
			//close(socket_receptor);
	}

	buffer2[ret]='\0';
//	char *buffer_aux= strdup(buffer);
	//free(buffer);
	buffer = strdup(buffer2);
	return buffer;
	//free(sbuffer);

}
void cerrar_conexion(int socket_)
{
	close(socket_);
}
