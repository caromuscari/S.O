/*
 * cpuManager.c
 *
 *  Created on: 16/4/2017
 *      Author: utnso
 */
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "mensaje.h"
#include "planificador.h"
#include "estructuras.h"
#include "manejo_errores.h"
#include "socket.h"

extern t_configuracion *config;
extern t_list *consolas;
extern t_queue *programas;

fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;

void realizar_handShake_consola(int nuevo_socket);
void manejo_conexiones_consolas();
int *get_CID();
void desconectar_consola(int socket);
void responder_solicitud(int socket, char *mensaje);

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
						char *mensaje_recibido = recibir(i, &controlador);
						if(controlador > 0)
						{
							error_sockets(&controlador, atoi(i));
							desconectar_consola(i);
						}
						else
						{
							responder_solicitud(i, mensaje_recibido);
						}
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
			//Aca deberia ir la validacion si el mensaje corresponde a una consola
			if(comparar_header("C", respuesta))
			{
				//Es una Consola, se puede agregar
				t_consola *nueva_consola;
				*(nueva_consola->socket) = nuevo_socket;
				nueva_consola->CID = get_CID();
				list_add(consolas, nueva_consola);
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

int *get_CID()
{
	int *ultimo_id = 1;

	void _mayor(t_consola *consola)
	{
		if(consola->CID == ultimo_id)
		{
			ultimo_id++;
		}
	}

	list_iterate(consolas, (void*)_mayor);
	return ultimo_id;
}

void responder_solicitud(int socket, char *mensaje)
{
	switch(get_codigo(mensaje)) {
		case 1 :
			responder_peticion_prog(socket, mensaje);
			break;
		default :
			//En caso de no entender el mensaje
			printf("deberia hacer algo por defecto");
	}
}

void desconectar_consola(int socket)
{
	bool _localizar(t_consola *con)
	{
		return (int)con->socket == socket;
	}
	t_consola *consola = list_remove_by_condition(consolas, (void*)_localizar);
	//faltaria el tema de eliminar todos los programas dependientes de esta consola
}

void responder_peticion_prog(int socket, char *mensaje)
{
	char *identificador = "K06";
	char *codigo = get_mensaje(mensaje);
	char *mensaje_envio =  armar_mensaje(identificador, codigo);

	//envio del codigo a memoria para ver si hay espacio
	enviar(config->cliente_memoria, mensaje_envio, &controlador);
	if (controlador > 0)
	{
		error_sockets(&controlador, string_itoa(config->cliente_memoria));
	}

	//recibo la respuesta de memoria
	char *mensaje_recibido = recibir(config->cliente_memoria, &controlador);
	if (controlador > 0)
	{
		error_sockets(&controlador, string_itoa(config->cliente_memoria));
	}

	if(get_codigo(mensaje_recibido) == 01)
	{
		enviar(socket, "K05", &controlador);
		if (controlador > 0)
		{
			error_sockets(&controlador, socket);
		}
	}
	else
	{
		agregar_nueva_prog(socket, mensaje);//deberia haber otro proceso aca
	}
}
