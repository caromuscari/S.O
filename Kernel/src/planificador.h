/*
 * planificador.h
 *
 *  Created on: 7/5/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

//Agrega a la cola un nuevo programa para ser atendido
void agregar_nueva_prog(int id_consola, int pid, char *mensaje);
void forzar_finalizacion(int pid);
void forzar_finalizacion_consola(int consola_id);

#endif /* PLANIFICADOR_H_ */
