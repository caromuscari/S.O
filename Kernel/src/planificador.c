/*
 * planificador.c
 *
 *  Created on: 7/5/2017
 *      Author: utnso
 */
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "estructuras.h"

extern t_list *list_cpus;
extern t_list *list_consolas;
extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern t_queue *cola_bloqueados;
extern t_configuracion *config;

void agregar_nueva_prog(int socket, char *mensaje);

void iniciar_planificador()
{

}

void programas_listos_A_ejecutar()
{
	int listos, cpus_disponibles;

	while(1)
	{
		listos = queue_size(cola_listos);
		cpus_disponibles = list_size(list_cpus);//deberian ser solo las que esten disponibles


		if((listos) && (cpus_disponibles))
		{
			t_PCB *proceso_listo = queue_pop(cola_listos);
			//list_add(t_list *, void *element);
			//queue_push(cola_listos, nuevo_proceso);
		}
	}
}

void programas_nuevos_A_listos()
{
	int procesando, listos, nuevos, multiprogramacion_dis;

	while(1)
	{
		nuevos = queue_size(cola_nuevos);
		procesando = list_size(list_ejecutando);
		listos = queue_size(cola_listos);
		multiprogramacion_dis = config->grado_multiprog - (procesando + listos);

		if((multiprogramacion_dis)&&(nuevos))
		{
			t_PCB *nuevo_proceso = queue_pop(cola_nuevos);
			queue_push(cola_listos, nuevo_proceso);
		}
	}
}

void agregar_nueva_prog(int socket, char *mensaje)
{

}

void bloquear_proceso(int pid)
{

}

void desbloquear_proceso(int pid)
{

}
