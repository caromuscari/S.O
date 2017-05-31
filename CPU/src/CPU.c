

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"
#include "log.h"
#include "funcionesCPU.h"
#include "funcionesParser.h"
#include <parser/metadata_program.h>
#include <commons/config.h>
#include "cosas.h"

int puertoK,puertoM;
char *ipK;
char *ipM;
int sockKerCPU;
int tam_paginas_memoria;
static const char* facil_ansisop =
		"begin\n"
		"variables a, b\n"
		"a = 3\n"
		"b = 5\n"
		"a = b + 12\n"
		"end\n"
		"\n";
static const char* otro_ansisop =
		"#!/usr/bin/ansisop\n"
		"begin\n"
		"# primero declaro las variables\n"
		"variables a, b\n"
		"a = 20\n"
		"print a\n"
		"end\n"
		"\n";
static const char* con_funcion_ansisop =
		"#!/usr/bin/ansisop\n"
		"begin variables a,g\n"
		"a = 1\n"
		"g <- doble a\n"
		"prints g\n"
		"variables i,b\n"
		"i = 1\n"
		":inicio_for\n"
		"i = i + 1\n"
		"jnz i inicio_for\n"
		"#fuera del for\n"
		"end\n"
		"function doble\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end\n"
		"\n";


void leerArchivoConfiguracion(char* argv);
void conexion_Kernel(int puertoK, char* ipK);
void conexion_Memoria(int puertoM,char* ipM);
int main(int argc, char *argv[])
{
	//char *programa =strdup(facil_ansisop);
	//char *programa =strdup(otro_ansisop);
	char *programa =strdup(con_funcion_ansisop);
	crear_archivo_log("/home/utnso/CPUlog");
	leerArchivoConfiguracion(argv[1]);
	conexion_Kernel(puertoK, ipK);
	//	conectarse con memoria
	conexion_Memoria(puertoM,ipM);
	int controlador = 0;
/*	int chau = 0;
	while(chau!=1){
		void *buff = malloc (7);
		char *idmensaje = malloc(2);
		char *sizemensaje= malloc (4); int largomensaje;
		escribir_log("Esperando mensajes del Kernel para ponerme a trabajar...",1);
		recibir(sockKerCPU,&controlador,buff,7);
		if(controlador!= 0){
			escribir_log("error recibiendo mensaje del Kernel, bai",2);
		}
		printf("recibi:%s",buff);
		memcpy(idmensaje,buff+1,2);

		switch (atoi(idmensaje)){
		case 7:
			memcpy(sizemensaje,buff+3,4); largomensaje= atoi(sizemensaje);
			free(sizemensaje);
			char *mensajeEntero = malloc(largomensaje);
			recibir(sockKerCPU,&controlador,mensajeEntero,largomensaje);
			//t_pcb pcb = deserializar_pcb(mensajeEntero)
			//procesar(pcb)

		}


	}
*/
	int pc = 0;
	t_metadata_program *metadata= metadata_desde_literal(programa);

	while(pc < metadata->instrucciones_size){
		char *linea = string_substring(programa,metadata->instrucciones_serializado[pc].start,metadata->instrucciones_serializado[pc].offset);
		analizadorLinea(linea,&funcionesTodaviaSirve,&funcionesKernelTodaviaSirve);
		pc ++;
		free(linea);
	}


	t_sentencia* sentencias =  armarIndiceCodigo(programa);

	int c;
	for(c=0;sentencias[c].offset_fin != -1 && sentencias[c].offset_inicio != -1; c++){
		printf("sentencias numero %d\n",c);
		char *linea=string_substring(programa,sentencias[c].offset_inicio,sentencias[c].offset_fin);
		printf("%s\n",linea);
		free(linea);

	}



	printf("Cantidad de etiqueta:%d\n",metadata->cantidad_de_etiquetas);
	printf("Cantidad de funciones:%d\n",metadata->cantidad_de_funciones);
	//int m=0;
	//while(m < metadata->cantidad_de_etiquetas + metadata->cantidad_de_funciones){
		int largo1et = strlen(metadata->etiquetas);
		char *etiqueta1 = strdup(metadata->etiquetas);
		int largo2et = strlen((metadata->etiquetas)+largo1et+5);
		char *etiqueta2 = strdup(metadata->etiquetas+5+largo1et);
	//}
		printf("1erET:%d%s,2daET:%d%s\n",largo1et,etiqueta1,largo2et,etiqueta2);
	printf("Etiquetas:%s\n",metadata->etiquetas);
	printf("Etiquetas size:%d\n",metadata->etiquetas_size);

	printf("size_t de un pcb :%d\n",sizeof(t_PCB));
	printf("size_t de un pcb :%d\n",sizeof(int));

	int k= metadata_buscar_etiqueta("doble",metadata->etiquetas,metadata->etiquetas_size);
	printf("buscaretiqueta DOBLE tpunteroinstruccion :%d\n",k);
	k= metadata_buscar_etiqueta("inicio_for",metadata->etiquetas,metadata->etiquetas_size);
		printf("buscaretiqueta INICIO_FOR tpunteroinstruccion :%d\n",k);

	char *linea = string_substring(programa,sentencias[k].offset_inicio,sentencias[k].offset_fin);
	printf("instruccion con el pc dado:%s\n",linea);
	free(linea);

	printf("Instruccion inicio:%d\n",metadata->instruccion_inicio);
	printf("Instrucciones serializado/t_instrucciones.offset:%d\n",metadata->instrucciones_serializado->offset);
	printf("Instrucciones serializado/t_instrucciones.start:%d\n",metadata->instrucciones_serializado->start);
	printf("Instrucciones:%d\n",metadata->instrucciones_size);

	int n=0;
	while(n < metadata->instrucciones_size){
		char *linea = string_substring(programa,metadata->instrucciones_serializado[n].start,metadata->instrucciones_serializado[n].offset);
		printf("primer instruccion:%s\n",linea);
		n++;
		free(linea);

	}
	printf("size sentencias%d\n",sizeof sentencias[0]);
	printf("size t_memoria%d\n",sizeof (t_memoria));
	printf("size t_stack_element%d\n",sizeof (t_stack_element));

	n=0;
	while(sentencias[n].offset_inicio != -1 &&sentencias[n].offset_fin != -1 ){
		n++;
	}
	printf("%d",sentencias[n].offset_fin);
	printf("Espero 12 y tengo:%d\n",n);
	printf("Espero 8 y es :%d\n",sizeof (t_etiqueta));
	char* et =armarIndiceEtiquetas(programa);
	printf("etiqueta1 :%s\n",string_substring(et,8,10));
	printf("etiqueta2:%s\n",string_substring(et,22,9));
	t_dictionary *dic = armarDiccionarioEtiquetas(et);
	printf("espero size 2 y es size:%d\n",dictionary_size(dic));
	printf("con doble me tira un pc de:%d y espero 8\n",dictionary_get(dic,"doble"));
	printf("con inicio_for me tira un pc de:%d y espero 5\n",dictionary_get(dic,"inicio_for"));
	free(et);


	free(sentencias);
free(programa);
metadata_destruir(metadata);
free(ipK);free(ipM);


}

