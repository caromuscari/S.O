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
#include "mensaje.h"
#include "metadata.h"
#include "socket.h"

extern t_list *list_cpus;
extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern t_configuracion *config;
extern int tam_pagina;
int controlador;

void finalizar_proceso(int pid, int codigo_finalizacion);
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
			t_program *program = queue_pop(cola_listos);
			t_cpu *cpu_disponible = list_remove_by_condition(list_cpus, (void*)_cpuLibre);

			//avisar que tiene que ejecutar
			int tam;//usar
			char *pcb_serializado = serializarPCB_KerCPU(*program->pcb,config->algoritmo,config->quantum,config->quantum_sleep);
			char *mensaje_env = armar_mensaje("K07", pcb_serializado);
			enviar(cpu_disponible->socket_cpu, mensaje_env, &controlador);

			if(controlador)
			{
				free(cpu_disponible);
			}
			else
			{
				list_add(list_ejecutando, program);
				*cpu_disponible->ejecutando = true;
				cpu_disponible->program = program;
				list_add(list_cpus, cpu_disponible);
			}
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
			queue_push(cola_listos, queue_pop(cola_nuevos));
		}
	}
}

void agregar_nueva_prog(int id_consola, int pid, char *mensaje)
{
	char *codigo = get_mensaje(mensaje);

	t_program *programa = malloc(sizeof(t_program));
	*programa->PID = pid;
	*programa->CID = id_consola;
	*programa->allocs = 0;
	*programa->frees = 0;
	*programa->syscall = 0;
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

	queue_push(cola_nuevos, programa);
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

void finalizar_proceso(int pid, int codigo_finalizacion)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	t_program *programa = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	programa->pcb->exit_code = codigo_finalizacion;
	list_add(list_finalizados, programa);
	//falta invocar funciones para limpiar asignacion de memoria dinamica y el TAP

}

int calcular_pag(char *mensaje)
{
	int tamanio = atoi(string_substring(mensaje, 3, 10));
	int paginas = (int)(tamanio/tam_pagina);

	if (tamanio % tam_pagina > 0)
	{
		paginas ++;
	}

	return paginas;
}

void forzar_finalizacion(int pid, int cid, int codigo_finalizacion)
{
	t_list *encontrados = list_create();
	int i, contador = 0;

	bool _buscar_program(t_program *pr)
	{
		if(pid)
		{
			return !pr->PID == pid;
		}
		else
		{
			return !pr->CID == cid;
		}
	}

	void _procesar_program(t_program *pr)
	{
		pr->pcb->exit_code = codigo_finalizacion;
		list_add(list_finalizados,pr);
	}

	contador = list_count_satisfying(list_ejecutando, (void*)_buscar_program);
	while(contador)
	{
		list_add(encontrados, list_remove_by_condition(list_ejecutando, (void*)_buscar_program));
		contador --;
	}

	contador = list_count_satisfying(list_bloqueados, (void*)_buscar_program);
	while(contador)
	{
		list_add(encontrados, list_remove_by_condition(list_bloqueados, (void*)_buscar_program));
		contador --;
	}

	controlador = queue_size(cola_nuevos);

	for(i=0;i<controlador;i++)
	{
		t_program *prog = queue_pop(cola_nuevos);

		if(_buscar_program(prog))
		{
			list_add(encontrados,prog);
		}
		else
		{
			queue_push(cola_nuevos,prog);
		}
	}

	controlador = queue_size(cola_listos);

	for(i=0;i<controlador;i++)
	{
		t_program *prog = queue_pop(cola_listos);

		if(_buscar_program(prog))
		{
			list_add(encontrados,prog);
		}
		else
		{
			queue_push(cola_listos,prog);
		}
	}

	list_iterate(encontrados, (void*)_procesar_program);
	list_destroy(encontrados);
}
