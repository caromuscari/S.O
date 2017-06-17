/*
 * mensajesCPU.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef SRC_FUNCIONESCPU_
#define SRC_FUNCIONESCPU_
#include <commons/collections/dictionary.h>
#include "estructuras.h"

int handshakeKernel(int sok);
int handshakeMemoria(int );
t_dictionary* armarDiccionarioEtiquetas(char *etiquetas_serializadas);
char* serializarPCB_CPUKer (t_PCB_CPU* pcb,int *);
t_PCB_CPU* deserializarPCB_KerCPU (char* );
int calcular_offset(t_list* args,t_list* vars);
int buscar_offset_variable(t_list* vars,char id);
void stack_destroy(t_stack_element *self);
void t_memoria_destroy(t_memoria *self);


#endif /* SRC_FUNCIONESCPU_ */
