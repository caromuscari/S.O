/*
 * sockets_server.h
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_SERVER_H_
#define SOCKETS_SERVER_H_


int iniciar_socket_server(char *ip, int puerto);
int escuchar_conexiones(int socketServidor);
int enviar(int socket_emisor, void *mensaje_a_enviar, int tamanio);
int recibir(int socket_receptor, void *buffer,int tamanio);

#endif /* SOCKETS_SERVER_H_ */
