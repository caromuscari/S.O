/*
 * socket.h
 *
 *  Created on: 10/10/2016
 *      Author: utnso
 */

#ifndef SOCKET_H_
#define SOCKET_H_


//Esta funcion genera un entero que representa el socket server
int iniciar_socket_server(char *ip, char *puerto);

//Esta funcion devuelve un entero del socket correspondiente al cliente aceptado
//Se puede usar en un while ya que es una funcion bloqueante
int escuchar_conexiones(int socketServidor);

//Esta funcion envia mensajes a traves del socket conectado
//Devuelve un entero significativo de que si el mensaje fue enviado o no
int enviar(int socket_emisor, char *mensaje_a_enviar, int *control);

//Esta funcion recibe mensajes a traves de un socket
char *recibir(int socket_receptor, int *controlador);

//Esta funcion elimina el socket
void cerrar_conexion(int socket_);

#endif /* SOCKET_H_ */
