#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include "estructuras.h"
#include "configuracion.h"
#include "socket.h"

char *ruta_config;
t_configuracion *config;
char *sem_id;
char *sem_in;
char *shared;
int socket_memoria;
int socket_fs;

void inicializar_variables();
void liberar_memoria();
void mostrar_configuracion();

int main(int argc, char*argv[])
{

	ruta_config = strdup(/*"/home/utnso/configuracion"*/argv[1]);
	char *ip_kernel = "127.0.0.1";

	inicializar_variables();
	leer_configuracion();
	mostrar_configuracion();

	socket_memoria = iniciar_socket_cliente(config->ip_memoria, config->puerto_memoria);
	//enviar(socket_memoria, "El handshake debeía ir acá");
	socket_fs = iniciar_socket_cliente(config->ip_fs, config->puerto_fs);
	enviar_cl(socket_fs, "SOY KERNEL, ME CONECTE LOKA");

	liberar_memoria();

	return EXIT_SUCCESS;
}

void inicializar_variables()
{
	config = malloc(sizeof(t_configuracion)); //creo que falta la ip del kernel
	config->algoritmo = strdup("");
	config->ip_fs = strdup("");
	config->ip_memoria = strdup("");
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
	free(config);
	/*list_destroy(config->sem_ids);
	list_destroy(config->sem_init);
	list_destroy(config->shared_vars);*/
}

//void liberar_lista_char()
