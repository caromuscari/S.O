//otro hilo de Kernel

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "estructuras.h"
#include "configuracion.h"
#include "socket.h"
#include "log.h"

extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern pthread_mutex_t mutex_lista_ejecutando;
extern pthread_mutex_t mutex_lista_finalizados;
extern pthread_mutex_t mutex_lista_bloqueados;
extern pthread_mutex_t mutex_cola_nuevos;
extern pthread_mutex_t mutex_cola_listos;
extern t_configuracion *config;

void generar_listados();
void leer_consola();
void mostrar_cola(t_queue *, char *);
void mostrar_listas(t_list *, char *);

void leer_consola()
{
	while (1)
	{
		char *leido = strdup("");
		scanf("%s",leido);

		if(strcmp(leido, "listado_procesos")) //muestra los procesos agrupados según el estado
		{
			generar_listados();
		}else if(strcmp(leido, "obtener_info_proceso"))
		{
			char *leido2 = strdup("");
			scanf("%s",leido2);
			//se podría tener una lista general así es más facil buscar el proceso
			free(leido2);
		}else if (strcmp(leido, "obtener_tabla_global_archivos"))
		{
			//todavía no está la estructura
		}else if (strcmp(leido, "modificar_grado_multiprogramacion"))
		{
			char *leido2 = strdup("");
			scanf("%s", leido2);
			int grado = atoi(leido2);
			config->grado_multiprog = grado;
			free(leido2);
		}else if (strcmp(leido, "finalizar_proceso"))
		{
			char *leido2 = strdup("");
			scanf("%s", leido2);
			//pensar lo de la lista general, si no, usar las actuales
			free(leido2);
		}else if (strcmp(leido, "detener_planificación"))
		{
			//crear un semaforo para compartir con el planificador, inicializado en 1
			//este comando lo pondria en block
		}
	free(leido);
	}
}

void generar_listados() // faltan los mutex
{
	pthread_mutex_lock(&mutex_cola_nuevos);
	t_queue *nuevos = cola_nuevos;
	pthread_mutex_unlock(&mutex_cola_nuevos);

	pthread_mutex_lock(&mutex_cola_listos);
	t_queue *listos = cola_listos;
	pthread_mutex_unlock(&mutex_cola_listos);

	pthread_mutex_lock(&mutex_lista_bloqueados);
	t_list *bloqueados = list_bloqueados;
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_list *ejecutando = list_ejecutando;
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	pthread_mutex_lock(&mutex_lista_finalizados);
	t_list *finalizados = list_finalizados;
	pthread_mutex_unlock(&mutex_lista_finalizados);

	mostrar_cola(nuevos, "Los siguientes son los procesos en la cola de Nuevos: \n");
	mostrar_cola(listos, "Los siguientes son los procesos en la cola de Listos: \n");
	mostrar_listas(bloqueados, "Los procesos que se encuentran bloqueados son: \n");
	mostrar_listas(ejecutando, "Los siguientes son los procesos en ejecución: \n");
	mostrar_listas(finalizados, "Los siguientes son los procesos que ya han finalizado: \n");
}

void mostrar_cola(t_queue *cola, char *procesos)
{
	printf("###########################################");
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
	printf("###########################################");
	printf("%s", procesos);
	int size = list_size(lista);
	int c = 0;

	while(c < size)
	{
		t_program *pr = malloc(sizeof(t_program));
		pr = list_get(lista, c);
		printf("%d \n", pr->PID);
		free(pr);
		c++;
	}
}

