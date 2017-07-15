//otro hilo de Kernel

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include "estructuras.h"
#include "planificador.h"
#include "configuracion.h"
#include "socket.h"
#include "log.h"

extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_list *global_fd;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern pthread_mutex_t mutex_lista_fs;
extern pthread_mutex_t mutex_lista_ejecutando;
extern pthread_mutex_t mutex_lista_finalizados;
extern pthread_mutex_t mutex_lista_bloqueados;
extern pthread_mutex_t mutex_cola_nuevos;
extern pthread_mutex_t mutex_cola_listos;
extern t_configuracion *config;
extern int flag_planificador;
//A partir de aca son cosas de mas para monitoreo
extern t_list *list_cpus;
extern t_list *list_consolas;


void generar_listados(int lista);
void leer_consola();
void imprimir_menu();
void mostrar_cola(t_queue *, char *);
void mostrar_listas(t_list *, char *);
void obtener_informacion(int pid);
void imprimir_tabla_archivos();
char *devolver_descripcion_error(int codigo);
int existe_pid(int pid);

void imprimir_info();

void leer_consola()
{
	while (1)
	{
		imprimir_menu();

		char *input, *input2;

		scanf("%ms", &input);

		switch (atoi(input))
		{
			case 1 :
				printf("Seleccione la lista o cola que desea visualizar:\n");
				printf("	1 - Todos\n");
				printf("	2 - Nuevos\n");
				printf("	3 - Listos\n");
				printf("	4 - Ejecutando\n");
				printf("	5 - Bloqueados\n");
				printf("	6 - Finalizados\n");

				scanf("%ms", &input2);

				if((atoi(input2) > 0) && (atoi(input2) < 7))
				{
					int number = atoi(input2);
					generar_listados(number);
				}
				else
				{
					printf("No se ingreso un numero valido\n");
				}

				imprimir_info();

				free(input);
				free(input2);
				break;
			case 2 :
				printf("Indique el PID del proceso a consultar: ");
				scanf("%ms", &input2);
				int number = atoi(input2);
				obtener_informacion(number);
				free(input);
				free(input2);
				break;
			case 3 :
				imprimir_tabla_archivos();
				free(input);
				break;
			case 4 :
				printf("El grado actual de multiprogramacion es: %i\n", config->grado_multiprog);
				printf("Indique el nuevo grado de multiprogramacion: ");
				scanf("%ms", &input2);
				int grado = atoi(input2);
				config->grado_multiprog = grado;
				free(input);
				free(input2);
				break;
			case 5 : ;
				printf("Indique el PID del proceso a finalizar: ");
				scanf("%ms", &input2);
				int numberKill = atoi(input2);

				if(existe_pid(numberKill))
				{
					forzar_finalizacion(numberKill, 0, 10, 1);
					printf("El proceso ha sido eliminado\n");
				}
				else
					printf("El proceso buscado no existe\n");

				free(input);
				free(input2);
				break;
			case 6 : ;
				flag_planificador = 0;
				printf("La planificacion se ha detenido\n");
				free(input);
				break;
			case 7 : ;
				flag_planificador = 1;
				printf("La planificacion se ha reanudado\n");
				free(input);
				break;
			case 8 : ;
				system("clear");
				break;
			default :
				printf("No se reconocio la opcion ingresada\n");
				free(input);
				break;
		}

	printf("\n\n");
	}
}

void generar_listados(int lista)
{
	if((lista == 1)	|| (lista == 2))
	{
		pthread_mutex_lock(&mutex_cola_nuevos);
		mostrar_cola(cola_nuevos, "Los siguientes son los procesos en la cola de Nuevos:\n");
		pthread_mutex_unlock(&mutex_cola_nuevos);
	}

	if((lista == 1)	|| (lista == 3))
	{
		pthread_mutex_lock(&mutex_cola_listos);
		mostrar_cola(cola_listos, "Los siguientes son los procesos en la cola de Listos:\n");
		pthread_mutex_unlock(&mutex_cola_listos);
	}

	if((lista == 1)	|| (lista == 4))
	{
		pthread_mutex_lock(&mutex_lista_ejecutando);
		mostrar_listas(list_ejecutando, "Los siguientes son los procesos en ejecucion:\n");
		pthread_mutex_unlock(&mutex_lista_ejecutando);
	}

	if((lista == 1)	|| (lista == 5))
	{
		pthread_mutex_lock(&mutex_lista_bloqueados);
		mostrar_listas(list_bloqueados, "Los procesos que se encuentran bloqueados son:\n");
		pthread_mutex_unlock(&mutex_lista_bloqueados);
	}

	if((lista == 1)	|| (lista == 6))
	{
		pthread_mutex_lock(&mutex_lista_finalizados);
		mostrar_listas(list_finalizados, "Los siguientes son los procesos que ya han finalizado:\n");
		pthread_mutex_unlock(&mutex_lista_finalizados);
	}
}

