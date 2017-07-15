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
#include <signal.h>

extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern t_dictionary * tiempo;
extern sem_t semaforo;
extern sem_t x;
extern int flag;
extern int socket_;
pthread_t hiloPrograma;
extern char * aImprimir;

void escuchar_mensaje();
void finalizar(char *pid, int socket_);
void senial();

void escuchar_mensaje()
{
	signal(SIGKILL,senial);
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char *mensaje;
	char *mensaje2;
	sem_init(&semaforo,0,2);
	sem_init(&x,0,2);

	while(flag==0)
	{
		sem_wait(&x);
		sem_wait(&semaforo);
		t_chequeo *sema = malloc(sizeof(t_chequeo));
		t_chequeo *smod = malloc(sizeof(t_chequeo));
		t_impresiones *cant = malloc(sizeof(t_impresiones));
		t_hilo *hilo = malloc(sizeof(t_hilo));
		mensaje = recibir(socket_,13);
		//escribir_log(mensaje);
		mensaje2 = get_codigo(mensaje);
		//escribir_log(mensaje2);

		switch(atoi(mensaje2))
		{
			case 4: ;
				char *pid;

				sema->valor=0;
				cant->cantidad=0;

				pid = recibir(socket_,1);

				dictionary_put(sem,pid,sema);
				dictionary_put(impresiones,pid,cant);

				pthread_create(&hiloPrograma,NULL,(void*)programa,pid);
				hilo->hilo= hiloPrograma;

				//escribir_log_con_numero("Se inicio el programa: ", atoi(pid));
				printf("Se inicio el programa: %s \n", pid);

				dictionary_put(p_pid,pid,hilo);
				dictionary_put(h_pid,string_itoa(hiloPrograma),pid);

				break;
			case 5:
				//escribir_log("No se pudo iniciar el programa por falta de memoria");
				printf("No se pudo iniciar el programa por falta de memoria \n");
				free(sema);
				free(cant);
				free(smod);
				free(hilo);
				break;
			case 9: ;
				char *pid2 = recibir(socket_,1);
				char *size = get_payload(mensaje);
				escribir_log(pid2);

				aImprimir = recibir(socket_, atoi(size)-1);
				//escribir_log_compuesto("A imprimir: ", aImprimir);

				smod=dictionary_get(sem,pid2);
				smod->valor=1;
				sem_wait(&x);

				free(sema);
				free(cant);
				free(hilo);
				free(pid2);
				break;
			case 10: ;
				char *pid3;

				pid3 = recibir(socket_,1);
				finalizar(pid3, socket_);

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
		sem_post(&x);
		sem_post(&semaforo);
		free(mensaje);
		free(mensaje2);
	}

	desconectar_consola();
	pthread_exit(NULL);
}

void finalizar(char *pid, int socket_)
{
	if(dictionary_has_key(p_pid,pid))
	{
		char *pid2;
		t_hilo *hilo = dictionary_get(p_pid,pid);
		char *var = string_itoa(hilo->hilo);

		if(pthread_cancel(hilo->hilo)==0)
		{
			pid2 = dictionary_get(h_pid,var);
			escribir_log_con_numero("Se finalizo el programa: ", atoi(pid));
			tiempofinal_impresiones(pid2);

			free(dictionary_remove(h_pid,var));
			free(dictionary_remove(p_pid,pid));
			free(dictionary_remove(impresiones,pid));
			free(dictionary_remove(sem,pid));
			free(dictionary_remove(tiempo,pid));
		}
		else escribir_log("No se pudo finalizar el programa");
	}
}

void senial()
{
	pthread_exit(NULL);
}
