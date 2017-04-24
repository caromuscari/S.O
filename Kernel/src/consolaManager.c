/*
 * cpuManager.c
 *
 *  Created on: 16/4/2017
 *      Author: utnso
 */
#include <pthread.h>
#include <commons/log.h>
#include <stdlib.h>
#include "estructuras.h"
#include "manejo_errores.h"
#include "socket.h"
#include <string.h>
#include <commons/string.h>
#include "mensaje.h"

extern t_configuracion *config;
fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;

void realizar_handShake_consola(int nuevo_socket);

void manejo_conexiones_consolas()
{
	//Seteo en 0 el master y temporal
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//Cargo el socket server
	FD_SET(config->server_consola, &master);

	//Cargo el socket mas grande
	fdmax = config->server_consola;

	//Bucle principal
	while (1)
	{
		read_fds = master;

		int selectResult = select(fdmax + 1, &read_fds, NULL, NULL, NULL);

		if (selectResult == -1)
		{
			break;
		}
		else
		{
			//Recorro los descriptores para ver quien llamo
			int i;
			for (i = 0; i <= fdmax; i++)
			{
				if (FD_ISSET(i, &read_fds))
				{
					//Se detecta alguien nuevo llamando?
					if (i == config->server_consola)
					{
						//Gestiono la conexion entrante

						int nuevo_socket = aceptar_conexion(config->server_consola, &controlador);

						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador > 0)
						{
							error_sockets(&controlador, "");
						}
						else
						{
							//funcion para realizar handshake con nueva conexion
							realizar_handShake_consola(nuevo_socket);
						}

						//Cargo la nueva conexion a la lista y actualizo el maximo
						FD_SET(nuevo_socket, &master);

						if (nuevo_socket > fdmax)
						{
							fdmax = nuevo_socket;
						}
					}
					else
					{
						//Es una conexion existente, respondo a lo que me pide
						//aqui deberia ir la funcion que tome el socket que me hablo y hacer algo
						//clientHandler((int) i);
						puts("alguien conectado me hablo y dijo:");
						puts(recibir(i, &controlador));
					}
				}
			}
		}
	}
}

void realizar_handShake_consola(int nuevo_socket)
{
	//Envio mensaje a consola pidiendo sus datos
	char *mensaje = "K00";
	enviar(nuevo_socket, mensaje, &controlador);

	if (controlador > 0)
	{
		error_sockets(&controlador, string_itoa(nuevo_socket));
		cerrar_conexion(nuevo_socket);
	}
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador);

		if (controlador > 0)
		{
			error_sockets(&controlador, string_itoa(nuevo_socket));
			cerrar_conexion(nuevo_socket);
		}
		else
		{
			//Aca deberia ir la validacion si el mensaje corresponde a cpu
			if(comparar_header("C", respuesta))
			{
				//Es una CPU, se puede hablar
				char *mensaje = "K01";
				enviar(nuevo_socket, mensaje, &controlador);

				if (controlador > 0)
				{
					error_sockets(&controlador, string_itoa(nuevo_socket));
					cerrar_conexion(nuevo_socket);
				}
				else
				{

				}
			}
			else
			{
				//El recien conectado NO corresponde a una consola
				char *mensaje = "Perdon no sos una Consola, Chau!";
				enviar(nuevo_socket, mensaje, &controlador);
				cerrar_conexion(nuevo_socket);
			}
		}
	}
}

void desconectar_consola(int socket)
{

}
