/*
 * hilo_usuario.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include "socket_client.h"
#include "mensaje.h"
#include <commons/string.h>
#include "log.h"
#include <commons/log.h>
#include "estructuras.h"

extern char * identi;
extern char * ingreso;
extern int socket_;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern pthread_t hiloMensaje;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern t_dictionary * tiempo;


char* leer_archivo(char*);
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(pthread_t pid, int socket_);
void desconectar_consola();
void cerrar_programas(char* key, void* data);
void tiempofinal_impresiones(long int pid);
//void limpiar_consola();

void* hilousuario ()
{
	//t_dictionary* switch_;
	//cargar_switch(switch_);
	while(1){
		scanf("%s",ingreso);
		//fgets(ingreso,20,stdin);

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
			default:

			}
		 */

		if(!strcmp(ingreso, "iniciar_programa")){
			printf("ingresar la ruta del programa: ");
			scanf("%s",identi);
			iniciar_programa(identi,socket_);
		}
		else{
			if(!strcmp(ingreso, "finalizar_programa")){
				printf("ingresar el PID del programa: ");
				scanf("%s",identi);
				finalizar_programa(atol(identi),socket_);
			}
			else{
				if(!strcmp(ingreso, "desconectar_consola")){
					desconectar_consola();
				}
				else{
					if(!strcmp(ingreso, "limpiar_consola")){
						//limpiar_consola();
						system("clear");
					}
					else{
						escribir_log("No se reconoce el pedido");
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
	char * mensaje_armado=strdup("");
	char * mensaje=strdup("");
	mensaje = leer_archivo(ruta);
	mensaje_armado= armar_mensaje("C01", mensaje);
	enviar(socket_, mensaje_armado,string_length(mensaje_armado));

	free(mensaje);
	free(mensaje_armado);

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
	fclose(archivo);
	return mensaje2;
}

void finalizar_programa(pthread_t pid, int socket_){
	char * mensaje = strdup("");
	long int pid2;
	char* pid3=string_itoa(pid);
	mensaje = armar_mensaje("C02",pid3);
	enviar(socket_, mensaje, sizeof(mensaje));
	if(pthread_cancel(pid)==0){
		escribir_log_con_numero("Se finalizo el programa", pid);
		tiempofinal_impresiones(pid);
		pid2=dictionary_get(h_pid,pid);
		dictionary_remove(h_pid,pid);
		dictionary_remove(p_pid,pid2);
		dictionary_remove(impresiones,pid);
		free(dictionary_remove(sem,pid));
		free(dictionary_remove(tiempo,pid));
	}
	else(escribir_log("No se pudo finalizar el programa"));
	free(mensaje);

}

void desconectar_consola(){
	void * valor;
	char *mensaje = strdup("");
	dictionary_iterator(h_pid,cerrar_programas);
	mensaje = armar_mensaje("C03", "");
	enviar(socket_, mensaje, string_length(mensaje));
	escribir_log("Se desconecta la consola");
	pthread_cancel(hiloMensaje);
	pthread_exit(valor);
}
void cerrar_programas(char* key, void* data){
	finalizar_programa(data,socket_);
}

void tiempofinal_impresiones(long int pid){
	time_t tiempoFinal= malloc(sizeof(time_t));
	t_impresiones * cant = malloc(sizeof(t_impresiones));
	time_t tiempoinicial = malloc(sizeof(time_t));
	double diferencia;
	tiempoFinal = time(NULL);
	cant = dictionary_get(impresiones,pid);
	tiempoinicial = dictionary_get(tiempo,pid);
	diferencia= difftime(tiempoinicial, tiempoFinal);
	escribir_log_con_numero("Inicio de ejecución : ", tiempoinicial);
	escribir_log_con_numero("Fin de ejecución : ", tiempoFinal);
	escribir_log_con_numero("Cantidad de impresiones: ", cant->cantidad);
	escribir_log_con_numero("Tiempo total de ejecución : ", diferencia);
	//printf("Inicio de ejecución : %i /n", tiempoinicial);
	//printf("Fin de ejecución : %i /n", tiempoFinal);
	//printf("El programa realizó %i impresiones /n", cant->cantidad);
	//printf("El tiempo total de ejecución fue %lf /n", diferencia);
	free(cant);
	free(tiempoinicial);
	free(tiempoFinal);
}

/*void limpiar_consola(){

}*/
