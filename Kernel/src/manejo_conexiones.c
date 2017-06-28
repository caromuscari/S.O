#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "mensaje.h"
#include "planificador.h"
#include "estructuras.h"
#include "cpuManager.h"
#include "consolaManager.h"
#include "manejo_errores.h"
#include "socket.h"
#include "log.h"

extern t_configuracion *config;
fd_set master;
fd_set read_fds;
int fdmax;

void realizar_handShake_consola(int nuevo_socket);
void realizar_handShake_cpu(int nuevo_socket);
void direccionar(int socket_rec);

void manejo_conexiones()
{
	int controlador = 0;

	escribir_log("Iniciando administrador de conexiones");
	//Seteo en 0 el master y temporal
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//Cargo el socket server
	FD_SET(config->server_consola, &master);
	FD_SET(config->server_cpu, &master);

	//Cargo el socket mas grande
	fdmax = config->server_consola;

	//Bucle principal
	while (1)
	{
		read_fds = master;

		int selectResult = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		escribir_log("Actividad detectada en administrador de conexiones");

		if (selectResult == -1)
		{
			break;
			escribir_error_log("Error en el administrador de conexiones");
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
						escribir_log("Se detecto actividad en el server consola");
						int nuevo_socket = aceptar_conexion(config->server_consola, &controlador);

						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador == 0)
						{
							realizar_handShake_consola(nuevo_socket);
						}

						//Cargo la nueva conexion a la lista y actualizo el maximo
						FD_SET(nuevo_socket, &master);

						if (nuevo_socket > fdmax)
						{
							fdmax = nuevo_socket;
						}
					}
					else if (i == config->server_cpu)
					{
						//Gestiono la conexion entrante
						escribir_log("Se detecto actividad en el server CPU");
						int nuevo_socket = aceptar_conexion(config->server_cpu, &controlador);

						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador == 0)
						{
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
						direccionar(i);
					}
				}
			}
		}
	}
}

void direccionar(int socket_rec)
{
	int controlador = 0;
	//Es una conexion existente, respondo a lo que me pide
	char *mensaje_recibido = recibir(socket_rec, &controlador);
	char *header = get_header(mensaje_recibido);

	if(controlador > 0)
	{
		escribir_log("Se eliminara una conexion");
		desconectar_consola(socket_rec);
		FD_CLR(socket_rec, &master);
	}
	else if(comparar_header(header,"C"))
	{
		escribir_log("Se recibio un mensaje de una consola");
		responder_solicitud_consola(socket_rec, mensaje_recibido);
	}
	else if(comparar_header(header,"P"))
	{
		escribir_log("Se recibio un mensaje de una CPU");
		responder_solicitud_cpu(socket_rec, mensaje_recibido);
	}
	else
	{
		escribir_log("Se recibio un mensaje no reconocido");
	}

	free(mensaje_recibido);
	free(header);
}
