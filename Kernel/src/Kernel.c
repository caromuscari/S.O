#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "estructuras.h"
#include "configuracion.h"
#include "socket.h"
#include "log.h"
#include "cpuManager.h"
#include "consolaManager.h"
#include "manejo_errores.h"

char *ruta_config;
t_configuracion *config;
char *sem_id;
char *sem_in;
char *shared;
t_list *cpus;
t_list *consolas;
t_list *programas;
t_log *log;

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
	handshakearFS();
	handshakearMemory();

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
	cpus = list_create();
	consolas = list_create();
	programas = list_create();
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
	list_destroy_and_destroy_elements(consolas, void(*element_destroyer)(void*));
	list_destroy_and_destroy_elements(programas, void(*element_destroyer)(void*));	*/
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
void handshakearMemory()
{
	int controlador = 0;
	char *mensaje = malloc(7);
	strcpy(mensaje,"K02");
	enviar(config->cliente_memoria, mensaje, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "Memoria");	}
	free(mensaje);
}

void crear_conexiones()
{
	int controlador = 0;
	config->server_cpu = iniciar_socket_server(config->ip_kernel, config->puerto_cpu, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "");	}

	config->server_consola = iniciar_socket_server(config->ip_kernel, config->puerto_prog, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "");	}

	config->cliente_fs = iniciar_socket_cliente(config->ip_fs, config->puerto_fs, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "");	}

	config->cliente_memoria = iniciar_socket_cliente(config->ip_memoria, config->puerto_memoria, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "");	}
}
