/*
 * mensajes.h
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#ifndef SRC_MENSAJES_H_
#define SRC_MENSAJES_H_

#include <parser/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionesCPU.h"

char *mensaje_escibir_memoria(int fpid,t_puntero direccion_variable,int cant_pag,int valor);
char *mensaje_semaforo(char * cod,char * semaforo,int *);
char *mensaje_leer_memoria(int fpid,t_puntero direccion_variable, int cant_pag,int *size);
char *mensaje_pcb(char *identificador, char *mensaje,int sizepcb);
char *mensaje_escribir_kernel(int fd,void *informacion,int tamanio,int *size);
char *mensaje_variable_kernel(int codigo,char *variable,int valor,int *size);
char *mensaje_heap(char *cod,int valor, int *size);
char *mensaje_borrar_cerrar(int cod,int fd,int *size);
char *mensaje_moverCursor(int fd,int posicion,int *size);
char *mensaje_abrir(char *direccion,t_banderas flags,int *size);

#endif /* SRC_MENSAJES_H_ */
