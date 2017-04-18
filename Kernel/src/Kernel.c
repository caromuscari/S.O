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

char *ruta_config;
t_configuracion *config;
char *sem_id;
char *sem_in;
char *shared;
int socket_memoria;
int socket_fs;
t_log *log;

void inicializar_variables();
void liberar_memoria();
void mostrar_configuracion();
void handshakearFS(int socket_fs);

int main(int argc, char*argv[])
{

	ruta_config = strdup("/home/utnso/Archivos/Kernel/configuracion"/*argv[1]*/);

	inicializar_variables();
	leer_configuracion();
	mostrar_configuracion();
	crear_archivo_log("/home/utnso/log_kernel");

	int controlador = 0;
	config->server_cpu = iniciar_socket_server(config->ip_kernel, config->puerto_cpu, &controlador);
	manejo_conexiones_cpu();

	socket_memoria = iniciar_socket_cliente(config->ip_memoria, config->puerto_memoria, &controlador);
	//enviar(socket_memoria, "El handshake debeía ir acá");
	socket_fs = iniciar_socket_cliente(config->ip_fs, config->puerto_fs, &controlador);
	handshakearFS(socket_fs);

	liberar_memoria();

	return EXIT_SUCCESS;
}

void inicializar_variables()
{
	config = malloc(sizeof(t_configuracion)); //creo que falta la ip del kernel
	config->algoritmo = strdup("");
	config->ip_fs = strdup("");
	config->ip_memoria = strdup("");
	config->ip_kernel = strdup("");
	/*config->sem_ids = list_create();
	config->sem_init = list_create();
	config->shared_vars = list_create();*/
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
	list_destroy(config->shared_vars);*/
}
void handshakearFS(int socket_fs){
	int controlador = 0;
	char *mensaje = malloc(7);
	strcpy(mensaje,"KERNEL");
	enviar(socket_fs, mensaje,&controlador);
	free(mensaje);
}
//void liberar_lista_char()
