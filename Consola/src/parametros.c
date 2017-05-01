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

extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern t_consola arch_config;

void inicializar_parametros()
{
	arch_config.ip= malloc(sizeof * arch_config.ip);
	arch_config.puerto= malloc(sizeof * arch_config.puerto);
	p_pid = dictionary_create();
	h_pid = dictionary_create();
}

void liberar_memoria()
{
	free(arch_config.ip);
	free(arch_config.puerto);
	dictionary_clean(p_pid);
	dictionary_destroy(p_pid);
	dictionary_clean(h_pid);
	dictionary_destroy(h_pid);
}

