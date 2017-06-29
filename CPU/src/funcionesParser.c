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
extern int sockKerCPU;
extern int sockMemCPU;
extern int tam_pagina_memoria;
extern int n;
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
		list_replace(pcb->in_stack,pcb->SP,stack_actual);
		posicion_retorno = nuevo_arg->offset;

	}else if ((identificador_variable >= 'A' && identificador_variable <= 'Z') || (identificador_variable >= 'a' && identificador_variable <= 'z')){
		// definir nueva variable
		t_memoria* nueva_var = malloc(sizeof(t_memoria));
			nueva_var->ID = identificador_variable;
			nueva_var->offset = calcular_offset(stack_actual->args,stack_actual->vars);
			nueva_var->size = 4;
			nueva_var->pag = 0;
		list_add(stack_actual->vars,nueva_var);
		list_replace(pcb->in_stack,pcb->SP,stack_actual);
		posicion_retorno = nueva_var->offset;
		//list_replace_and_destroy_element(pcb->in_stack, pcb->SP,stack_actual,(void*) stack_destroy);
	}else{
		char * aux= string_from_format("DEFINIR VARIABLE:%c identificador no aceptado",identificador_variable);
		escribir_log(aux,2);
		free(aux);
		return NO_EXISTE_VARIABLE;
	}
	char *aux= string_from_format("Se ejecutó DEFINIR VARIABLE - identificador:%c - retorno:%d",identificador_variable,posicion_retorno);
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
			char * aux= string_from_format("OBTENER POSICION VARIABLE:%c no existe variable",identificador_variable);
			escribir_log(aux,2);
			free(aux);
			return posicion_retorno;

		}
		char *aux= string_from_format("Se ejecutó OBTENER POSICION VARIABLE - identificador:%c - retorno:%d",identificador_variable,posicion_retorno);
		escribir_log(aux,1);
		free(aux);
		return posicion_retorno;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {

	char * mensaje = malloc(19);
	char * pid; char * pagina; char *offset; char *tam;
	char * aux_ceros;
	int desplazamiento=0;
	int controlador=0;

	pid = string_itoa(pcb->PID);
	pagina = string_itoa(calcular_pagina(direccion_variable,pcb->cant_pag));
	offset = string_itoa(direccion_variable);
	tam = strdup("0004");
	// PID
	memcpy(mensaje+desplazamiento,"P01",3);
	desplazamiento += 3;
	aux_ceros = string_repeat('0',4-strlen(pid));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pid));
	free(aux_ceros);
	desplazamiento += 4-strlen(pid);
	memcpy(mensaje+desplazamiento,pid,strlen(pid));
	desplazamiento += strlen(pid);
	// PAGINA
	aux_ceros = string_repeat('0',4-strlen(pagina));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pagina));
	free(aux_ceros);
	desplazamiento += 4-strlen(pagina);
	memcpy(mensaje+desplazamiento,pagina,strlen(pagina));
	desplazamiento += strlen(pagina);
	// OFFSET
	aux_ceros = string_repeat('0',4-strlen(offset));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(offset));
	free(aux_ceros);
	desplazamiento += 4-strlen(offset);
	memcpy(mensaje+desplazamiento,offset,strlen(offset));
	desplazamiento += strlen(offset);
	// TAMAÑO
	memcpy(mensaje+desplazamiento,tam,4);

	enviar(sockMemCPU,mensaje,&controlador,19);
	char * mensaje_aux= malloc(13);
	recibir(sockMemCPU,&controlador,mensaje_aux,13);
	char *tam_rest = string_substring(mensaje_aux,3,13);
	int tam_resto = atoi(tam_rest);
	free(mensaje_aux); free(tam_rest);
	char * valor_str = malloc(tam_resto);
	recibir(sockMemCPU,&controlador,valor_str,tam_resto);
	int valor = atoi(valor_str);
	free(valor_str);
	free(mensaje);
	free(pid); free(pagina); free(offset);free(tam);



	char * str_aux= string_from_format("Se ejecuto DEREFERENCIAR en posicion %d y retorno %d",direccion_variable,valor);
	escribir_log(str_aux,1);
	free(str_aux);

		return valor;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {

	char * mensaje = malloc(19+ sizeof(int));
	char * pid; char * pagina; char *offset; char *tam;
	char * aux_ceros;
	int desplazamiento=0;
	int controlador=0;
	pid = string_itoa(pcb->PID);
	pagina = string_itoa(calcular_pagina(direccion_variable,pcb->cant_pag));
	offset = string_itoa(calcular_offset_respecto_pagina(direccion_variable));
	tam = strdup("0004");
	// COD
	memcpy(mensaje+desplazamiento,"P08",3);
	desplazamiento += 3;
	// PID
	aux_ceros = string_repeat('0',4-strlen(pid));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pid));
	free(aux_ceros);
	desplazamiento += 4-strlen(pid);
	memcpy(mensaje+desplazamiento,pid,strlen(pid));
	desplazamiento += strlen(pid);
	// PAGINA
	aux_ceros = string_repeat('0',4-strlen(pagina));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pagina));
	free(aux_ceros);
	desplazamiento += 4-strlen(pagina);
	memcpy(mensaje+desplazamiento,pagina,strlen(pagina));
	desplazamiento += strlen(pagina);
	// OFFSET
	aux_ceros = string_repeat('0',4-strlen(offset));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(offset));
	free(aux_ceros);
	desplazamiento += 4-strlen(offset);
	memcpy(mensaje+desplazamiento,offset,strlen(offset));
	desplazamiento += strlen(offset);
	// TAMAÑO
	memcpy(mensaje+desplazamiento,tam,4);
	desplazamiento +=4;
	// VALOR
	char * str_valor = string_itoa(valor);
	aux_ceros = string_repeat('0',4-strlen(str_valor));
	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(str_valor));
	free(aux_ceros);
	desplazamiento += 4-strlen(str_valor);
	memcpy(mensaje+desplazamiento,str_valor,strlen(str_valor));
	desplazamiento += strlen(str_valor);

	enviar(sockMemCPU,mensaje,&controlador,19+sizeof(int));

	free(mensaje);
	free(pid); free(pagina); free(offset);free(tam);

	char * respuesta = malloc(2);
	recibir(sockMemCPU,&controlador,respuesta,2);
	if (string_equals_ignore_case(respuesta,"OK")){
		char * str_aux= string_from_format("Se ejecuto ASIGNAR en posicion %d y valor %d",direccion_variable,valor);
		escribir_log(str_aux,1);
		free(str_aux);
	} else {
		char * str_aux= string_from_format("ERROR ASIGNANDO en posicion %d y valor %d",direccion_variable,valor);
		escribir_log(str_aux,1);
		free(str_aux);
	}

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	char * mensaje = malloc(13+strlen(variable));
	char * aux_ceros;
	char * leng = string_itoa(strlen(variable));
	int desplazamiento =0; int controlador=0;
	// COD
	memcpy(mensaje+desplazamiento,"P09",3);
	desplazamiento += 3;
	// TAMAÑO RESTO MENSAJE
	aux_ceros = string_repeat('0',10-strlen(leng));
	memcpy(mensaje+desplazamiento,aux_ceros,10-strlen(leng));
	free(aux_ceros);
	desplazamiento += 10-strlen(leng);
	memcpy(mensaje+desplazamiento,leng,strlen(leng));
	desplazamiento += strlen(leng);
	free(leng);
	// VARIABLE COMPARTIDA
	memcpy(mensaje+desplazamiento,variable,strlen(variable));

	enviar(sockKerCPU,mensaje,&controlador,13+strlen(variable));
	free(mensaje);

	char * mensaje_r = malloc(7);
	recibir(sockKerCPU,&controlador,mensaje_r,7);
	char * str_var = malloc(4); int valor_var = 0;
	memcpy(str_var,mensaje_r+3,4);
	valor_var = atoi(str_var);
	free(str_var);free(mensaje_r);

	char * aux= string_from_format("Ejecute OBTENER VALOR COMPARTIDA de %s y es %d",variable,valor_var);
	escribir_log(aux,1);
	free(aux);

	return valor_var;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor) {

	char * mensaje = malloc(13+strlen(variable)+4);
	int desplazamiento = 0;
	char * aux_ceros;
	char * leng = string_itoa(strlen(variable)+4);
	char * str_valor = string_itoa(valor);
	int controlador = 0;
	// COD
	memcpy(mensaje+desplazamiento,"P10",3);
	desplazamiento += 3;
	// LENGTH VAR
	aux_ceros = string_repeat('0',10-strlen(leng));
	memcpy(mensaje+desplazamiento,aux_ceros,10-strlen(leng));
	free(aux_ceros);
	desplazamiento += 10-strlen(leng);
	memcpy(mensaje+desplazamiento,leng,strlen(leng));
	desplazamiento += strlen(leng);
	free(leng);
	// VARIABLE
	memcpy(mensaje+desplazamiento,variable,strlen(variable));
	desplazamiento += strlen(variable);
	// VALOR
	aux_ceros = string_repeat('0',4-strlen(str_valor));
	memcpy(mensaje+desplazamiento,aux_ceros, 4-strlen(str_valor));
	free(aux_ceros);
	desplazamiento += 4-strlen(str_valor);
	memcpy(mensaje+desplazamiento,str_valor,strlen(str_valor));

	enviar(sockKerCPU,mensaje,&controlador,17+strlen(variable));
	free(mensaje); free(str_valor);

	char * respuesta= malloc(2);
	recibir(sockKerCPU,&controlador,respuesta,2);
	if(string_equals_ignore_case(respuesta,"OK")){
		char * aux =string_from_format("Ejecute ASIGNAR VALOR COMPARTIDA %s valor %d",variable,valor);
		escribir_log(aux,1);
		free(aux);
		free(respuesta);
		return valor;
	}else{
		char * aux =string_from_format("ERROR ASIGNANDO VALOR COMPARTIDA %s valor %d",variable,valor);
		escribir_log(aux,1);
		free(aux);
		free(respuesta);
		return -1;
	}
}
void irAlLabel(t_nombre_etiqueta etiqueta) {

	char *eti = strdup(etiqueta);
	string_trim(&eti);
	pcb->PC = (int)dictionary_get(pcb->dicc_et,eti);
	free(eti);

	char * aux = string_from_format("Ejecute irALabel:%s",etiqueta);
	escribir_log(aux,1);
	free(aux);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	escribir_log(string_from_format("Ejecute llamarConRetorno etiqueta:%s,donde_retornar:%d",etiqueta,donde_retornar),1);
}
void llamarSinRetorno (t_nombre_etiqueta etiqueta){
	escribir_log("Ejecute llamarSinRetorno",1);
}

void finalizar(void) {
	n=1;
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
