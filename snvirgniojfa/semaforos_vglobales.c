#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include "estructuras.h"
#include "planificador.h"
#include "log.h"
#include "socket.h"
#include "mensaje.h"
#include "metadata.h"
#include "cpuManager.h"

extern char *sem_id;
extern char *sem_in;
extern char *shared;
extern t_dictionary *sems;
extern t_dictionary *vglobales;

void inicializar_sems();
void sem_signal(t_program *prog, char *sema, int socket_);
void sem_wait_(t_program *proceso, char *sema, int socket_);
void inicializar_vglobales();
int lock_vglobal(t_vglobal *vg, int prog);
void unlock_vglobal();
void set_vglobal(char *vglobal, int num, t_program *prog, int socket_);
void get_vglobal(char *vglobal, int num, t_program *prog, int socket_);

void inicializar_sems()
{
	sems = dictionary_create();
	char **ids = string_split(sem_id, ",");
	char **ins = string_split(sem_in, ",");
	int n = 0;

	while (ids[n] != NULL)
	{
		t_sem *sem = malloc (sizeof(t_sem));
		sem->value = atoi(ins[n]);
		sem->procesos = queue_create();
		dictionary_put(sems, ids[n],sem);
		n++;
	}
	n = 0;
	while (ids[n] != NULL && ins[n] != NULL)
	{
		free(ids[n]);
		free(ins[n]);
	}

	free(ids);
	free(ins);
}

void inicializar_vglobales()
{
	vglobales = dictionary_create();
	char **ids = string_split(shared, ",");
	int n = 0;

	while (ids[n] != NULL)
	{
		t_vglobal *vg = malloc (sizeof(t_vglobal));
		vg->mutex_ = 1;
		vg->value = 0;
		vg->procesos = queue_create();
		dictionary_put(vglobales ,ids[n] ,vg);
		n++;
	}
	n = 0;
	while (ids[n] != NULL)
	{
		free(ids[n]);
	}
	free(ids);
}

void sem_wait_(t_program *proceso, char *sema, int socket_)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);
	int controlador;
	if(sem != NULL)
	{
		if (sem->value  > 0)
		{
			escribir_log_compuesto("Se realiza un wait al semaforo: ",sema);

			sem->value --;
		}else
		{
			enviar(socket_, "K2300000000000000", &controlador);

			char *mensaje = recibir(socket_, &controlador);
			char *header = get_header(mensaje);
			if(comparar_header(header, "P"))
			{
				char *codi = get_codigo(mensaje);
				int codigo = atoi(codi);
				if(codigo == 23)
				{
					char *mensaje_r2 = get_mensaje(mensaje);
					t_PCB *pcb_actualizado2 =deserializarPCB_CPUKer(mensaje_r2);
					actualizar_pcb(proceso, pcb_actualizado2);
					free(mensaje_r2);
				}
				free(codi);
			}

			bloquear_proceso(proceso->PID);

			queue_push(sem->procesos, (void *)proceso);
			sem->value --;

			free(mensaje);
			free(header);
		}
	}else
	{
		forzar_finalizacion(proceso->PID, 0, 11, 1);//sacar proceso, hace wait en un semaforo inexistente
	}
}

void sem_signal(t_program *prog, char *sema, int socket_)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);

	if(sem != NULL)
	{
		sem->value ++;

		if (sem->value <= 0)
		{
			int proc = (int)queue_pop(sem->procesos);
			desbloquear_proceso(proc);
		}
		int controlador;
		enviar(socket_, "OK000000000000000", &controlador);
	}else
	{
		//eliminar el proceso, signal a semaforo invalido
		forzar_finalizacion(prog->PID, 0, 11, 1);
	}
}

int lock_vglobal(t_vglobal *vg, int prog)
{
	vg->mutex_ --;
	if(vg->mutex_ < 0)
	{
		queue_push(vg->procesos,(void *) prog);
		return 0;
	}else return 1;
}

void set_vglobal(char *vglobal, int num, t_program *prog, int socket_)
{
	t_vglobal *vg = (t_vglobal *)dictionary_get(vglobales, vglobal);
	int controlador;
	if(vg != NULL)
	{
		enviar(socket_, "OK000000000000000", &controlador);
		int sem = lock_vglobal(vg, prog->PID);
		if(sem)
		{
			vg->value = num;
			unlock_vglobal(vg);
		}
		else
		{
			enviar(socket_, "K2300000000000000", &controlador);

			char *mensaje = recibir(socket_, &controlador);
			char *header = get_header(mensaje);
			if(comparar_header(header, "P"))
			{
				char *codi = get_codigo(mensaje);
				int codigo = atoi(codi);
				if(codigo == 23)
				{
					char *mensaje_r2 = get_mensaje(mensaje);
					t_PCB *pcb_actualizado2 =deserializarPCB_CPUKer(mensaje_r2);
					actualizar_pcb(prog, pcb_actualizado2);
					free(mensaje_r2);
				}
				free(codi);
			}

			bloquear_proceso(prog->PID); //bloquear proceso
		}
	}else forzar_finalizacion(prog->PID, 0, 12, 1);
}

void get_vglobal(char *vglobal, int num, t_program *prog, int socket_)
{
	t_vglobal *vg = (t_vglobal *)dictionary_get(vglobales, vglobal);
	int controlador1;

	if(vg != NULL)
	{
		char *pid_aux = string_itoa(vg->value);
		int size_pid = string_length(pid_aux);
		char *completar = string_repeat('0', 4 - size_pid);
		char *mensaje = strdup("K92");
		string_append(&mensaje, completar);
		string_append(&mensaje, pid_aux);

		enviar(socket_, mensaje, &controlador1);

		free(mensaje);
		free(pid_aux);
		free(completar);
	} else forzar_finalizacion(prog->PID, 0, 12, 1);
}

void unlock_vglobal(t_vglobal *vg)
{
	vg->mutex_++;
	if(vg->mutex_ <= 0)
	{
		int proc = (int)queue_pop(vg->procesos);
		desbloquear_proceso(proc);
	}
}
