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
extern pthread_mutex_t mutex_planificador;
extern pthread_mutex_t mutex_lista_cpus;
extern pthread_mutex_t mutex_lista_ejecutando;
extern pthread_mutex_t mutex_lista_finalizados;
extern pthread_mutex_t mutex_lista_bloqueados;
extern pthread_mutex_t mutex_cola_nuevos;
extern pthread_mutex_t mutex_cola_listos;
extern t_configuracion *config;
extern int tam_pagina;
int controlador;

void finalizar_proceso(int pid, int codigo_finalizacion);
void desbloquear_proceso(int pid);
void bloquear_proceso(int pid);
void programas_nuevos_A_listos();
void programas_listos_A_ejecutar();
int calcular_pag(char *mensaje);

void programas_listos_A_ejecutar()
{
	int listos, cpus_disponibles;
	int *tam_prog;

	int _cpuLibre(t_cpu *una_cpu)
	{
		return !(una_cpu->ejecutando);
	}

	while(1)
	{
		pthread_mutex_lock(&mutex_planificador);

		listos = queue_size(cola_listos);
		cpus_disponibles = list_count_satisfying(list_cpus, (void*)_cpuLibre);

		if((listos) && (cpus_disponibles))
		{
			pthread_mutex_lock(&mutex_cola_listos);
			t_program *program = queue_pop(cola_listos);
			pthread_mutex_unlock(&mutex_cola_listos);

			pthread_mutex_lock(&mutex_lista_cpus);
			t_cpu *cpu_disponible = list_remove_by_condition(list_cpus, (void*)_cpuLibre);
			pthread_mutex_unlock(&mutex_lista_cpus);

			char *pcb_serializado = serializarPCB_KerCPU(program->pcb,config->algoritmo,config->quantum,config->quantum_sleep,&tam_prog);
			char *mensaje_env = armar_mensaje("K07", pcb_serializado);
			enviar(cpu_disponible->socket_cpu, mensaje_env, &controlador);

			if(controlador)
			{
				free(cpu_disponible);
			}
			else
			{
				pthread_mutex_lock(&mutex_lista_ejecutando);
				list_add(list_ejecutando, program);
				pthread_mutex_unlock(&mutex_lista_ejecutando);

				*cpu_disponible->ejecutando = true;
				cpu_disponible->program = program;

				pthread_mutex_lock(&mutex_lista_cpus);
				list_add(list_cpus, cpu_disponible);
				pthread_mutex_unlock(&mutex_lista_cpus);
			}
		}

		pthread_mutex_unlock(&mutex_planificador);
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
			pthread_mutex_lock(&mutex_cola_nuevos);
			pthread_mutex_lock(&mutex_cola_listos);
			queue_push(cola_listos, queue_pop(cola_nuevos));
			pthread_mutex_unlock(&mutex_cola_nuevos);
			pthread_mutex_unlock(&mutex_cola_listos);
		}
	}
}

void agregar_nueva_prog(int id_consola, int pid, char *mensaje)
{
	char *codigo = get_mensaje(mensaje);

	t_program *programa = malloc(sizeof(t_program));
	programa->PID = pid;
	programa->CID = id_consola;
	programa->allocs = 0;
	programa->frees = 0;
	programa->syscall = 0;
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

	pthread_mutex_lock(&mutex_cola_nuevos);
	queue_push(cola_nuevos, programa);
	pthread_mutex_unlock(&mutex_cola_nuevos);
}

void bloquear_proceso(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return !(pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_PCB *proc = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	pthread_mutex_lock(&mutex_lista_bloqueados);
	list_add(list_bloqueados, proc);
	pthread_mutex_unlock(&mutex_lista_bloqueados);
}

void desbloquear_proceso(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return !(pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_bloqueados);
	t_PCB *proc = list_remove_by_condition(list_bloqueados, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	pthread_mutex_lock(&mutex_cola_listos);
	queue_push(cola_listos, proc);
	pthread_mutex_unlock(&mutex_cola_listos);
}

void finalizar_proceso(int pid, int codigo_finalizacion)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return !(pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_program *programa = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	programa->pcb->exit_code = codigo_finalizacion;

	pthread_mutex_lock(&mutex_lista_finalizados);
	list_add(list_finalizados, programa);
	pthread_mutex_unlock(&mutex_lista_finalizados);
	//falta invocar funciones para limpiar asignacion de memoria dinamica y el TAP
	//falta funcion para liberar toma de semaforos y darselos a otros
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
			return !pr->PID == pid;
		else
			return !pr->CID == cid;
	}

	void _procesar_program(t_program *pr)
	{
		pr->pcb->exit_code = codigo_finalizacion;
		pthread_mutex_lock(&mutex_lista_finalizados);
		list_add(list_finalizados,pr);
		pthread_mutex_unlock(&mutex_lista_finalizados);
	}

	contador = list_count_satisfying(list_ejecutando, (void*)_buscar_program);
	while(contador)
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

void finalizar_quantum(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return !(pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_program *programa = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	pthread_mutex_lock(&mutex_cola_listos);
	queue_push(cola_listos, programa);
	pthread_mutex_unlock(&mutex_cola_listos);
}
