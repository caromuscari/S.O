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
void forzar_finalizacion(int pid, int cid, int codigo_finalizacion);
void programas_listos_A_ejecutar();
void programas_nuevos_A_listos();
void desbloquear_proceso(int);

#endif /* PLANIFICADOR_H_ */
