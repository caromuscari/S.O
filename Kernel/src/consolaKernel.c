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
#include "configuracion.h"
#include "socket.h"
#include "log.h"

extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern pthread_mutex_t mutex_planificador;
extern pthread_mutex_t mutex_lista_ejecutando;
extern pthread_mutex_t mutex_lista_finalizados;
extern pthread_mutex_t mutex_lista_bloqueados;
extern pthread_mutex_t mutex_cola_nuevos;
extern pthread_mutex_t mutex_cola_listos;
extern t_configuracion *config;

void generar_listados(int lista);
void leer_consola();
void imprimir_menu();
void mostrar_cola(t_queue *, char *);
void mostrar_listas(t_list *, char *);

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
					generar_listados(atoi(input2));
				else
					printf("No se ingreso un numero valido\n");
				break;
			case 2 :
				printf("Indique el PID del proceso a consultar: ");
				scanf("%s", input2);
				printf("Deberia devolver informacion del proceso");
				break;
			case 3 :
				break;
			case 4 :
				printf("Indique el nuevo grado de multiprogramacion: ");
				scanf("%s", input2);
				int grado = atoi(input2);
				config->grado_multiprog = grado;
				break;
			case 5 :
				printf("Indique el PID del proceso a finalizar: ");
				scanf("%s", input2);
				//pensar lo de la lista general, si no, usar las actuales
				break;
			case 6 : ;
				pthread_mutex_lock(&mutex_planificador);
				break;
			case 7 : ;
				pthread_mutex_unlock(&mutex_planificador);
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

void obtener_informacion(int pid)
{
	t_list *encontrados = list_create();
	int i, contador = 0;
	int corte;

	bool _buscar_program(t_program *pr)
	{
			return !pr->PID == pid;
	}

	corte = list_any_satisfying(list_ejecutando, (void*)_buscar_program);

	if(contador)
	{
		pthread_mutex_lock(&mutex_lista_ejecutando);
		list_add(encontrados, list_remove_by_condition(list_ejecutando, (void*)_buscar_program));
		pthread_mutex_unlock(&mutex_lista_ejecutando);
		contador --;
	}

	contador = list_count_satisfying(list_bloqueados, (void*)_buscar_program);
	while(contador)
	{
		pthread_mutex_lock(&mutex_lista_bloqueados);
		list_add(encontrados, list_remove_by_condition(list_bloqueados, (void*)_buscar_program));
		pthread_mutex_unlock(&mutex_lista_bloqueados);
		//deberia meter una funcion aca que habilite el semaforo que este estaba tomando
		contador --;
	}

	controlador = queue_size(cola_nuevos);

	for(i=0;i<controlador;i++)
	{
		pthread_mutex_lock(&mutex_cola_nuevos);
		t_program *prog = queue_pop(cola_nuevos);
		pthread_mutex_unlock(&mutex_cola_nuevos);

		if(_buscar_program(prog))
		{
			list_add(encontrados,prog);
		}
		else
		{
			pthread_mutex_lock(&mutex_cola_nuevos);
			queue_push(cola_nuevos,prog);
			pthread_mutex_unlock(&mutex_cola_nuevos);
		}
	}

	controlador = queue_size(cola_listos);

	for(i=0;i<controlador;i++)
	{
		pthread_mutex_lock(&mutex_cola_listos);
		t_program *prog = queue_pop(cola_listos);
		pthread_mutex_unlock(&mutex_cola_listos);

		if(_buscar_program(prog))
		{
			list_add(encontrados,prog);
		}
		else
		{
			pthread_mutex_lock(&mutex_cola_listos);
			queue_push(cola_listos,prog);
			pthread_mutex_unlock(&mutex_cola_listos);
		}
	}

	list_iterate(encontrados, (void*)_procesar_program);
	list_destroy(encontrados);
}


