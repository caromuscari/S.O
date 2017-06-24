/*
 * fsecundarias.h
 *
 *  Created on: 11/6/2017
 *      Author: utnso
 */

#ifndef FSECUNDARIAS_H_
#define FSECUNDARIAS_H_

void armar_archivo(FILE * archivo);
char * armar_pathBloque(char *path,int bloqueSig,t_arch *archivo);
t_arch *leer_archivo(char * path);
char * armar_path(char *mensaje);
int agregar_bloque();
void modificar_archivo(char* path, int tamanio, char* bloques);
char * crear_string_bloques(char ** bloques, char * bloques_nuevos);

#endif /* FSECUNDARIAS_H_ */
