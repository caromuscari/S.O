/*
 * funcionesParser.c
 *
 *  Created on: 31/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include "funcionesParser.h"
#include "funcionesCPU.h"
#include "estructuras.h"
#include "socket.h"
#include "log.h"

extern t_PCB_CPU* pcb;
extern int tam_pagina_memoria;
int FD;

t_puntero definirVariable(t_nombre_variable identificador_variable) {

	t_stack_element* stack_actual = list_get(pcb->in_stack,pcb->SP);
	t_puntero posicion_retorno;
	if(identificador_variable >= '0' && identificador_variable <= '9'){
		// definir nuevo argumento
		t_memoria* nuevo_arg = malloc(sizeof(t_memoria));
			nuevo_arg->ID = identificador_variable;
			nuevo_arg->offset = calcular_offset(stack_actual->args,stack_actual->vars);
			nuevo_arg->size = 4;
			nuevo_arg->pag = 0;
		list_add(stack_actual->vars,nuevo_arg);
		list_replace(pcb->in_stack,pcb->SP,nuevo_arg);
		posicion_retorno = nuevo_arg->offset;

	}else if ((identificador_variable >= 'A' && identificador_variable <= 'Z') || (identificador_variable >= 'a' && identificador_variable <= 'z')){
		// definir nueva variable
		t_memoria* nueva_var = malloc(sizeof(t_memoria));
			nueva_var->ID = identificador_variable;
			nueva_var->offset = calcular_offset(stack_actual->args,stack_actual->vars);
			nueva_var->size = 4;
			nueva_var->pag = 0;
		list_add(stack_actual->vars,nueva_var);
		list_replace(pcb->in_stack,pcb->SP,nueva_var);
		posicion_retorno = nueva_var->offset;
		//list_replace_and_destroy_element(pcb->in_stack, pcb->SP,stack_actual,(void*) stack_destroy);
	}else{
		char * aux= string_from_format("definirVariable:%c identificador no aceptado",identificador_variable);
		escribir_log(aux,2);
		free(aux);
		return NO_EXISTE_VARIABLE;
	}
	char *aux= string_from_format("Se ejecutó definirVariable - identificador:%c - retorno:%d",identificador_variable,posicion_retorno);
	escribir_log(aux,1);
	free(aux);
	return posicion_retorno;

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	t_stack_element* stack_actual = list_get(pcb->in_stack,pcb->SP);
	t_puntero posicion_retorno;
		if(identificador_variable >= '0' && identificador_variable <= '9'){

			int pos = identificador_variable - '0';
			t_memoria* arg=list_get(stack_actual->args,pos);
			posicion_retorno= arg->offset;

		}else if ((identificador_variable >= 'A' && identificador_variable <= 'Z') || (identificador_variable >= 'a' && identificador_variable <= 'z')){

			posicion_retorno = buscar_offset_variable(stack_actual->vars,identificador_variable);

		}
		if(posicion_retorno == -1){
			char * aux= string_from_format("obtenerPosicionVariable:%c no existe variable",identificador_variable);
			escribir_log(aux,2);
			free(aux);
			return posicion_retorno;

		}
		char *aux= string_from_format("Se ejecutó definirVariable - identificador:%c - retorno:%d",identificador_variable,posicion_retorno);
		escribir_log(aux,1);
		free(aux);
		return posicion_retorno;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
	escribir_log("Ejecute dereferenciar",1);
		return 0;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	escribir_log("Ejecute asignar",1);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	escribir_log("Ejecute obtenerValorCompartida",1);
		return 0;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor) {
	escribir_log("Ejecute asignarValorCompartida",1);
			return 0;

}
void irAlLabel(t_nombre_etiqueta etiqueta) {
	escribir_log(string_from_format("Ejecute irALabel:%s",etiqueta),1);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	escribir_log(string_from_format("Ejecute llamarConRetorno etiqueta:%s,donde_retornar:%d",etiqueta,donde_retornar),1);
}
void llamarSinRetorno (t_nombre_etiqueta etiqueta){
	escribir_log("Ejecute llamarSinRetorno",1);
}

void finalizar(void) {
	escribir_log("Ejecute finalizar",1);
}

void retornar(t_valor_variable retorno) {
	escribir_log("Ejecute retornar",1);
}

void wait(t_nombre_semaforo identificador_semaforo) {
	escribir_log("Ejecute wait",1);
}

void signale(t_nombre_semaforo identificador_semaforo) {
	escribir_log("Ejecute signale",1);
}
void escribir (t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	escribir_log("Ejecute Escribir",1);
}
void moverCursor (t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
	escribir_log("Ejecute moverCursor",1);
}
void liberar (t_puntero puntero){
	escribir_log("Ejecute liberar",1);
}
t_puntero reservar (t_valor_variable espacio){
	escribir_log("Ejecute reservar",1);
	return 1;
}
t_descriptor_archivo abrir (t_direccion_archivo direccion, t_banderas flags){
	escribir_log("Ejecute abrir",1);
	return 1;
}
void leer (t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	escribir_log("Ejecute leer",1);
}
void cerrar (t_descriptor_archivo descriptor_archivo){
	escribir_log("Ejecute cerrar",1);
}
void borrar (t_descriptor_archivo direccion){
	escribir_log("Ejecute borrar",1);
}

AnSISOP_funciones funcionesTodaviaSirve = {
		.AnSISOP_definirVariable = definirVariable,
		.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
		.AnSISOP_dereferenciar = dereferenciar,
		.AnSISOP_asignar = asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel = irAlLabel,
		.AnSISOP_llamarConRetorno =llamarConRetorno,
		.AnSISOP_llamarSinRetorno = llamarSinRetorno,
		.AnSISOP_retornar = retornar,
		.AnSISOP_finalizar = finalizar,};

AnSISOP_kernel funcionesKernelTodaviaSirve = {
				.AnSISOP_wait = wait,
				.AnSISOP_signal =signale,
				.AnSISOP_reservar = reservar,
				.AnSISOP_liberar =liberar,
				.AnSISOP_abrir = abrir,
				.AnSISOP_borrar = borrar,
				.AnSISOP_cerrar = cerrar,
				.AnSISOP_escribir = escribir,
				.AnSISOP_leer = leer,
				.AnSISOP_moverCursor = moverCursor,
};
