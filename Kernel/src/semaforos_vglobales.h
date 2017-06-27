/*
 * semaforos.h
 *
 *  Created on: 27/5/2017
 *      Author: utnso
 */

#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

void inicializar_sems();
void sem_wait(int, char *);
void sem_signal(int, char *);
int lock_vglobal(t_vglobal *vg, int prog);
void set_vglobal(char *vglobal, int num, int prog);
void unlock_vglobal(t_vglobal *vg);

#endif /* SEMAFOROS_H_ */
