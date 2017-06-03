/*
 * planificador.c

 *
 *  Created on: 7/5/2017
 *      Author: utnso
 */
#include <stdbool.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "estructuras.h"

extern t_list *list_cpus;
extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern t_configuracion *config;

void agregar_nueva_prog(int socket, char *mensaje);
void finalizar_proceso(int pid);
void desbloquear_proceso(int pid);
void bloquear_proceso(int pid);
void programas_nuevos_A_listos();
void programas_listos_A_ejecutar();
void iniciar_planificador();


void iniciar_planificador()
{

}

void programas_listos_A_ejecutar()
{
	int listos, cpus_disponibles;

	int _cpuLibre(t_cpu *una_cpu)
	{
		return (!(una_cpu->ejecutando));
	}

	while(1)
	{
		listos = queue_size(cola_listos);
		cpus_disponibles = list_count_satisfying(list_cpus, (void*)_cpuLibre);

		if((listos) && (cpus_disponibles))
		{
			t_PCB *proceso_listo = queue_pop(cola_listos);
			t_cpu *cpu_disponible = list_remove_by_condition(list_cpus, (void*)_cpuLibre);
			list_add(list_ejecutando, proceso_listo);
			cpu_disponible->ejecutando = true;
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
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	t_PCB *proc = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	list_add(list_bloqueados, proc);
}

void desbloquear_proceso(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	t_PCB *proc = list_remove_by_condition(list_bloqueados, (void*)_buscar_proceso);
	queue_push(cola_listos, proc);
}

void finalizar_proceso(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	t_PCB *proc = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	list_add(list_finalizados, proc);
}
