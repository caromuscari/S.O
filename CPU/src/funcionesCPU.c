/*
 * mensajesCPU.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include "funcionesCPU.h"
#include "socket.h"
#include "log.h"

t_dictionary* armarDiccionarioEtiquetas(void *etiquetas_serializadas){
	t_dictionary* dicc = dictionary_create();
	int n=0;
	int cantEtiquetas=0;
	memcpy(&cantEtiquetas,etiquetas_serializadas+4,2);
	int inicio= 6;
	while(n!=cantEtiquetas){
	int lengkey=0;char *key_aux;int pasar_pc=0;
	memcpy(&lengkey,etiquetas_serializadas+inicio,2);
	key_aux= string_substring(etiquetas_serializadas,inicio+2,lengkey);
	memcpy(&pasar_pc,etiquetas_serializadas+inicio+2+lengkey,2);
	dictionary_put(dicc,key_aux,(void *)pasar_pc);
	inicio = inicio+4+lengkey;
	n++;
	}
	return dicc;

}

void handshakeKernel(int socketKP){
	char *handshake = malloc(4);
	memset(handshake,'\0',4);
	int control=0;
	recibir(socketKP,&control,handshake,3);
	if (control !=0 ){
		escribir_log("error recibiendo mensaje del Kernel",2);
	}else {

		if(strcmp(handshake,"K00") == 0){
			escribir_log("mensaje de conexion con Kernel recibido",1);
			memcpy(handshake,"P00",3);
			enviar(socketKP,handshake,&control,3);
			if(control != 0){
				escribir_log("error enviando mensaje al Kernel",2);
			}
			escribir_log("handshake Kernel realizado exitosamente",1);
		}
	}
	free (handshake);

}
int handshakeMemoria(int socketMP){
	char *handshake = malloc(7);
	memset(handshake,'\0',7);
	int control=0; int tamanopag=0;
	memcpy(handshake,"P00",3);
	enviar(socketMP,handshake,&control,3);

	recibir(socketMP,&control,handshake,7);
	if (control !=0 ){
		escribir_log("error recibiendo mensaje de la Memoria",2);
	}else {

		if(strncmp(handshake,"M00",3) == 0){
			escribir_log("mensaje de conexion con Memoria recibido",1);
			if(control != 0){
				escribir_log("error enviando mensaje al Memoria",2);
			}
			escribir_log("handshake Memoria realizado exitosamente",1);
			char *tampag = malloc(4);
			memcpy(tampag,handshake+3,4);
			tamanopag = atoi(tampag); free(tampag);

		}
	}
	free (handshake);
	return tamanopag;

}
