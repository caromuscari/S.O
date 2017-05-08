/*
 * escuchar_mensaje.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include "estructuras.h"
#include "socket_client.h"
#include "mensaje.h"
#include "log.h"



void * escucha_mensaje(int socket_){
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
