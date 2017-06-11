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
char* serializarPCB_CPUKer (t_PCB_CPU pcb);
t_PCB_CPU deserializarPCB_KerCPU (char* );


#endif /* SRC_FUNCIONESCPU_ */
