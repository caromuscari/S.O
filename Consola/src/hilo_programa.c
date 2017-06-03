/*
 * hilo_programa.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include "socket_client.h"
#include "mensaje.h"
#include <semaphore.h>
#include <time.h>
#include "estructuras.h"

extern int socket_;
extern int tamAimprimir;
extern t_dictionary * sem;
extern sem_t semaforo;
time_t tiempoInicial;
extern t_dictionary * tiempo;
extern t_dictionary * impresiones;

void * programa (long int pid){
	tiempoInicial=time(NULL);
	dictionary_put(tiempo,pid,tiempoInicial);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char * mensaje=strdup("");
	int identificador;
	t_chequeo * semp;
	t_impresiones * impresiones;
	impresiones=dictionary_get(impresiones,pid);
	semp=dictionary_get(sem,pid);
	while(1)
	{
		if(semp->valor==1)
		{	sem_wait(&semaforo);
			mensaje=recibir(socket_,tamAimprimir);
			printf("%s", mensaje);
			impresiones->cantidad++;
		}

	}
}
