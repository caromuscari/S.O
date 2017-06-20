/*
 * estructuras.h
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

typedef struct {
	char * ip;
	int puerto;
}t_consola;

typedef struct{
	int valor;
}t_chequeo;

typedef struct{
	int cantidad;
}t_impresiones;

typedef struct{
	pthread_t hilo;
}t_hilo;



#endif /* ESTRUCTURAS_H_ */
