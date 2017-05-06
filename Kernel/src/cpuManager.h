/*
 * cpuManager.h
 *
 *  Created on: 17/4/2017
 *      Author: utnso
 */

#ifndef CPUMANAGER_H_
#define CPUMANAGER_H_

void manejo_conexiones_cpu();
void realizar_handShake_cpu(int);
void agregar_lista_cpu(int , char *);
void inicializar_listas_cpu();
void actualizar_pcb();

#endif /* CPUMANAGER_H_ */
