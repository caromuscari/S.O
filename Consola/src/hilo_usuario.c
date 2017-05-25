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
char* leer_archivo(char*);
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(pthread_t pid, int socket_);
void desconectar_consola();
void cerrar_programas(char* key, void* data);
//void limpiar_consola();

void* hilousuario ()
{
	//t_dictionary* switch_;
	//cargar_switch(switch_);
	while(1){
		scanf("%s",ingreso);

		/*int data=dictionary_get(switch_,ingreso);
		  switch(data){
		  	case 1:
		  		printf("ingresar la ruta del programa: ");
				scanf("%s",identi);
				iniciar_programa(identi,socket_);
				break;
			case 2:
				printf("ingresar el PID del programa: ");
				scanf("%s",identi);
				finalizar_programa(atol(identi),socket_);
				break;
			case 3:
				desconectar_consola();
				break;
			case 4:
				system("clear");
				break;
			}
		 */

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
						//limpiar_consola();
						system("clear");
					}
				}
			}
		}

	}
}

/*void cargar_switch(t_dictionary * switch_){
	switch_=dictionary_create();
	dictionary_put(switch_,"iniciar_programa",1);
	dictionary_put(switch_,"finalizar_programa",2);
	dictionary_put(switch_,"desconectar_consola",3);
	dictionary_put(switch_,"limpiar_consola",4);
}*/

void iniciar_programa(char * ruta, int socket_){
	int pid;
	pthread_t hiloPrograma;
	char * identificador=malloc(3);
	char * mensaje_armado=malloc(sizeof * mensaje_armado);
	char * mensaje_recibido=malloc(sizeof * mensaje_recibido);
	char * mensaje;
	mensaje = leer_archivo(ruta);
	mensaje_armado= armar_mensaje("C01", mensaje);
	enviar(socket_, mensaje_armado,sizeof (mensaje_armado));
	mensaje_recibido = recibir(socket_);
	identificador = get_header(mensaje_recibido);
	string_append(identificador, string_itoa(get_codigo(mensaje_recibido)));
	if (identificador == "K04"){
		pthread_create(&hiloPrograma, NULL, (void*) programa, NULL);
		escribir_log("Se inicio el programa");
		pid= atoi(get_mensaje(mensaje_recibido));
		dictionary_put(p_pid,pid,hiloPrograma);
		dictionary_put(h_pid,hiloPrograma,pid);
	}
	else {
		if(identificador == "K05"){
			printf("%s","no se pudo iniciar el programa");
			escribir_log("No se pudo iniciar el programa");
		}
	}
	free(mensaje);
	free(mensaje_armado);
	free(mensaje_recibido);
	free(identificador);

}

char * leer_archivo(char * ruta){
	FILE* archivo;
	long int final;
	char * mensaje;
	char * mensaje2;
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
	free(mensaje);
	free(mensaje2);
	fclose(archivo);
	return mensaje2;
}

void finalizar_programa(pthread_t pid, int socket_){
	char * mensaje = armar_mensaje("C02",pid);
	int pid2;
	enviar(socket_, mensaje, sizeof(mensaje));
	if(pthread_cancel(pid)==0){
		escribir_log("Se finalizo el programa");
		pid2=dictionary_get(h_pid,pid);
		dictionary_remove(h_pid,pid);
		dictionary_remove(p_pid,pid2);
	}
	else(escribir_log("No se pudo finalizar el programa"));

}

void desconectar_consola(){
	//while(!dictionary_is_empty(h_pid)){
	dictionary_iterator(h_pid,cerrar_programas);
	//}
	escribir_log("Se desconecta la consola");
	//pthread_exit();

}
void cerrar_programas(char* key, void* data){
	finalizar_programa(data,socket_);
}

/*void limpiar_consola(){

}*/
