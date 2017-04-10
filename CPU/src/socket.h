/*
 * socket.h
 *
 *  Created on: 10/10/2016
 *      Author: utnso
 */

#include <stdint.h>
#ifndef SOCKET_H_
#define SOCKET_H_
typedef struct t_DATA{
		int tipoOper ; // TipoDeOperacion FUSE
		char *path;
		char * data_info;
		uint32_t offset;
	} t_DATA;

//Esta funcion genera un socket de tipo cliente (no puede aceptar conexiones, solo se conecta)
int iniciar_socket_cliente(char *ip, char *puerto);
//Esta funcion envia mensajes a traves del socket conectado
//Devuelve un entero significativo de que si el mensaje fue enviado o no
int enviar(int socket_emisor, char mensaje_a_enviar[1024]);

//Esta funcion recibe mensajes a traves de un socket
char *recibir(int socket_receptor);
//Esta funcion envia un STRUCT a traves del socket conectado
//Devuelve un entero significativo de que si el mensaje fue enviado o no
int enviarDATA(int socket_emisor, t_DATA *DATA_a_enviar);

//Esta funcion recibe un STRUCT a traves de un socket
t_DATA *recibirDATA(int socket_receptor);


#endif /* SOCKET_H_ */
