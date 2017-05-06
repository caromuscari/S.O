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


t_consola arch_config;
int socket_;
//char* inicio_ejecucion;
pthread_t hiloUsuario;
pthread_t hiloEscucha;
char * identi;
char * ingreso;
t_log *log;
t_dictionary * p_pid;
t_dictionary * h_pid;


void inicializar_parametros();
void liberar_memoria();
void leer_archivo_configuracion(char * ruta);
void handshake(int socket_);
void * escucha_mensaje();
void * funcion ();
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(char * pid, int socket_);
void desconectar_consola();
void limpiar_consola();
void programa ();

int main(int argc, char * argv[]) {


	inicializar_parametros();
	crear_log("/home/utnso/log_consola.txt");
	leer_archivo_configuracion(argv[1]);
	socket_ = iniciar_socket_cliente(arch_config.ip, arch_config.puerto);
	handshake(socket_);
	pthread_create(&hiloUsuario, NULL, (void*) funcion, NULL);
	pthread_create(&hiloEscucha, NULL, (void*) escucha_mensaje, NULL);
	pthread_join(hiloEscucha, NULL);
	pthread_join(hiloUsuario, NULL);

	liberar_memoria();
}

void crear_log(char* file)
{
	log = log_create(file,"CONSOLA",true, LOG_LEVEL_INFO);
	log_info(log, "Se crea el archivo de log");
}

void leer_archivo_configuracion(char * ruta)
{
	t_config * configConsola = config_create(ruta);
	arch_config.ip = config_get_string_value(configConsola,"IP_KERNEL");
	arch_config.puerto = config_get_string_value(configConsola,"PUERTO_KERNEL");
	printf(arch_config.ip);
	printf(arch_config.puerto);
	config_destroy(configConsola);

}

void handshake(int socket_){
	char * mensaje= malloc(7);
	char * mensaje_recibido= malloc(7);
	mensaje_recibido = recibir(socket_);
	mensaje=armar_mensaje("C00","");
	enviar(socket_,mensaje);
	free(mensaje);
	free(mensaje_recibido);
}
void* funcion ()
{
	while(1){
		scanf("%d",ingreso);
		identi=string_split(ingreso," ");

		if(identi[0] == "iniciar_programa"){
			iniciar_programa(identi[1],socket_);
		}
		else{
			if(identi[0] == "finalizar_programa"){
				finalizar_programa(identi[1],socket_);
			}
			else{
				if(identi[0] == "desconectar_consola"){
					desconectar_consola();
				}
				else{
					if(identi[0] == "limpiar_consola"){
						limpiar_consola();
					}
				}
			}
		}
		/*switch(*identi){
		case 1:
			iniciar_programa();
			break;
		case 2:
			finalizar_programa();
			break;
		case 3:
			desconectar_consola();
			break;
		case 4:
			limpiar_consola();
			break;*/

	}
}

void iniciar_programa(char * ruta, int socket_){
	FILE* archivo;
	long int final;
	char * mensaje;
	char * mensaje_armado;
	char * mensaje_recibido;
	char * identificador;
	pthread_t hiloPrograma;
	archivo = fopen(ruta,"r");
	fseek( archivo, 0L, SEEK_END );
	final = ftell( archivo );
	memcpy(mensaje, archivo, final);
	//printf(mensaje);
	mensaje_armado= armar_mensaje("C01", mensaje);
	enviar(socket_, mensaje_armado);
	mensaje_recibido = recibir(socket_);
	identificador = get_header(mensaje_recibido);
	if (identificador == "K04"){
		pthread_create(&hiloPrograma, NULL, (void*) programa, NULL);
	}
	else printf("no se pudo iniciar el programa");

}

void finalizar_programa(char * pid, int socket_){

}

void desconectar_consola(){

}

void limpiar_consola(){

}
void programa (){

}
void * escucha_mensaje(){
	while(1){

	}
}

