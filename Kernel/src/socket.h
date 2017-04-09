#ifndef KERNEL_PRUEBAS_SRC_SOCKET_CLIENT_H_
#define KERNEL_PRUEBAS_SRC_SOCKET_CLIENT_H_

//Esta funcion genera un socket de tipo cliente (no puede aceptar conexiones, solo se conecta)
int iniciar_socket_cliente(char *ip, int puerto/*char *puerto*/);

//Esta funcion envia mensajes a traves del socket conectado
//Devuelve un entero significativo de que si el mensaje fue enviado o no
int enviar_cl(int socket_emisor, char *mensaje_a_enviar);

//Esta funcion recibe mensajes a traves de un socket
char *recibir_cl(int socket_receptor);

int iniciar_socket_server(char *ip, char *puerto);

int escuchar_conexiones(int socketServidor);

void cerrar_conexion(int socket_);

int iniciar_socket_server(char *ip, char *puerto);

int enviar(int socket_emisor, char *mensaje_a_enviar, int *control);

char *recibir(int socket_receptor, int *controlador);

#endif /* KERNEL_PRUEBAS_SRC_SOCKET_CLIENT_H_ */
