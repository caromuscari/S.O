/*
 ============================================================================
 Name        : Consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include "estructuras.h"
#include "socket_client.h"
#include "mensaje.h"
#include "log.h"
#include <signal.h>

t_consola arch_config;
int socket_;
pthread_t hiloUsuario;
char * identi;
char *ingreso;
t_log * log;
t_dictionary * p_pid;
t_dictionary * h_pid;


void inicializar_parametros();
void liberar_memoria();
void leer_archivo_configuracion(char * ruta);
void handshake(int socket_);
void * hilousuario ();
void terminar();

int main(int argc, char * argv[]) {


	inicializar_parametros();
	crear_archivo_log("/home/utnso/log_consola.txt");
	leer_archivo_configuracion(argv[1]);
	signal(SIGTERM,terminar);
	socket_ = iniciar_socket_cliente(arch_config.ip, arch_config.puerto);
	handshake(socket_);
	pthread_create(&hiloUsuario, NULL, (void*) hilousuario, NULL);
	pthread_join(hiloUsuario, NULL);

	liberar_memoria();
}

void terminar(){

}
void leer_archivo_configuracion(char * ruta)
{
	t_config * configConsola = config_create(ruta);
	arch_config.ip = config_get_string_value(configConsola,"IP_KERNEL");
	arch_config.puerto = config_get_string_value(configConsola,"PUERTO_KERNEL");
	escribir_log(arch_config.ip);
	escribir_log(arch_config.puerto);
	config_destroy(configConsola);

}

void handshake(int socket_){
	char * mensaje= malloc(7);
	char * mensaje_recibido= malloc(7);
	mensaje_recibido = recibir(socket_);
	if(get_header(mensaje_recibido)=="K"){
		mensaje=armar_mensaje("C00","");
		enviar(socket_,mensaje,sizeof(mensaje));
	}
	escribir_log("Se hizo el handshake");
	free(mensaje);
	free(mensaje_recibido);
}