void leerArchivoConfiguracion(char* argv)
{

	t_config *configuracion;
	configuracion = config_create(argv);
	ipK = malloc(10);
	ipM = malloc(10);
	if(config_has_property(configuracion,"PUERTO_KERNEL")&&
			config_has_property(configuracion,"PUERTO_MEMORIA")&&
			config_has_property(configuracion,"IP_KERNEL")&&
			config_has_property(configuracion,"IP_MEMORIA")){
	puertoK = config_get_int_value(configuracion, "PUERTO_KERNEL");
	puertoM = config_get_int_value(configuracion, "PUERTO_MEMORIA");
	strcpy(ipK, config_get_string_value(configuracion, "IP_KERNEL"));
	strcpy(ipM, config_get_string_value(configuracion, "IP_MEMORIA"));

	escribir_log(string_from_format("archivo de configiguracion leido \n PUERTO_KERNEL:%d \n PUERTO_MEMORIA:%d \n IP_KERNEL:%s \n IP_MEMORIA:%s",puertoK,puertoM,ipK,ipM),1);
	}else {
		escribir_log("archivo de configiguracion incorrecto",2);
	}
	config_destroy(configuracion);
}

void conexion_Kernel(int puertoK, char* ipK) {
	int controladorConexion = 0;
	sockKerCPU = iniciar_socket_cliente(ipK, puertoK, &controladorConexion);
	if (controladorConexion == 0) {
		escribir_log("Exitos conectandose al Kernel", 1);
	} else {
		escribir_log(string_itoa(controladorConexion),2);
	}
	handshakeKernel(sockKerCPU);
}
void conexion_Memoria(int puerto, char* ip) {
	int controladorConexion = 0;
	sockKerCPU = iniciar_socket_cliente(ip, puerto, &controladorConexion);
	if (controladorConexion == 0) {
		escribir_log("Exitos conectandose al Kernel", 1);
	} else {
		escribir_log(string_itoa(controladorConexion),2);
	}
	tam_paginas_memoria = handshakeMemoria(sockKerCPU);
}
