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
extern t_list *list_consolas;
extern pthread_mutex_t mutex_lista_consolas;

fd_set master;
fd_set read_fds;
int fdmax;
int controlador;
extern int ultimo_pid;

void realizar_handShake_consola(int nuevo_socket);
int get_CID();
void desconectar_consola(int socket);
void responder_solicitud(int socket, char *mensaje);
void responder_peticion_prog(int socket, char *mensaje);
int buscar_consola(int socket);

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
					else
					{
						//Es una conexion existente, respondo a lo que me pide
						char *mensaje_recibido = recibir(i, &controlador);

						if(controlador > 0)
						{
							desconectar_consola(i);
							FD_CLR(i, &master);
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
	char *mensaje = armar_mensaje("K00","");
	enviar(nuevo_socket, mensaje, &controlador);

	if (controlador > 0)
		cerrar_conexion(nuevo_socket);
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador);

		if (controlador > 0)
			cerrar_conexion(nuevo_socket);
		else
		{
			//Aca deberia ir la validacion si el mensaje corresponde a una consola
			if(comparar_header("C", respuesta))
			{
				//Es una Consola, se puede agregar
				t_consola *nueva_consola = malloc(sizeof(t_consola));
				nueva_consola->socket = nuevo_socket;
				nueva_consola->CID = get_CID();

				pthread_mutex_lock(&mutex_lista_consolas);
				list_add(list_consolas, nueva_consola);
				pthread_mutex_unlock(&mutex_lista_consolas);
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

int get_CID()
{
	int ultimo_id = 1;

	void _mayor(t_consola *consola)
	{
		if(consola->CID == ultimo_id)
		{
			ultimo_id++;
		}
	}
	list_iterate(list_consolas, (void*)_mayor);
	return ultimo_id;
}

void responder_solicitud(int socket, char *mensaje)
{
	switch(get_codigo(mensaje)) {
		case 1 :
			responder_peticion_prog(socket, mensaje);
			break;
		case 2 : ;
			int pid = atoi(get_mensaje(mensaje));
			forzar_finalizacion(pid, 0, 0);
			break;
		case 3 : ;
			int consola_id = atoi(get_mensaje(mensaje));
			forzar_finalizacion(0, consola_id, 0);
			desconectar_consola(socket);
			break;
		default : ;
			//No se comprende el mensaje recibido por consola
			char *msj_unknow = "K08";
			enviar(socket, msj_unknow, &controlador);
			if (controlador > 0) desconectar_consola(socket);
	}
}

void eliminar_consola(int consola_id)
{
	bool _localizar(t_consola *con)
	{
		return (con->CID == consola_id);
	}

	void liberar_consola(t_consola *consola)
	{
		free(consola);
	}

	pthread_mutex_lock(&mutex_lista_consolas);
	list_remove_and_destroy_by_condition(list_consolas, (void*)_localizar, (void*)liberar_consola);
	pthread_mutex_unlock(&mutex_lista_consolas);
}

void responder_peticion_prog(int socket, char *mensaje)
{
	char *identificador = "K06";
	char *codigo = get_mensaje(mensaje);
	char *mensaje_envio =  armar_mensaje(identificador, codigo);

	//envio del codigo a memoria para ver si hay espacio
	enviar(config->cliente_memoria, mensaje_envio, &controlador);

	//recibo la respuesta de memoria
	char *mensaje_recibido = recibir(config->cliente_memoria, &controlador);
	int codigo_m = get_codigo(mensaje_recibido);

	if(codigo_m == 3)
	{
		enviar(socket, "K05", &controlador);
	}
	else
	{
		ultimo_pid ++;
		int consola = buscar_consola(socket);
		agregar_nueva_prog(consola, ultimo_pid, mensaje);

		char *mensaje_conf =  armar_mensaje("K04", string_itoa(ultimo_pid));
		enviar(socket, mensaje_conf, &controlador);
	}
	if(controlador > 0) cerrar_conexion(socket);
}

int buscar_consola(int socket)
{
	bool _buscar_consola_lst(t_consola *consola)
	{
		return (consola->socket == socket);
	}

	pthread_mutex_lock(&mutex_lista_consolas);
	t_consola *cons = list_find(list_consolas, (void*)_buscar_consola_lst);
	pthread_mutex_unlock(&mutex_lista_consolas);

	return cons->CID;
}

void desconectar_consola(int socket)
{
	int consola_muere = buscar_consola(socket);
	if(consola_muere)
	{
		forzar_finalizacion(0, consola_muere, 0);
		eliminar_consola(consola_muere);
	}
	cerrar_conexion(socket);
}
