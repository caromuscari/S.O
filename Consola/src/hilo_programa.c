/*
 * hilo_programa.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>
#include "socket_client.h"
#include "mensaje.h"

extern int socket_;

void * programa (){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	char * mensaje=malloc(sizeof *mensaje);
	int identificador;
	while(1)
	{
		mensaje=recibir(socket_);
		identificador=get_codigo(mensaje);
		switch(identificador)
		{
			case 01 :
				//archivo_Pokedex();
				break;
			case 02 :
				//otorgamiento_turno();
				break;
			case 03 :
				//consulta_objetivo();
				break;
		}
	}
}
