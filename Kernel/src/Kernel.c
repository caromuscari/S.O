#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include "estructuras.h"
#include "configuracion.h"
#include "socket.h"
#include "log.h"
#include "cpuManager.h"
#include "consolaManager.h"
#include "manejo_errores.h"
#include "memoria.h"
#include "planificador.h"

char *ruta_config;
t_configuracion *config;
char *sem_id;
char *sem_in;
char *shared;
t_dictionary *sems;
t_list *list_cpus;
t_list *list_consolas;
t_list *list_ejecutando;
t_list *list_finalizados;
t_list *list_bloqueados;
t_list *global_fd;
t_queue *cola_nuevos;
t_queue *cola_listos;
t_log *log;
int ultimo_pid = 0;
int tam_pagina = 0;

void inicializar_variables();
void liberar_memoria();
void mostrar_configuracion();
void handshakearFS();
void handshakearMemory();
void crear_conexiones();

int main(int argc, char*argv[])
{
	ruta_config = strdup("/home/utnso/Archivos/Kernel/configuracion"/*argv[1]*/);

	inicializar_variables();
	leer_configuracion();
	//mostrar_configuracion();
	crear_archivo_log("/home/utnso/log_kernel");

	crear_conexiones();
	handshakearMemory();
	manejo_conexiones_consolas();
	handshakearFS();

	//manejo_conexiones_cpu();
	//manejo_conexiones_consola();

	liberar_memoria();

	return EXIT_SUCCESS;
}

void inicializar_variables()
{
	config = malloc(sizeof(t_configuracion));
	config->algoritmo = strdup("");
	config->ip_fs = strdup("");
	config->ip_memoria = strdup("");
	config->ip_kernel = strdup("");
	/*config->sem_ids = list_create();
	config->sem_init = list_create();
	config->shared_vars = list_create();*/
	list_cpus = list_create();
	list_consolas = list_create();
	list_ejecutando = list_create();
	list_finalizados = list_create();
	cola_nuevos = queue_create();
	cola_listos = queue_create();
	list_bloqueados = list_create();
}

void mostrar_configuracion()
{
	printf("Configuracion del proceso Kernel: \n");
	printf("Puerto Programa: %d \n", config->puerto_prog);
	printf("Puerto CPU: %d \n", config->puerto_cpu);
	printf("IP Memoria: %s \n", config->ip_memoria);
	printf("IP Kernel: %s \n", config->ip_kernel);
	printf("Puerto Memoria: %d \n", config->puerto_memoria);
	printf("IP File System: %s \n", config->ip_fs);
	printf("Puerto File System: %d \n", config->puerto_fs);
	printf("Quantum para RR: %d \n", config->quantum);
	printf("Quantum para ejecutar cada sentencia: %d \n",config->quantum_sleep);
	printf("Algoritmo: %s \n", config->algoritmo);
	printf("Grado de Multiprogramacion: %d \n", config->grado_multiprog);
	printf("SEM ID's: %s \n", sem_id);
	printf("Valor Inicial de Semaforos: %s \n", sem_in);
	printf("Variables compartidas: %s \n", shared);
	printf("Stack Size: %d \n", config->stack_size);

	free(sem_id);
	free(sem_in);
	free(shared);
	free(ruta_config);
}

void liberar_memoria()
{
	free(config->algoritmo);
	free(config->ip_fs);
	free(config->ip_memoria);
	free(config->ip_kernel);
	free(config);
	/*list_destroy(config->sem_ids);
	list_destroy(config->sem_init);
	list_destroy(config->shared_vars);
	list_destroy_and_destroy_elements(cpus, void(*element_destroyer)(void*));
	list_destroy_and_destroy_elements(consolas, void(*element_destroyer)(void*));	*/
}

void handshakearFS()
{
	int controlador = 0;
	char *mensaje = malloc(7);
	strcpy(mensaje,"K03");
	enviar(config->cliente_fs, mensaje, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "FileSystem");	}
	free(mensaje);
}
//void liberar_lista_char()


void crear_conexiones()
{
	int controlador = 0;
	config->server_cpu = iniciar_socket_server(config->ip_kernel, config->puerto_cpu, &controlador);
	config->server_consola = iniciar_socket_server(config->ip_kernel, config->puerto_prog, &controlador);
	//config->cliente_fs = iniciar_socket_cliente(config->ip_fs, config->puerto_fs, &controlador);
	config->cliente_memoria = iniciar_socket_cliente(config->ip_memoria, config->puerto_memoria, &controlador);
}
