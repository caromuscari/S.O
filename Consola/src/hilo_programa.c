/*
 * hilo_programa.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include "socket_client.h"
#include "mensaje.h"
#include "log.h"
#include <semaphore.h>
#include <time.h>
#include "estructuras.h"

extern int socket_;
extern int tamAimprimir;
extern t_dictionary * sem;
extern sem_t semaforo;
time_t *tiempoInicial;
extern t_dictionary * tiempo;
extern t_dictionary * impresiones;

void * programa (char* pid){
	dictionary_put(tiempo,pid,tiempoInicial);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char * mensaje=strdup("");
	t_chequeo * semp = malloc(sizeof(t_chequeo));
	t_impresiones * impresiones2 = malloc(sizeof(t_impresiones));
	tiempoInicial=malloc(sizeof(time_t));
	*tiempoInicial=time(NULL);
	impresiones2=dictionary_get(impresiones,pid);
	semp=dictionary_get(sem,pid);
	while(1)
	{
		if(semp->valor==1)
		{	sem_wait(&semaforo);
			mensaje=recibir(socket_,tamAimprimir);
			printf("%s", mensaje);
			impresiones2->cantidad++;
		}

	}
	free(mensaje);
	free(semp);
	free(impresiones2);
	free(tiempoInicial);
}
