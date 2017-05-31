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
#include "socket.h"
#include "log.h"


t_puntero definirVariable(t_nombre_variable identificador_variable) {
	escribir_log("Ejecute definirVariable",1);
	return 0;

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	escribir_log("Ejecute obtenerPosicionVariable",1);
	return 0;
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
