#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <pthread.h>
#include "estructuras.h"
#include "semaforos_vglobales.h"
#include "mensaje_consola.h"
#include "cpuManager.h"
#include "mensaje.h"
#include "memoria2.h"
#include "metadata.h"
#include "socket.h"
#include "log.h"

extern t_list *list_cpus;
extern t_list *list_ejecutando;
extern t_list *list_finalizados;
extern t_list *list_bloqueados;
extern t_queue *cola_nuevos;
extern t_queue *cola_listos;
extern pthread_mutex_t mutex_lista_cpus;
extern pthread_mutex_t mutex_lista_ejecutando;
extern pthread_mutex_t mutex_lista_finalizados;
extern pthread_mutex_t mutex_lista_bloqueados;
extern pthread_mutex_t mutex_cola_nuevos;
extern pthread_mutex_t mutex_cola_listos;
extern pthread_mutex_t mutex_actualizar_multip;
extern sem_t sem_grad_multi;
extern sem_t sem_nuevos;
extern sem_t sem_listos;
extern sem_t sem_cpus;
extern t_configuracion *config;
extern int tam_pagina;
extern int diferencia_multi;
int controlador;

void finalizar_proceso(int pid, int codigo_finalizacion);
void desbloquear_proceso(int pid);
void bloquear_proceso(int pid);
void programas_nuevos_A_listos();
void programas_listos_A_ejecutar();
void forzar_finalizacion(int pid, int cid, int codigo_finalizacion, int aviso);
void agregar_nueva_prog(int id_consola, int pid, char *mensaje, int socket_con);
void finalizar_quantum(int pid);
int calcular_pag(char *mensaje);

void programas_listos_A_ejecutar()
{
	int tam_prog;

	bool _cpuLibre(t_cpu *una_cpu)
	{
		return !(una_cpu->ejecutando);
	}

	while(1)
	{
		sem_wait(&sem_listos);
		sem_wait(&sem_cpus);
		tam_prog = 0;

		pthread_mutex_lock(&mutex_cola_listos);
		t_program *program = queue_pop(cola_listos);
		pthread_mutex_unlock(&mutex_cola_listos);

		pthread_mutex_lock(&mutex_lista_cpus);
		t_cpu *cpu_disponible = list_remove_by_condition(list_cpus, (void*)_cpuLibre);
		pthread_mutex_unlock(&mutex_lista_cpus);

		char *pcb_serializado = serializarPCB_KerCPU(program->pcb,config->algoritmo,config->quantum,config->quantum_sleep,&tam_prog);
		char *mensaje_env = armar_mensaje_pcb("K07", pcb_serializado, tam_prog);

		escribir_log(mensaje_env);

		enviar_pcb(cpu_disponible->socket_cpu, mensaje_env, &controlador, tam_prog+13);
		free(pcb_serializado);

		//Fallo el envio de la pcb a la cpu, se debe eliminar la cpu
		if(controlador>0)
		{
			int i;
			escribir_log_error_con_numero("Ha fallado el envio de una PCB con la CPU: ",cpu_disponible->cpu_id);
			free(cpu_disponible);

			pthread_mutex_lock(&mutex_cola_listos);
			int size = queue_size(cola_listos);
			queue_push(cola_listos,program);
			pthread_mutex_unlock(&mutex_cola_listos);

			for(i=0;i<size;i++)
			{
				pthread_mutex_lock(&mutex_cola_listos);
				queue_push(cola_listos,queue_pop(cola_listos));
				pthread_mutex_unlock(&mutex_cola_listos);
			}
			sem_post(&sem_listos);
		}
		else
		{
			pthread_mutex_lock(&mutex_lista_ejecutando);
			list_add(list_ejecutando, program);
			pthread_mutex_unlock(&mutex_lista_ejecutando);

			cpu_disponible->ejecutando = true;
			cpu_disponible->program = program;

			pthread_mutex_lock(&mutex_lista_cpus);
			list_add(list_cpus, cpu_disponible);
			pthread_mutex_unlock(&mutex_lista_cpus);
		}
		free(mensaje_env);
	}
}

