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
int controlador_cpu = 0;

void realizar_handShake_cpu(int nuevo_socket);
void agregar_lista_cpu(int nuevo_socket, char *mensaje_con_datos);

void manejo_conexiones_cpu()
{
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

						int nuevo_socket = aceptar_conexion(config->server_cpu, &controlador_cpu);

						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador_cpu > 0)
						{
							error_sockets(&controlador_cpu, "");
						}
						else
						{
							//funcion para realizar handshake con nueva conexion
							realizar_handShake_cpu(nuevo_socket);
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
						puts(recibir(i, &controlador_cpu));
					}
				}
			}
		}
	}
}

void realizar_handShake_cpu(int nuevo_socket)
{
	//Envio mensaje a CPU pidiendo sus datos
	char *mensaje = "K00";
	enviar(nuevo_socket, mensaje, &controlador_cpu);

	if (controlador_cpu > 0)
	{
		error_sockets(&controlador_cpu, string_itoa(nuevo_socket));
		cerrar_conexion(nuevo_socket);
	}
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador_cpu);

		if (controlador_cpu > 0)
		{
			error_sockets(&controlador_cpu, string_itoa(nuevo_socket));
			cerrar_conexion(nuevo_socket);
		}
		else
		{
			//Aca deberia ir la validacion si el mensaje corresponde a cpu
			if(comparar_header("P", respuesta))
			{
				//Es una CPU, se puede hablar
				char *mensaje = "K01";
				enviar(nuevo_socket, mensaje, &controlador_cpu);

				if (controlador_cpu > 0)
				{
					error_sockets(&controlador_cpu, string_itoa(nuevo_socket));
					cerrar_conexion(nuevo_socket);
				}
				else
				{
					char *mensaje_con_datos = recibir(nuevo_socket, &controlador_cpu);

					if (controlador_cpu > 0)
					{
						error_sockets(&controlador_cpu, string_itoa(nuevo_socket));
						cerrar_conexion(nuevo_socket);
					}
					else
					{
						agregar_lista_cpu(nuevo_socket, mensaje_con_datos);
					}
				}
			}
			else
			{
				//El recien conectado NO corresponde a una CPU
				char *mensaje = "Perdon no sos una CPU, Chau!";
				enviar(nuevo_socket, mensaje, &controlador_cpu);
				cerrar_conexion(nuevo_socket);
			}
		}
	}
}

void agregar_lista_cpu(int nuevo_socket, char *mensaje_con_datos)
{
	//me paso los datos y deberia guardarlos en mi lista de cpu
}
