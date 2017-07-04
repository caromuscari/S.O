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

extern char *sem_id;
extern char *sem_in;
extern char *shared;
extern t_dictionary *sems;
extern t_dictionary *vglobales;

void inicializar_sems();
void sem_signal(t_program*, char *);
void sem_wait(t_program *, char *);
void inicializar_vglobales();
int lock_vglobal(t_vglobal *vg, int prog);
void unlock_vglobal();

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

void sem_wait(t_program *proceso, char *sema)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);

	if(sem != NULL)
	{
		if (sem->value  > 0)
		{
			escribir_log_compuesto("Se realiza un wait al semaforo: ",sema);

			sem->value --;
		}else
		{
			//bloquear proceso (hablar con lean, lo estamos manejando mal)
			queue_push(sem->procesos, (void *)proceso);
			sem->value --;
		}
	}else
	{
		//sacar proceso, hace wait en un semaforo inexistente
	}
}

void sem_signal(t_program *prog, char *sema)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);

	if((sem != NULL)||(strcmp(sema,"$")))
	{
		sem->value ++;

		if (sem->value <= 0)
		{
			int proc = (int)queue_pop(sem->procesos);
			desbloquear_proceso(proc);
		}
	}else
	{
		//eliminar el proceso, signal a semaforo invalido
		forzar_finalizacion(prog->PID, 0, -11);
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

void set_vglobal(char *vglobal, int num, int prog)
{
	t_vglobal *vg = (t_vglobal *)dictionary_get(vglobales, vglobal);

	if(vg != NULL)
	{
		int sem = lock_vglobal(vg, prog);
		if(sem)
		{
			vg->value = num;
			unlock_vglobal(vg);
		}else ; //bloquear proceso
	}
}

void unlock_vglobal(t_vglobal *vg)
{
	vg->mutex_ ++;
	if(vg->mutex_ <= 0)
	{
		queue_pop(vg->procesos);
	}
}
