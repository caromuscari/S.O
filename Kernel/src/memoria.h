/*
 * memoria.h
 *
 *  Created on: 1/5/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

void manejar_respuesta(char *);
void handshakearMemory();
void pedir_tamano_pag();
void reservar_memoria_din(t_program *, int);
int ubicar_bloque(t_pagina *,int, int *);
HeapMetadata* find_first_fit(t_list *hs, int t_sol);

#endif /* MEMORIA_H_ */
