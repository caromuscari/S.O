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
#include <semaphore.h>
#include <commons/string.h>
#include "socket_client.h"
#include "hilo_usuario.h"
#include "hilo_programa.h"
#include "estructuras.h"
#include "log.h"
#include "mensaje.h"


extern int socket_;
pthread_t hiloPrograma;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern int tamAimprimir;
extern sem_t semaforo;
extern int flag;

void escuchar_mensaje();

void escuchar_mensaje()
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char *mensaje;
	char *mensaje2;
	sem_init(&semaforo,0,1);

	while(flag==0)
	{
		sem_wait(&semaforo);
		t_chequeo *sema = malloc(sizeof(t_chequeo));
		t_chequeo *smod = malloc(sizeof(t_chequeo));
		t_impresiones *cant = malloc(sizeof(t_impresiones));
		t_hilo *hilo = malloc(sizeof(t_hilo));
		mensaje=recibir(socket_,13);
		escribir_log(mensaje);
		mensaje2=get_codigo(mensaje);
		escribir_log(mensaje2);

		switch(atoi(mensaje2))
		{
			case 4: ;
				char *pid;

				sema->valor=0;
				cant->cantidad=0;
				hilo->hilo= hiloPrograma;

				pid=recibir(socket_,1);
				escribir_log(pid);

				pthread_create(&hiloPrograma, NULL, (void*) programa, pid);

				escribir_log("Se inicio el programa");

				dictionary_put(p_pid,pid,hilo);
				dictionary_put(h_pid,string_itoa(hiloPrograma),pid);
				dictionary_put(sem,pid,sema);
				dictionary_put(impresiones,pid,cant);

				break;
			case 5:
				printf("%s","no se pudo iniciar el programa");
				escribir_log("No se pudo iniciar el programa");

				free(sema);
				free(cant);
				free(smod);
				free(hilo);

				break;
			case 9: ;
				char * pid2;

				pid2=recibir(socket_,1);
				char *size = get_payload(mensaje);
				tamAimprimir= atoi(size);

				smod=dictionary_get(sem,pid);
				smod->valor=1;

				free(sema);
				free(cant);
				free(hilo);
				free(pid2);

				break;
			case 10: ;
				char * pid3;
				char * hpid=strdup("");

				pid3=recibir(socket_,1);
				hpid=dictionary_get(p_pid,pid3);
				finalizar_programa(hpid, socket_);

				free(sema);
				free(cant);
				free(smod);
				free(hilo);
				free(pid3);

				break;
			default:
				escribir_log("Mensaje incorrecto del Kernel");
				flag=1;
				free(sema);
				free(cant);
				free(smod);
				free(hilo);
				break;
		}
		sem_post(&semaforo);
		free(mensaje);
		free(mensaje2);
	}

}
