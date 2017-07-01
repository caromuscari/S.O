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
#include "manejo_errores.h"
#include "socket.h"
#include "log.h"

extern t_configuracion *config;
extern t_list *list_consolas;
extern pthread_mutex_t mutex_lista_consolas;

fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;
extern int ultimo_pid;

int get_CID();
void desconectar_consola(int socket);
void responder_solicitud_consola(int socket, char *mensaje);
void responder_peticion_prog(int socket, char *mensaje);
int buscar_consola(int socket);

void realizar_handShake_consola(int nuevo_socket)
{
	//Envio mensaje a consola pidiendo sus datos
	char *mensaje = armar_mensaje("K00","");
	enviar(nuevo_socket, mensaje, &controlador);
	escribir_log("Enviado mensaje para handshake con consola");//despues borrar

	if (controlador > 0)
	{
		cerrar_conexion(nuevo_socket);
		escribir_error_log("Fallo el Handshake con el administrador consola");
	}
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador);
		escribir_log("Recibido mensaje para handshake con consola");//despues borrar

		if (controlador > 0)
		{
			cerrar_conexion(nuevo_socket);
			escribir_error_log("Fallo el Handshake con el administrador consola");
		}
		else
		{
			//Aca deberia ir la validacion si el mensaje corresponde a una consola
			char *header = get_header(respuesta);
			if(comparar_header("C", header))
			{
				escribir_log("Se ha conectado una nueva consola");
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
				escribir_log("El administrador de consola rechazo una conexion");
				char *mensaje = "Perdon no sos una Consola, Chau!";
				enviar(nuevo_socket, mensaje, &controlador);
				cerrar_conexion(nuevo_socket);
			}
			free(mensaje);
			free(header);
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

void responder_solicitud_consola(int socket, char *mensaje)
{
	char *codigo = get_codigo(mensaje);
	int cod = atoi(codigo);
	switch(cod)
	{
		case 1 :
			escribir_log("Llego una solicitud de inicio de programa");
			responder_peticion_prog(socket, mensaje);
			break;
		case 2 : ;
			escribir_log("Llego una solicitud de finalizacion de programa");
			char *pid_c = get_mensaje(mensaje);
			int pid = atoi(pid_c);
			forzar_finalizacion(pid, 0, -7);
			free(pid_c);
			break;
		case 3 : ;
			char *con = get_mensaje(mensaje);
			int consola_id = atoi(con);
			forzar_finalizacion(0, consola_id, -6);
			desconectar_consola(socket);
			free(con);
			break;
		default : ;
			escribir_log("El administrador de consola recibio un mensaje desconocido");
			//No se comprende el mensaje recibido por consola
			char *msj_unknow = "K08";
			enviar(socket, msj_unknow, &controlador);
			if (controlador > 0) desconectar_consola(socket);
	}
	free(codigo);
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
	char *cod = get_codigo(mensaje_recibido);
	int codigo_m = atoi(cod);

	if(codigo_m == 3)
	{
		escribir_log("No se puede guardar codigo en memoria");
		enviar(socket, "K05", &controlador);
	}
	else
	{
		escribir_log("Se puede guardar codigo en memoria");
		ultimo_pid ++;
		int consola = buscar_consola(socket);
		agregar_nueva_prog(consola, ultimo_pid, mensaje, socket);

		char *mensaje_conf =  armar_mensaje("K04", string_itoa(ultimo_pid));
		escribir_log_compuesto("este es el mensaje de un proceso recien creado: ",mensaje_conf);
		enviar(socket, mensaje_conf, &controlador);
		free(mensaje_conf);
	}
	if(controlador > 0) cerrar_conexion(socket);

	free(codigo);
	free(mensaje_envio);
	free(mensaje_recibido);
	free(cod);
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
