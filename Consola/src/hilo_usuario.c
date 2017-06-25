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

extern int socket_;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern pthread_t hiloMensaje;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern t_dictionary * tiempo;
extern int flag;


char* leer_archivo(char*);
void iniciar_programa(char * ruta, int socket_);
void finalizar_programa(char * pid, int socket_);
void desconectar_consola();
void cerrar_programas(char* key, void* data);
void tiempofinal_impresiones(char* pid);
void mostrar_pids(char* key, void* data);

void hilousuario ()
{
	//t_dictionary* switch_;
	//cargar_switch(switch_);
	while(flag == 0)
	{
		char *ingreso;
		char *identi;
		scanf("%ms", &ingreso);

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
			scanf("%ms",&identi);
			iniciar_programa(identi,socket_);
			free(identi);
		}
		else{
			if(!strcmp(ingreso, "finalizar_programa")){
				dictionary_iterator(p_pid,(void *)mostrar_pids);

				printf("ingresar el pid del hilo: ");
				scanf("%ms",&identi);

				finalizar_programa(identi,socket_);
				free(identi);
			}
			else{
				if(!strcmp(ingreso, "desconectar_consola")){
					desconectar_consola();
				}
				else{
					if(!strcmp(ingreso, "limpiar_consola")){
						system("clear");
					}
					else{
						escribir_log("No se reconoce el pedido");
					}
				}
			}
		}
		free(ingreso);
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
	char * mensaje_armado;
	char * mensaje;
	mensaje = leer_archivo(ruta);
	mensaje_armado= armar_mensaje("C01", mensaje);
	enviar(socket_, mensaje_armado,string_length(mensaje_armado));

	free(mensaje);
	free(mensaje_armado);

}

char * leer_archivo(char * ruta){
	FILE* archivo;
	long int final;

	archivo = fopen(ruta,"r");
	fseek( archivo, 0L, SEEK_END );
	final = ftell( archivo );
	fseek(archivo,0,0);
	char mensaje2[final];

	fread(mensaje2,sizeof(char),final,archivo);

	fclose(archivo);
	char *r = strdup(mensaje2);

	return r;
}

void finalizar_programa(char * pid, int socket_)
{
	char * mensaje;
	char* pid2;
	long int pid3;

	pid3 = atol(pid);

	if(pthread_cancel(pid3)==0)
	{
		pid2=dictionary_get(h_pid,pid);
		mensaje = armar_mensaje("C02",pid2);
		enviar(socket_, mensaje, string_length(mensaje));
		escribir_log_con_numero("Se finalizo el programa", pid3);
		tiempofinal_impresiones(pid2);

		free(dictionary_remove(h_pid,pid));
		free(dictionary_remove(p_pid,pid2));
		free(dictionary_remove(impresiones,pid2));
		free(dictionary_remove(sem,pid2));
		free(dictionary_remove(tiempo,pid2));
		free(mensaje);
	}

	else(escribir_log("No se pudo finalizar el programa"));

}

void mostrar_pids(char* key, void* data)
{
	char * pid = strdup("");
	char * hpid = strdup("");

	string_append(&pid,"PID=");
	string_append(&pid,key);
	string_append(&pid,"     ");

	string_append(&hpid,"PID HILO=");
	string_append(&hpid,data);

	escribir_log_compuesto(pid,hpid);

	free(pid);
	free(hpid);
}

void tiempofinal_impresiones(char* pid){
	time_t *tiempoFinal= malloc(sizeof(time_t));
	t_impresiones * cant;
	time_t *tiempoinicial;
	double diferencia;

	cant = dictionary_get(impresiones,pid);
	tiempoinicial = dictionary_get(tiempo,pid);

	*tiempoFinal = time(NULL);

	diferencia= difftime(*tiempoinicial, *tiempoFinal);

	escribir_log_con_numero("Inicio de ejecución : ", *tiempoinicial);
	escribir_log_con_numero("Fin de ejecución : ", *tiempoFinal);
	escribir_log_con_numero("Cantidad de impresiones: ", cant->cantidad);
	escribir_log_con_numero("Tiempo total de ejecución : ", diferencia);

	free(tiempoFinal);
}

void desconectar_consola(){
	char *mensaje;
	dictionary_iterator(h_pid,(void*)cerrar_programas);
	mensaje = armar_mensaje("C03", "");
	enviar(socket_, mensaje, string_length(mensaje));
	escribir_log("Se desconecta la consola");
	pthread_cancel(hiloMensaje);
	free(mensaje);
	flag = 1;
	pthread_exit(NULL);
}
void cerrar_programas(char* key, void* data){
	finalizar_programa(key,socket_);
}



