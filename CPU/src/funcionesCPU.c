/*
 * mensajesCPU.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"

/*
t_puntero definirVariable(t_nombre_variable identificador_variable) {

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {

}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
}
void irAlLabel(t_nombre_etiqueta etiqueta) {
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
}

void finalizar(void) {
}

void retornar(t_valor_variable retorno) {
}

void wait(t_nombre_semaforo identificador_semaforo) {
}

void signale(t_nombre_semaforo identificador_semaforo) {
}

AnSISOP_funciones funciones = {
.AnSISOP_definirVariable = definirVariable,
.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
.AnSISOP_dereferenciar = dereferenciar,
.AnSISOP_asignar = asignar,
.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
.AnSISOP_asignarValorCompartida = asignarValorCompartida,
.AnSISOP_irAlLabel = irAlLabel,
.AnSISOP_llamarConRetorno =llamarConRetorno,
.AnSISOP_retornar = retornar,
.AnSISOP_finalizar = finalizar,};

AnSISOP_kernel funcionesKernel = {
		.AnSISOP_wait = wait,
		.AnSISOP_signal =signale, };
*/
void handshakeKernel(int socketKP){
	void *handshake = malloc(3);
	memcpy(handshake,"P00",3);
	int control=0;
	enviar(socketKP,handshake,&control);
}
