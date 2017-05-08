/*
 * hilo_usuario.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>
#include "socket_client.h"
#include "mensaje.h"
#include <commons/string.h>
#include "log.h"
#include <commons/log.h>

extern char * identi;
extern char * ingreso;
extern int socket_;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;

void programa ();
//void leer_archivo();
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(pthread_t pid, int socket_);
void desconectar_consola();
void limpiar_consola();

void* hilousuario ()
{
	while(1){
		scanf("%s",ingreso);

		if(ingreso == "iniciar_programa"){
			printf("ingresar la ruta del programa: ");
			scanf("%s",identi);
			iniciar_programa(identi,socket_);
		}
		else{
			if(ingreso == "finalizar_programa"){
				printf("ingresar el PID del programa: ");
				scanf("%s",identi);
				finalizar_programa(atol(identi),socket_);
			}
			else{
				if(ingreso == "desconectar_consola"){
					desconectar_consola();
				}
				else{
					if(ingreso == "limpiar_consola"){
						limpiar_consola();
					}
				}
			}
		}

	}
}

void iniciar_programa(char * ruta, int socket_){
	FILE* archivo;
	long int final;
	char * mensaje;
	char * mensaje_armado=malloc(sizeof * mensaje_armado);
	char * mensaje_recibido=malloc(sizeof * mensaje_recibido);
	char * identificador=malloc(3);
	char * mensaje2;
	int pid;
	pthread_t hiloPrograma;
	archivo = fopen(ruta,"r");
	fseek( archivo, 0L, SEEK_END );
	final = ftell( archivo );
	fseek(archivo,0,0);
	mensaje=string_new();
	mensaje2=string_new();
	while(!feof(archivo)){
		fgets(mensaje,final,archivo);
		string_append(&mensaje2,mensaje);
	}
	mensaje_armado= armar_mensaje("C01", mensaje2);
	enviar(socket_, mensaje_armado,sizeof (mensaje_armado));
	mensaje_recibido = recibir(socket_);
	identificador = get_header(mensaje_recibido);
	string_append(identificador, get_codigo(mensaje_recibido));
	if (identificador == "K04"){
		pthread_create(&hiloPrograma, NULL, (void*) programa, NULL);
		escribir_log("Se inicio el programa");
		pid= atoi(get_mensaje(mensaje_recibido));
		dictionary_put(p_pid,pid,hiloPrograma);
		dictionary_put(h_pid,hiloPrograma,pid);
	}
	else {
		if(identificador == "K05"){
			printf("no se pudo iniciar el programa");
			escribir_log("No se pudo iniciar el programa");
		}
	}
	fclose(archivo);
	free(mensaje);
	free(mensaje2);
	free(mensaje_armado);
	free(mensaje_recibido);
	free(identificador);
	free(pid);

}

//void leer_archivo(){

//}

void finalizar_programa(pthread_t pid, int socket_){

}

void desconectar_consola(){

}

void limpiar_consola(){

}