void mostrar_cola(t_queue *cola, char *procesos)
{
	printf("###########################################\n");
	printf("%s", procesos);

	int i;
	int size = queue_size(cola);

	for(i=0;i<size;i++)
	{
		t_program *pr = queue_pop(cola);
		printf("%d \n", pr->PID);
		queue_push(cola,pr);
	}
}

void mostrar_listas(t_list *lista, char *procesos)
{
	printf("###########################################\n");
	printf("%s", procesos);

	void _mostrar(t_program *pr)
	{
		printf("%d \n", pr->PID);
	}

	list_iterate(lista, (void*)_mostrar);
}

void obtener_informacion(int pid)
{
	char *lista;
	int encontrado = 0;
	int i, size;
	t_program *found;
	found->PID = 0;

	void _buscar_program(t_program *pr)
	{
		if(pr->PID == pid)
		{
			found = pr;
			encontrado = 1;
		}
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	list_iterate(list_ejecutando, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	if(encontrado)	lista =	strdup("Ejecutando");
	encontrado = 0;

	pthread_mutex_lock(&mutex_lista_bloqueados);
	list_iterate(list_bloqueados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	if(encontrado)	lista =	strdup("Bloqueado");
	encontrado = 0;

	pthread_mutex_lock(&mutex_lista_finalizados);
	list_iterate(list_finalizados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_finalizados);

	if(encontrado)	lista =	strdup("Finalizado");
	encontrado = 0;

	pthread_mutex_lock(&mutex_cola_nuevos);
	size = queue_size(cola_nuevos);
	for(i=0;i<size;i++)
	{
		t_program *pr = queue_pop(cola_nuevos);
		_buscar_program(pr);
		queue_push(cola_nuevos,pr);
	}
	size = 0; i = 0;
	pthread_mutex_unlock(&mutex_cola_nuevos);

	if(encontrado)	lista =	strdup("Cola de nuevos");
	encontrado = 0;

	pthread_mutex_lock(&mutex_cola_listos);
	size = queue_size(cola_listos);
	for(i=0;i<size;i++)
	{
		t_program *pr = queue_pop(cola_listos);
		_buscar_program(pr);
		queue_push(cola_listos,pr);
	}
	size = 0; i = 0;
	pthread_mutex_unlock(&mutex_cola_listos);

	if(encontrado)	lista =	strdup("Cola de listos");
	encontrado = 0;

	if(found->PID == 0)
	{
		printf("El proceso buscado no existe\n");
	}
	else
	{
		char *desc = devolver_descripcion_error(found->pcb->exit_code);
		printf("Id Proceso: %i\n", found->PID);
		printf("Id Consola: %i\n", found->CID);
		printf("Status de proceso: %s\n", lista);
		printf("Cantidad de allocations: %i\n", found->allocs);
		printf("Cantidad de frees: %i\n", found->frees);
		printf("Cantidad de Syscalls: %i\n", found->syscall);
		printf("Cantidad de paginas: %i\n", found->pcb->cant_pag);
		printf("Exit code: %i  Descripcion: %s\n", found->pcb->exit_code, desc);
		free(lista);
		free(desc);
	}
}

void imprimir_tabla_archivos()
{
	printf("Tabla de archivos:\n");
	void _imprimir(t_TAG *tg)
	{
		printf("Archivo: %s\n", tg->path);
		printf("Open: %i\n\n", tg->open_);
	}

	pthread_mutex_lock(&mutex_lista_fs);
	list_iterate(global_fd, (void*)_imprimir);
	pthread_mutex_unlock(&mutex_lista_fs);
}

void imprimir_menu()
{
	printf("Seleccione el numero de la opcion a ejecutar\n");
	printf("	1 - Listar procesos\n");
	printf("	2 - Obtener informacion de proceso\n");
	printf("	3 - Obtener tabla global de archivos\n");
	printf("	4 - Modificar grado de multiprogramacion\n");
	printf("	5 - Finalizar proceso\n");
	printf("	6 - Detener planificacion\n");
	printf("	7 - Reanudar planificacion\n");
	printf("	8 - Limpiar pantalla\n\n");
}

int existe_pid(int pid)
{
	int size, i;
	int encontrado = 0;

	void _buscar_program(t_program *pr)
	{
		if(pr->PID == pid)
			encontrado = 1;
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	list_iterate(list_ejecutando, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	pthread_mutex_lock(&mutex_lista_bloqueados);
	list_iterate(list_bloqueados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	pthread_mutex_lock(&mutex_lista_finalizados);
	list_iterate(list_finalizados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_finalizados);

	pthread_mutex_lock(&mutex_cola_nuevos);
	size = queue_size(cola_nuevos);
	for(i=0;i<size;i++)
	{
		t_program *pr = queue_pop(cola_nuevos);
		_buscar_program(pr);
		queue_push(cola_nuevos,pr);
	}
	size = 0; i = 0;
	pthread_mutex_unlock(&mutex_cola_nuevos);

	pthread_mutex_lock(&mutex_cola_listos);
	size = queue_size(cola_listos);
	for(i=0;i<size;i++)
	{
		t_program *pr = queue_pop(cola_listos);
		_buscar_program(pr);
		queue_push(cola_listos,pr);
	}
	size = 0; i = 0;
	pthread_mutex_unlock(&mutex_cola_listos);

	return encontrado;
}

void imprimir_info()
{
	escribir_log("Lista de CPU's\n\n");
	void _imprimir_cpu(t_cpu *cpu)
	{
		escribir_log_con_numero("Numero de CPU: ",cpu->cpu_id);
		escribir_log_con_numero("Estado de ejecutando: ",cpu->ejecutando);
		printf("\n");
	}
	list_iterate(list_cpus, (void*)_imprimir_cpu);

	escribir_log("Lista de Consolas\n\n");
	void _imprimir_consola(t_consola *consola)
	{
		escribir_log_con_numero("Numero de Consola: ",consola->CID);
		printf("\n");
	}

	list_iterate(list_consolas, (void*)_imprimir_consola);
}

char *devolver_descripcion_error(int codigo)
{
	char *descripcion;

	switch (codigo)
	{
	case 0 :
		descripcion = strdup("El programa finalizo correctamente");
		break;
	case -1 :
		descripcion = strdup("No se pudieron reservar recursos para ejecutar el programa");
		break;
	case -2 :
		descripcion = strdup("El programa intento acceder a un archivo que no existe");
		break;
	case -3 :
		descripcion = strdup("El programa intento leer un archivo sin permisos");
		break;
	case -4 :
		descripcion = strdup("El programa intento escribir un archivo sin permisos");
		break;
	case -5 :
		descripcion = strdup("Excepcion de memoria");
		break;
	case -6 :
		descripcion = strdup("Finalizado a traves de desconexion de consola");
		break;
	case -7 :
		descripcion = strdup("Finalizado a traves del comando Finalizar Programa de la consola");
		break;
	case -8 :
		descripcion = strdup("Se intento reservar mas memoria que el tamanio de una pagina");
		break;
	case -9 :
		descripcion = strdup("No se pueden asignar mas paginas al proceso");
		break;
	case -11 :
		descripcion = strdup("Se intento acceder a un semaforo inexistente");
		break;
	case -12 :
		descripcion = strdup("Se intento acceder a una variable inexistente");
		break;
	case -20 :
		descripcion = strdup("Error sin definicion");
		break;
	case 1 :
		descripcion = strdup("El proceso aun no ha finalizado");
		break;
	default :
		descripcion = strdup("");
		break;
	}

	return descripcion;
}
