/*
 * planificador.c

 *
 *  Created on: 7/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include "estructuras.h"

extern t_list *list_cpus;
extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern t_configuracion *config;
extern int tam_pagina;

void finalizar_proceso(int pid);
void desbloquear_proceso(int pid);
void bloquear_proceso(int pid);
void programas_nuevos_A_listos();
void programas_listos_A_ejecutar();
void iniciar_planificador();
int calcular_pag(char *mensaje);

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
			*cpu_disponible->ejecutando = true;
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

void agregar_nueva_prog(int id_consola, int pid, char *mensaje)
{
	char *codigo = get_mensaje(mensaje);

	t_program *programa = malloc(sizeof(t_program));
	*programa->PID = pid;
	programa->memoria_dinamica = list_create();
	programa->TAP = list_create();
	programa->pcb = malloc(sizeof(t_PCB));
	programa->pcb->PC = 0;
	programa->pcb->PID = pid;
	programa->pcb->SP = 0;//ver que es esto!!!
	programa->pcb->cant_pag = calcular_pag(mensaje);
	programa->pcb->in_cod = armarIndiceCodigo(codigo);
	programa->pcb->in_et = armarIndiceEtiquetas(codigo);
	programa->pcb->in_stack = armarIndiceStack(codigo);

	//falta anexar a cola de nuevos!!
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

int calcular_pag(char *mensaje)
{
	int tamanio = atoi(string_substring(mensaje, 3, 10));
	int paginas = (int)(tamanio/tam_pagina);

	if (tamanio % tam_pagina > 0)
	{
		paginas =+ 1;
	}

	return paginas;
}

void forzar_finalizacion(int pid)
{
	//aca deberia buscar el proceso con el pid y matarlo
}

void forzar_finalizacion_consola(int consola_id)
{

}
