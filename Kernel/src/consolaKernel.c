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

void generar_listados(int lista);
void leer_consola();
void imprimir_menu();
void mostrar_cola(t_queue *, char *);
void mostrar_listas(t_list *, char *);
void obtener_informacion(int pid);
void imprimir_tabla_archivos();

void leer_consola()
{
	while (1)
	{
		imprimir_menu();

		char *input = strdup("");
		char *input2 = strdup("");

		scanf("%s", input);

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

				scanf("%s", input2);

				if((atoi(input2) > 0) && (atoi(input2) < 7))
				{
					int number = atoi(input2);
					generar_listados(number);
				}
				else
					printf("No se ingreso un numero valido\n");
				break;
			case 2 :
				printf("Indique el PID del proceso a consultar: ");
				scanf("%s", input2);
				int number = atoi(input2);
				obtener_informacion(number);
				break;
			case 3 :
				imprimir_tabla_archivos();
				break;
			case 4 :
				printf("Indique el nuevo grado de multiprogramacion: ");
				scanf("%s", input2);
				int grado = atoi(input2);
				config->grado_multiprog = grado;
				break;
			case 5 : ;
				printf("Indique el PID del proceso a finalizar: ");
				scanf("%s", input2);
				int numberKill = atoi(input2);
				forzar_finalizacion(numberKill, 0, 0);
				//falta algun tipo de chequeo por si el numero que puso no existe!!!
				break;
			case 6 : ;
				flag_planificador = 0;
				printf("La planificacion se ha detenido\n");
				break;
			case 7 : ;
				flag_planificador = 1;
				printf("La planificacion se ha reanudado\n");
				break;
			default :
				printf("No se reconocio la opcion ingresada\n");
				break;
		}

	free(input);
	free(input2);
	printf("\n\n");
	}
}

void generar_listados(int lista)
{
	if((lista == 1)	|| (lista == 2))
	{
		pthread_mutex_lock(&mutex_cola_nuevos);
		t_queue *nuevos = cola_nuevos;
		mostrar_cola(nuevos, "Los siguientes son los procesos en la cola de Nuevos:\n");
		pthread_mutex_unlock(&mutex_cola_nuevos);
	}

	if((lista == 1)	|| (lista == 3))
	{
		pthread_mutex_lock(&mutex_cola_listos);
		t_queue *listos = cola_listos;
		mostrar_cola(listos, "Los siguientes son los procesos en la cola de Listos:\n");
		pthread_mutex_unlock(&mutex_cola_listos);
	}

	if((lista == 1)	|| (lista == 4))
	{
		pthread_mutex_lock(&mutex_lista_ejecutando);
		t_list *ejecutando = list_ejecutando;
		mostrar_listas(ejecutando, "Los siguientes son los procesos en ejecucion:\n");
		pthread_mutex_unlock(&mutex_lista_ejecutando);
	}

	if((lista == 1)	|| (lista == 5))
	{
		pthread_mutex_lock(&mutex_lista_bloqueados);
		t_list *bloqueados = list_bloqueados;
		mostrar_listas(bloqueados, "Los procesos que se encuentran bloqueados son:\n");
		pthread_mutex_unlock(&mutex_lista_bloqueados);
	}

	if((lista == 1)	|| (lista == 6))
	{
		pthread_mutex_lock(&mutex_lista_finalizados);
		t_list *finalizados = list_finalizados;
		mostrar_listas(finalizados, "Los siguientes son los procesos que ya han finalizado:\n");
		pthread_mutex_unlock(&mutex_lista_finalizados);
	}
}

void mostrar_cola(t_queue *cola, char *procesos)
{
	printf("###########################################\n");
	printf("%s", procesos);
	while (!queue_is_empty(cola))
	{
		t_program *pr = malloc(sizeof(t_program));
		pr = queue_pop(cola);
		printf("%d \n", pr->PID);
		free(pr);
	}
}

void mostrar_listas(t_list *lista, char *procesos)
{
	printf("###########################################\n");
	printf("%s", procesos);

	while(!list_is_empty(lista))
	{
		t_program *pr = malloc(sizeof(t_program));
		pr = list_remove(lista, 1);
		printf("%d \n", pr->PID);
		free(pr);
	}
}

void obtener_informacion(int pid)
{
	char *lista;
	int encontrado = 0;
	t_program *found;
	t_list *lst_nuevos = list_create();
	t_list *lst_listos = list_create();

	void _buscar_program(t_program *pr)
	{
		if(!(pr->PID == pid))
		{
			found = pr;
			encontrado = 1;
		}
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	list_iterate(list_ejecutando, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	if(encontrado)	lista =	"Ejecutando";
	encontrado = 0;

	pthread_mutex_lock(&mutex_lista_bloqueados);
	list_iterate(list_bloqueados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	if(encontrado)	lista =	"Bloqueado";
	encontrado = 0;

	pthread_mutex_lock(&mutex_lista_finalizados);
	list_iterate(list_finalizados, (void*)_buscar_program);
	pthread_mutex_unlock(&mutex_lista_finalizados);

	if(encontrado)	lista =	"Finalizado";
	encontrado = 0;

	pthread_mutex_lock(&mutex_cola_nuevos);
	t_queue *nuevos = cola_nuevos;
	pthread_mutex_unlock(&mutex_cola_nuevos);

	while(queue_size(nuevos))
	{
		list_add(lst_nuevos,queue_pop(nuevos));
	}

	list_iterate(lst_nuevos, (void*)_buscar_program);
	if(encontrado)	lista =	"Cola de nuevos";
	encontrado = 0;

	pthread_mutex_lock(&mutex_cola_listos);
	t_queue *listos = cola_listos;
	pthread_mutex_unlock(&mutex_cola_listos);

	while(queue_size(listos))
	{
		list_add(lst_listos,queue_pop(listos));
	}

	list_iterate(lst_listos, (void*)_buscar_program);
	if(encontrado)	lista =	"Cola de listos";
	encontrado = 0;

	if(found == NULL)
	{
		printf("El proceso buscado no existe\n");
	}
	else
	{
		printf("Id Proceso: %i\n", found->PID);
		printf("Id Consola: %i\n", found->CID);
		printf("Status de proceso: %s\n", lista);
		printf("Cantidad de allocations: %i\n", found->allocs);
		printf("Cantidad de frees: %i\n", found->frees);
		printf("Cantidad de Syscalls: %i\n", found->syscall);
	}
}

void imprimir_tabla_archivos()
{
	void _imprimir(t_TAG *tg)
	{
		printf("Archivo: %s\n", tg->path);
		printf("Open: %i\n\n", tg->open);
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
	printf("	7 - Reanudar planificacion\n\n");
}
