#include <pthread.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include "manejo_errores.h"
#include "fileSystem.h"
#include "estructuras.h"
#include "mensaje.h"
#include "socket.h"

extern t_configuracion *config;
extern t_list *list_cpus;
extern pthread_mutex_t mutex_lista_cpus;
fd_set master;
fd_set read_fds;
int fdmax;
int controlador_cpu = 0;
int id = 0;

void realizar_handShake_cpu(int);
void agregar_lista_cpu(int);
int get_cpuId();
void actualizar_pcb();
void responder_solicitud_cpu(int socket_);
t_program *programa_ejecutando(int socket_);
int get_offset(char *mensaje);
int get_fd(char *mensaje);

void manejo_conexion_cpu()
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
						if (controlador_cpu == 0)
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
						responder_solicitud_cpu(i);
					}
				}
			}
		}
	}
}

void realizar_handShake_cpu(int nuevo_socket)
{
	//Envio mensaje a CPU pidiendo sus datos
	char *mensaje = armar_mensaje("K00","");
	enviar(nuevo_socket, mensaje, &controlador_cpu);

	if (controlador_cpu > 0)
		cerrar_conexion(nuevo_socket);
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador_cpu);

		if (controlador_cpu > 0)
			cerrar_conexion(nuevo_socket);
		else
		{
			//Aca deberia ir la validacion si el mensaje corresponde a cpu
			if(comparar_header("P", respuesta))
				agregar_lista_cpu(nuevo_socket);
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

void actualizar_pcb()
{

}

void agregar_lista_cpu(int nuevo_socket)
{
	t_cpu *nueva_cpu = malloc(sizeof(t_cpu));
	nueva_cpu->socket_cpu = nuevo_socket;
	nueva_cpu->program = malloc(sizeof(t_program));
	nueva_cpu->ejecutando = false;
	nueva_cpu->cpu_id = get_cpuId();

	pthread_mutex_lock(&mutex_lista_cpus);
	list_add(list_cpus, nueva_cpu);
	pthread_mutex_unlock(&mutex_lista_cpus);
}

int get_cpuId()
{
	int ultimo_id = 1;

	void _mayor(t_cpu *cpu)
	{
		if(cpu->cpu_id == ultimo_id)
		{
			ultimo_id++;
		}
	}
	list_iterate(list_cpus, (void*)_mayor);
	return ultimo_id;
}

void responder_solicitud_cpu(int socket_)
{
	int controlador;
	char *mensaje = recibir(socket_, &controlador);

	if(controlador > 0)
	{
		//desconectar_consola(i);
		FD_CLR(socket_, &master);
	}
	else if(comparar_header(mensaje, "P"))
	{
		t_program *prog = programa_ejecutando(socket_);
		switch(get_codigo(mensaje)) {
			case 2 :
				abrir_crear(mensaje, prog, socket_);
				break;
			case 3 : ;
				int offset = get_offset(mensaje);
				int fd = get_fd(mensaje);
				mover_puntero(socket_, offset, fd, prog);
				break;
			default : ;
				//No se comprende el mensaje recibido por cpu
				char *msj_unknow = "K08";
				enviar(socket_, msj_unknow, &controlador);
				//if (controlador > 0) desconectar_consola(socket_);
		}
	}else ; //emisor no reconocido
}

t_program *programa_ejecutando(int socket_)
{
	pthread_mutex_lock(&mutex_lista_cpus);
	t_list *cpus_aux = list_cpus;
	pthread_mutex_unlock(&mutex_lista_cpus);

	bool _cpu_por_socket(t_cpu cpu)
	{
		return cpu.socket_cpu == socket_ ;
	}

	t_cpu *cpu_ejecutando = list_find(cpus_aux, (void *)_cpu_por_socket);

	return cpu_ejecutando->program;
}

int get_fd(char *mensaje)
{
	char *payload = string_substring(mensaje, 0, 2);
	int payload2 = atoi(payload);
	free(payload);
	return atoi(string_substring(mensaje, 3, payload2));
}

int get_offset(char *mensaje)
{
	char *payload = string_substring(mensaje, 0, 2);
	int payload2 = atoi(payload);
	char *payload3 = string_substring(mensaje, 2 + payload2, 4);
	int payload4 = atoi (payload3);

	free(payload);
	free(payload3);

	return atoi(string_substring(mensaje ,(2+payload2+4), payload4));
}
