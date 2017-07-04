/*
 * semaforos.h
 *
 *  Created on: 27/5/2017
 *      Author: utnso
 */

#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

void inicializar_sems();
int lock_vglobal(t_vglobal *vg, int prog);
void set_vglobal(char *vglobal, int num, int prog);
void unlock_vglobal(t_vglobal *vg);
void sem_signal(t_program*, char *);
void sem_wait(t_program *, char *);

#endif /* SEMAFOROS_H_ */
