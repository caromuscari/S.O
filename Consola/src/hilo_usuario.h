/*
 * hilo_usuario.h
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#ifndef HILO_USUARIO_H_
#define HILO_USUARIO_H_

void *hilousuario ();
char *leer_archivo(char*);
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(char* pid, int socket_);
void desconectar_consola();
void cerrar_programas(char* key, void* data);
void tiempofinal_impresiones(char* pid);

#endif /* HILO_USUARIO_H_ */
