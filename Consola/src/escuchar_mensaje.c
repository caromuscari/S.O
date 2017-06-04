/*
 * escuchar_mensaje.c
 *
 *  Created on: 28/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include "socket_client.h"
#include "hilo_usuario.h"
#include "estructuras.h"
#include "log.h"
#include "mensaje.h"
#include <semaphore.h>

extern int socket_;
pthread_t hiloPrograma;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern int tamAimprimir;
extern sem_t semaforo;


void *programa (int pid);

void * escuchar_mensaje(){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char * mensaje=strdup("");
	int mensaje2;
	long int pid;
	sem_init(&semaforo,0,1);
	while(1)
	{
		sem_wait(&semaforo);
		t_chequeo * sema = malloc(sizeof(t_chequeo));
		t_chequeo * smod = malloc(sizeof(t_chequeo));
		t_impresiones * cant = malloc(sizeof(t_impresiones));
		mensaje=recibir(socket_,13);
		mensaje2=get_codigo(mensaje);
		switch(mensaje2)
		{
			case 4:
				sema->valor=0;
				cant->cantidad=0;
				pid= atol(recibir(socket_,2));
				pthread_create(&hiloPrograma, NULL, (void*) programa(pid), NULL);
				escribir_log("Se inicio el programa");
				dictionary_put(p_pid,pid,hiloPrograma);
				dictionary_put(h_pid,hiloPrograma,pid);
				dictionary_put(sem,pid,sema);
				dictionary_put(impresiones,pid,cant);
				free(sema);
				free(cant);
				free(smod);
				break;
			case 5:
				printf("%s","no se pudo iniciar el programa");
				escribir_log("No se pudo iniciar el programa");
				free(sema);
				free(cant);
				free(smod);
				break;
			case 9:
				pid=atol(recibir(socket_,2));
				tamAimprimir= get_payload(mensaje);
				smod=dictionary_get(sem,pid);
				smod->valor=1;
				free(sema);
				free(cant);
				free(smod);
				break;
			case 10:
				pid=atol(recibir(socket_,2));
				finalizar_programa(pid,socket_);
				free(sema);
				free(cant);
				free(smod);
				break;
			default:
				escribir_log("Mensaje incorrecto del Kernel");
				free(sema);
				free(cant);
				free(smod);
				break;
		}
		sem_post(&semaforo);
	}

	free(mensaje);
}
