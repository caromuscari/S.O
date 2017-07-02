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
#include <semaphore.h>
#include <time.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include "socket_client.h"
#include "mensaje.h"
#include "log.h"
#include "estructuras.h"

extern int socket_;
extern int tamAimprimir;
extern t_dictionary * sem;
extern sem_t semaforo;
time_t *tiempoInicial;
extern t_dictionary * tiempo;
extern t_dictionary * impresiones;

void programa(char* pid);

void programa(char* pid)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char *mensaje;
	t_chequeo *semp;
	t_impresiones *impresiones2;
	//struct tm* tm_info;
	t_tiempo * tiempo2;

	tiempo2 = malloc(sizeof(t_tiempo));
	tiempoInicial = malloc(sizeof(time_t));
	tiempo2->tiempo = malloc(sizeof(time_t));

	*tiempoInicial = time(NULL);
	tiempo2->tm_info = localtime(tiempoInicial);
	strftime(tiempo2->buffer, 26, "%Y-%m-%d %H:%M:%S", tiempo2->tm_info);

	tiempo2->tiempo = tiempoInicial;
	dictionary_put(tiempo,pid,tiempo2);

	impresiones2 = dictionary_get(impresiones,pid);
	semp = dictionary_get(sem,pid);

	while(1)
	{
		if(semp->valor==1)
		{
			sem_wait(&semaforo);
			mensaje = recibir(socket_,tamAimprimir);
			printf("%s", mensaje);
			impresiones2->cantidad++;
			free(mensaje);
		}
	}
	free(tiempoInicial);
	free(tiempo2);
}
