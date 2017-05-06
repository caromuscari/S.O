/*
 * estructuras.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

//Estructura definida para el modulo de memoria
typedef struct{
	char *IP;
	char *PUERTO;
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	int REEMPLAZO_CACHE;
	int RETARDO_MEMORIA;
	int SOCKET;
}t_memoria;

#endif /* ESTRUCTURAS_H_ */