void programas_nuevos_A_listos()
{
	while(1)
	{
		sem_wait(&sem_nuevos);
		sem_wait(&sem_grad_multi);

		escribir_log("Se ha movido un proceso de nuevos a listos");
		pthread_mutex_lock(&mutex_cola_nuevos);
		pthread_mutex_lock(&mutex_cola_listos);
		queue_push(cola_listos, queue_pop(cola_nuevos));
		pthread_mutex_unlock(&mutex_cola_nuevos);
		pthread_mutex_unlock(&mutex_cola_listos);

		sem_post(&sem_listos);
	}
}

void agregar_nueva_prog(int id_consola, int pid, char *mensaje, int socket_con)
{
	char *codigo = get_mensaje(mensaje);

	t_program *programa = malloc(sizeof(t_program));
	programa->PID = pid;
	programa->socket_consola = socket_con;
	programa->CID = id_consola;
	programa->allocs = 0;
	programa->frees = 0;
	programa->syscall = 0;
	programa->memoria_dinamica = list_create();
	programa->TAP = list_create();
	programa->semaforos = list_create();
	programa->posiciones = dictionary_create();
	programa->pcb = malloc(sizeof(t_PCB));
	programa->pcb->PC = 0;
	programa->pcb->PID = pid;
	programa->pcb->SP = 0;//ver que es esto!!!
	programa->pcb->exit_code = 1;
	programa->pcb->cant_pag = calcular_pag(mensaje);
	programa->pcb->in_cod = armarIndiceCodigo(codigo);
	programa->pcb->in_et = armarIndiceEtiquetas(codigo);
	programa->pcb->in_stack = armarIndiceStack(codigo);

	escribir_log("Se ha agregado un nuevo proceso a la cola de listos");

	pthread_mutex_lock(&mutex_cola_nuevos);
	queue_push(cola_nuevos, programa);
	pthread_mutex_unlock(&mutex_cola_nuevos);

	sem_post(&sem_nuevos);

	free(codigo);
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

	escribir_log_con_numero("Se ha bloqueado el proceso: ", proc->PID);

	pthread_mutex_lock(&mutex_lista_bloqueados);
	list_add(list_bloqueados, proc);
	pthread_mutex_unlock(&mutex_lista_bloqueados);
}

void desbloquear_proceso(int pid)
{
	int _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_bloqueados);
	t_PCB *proc = list_remove_by_condition(list_bloqueados, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_bloqueados);

	escribir_log_con_numero("Se ha desbloqueado el proceso: ", proc->PID);

	pthread_mutex_lock(&mutex_cola_listos);
	queue_push(cola_listos, proc);
	pthread_mutex_unlock(&mutex_cola_listos);
}

void finalizar_proceso(int pid, int codigo_finalizacion)
{
	bool _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_program *programa = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	programa->pcb->exit_code = codigo_finalizacion;

	escribir_log_con_numero("Se ha finalizado el proceso: ", programa->PID);
	avisar_consola_proceso_murio(programa);

	sem_signal(programa, "", programa->socket_consola, 1);
	list_destroy_and_destroy_elements(programa->memoria_dinamica, (void *)liberar_pagina);

	pthread_mutex_lock(&mutex_lista_finalizados);
	list_add(list_finalizados, programa);
	pthread_mutex_unlock(&mutex_lista_finalizados);
}

