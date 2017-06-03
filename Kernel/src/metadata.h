/*
 * metadata.h
 *
 *  Created on: 1/6/2017
 *      Author: utnso
 */

#ifndef METADATA_H_
#define METADATA_H_

t_sentencia* armarIndiceCodigo (char *codigoPrograma);
void* armarIndiceEtiquetas(char *codigoPrograma);
t_list* armarIndiceStack (char *codigoPrograma);
void* serializarPCB(t_PCB pcb);

#endif /* METADATA_H_ */
