/*
 * parametros.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include <commons/collections/dictionary.h>
#include "log.h"
#include <commons/log.h>

extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_dictionary * sem;
extern t_dictionary * tiempo;
extern t_dictionary * impresiones;
extern t_consola * arch_config;
extern char * ingreso;
extern char * identi;

void inicializar_parametros()
{
	arch_config = malloc(sizeof(t_consola));
	arch_config->ip= strdup("");
	ingreso= strdup("");
	identi= strdup("");
	p_pid = dictionary_create();
	h_pid = dictionary_create();
	sem = dictionary_create();
	impresiones = dictionary_create();
	tiempo = dictionary_create();
}

void liberar_memoria()
{
	free(arch_config->ip);
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
	free(arch_config);
	liberar_log();
}