void forzar_finalizacion(int pid, int cid, int codigo_finalizacion, int aviso)
{
	t_list *procesos = list_create();
	t_list *procesos_ejecutando = list_create();
	int i, contador = 0;

	bool _buscar_program(t_program *pr)
	{
		if(pid)
			return pr->PID == pid;
		else
			return pr->CID == cid;
	}

	void _finalizar_proceso_ejecutando(t_program *pr)
	{
		pedir_pcb_error(pr,codigo_finalizacion);
	}

	void _finalizar_proceso(t_program *pr)
	{
		pr->pcb->exit_code = (-1)*codigo_finalizacion;
		if(aviso) avisar_consola_proceso_murio(pr);
		list_destroy_and_destroy_elements(pr->memoria_dinamica, (void *)liberar_pagina);

		pthread_mutex_lock(&mutex_lista_finalizados);
		list_add(list_finalizados,pr);
		pthread_mutex_unlock(&mutex_lista_finalizados);
	}

	contador = list_count_satisfying(list_ejecutando, (void*)_buscar_program);
	while(contador)
	{
		pthread_mutex_lock(&mutex_lista_ejecutando);
		t_program *proo = list_remove_by_condition(list_ejecutando, (void*)_buscar_program);
		list_add(procesos_ejecutando,proo);
		list_add(list_ejecutando,proo);
		pthread_mutex_unlock(&mutex_lista_ejecutando);
		contador --;
	}

	contador = list_count_satisfying(list_bloqueados, (void*)_buscar_program);
	while(contador)
	{
		pthread_mutex_lock(&mutex_lista_bloqueados);
		list_add(procesos, list_remove_by_condition(list_bloqueados, (void*)_buscar_program));
		pthread_mutex_unlock(&mutex_lista_bloqueados);
		//deberia meter una funcion aca que habilite el semaforo que este estaba tomando
		contador --;
	}

	controlador = queue_size(cola_nuevos);

	for(i=0;i<controlador;i++)
	{
		escribir_log("Planificador -- antes de copiar lista de nuevos");
		pthread_mutex_lock(&mutex_cola_nuevos);
		t_program *prog = queue_pop(cola_nuevos);
		pthread_mutex_unlock(&mutex_cola_nuevos);

		if(_buscar_program(prog))
		{
			escribir_log("Planificador -- antes de copiar a lista de encontrados");
			list_add(procesos,prog);
		}
		else
		{
			escribir_log("Planificador -- antes de devolver a lista de nuevos");
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
			list_add(procesos,prog);
		}
		else
		{
			pthread_mutex_lock(&mutex_cola_listos);
			queue_push(cola_listos,prog);
			pthread_mutex_unlock(&mutex_cola_listos);
		}
	}

	list_iterate(procesos_ejecutando, (void*)_finalizar_proceso_ejecutando);
	list_iterate(procesos, (void*)_finalizar_proceso);

	list_destroy(procesos);
	list_destroy(procesos_ejecutando);
}

int calcular_pag(char *mensaje)
{
	char *tam = string_substring(mensaje, 3, 10);
	int tamanio = atoi(tam);
	int paginas = (int)(tamanio/tam_pagina);

	if (tamanio % tam_pagina > 0)
	{
		paginas ++;
	}
	free(tam);
	return paginas;
}

void finalizar_quantum(int pid)
{
	bool _buscar_proceso(t_PCB *un_proceso)
	{
		return (pid == un_proceso->PID);
	}

	pthread_mutex_lock(&mutex_lista_ejecutando);
	t_program *programa = list_remove_by_condition(list_ejecutando, (void*)_buscar_proceso);
	pthread_mutex_unlock(&mutex_lista_ejecutando);

	pthread_mutex_lock(&mutex_cola_listos);
	queue_push(cola_listos, programa);
	pthread_mutex_unlock(&mutex_cola_listos);
}

void actualizar_grado_multiprogramacion()
{
	while(1)
	{
		pthread_mutex_lock(&mutex_actualizar_multip);

		while(diferencia_multi != 0)
		{
			if(diferencia_multi>0)
			{
				sem_post(&sem_grad_multi);
				diferencia_multi--;
			}
			else
			{
				sem_wait(&sem_grad_multi);
				diferencia_multi++;
			}
		}

		pthread_mutex_unlock(&mutex_actualizar_multip);
	}
}
