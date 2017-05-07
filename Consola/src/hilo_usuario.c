/*
 * hilo_usuario.c
 *
 *  Created on: 6/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <pthread.h>

extern char * identi;
extern char * ingreso;
extern int socket_;

void programa ();

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
	mensaje=malloc(final);
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
