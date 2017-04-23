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

extern t_configuracion *config;
fd_set master;
fd_set read_fds;
int fdmax;

void manejo_conexiones_cpu()
{
	int controlador = 0;

	//Seteo en 0 el master y temporal
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//Cargo el socket server
	FD_SET(config->server_cpu, &master);

	//Cargo el socket mas grande
	fdmax = config->server_cpu;

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
					if (i == config->server_cpu)
					{
						//Gestiono la conexion entrante

						int nuevo_socket = aceptar_conexion(config->server_cpu, &controlador);

						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador > 0)
						{
							error_sockets(&controlador, "");
						}
						else
						{
							//funcion para realizar handshake con nueva conexion
							puts("Se conecto alguien nuevo");
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
