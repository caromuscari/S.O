/*
 * funcionesParser.c
 *
 *  Created on: 31/5/2017
 *      Author: utnso
 */

#include "funcionesParser.h"

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <parser/parser.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "estructuras.h"
#include "funcionesCPU.h"
#include "log.h"
#include "mensajes.h"
#include "socket.h"

extern t_PCB_CPU* pcb;
extern int sockKerCPU;
extern int sockMemCPU;
extern int fifo;
int FD;
extern int accion_siguiente;

t_puntero definirVariable(t_nombre_variable identificador_variable) {

	t_stack_element* stack_actual = list_get(pcb->in_stack,pcb->SP);
	t_puntero posicion_retorno;
	if(identificador_variable >= '0' && identificador_variable <= '9'){
		// definir nuevo argumento
		t_memoria* nuevo_arg = malloc(sizeof(t_memoria));
		nuevo_arg->ID = identificador_variable;
		nuevo_arg->offset = calcular_offset(stack_actual->args,stack_actual->vars);
		nuevo_arg->size = 4;
		nuevo_arg->pag = calcular_pagina(nuevo_arg->offset,0);
		list_add(stack_actual->vars,nuevo_arg);
		list_replace(pcb->in_stack,pcb->SP,stack_actual);
		posicion_retorno = nuevo_arg->offset;

	}else if ((identificador_variable >= 'A' && identificador_variable <= 'Z') || (identificador_variable >= 'a' && identificador_variable <= 'z')){
		// definir nueva variable
		t_memoria* nueva_var = malloc(sizeof(t_memoria));
		nueva_var->ID = identificador_variable;
		nueva_var->offset = calcular_offset(stack_actual->args,stack_actual->vars);
		nueva_var->size = 4;
		nueva_var->pag = calcular_pagina(nueva_var->offset,0);
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


	int size=0;
	int controlador=0;

	char * mensaje = mensaje_leer_memoria(pcb->PID,direccion_variable,pcb->cant_pag,4,&size);
	enviar(sockMemCPU,mensaje,&controlador,size);

	char* mensaje_aux= malloc(4);
	//recibir(sockMemCPU,&controlador,mensaje_aux,13);
	recv(sockMemCPU,mensaje_aux,4,MSG_WAITALL);
	/*char *tam_rest = malloc(10);
	memcpy(tam_rest,mensaje_aux+3,10);
	int tam_resto = atoi(tam_rest);

	if(tam_resto != 2){
		tam_resto = 2;
	}
	char *valor_str = malloc(tam_resto);
	recibir(sockMemCPU,&controlador,valor_str,tam_resto);
	int valor = atoi(valor_str);

	free(valor_str); ; free(tam_rest);*/
	int valor = atoi(mensaje_aux);
	free(mensaje_aux);free(mensaje);





	char * str_aux= string_from_format("Se ejecuto DEREFERENCIAR en posicion %d y retorno %d",direccion_variable,valor);
	escribir_log(str_aux,1);
	free(str_aux);

	return valor;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {

	char *mensaje = mensaje_escibir_memoria(pcb->PID,direccion_variable,pcb->cant_pag,valor);
	int controlador=0;
	enviar(sockMemCPU,mensaje,&controlador,19+sizeof(int));
	free(mensaje);

	char * respuesta = malloc(13);
	recibir(sockMemCPU,&controlador,respuesta,13);

	if (strncmp(respuesta,"M02",3)==0){
		char * str_aux= string_from_format("Se ejecuto ASIGNAR en posicion %d y valor %d",direccion_variable,valor);
		escribir_log(str_aux,1);
		free(str_aux);

	} else if(strncmp(respuesta,"M03",3)==0){
		char * str_aux= string_from_format("ERROR ASIGNANDO en posicion %d y valor %d",direccion_variable,valor);
		escribir_log(str_aux,1);
		free(str_aux);
	}
	free(respuesta);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {

	char *variable_compartida = strdup(variable);
	string_trim(&variable_compartida);
	int size=0; int controlador=0;
	char *mensaje = mensaje_variable_kernel(9,variable_compartida,0,&size);

	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);free(variable_compartida);

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


	int size,v_retorno;
	int controlador = 0;
	char * variable_compartida = strdup(variable);
	string_trim(&variable_compartida);

	char *mensaje = mensaje_variable_kernel(10,variable_compartida,valor,&size);

	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);free(variable_compartida);

	char * respuesta= malloc(17);
	recibir(sockKerCPU,&controlador,respuesta,17);

	if(strncmp(respuesta,"OK",2)==0){
		char * aux =string_from_format("Ejecute ASIGNAR VALOR COMPARTIDA %s valor %d",variable,valor);
		escribir_log(aux,1);
		free(aux);
		v_retorno = valor;

	}else if(strncmp(respuesta,"K21",3)==0){
		fifo = FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;
		char *str_cod_error = string_substring(respuesta,13,4);
		int codigo_error = (-1)* (atoi(str_cod_error));
		pcb->exit_code = codigo_error;
		char * aux =string_from_format("ERROR ASIGNANDO VALOR COMPARTIDA %s valor %d",variable,valor);
		escribir_log(aux,1);
		free(aux);
		free(str_cod_error);
		v_retorno = -1;
	}

	free(respuesta);
	return v_retorno;
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

	pcb->SP ++;
	t_stack_element * nuevo_el = malloc(sizeof (t_stack_element));
	nuevo_el->pos = pcb->SP;
	nuevo_el->retPos = pcb->PC;
	nuevo_el->retVar.ID = '\0';
	nuevo_el->retVar.offset = donde_retornar;
	nuevo_el->retVar.size = 4;
	nuevo_el->retVar.pag = calcular_pagina(donde_retornar,0);
	nuevo_el->args = list_create();
	nuevo_el->vars = list_create();
	list_add(pcb->in_stack,nuevo_el);

	char * eti = strdup(etiqueta);
	string_trim(&eti);
	pcb->PC =(int) dictionary_get(pcb->dicc_et,eti);
	free(eti);

	escribir_log(string_from_format("Ejecute LLAMAR CON RETORNO etiqueta:%s,donde_retornar:%d",etiqueta,donde_retornar),1);
}

void llamarSinRetorno (t_nombre_etiqueta etiqueta){

	escribir_log("Ejecute llamarSinRetorno",1);
}

void finalizar(void) {
	fifo = FINALIZAR_PROGRAMA;
	accion_siguiente = FINALIZAR_PROGRAMA;


	escribir_log("Ejecute FINALIZAR",1);
}

void retornar(t_valor_variable retorno) {

	t_stack_element *aux_stack_el = list_get(pcb->in_stack,pcb->SP);

	char * mensaje = mensaje_escibir_memoria(pcb->PID,aux_stack_el->retVar.offset,pcb->cant_pag,retorno);
	int controlador =0 ;
	enviar(sockMemCPU,mensaje,&controlador,19);
	free(mensaje);

	char * respuesta  = malloc(13);
	recibir(sockKerCPU,&controlador,respuesta,13);
	if(strncmp(respuesta,"M02",3)==0){
		char * aux = string_from_format("ALMACENAR BYTES de valor de retorno ",retorno);
		escribir_log(aux,1);
		free(aux);
		pcb->SP --;
		pcb->PC = aux_stack_el->retPos;
		list_remove_and_destroy_element(pcb->in_stack, pcb->SP++,(void*)stack_destroy);
		escribir_log("Ejecute RETORNAR ",1);

	}else if(strncmp(respuesta,"M03",3)== 0){
		char * aux = string_from_format("ERROR ejecutando RETORNAR con valor de retorno %d",retorno);
		escribir_log(aux,2);
		free(aux);
	}
	free(respuesta);
}

void ts_wait(t_nombre_semaforo identificador_semaforo) {

	int controlador=0;int size=0;
	char *semaforo = strdup(identificador_semaforo);
	string_trim(&semaforo);
	char *mensaje = mensaje_semaforo("P14",semaforo,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *respuesta = malloc(17);
	recibir(sockKerCPU,&controlador,respuesta,17);

	if(strncmp(respuesta,"OK",2) == 0){
		char * logi = string_from_format("Ejecute WAIT sobre semaforo %s",identificador_semaforo);
		escribir_log(logi,1);
		free(logi);
	}else if(strncmp(respuesta,"K23",3)==0){
		fifo = FINALIZAR_PROGRAMA;
		accion_siguiente = BLOQUEAR_PROCESO;
		char * logi = string_from_format("Ejecute WAIT sobre semaforo %s y me bloqueo",identificador_semaforo);
		escribir_log(logi,1);
		free(logi);

	}else if(strncmp(respuesta,"K21",3)==0){
		fifo = FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;
		char *cod_error= string_substring(respuesta,13,4);
		int codigo_error= (-1)*(atoi(cod_error));
		pcb->exit_code = codigo_error;
		char * logi = string_from_format("ERROR ejecutando WAIT sobre semaforo %s",identificador_semaforo);
		escribir_log(logi,2);
		free(logi);free(cod_error);
	}
	free(respuesta);

}

void ts_signale(t_nombre_semaforo identificador_semaforo) {

	int size=0; int controlador = 0;
	char *mensaje = mensaje_semaforo("P15",identificador_semaforo,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *respuesta = malloc(17);
	recibir(sockMemCPU,&controlador,respuesta,17);

	if(strncmp(respuesta,"OK",2) == 0){

		char * logi = string_from_format("Ejecute SIGNAL sobre semaforo %s",identificador_semaforo);
		escribir_log(logi,1);
		free(logi);

	}else if(strncmp(respuesta,"K21",2)==0){

		fifo = FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;

		char *cod_error= string_substring(respuesta,13,4);
		int codigo_error= (-1)*(atoi(cod_error));
		pcb->exit_code = codigo_error;

		char * logi = string_from_format("Ejecute SIGNAL sobre semaforo %s y resulto erroneo",identificador_semaforo);
		escribir_log(logi,2);
		free(logi);
		free(cod_error);
	}
	free(respuesta);
}

t_puntero reservar (t_valor_variable espacio){

	int size;int controlador = 0;int puntero_retorno=0;
	char *mensaje = mensaje_heap("P17",espacio,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *respuesta= malloc(13);
	recibir(sockKerCPU,&controlador,respuesta,13);

	if(strncmp(respuesta,"K99",3)==0){

		char *str_size_mensaje = string_substring(respuesta,3,10);
		int size_mensaje = atoi(str_size_mensaje);

		char *str_puntero = malloc(size_mensaje);
		recibir(sockKerCPU,&controlador,str_puntero,size_mensaje);
		puntero_retorno = atoi(str_puntero);

		char *aux_log = string_from_format("Ejecute RESERVAR heap de %d y Kernel lo aloco en el puntero %d",espacio,puntero_retorno);
		escribir_log(aux_log,1);
		free(aux_log);
		free(str_size_mensaje);
		free(str_puntero);

	}else  if(strncmp(respuesta,"K21",3)==0){

		char *str_error=string_substring(respuesta,13,4);
		int cod_error = atoi(str_error);
		pcb->exit_code = cod_error;

		fifo= FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;

		char *aux_log = string_from_format("ERROR RESERVANDO heap de %d",espacio);
		escribir_log(aux_log,2);
		free(aux_log);
		free(str_error);

	}

	free(respuesta);
	return puntero_retorno;
}

void liberar (t_puntero puntero){

	int size;int controlador;
	char *mensaje = mensaje_heap("P18",puntero,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *respuesta= malloc(13);
	recibir(sockKerCPU,&controlador,respuesta,13);

	if(strncmp(respuesta,"OK",2)==0){

		char *aux_log = string_from_format("Ejecute LIBERAR de memoria reservada en puntero %d",puntero);
		escribir_log(aux_log,1);
		free(aux_log);

	}else  if(strncmp(respuesta,"K21",3)==0){

		char *str_error=string_substring(respuesta,13,4);
		int cod_error = atoi(str_error);
		pcb->exit_code = cod_error;

		fifo= FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;

		char *aux_log = string_from_format("ERROR LIBERANDO puntero %d",puntero);
		escribir_log(aux_log,2);
		free(aux_log);
		free(str_error);

	}

	free(respuesta);
}

void escribir (t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	char * logi = string_from_format("Void * informacion:%s, tamanio:%d",(char *) informacion,tamanio);
	escribir_log(logi,1);
	free(logi);

	int size=0;int controlador=0;
	char *mensaje= mensaje_escribir_kernel(descriptor_archivo,informacion,tamanio,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char * respuesta= malloc(17);
	recibir(sockKerCPU,&controlador,respuesta,17);
	if(strncmp(respuesta,"K21",3)==0){

		fifo= FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;

		char *cod_error= string_substring(respuesta,13,4);
		int codigo_error = (-1)*(atoi(cod_error));
		pcb->exit_code = codigo_error;

		char * logi = string_from_format("ERROR:%d ESCRIBIENDO con file descriptor %d",cod_error,descriptor_archivo);
		escribir_log(logi,2);
		free(logi);
		free(cod_error);

	}else{

		char * logi = string_from_format("Ejecute ESCRIBIR con file descriptor %d",descriptor_archivo);
		escribir_log(logi,1);
		free(logi);
	}

	free(respuesta);

}

t_descriptor_archivo abrir (t_direccion_archivo direccion, t_banderas flags){

	int size; int controlador;
	char *aux_dire = strdup(direccion);
	string_trim(&aux_dire);

	char *mensaje = mensaje_abrir(aux_dire,flags,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *respuesta = malloc(13);
	char *str_tam;
	int tam;int fd;

	recibir(sockKerCPU,&controlador,respuesta,13);
	str_tam = string_substring(respuesta,3,10);
	tam = atoi(str_tam);
	char *str_fd = malloc(tam);
	recibir(sockKerCPU,&controlador,str_fd,tam);
	fd = atoi(str_fd);

	if(fd >= 0){
		char *aux_log = string_from_format("Ejecute ABRIR direccion %s y se le asoció el FD %d",direccion,fd);
		escribir_log(aux_log,1);
		free(aux_log);
	}else {
		fifo = FINALIZAR_POR_ERROR;
		accion_siguiente = FINALIZAR_POR_ERROR;

		pcb->exit_code = fd;

		char *aux_log = string_from_format("ERROR ABRIENDO direccion %s",direccion);
		escribir_log(aux_log,2);
		free(aux_log);

	}

	free(respuesta);
	free(str_tam);
	free(str_fd);

	return fd;
}

void moverCursor (t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){

	int size; int controlador;
	char *mensaje = mensaje_moverCursor(descriptor_archivo,posicion,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *aux_log = string_from_format("Ejecute CERRAR en FD %d",descriptor_archivo);
	escribir_log(aux_log,1);
	free(aux_log);
}

void leer (t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	escribir_log("Ejecute leer",1);
}

void cerrar (t_descriptor_archivo descriptor_archivo){

	int size; int controlador;
	char *mensaje = mensaje_borrar_cerrar(6,descriptor_archivo,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *aux_log = string_from_format("Ejecute CERRAR en FD %d",descriptor_archivo);
	escribir_log(aux_log,1);
	free(aux_log);
}

void borrar (t_descriptor_archivo descriptor_archivo){

	int size; int controlador;
	char *mensaje = mensaje_borrar_cerrar(7,descriptor_archivo,&size);
	enviar(sockKerCPU,mensaje,&controlador,size);
	free(mensaje);

	char *aux_log = string_from_format("Ejecute CERRAR en FD %d",descriptor_archivo);
	escribir_log(aux_log,1);
	free(aux_log);
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
		.AnSISOP_wait = ts_wait,
		.AnSISOP_signal = ts_signale,
		.AnSISOP_reservar = reservar,
		.AnSISOP_liberar =liberar,
		.AnSISOP_abrir = abrir,
		.AnSISOP_borrar = borrar,
		.AnSISOP_cerrar = cerrar,
		.AnSISOP_escribir = escribir,
		.AnSISOP_leer = leer,
		.AnSISOP_moverCursor = moverCursor,
};
