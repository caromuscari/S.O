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

extern char *sem_id;
extern char *sem_in;
extern t_dictionary *sems;
extern t_log *log;

void inicializar_sems();
void sem_wait(int, char *);
void sem_signal(int, char *);
void inicializar_vglobales();
void lock_vglobal();
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

void sem_wait(int proceso, char *sema)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);

	if(sem != NULL)
	{
		if (sem->value  > 0)
		{
			//acá va un log
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

void sem_signal(int proceso, char *sema)
{
	t_sem *sem = (t_sem *)dictionary_get(sems, sema);

	if (sem != NULL)
	{
		sem->value ++;

		if (sem->value <= 0)
		{
			queue_pop(sem->procesos);
			//desbloquear el proceso
			//el proceso que sale de la cola deberia ser insertado en la cola de ready
		}
	}else
	{
		//eliminar el proceso, signal a semaforo invalido
	}
}

void inicializar_vglobales()
{

}

void lock_vglobal()
{

}

void unlock_vglobal()
{

}
