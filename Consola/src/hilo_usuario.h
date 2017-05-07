/*
 * hilo_usuario.h
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#ifndef HILO_USUARIO_H_
#define HILO_USUARIO_H_


void* funcion ();
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(char * pid, int socket_);
void desconectar_consola();
void limpiar_consola();

#endif /* HILO_USUARIO_H_ */
