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

#endif /* SEMAFOROS_H_ */
