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
#include "log.h"
#include "semaforos_vglobales.h"

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
void responder_solicitud_cpu(int socket_, char *mensaje);
t_program *programa_ejecutando(int socket_);
int get_offset(char *mensaje);
int get_fd(char *mensaje);
char *get_variable(char *mensaje);
char *get_numero(char *mensaje);

void realizar_handShake_cpu(int nuevo_socket)
{
	//Envio mensaje a CPU pidiendo sus datos
	char *mensaje = strdup("K00");
	enviar(nuevo_socket, mensaje, &controlador_cpu);

	if (controlador_cpu > 0)
	{
		cerrar_conexion(nuevo_socket);
		escribir_error_log("Fallo el Handshake con el administrador CPU");
	}
	else
	{
		char *respuesta = recibir(nuevo_socket, &controlador_cpu);

		escribir_log(respuesta);

		if (controlador_cpu > 0)
		{
			cerrar_conexion(nuevo_socket);
			escribir_error_log("Fallo el Handshake con el administrador CPU");
		}
		else
		{
			char *header = get_header(respuesta);
			//Aca deberia ir la validacion si el mensaje corresponde a cpu
			if(comparar_header("P", header))
			{
				escribir_log("Se ha conectado una nueva CPU");
				agregar_lista_cpu(nuevo_socket);
			}
			else
			{
				//El recien conectado NO corresponde a una CPU
				escribir_log("El administrador de CPUs rechazo una conexion");
				char *mensaje = "Perdon no sos una CPU, Chau!";
				enviar(nuevo_socket, mensaje, &controlador_cpu);
				cerrar_conexion(nuevo_socket);
			}
			free(header);
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
	nueva_cpu->ejecutando = 0;
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

void responder_solicitud_cpu(int socket_, char *mensaje)
{
	char *header = get_header(mensaje);
	if(comparar_header(header, "P"))
	{
		t_program *prog = programa_ejecutando(socket_);
		char *cod = get_codigo(mensaje);
		int codigo = atoi(cod);

		switch(codigo) {
			case 2 :
				escribir_log("Se recibio una peticion de CPU de abrir crear");
				abrir_crear(mensaje, prog, socket_);
				break;
			case 3 : ;
				escribir_log("Se recibio una peticion de CPU de mover puntero");
				int offset = get_offset(mensaje);
				int fd = get_fd(mensaje);
				mover_puntero(socket_, offset, fd, prog);
				break;
			case 9 : ;
				escribir_log("Se recibió una petición de CPU para obtener valor de variable compartida");
				break;

			case 10 : ;
				escribir_log("Se recibió una petición de CPU para setear valor de variable compartida");
				char *variable = get_variable(mensaje);
				char *numero = get_numero(mensaje);
				int num = atoi(numero);
				set_vglobal(variable, num, prog->PID);
				free(variable);
				free(numero);
				break;
			case 11: ;
				int controlador;
				char *mensaje_recibido = get_mensaje(mensaje);
				char *mensaje_enviar = armar_mensaje("K09",mensaje_recibido);
				enviar(prog->socket_consola, mensaje_enviar, &controlador);
				enviar(socket_, "ok", &controlador);
				free(mensaje_recibido);
				free(mensaje_enviar);
				break;
			default : ;
				//No se comprende el mensaje recibido por cpu
				escribir_error_log("Se recibio una peticion de CPU desconocida");
				char *msj_unknow = "K08";
				enviar(socket_, msj_unknow, &controlador_cpu);
				free(msj_unknow);
				//if (controlador > 0) desconectar_consola(socket_);
		} free(header);
	}else
		escribir_error_log("No se reconocio el mensaje de CPU"); //emisor no reconocido
}

t_program *programa_ejecutando(int socket_)
{
	pthread_mutex_lock(&mutex_lista_cpus);
	t_list *cpus_aux = list_cpus;
	pthread_mutex_unlock(&mutex_lista_cpus);

	bool _cpu_por_socket(t_cpu *cpu)
	{
		return !(cpu->socket_cpu == socket_);
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

char *get_variable(char *mensaje)
{
	char *payload = string_substring(mensaje, 3, 10);
	int payload1 = atoi(payload);
	free(payload);
	return string_substring(mensaje, 13, payload1-4);
}

char *get_numero(char *mensaje)
{
	char *payload = string_substring(mensaje, 3, 10);
	int desde = 13 + atoi(payload) - 4;
	free(payload);
	return string_substring(mensaje, desde, 4);
}
