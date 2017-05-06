/*
 * log.c
 *
 *  Created on: 29/11/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/string.h>

t_log *log;

void crear_archivo_log(char *file)
{

	log = log_create(file,"CPU",false, LOG_LEVEL_INFO);
	log_info(log, string_from_format("se crea archivo de log en ruta: %s",file));
}

void escribir_log(char *mensaje,int cod)
{
	switch(cod){
	case 1:
		log_info(log, mensaje);
		break;
	case 2:
		log_error(log,mensaje);
		break;
	}

}

void liberar_log()
{
	log_destroy(log);
}
