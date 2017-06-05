/*
 * metadata.h
 *
 *  Created on: 1/6/2017
 *      Author: utnso
 */

#ifndef METADATA_H_
#define METADATA_H_

typedef struct
{
	int offset_inicio;
	int offset_fin;
}t_sentencia;

typedef struct
{
	int PID;
	int PC;
	int cant_pag;
	int SP;
	t_sentencia* in_cod;
	char* in_et;
	t_list* in_stack;  //lista de t_stack_element
	int exit_code;
}t_PCB;
typedef struct
{
	char ID;
	int pag;
	int offset;
	int size;
}__attribute__((__packed__))t_memoria;


t_sentencia* armarIndiceCodigo (char *codigoPrograma);
char* armarIndiceEtiquetas(char *codigoPrograma);
t_list* armarIndiceStack(char *codigoPrograma);
char* serializarPCB_KerCPU (t_PCB,char *,int,int );
t_PCB deserializarPCB_CPUKer (char* );

#endif /* METADATA_H_ */
