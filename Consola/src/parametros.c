/*
 * parametros.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"
#include <commons/collections/dictionary.h>
#include "log.h"
#include <commons/log.h>

extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_dictionary * sem;
extern t_dictionary * tiempo;
extern t_dictionary * impresiones;
extern t_consola arch_config;
extern char * ingreso;
extern char * identi;

void inicializar_parametros()
{
	arch_config.ip= malloc(sizeof * arch_config.ip);
	arch_config.puerto= malloc(sizeof * arch_config.puerto);
	ingreso= malloc(sizeof *ingreso);
	identi= malloc(sizeof *identi);
	p_pid = dictionary_create();
	h_pid = dictionary_create();
	sem = dictionary_create();
	impresiones = dictionary_create();
	tiempo = dictionary_create();
}

void liberar_memoria()
{
	free(arch_config.ip);
	free(arch_config.puerto);
	free(ingreso);
	free(identi);
	dictionary_clean(p_pid);
	dictionary_destroy(p_pid);
	dictionary_clean(h_pid);
	dictionary_destroy(h_pid);
	dictionary_clean(sem);
	dictionary_destroy(sem);
	dictionary_clean(tiempo);
	dictionary_destroy(tiempo);
	dictionary_clean(impresiones);
	dictionary_destroy(impresiones);
	liberar_log();
}

