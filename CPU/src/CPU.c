

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <parser/metadata_program.h>
#include "log.h"
#include "socket.h"
#include "funcionesCPU.h"
#include "funcionesParser.h"
#define ROUNDROBIN "RR"
#define FIFO "FF"

int puertoK,puertoM;
char *ipK;
char *ipM;
int sockKerCPU;
int sockMemCPU;
int tam_pagina_memoria;
t_PCB_CPU* pcb;
char* programa;
static const char* bobo_ansisop =
		"begin\n"
		"variables a\n"
		"a = 2\n"
		"end\n"
		"\n";
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
int conexion_Kernel(int puertoK, char* ipK);
void conexion_Memoria(int puertoM,char* ipM);
void procesar();
char* pedir_linea_memoria();

int main(int argc, char *argv[])
{
	int chau = 0;
	//char *programa =strdup(facil_ansisop);
	//char *programa =strdup(otro_ansisop);
	//programa =strdup(con_funcion_ansisop);
	programa = strdup(bobo_ansisop);
	crear_archivo_log("/home/utnso/CPUlog");
	leerArchivoConfiguracion(argv[1]);
	int res = conexion_Kernel(puertoK, ipK);
	if(res != 0){
		cerrar_conexion(sockKerCPU);
		printf("RES ES:%d\n",res);
		chau=1;
	}
	//	conectarse con memoria
	//conexion_Memoria(puertoM,ipM);
	int controlador = 0;
	while(chau!=1){
		char *buff = malloc (13);
		char *idmensaje = malloc(2);
		char *sizemensaje= malloc (10);
		int largomensaje  = 0;
		escribir_log("Esperando mensajes del Kernel para ponerme a trabajar...",1);
		recibir(sockKerCPU,&controlador,buff,13);
		if(controlador != 0){
			escribir_log("error recibiendo mensaje del Kernel, bai",2);
			chau = 1;
		}
		memcpy(idmensaje,buff+1,2);

		switch (atoi(idmensaje)){
		case 7:
			memcpy(sizemensaje,buff+3,10);largomensaje = atoi(sizemensaje);
			free(sizemensaje);
			char *mensajeEntero = malloc(largomensaje);
			recibir(sockKerCPU,&controlador,mensajeEntero,largomensaje);
			//printf("hola\n");
			pcb = deserializarPCB_KerCPU(mensajeEntero);
			procesar();

		}


	}
//printf("Me fui\n");
free(programa);
free(ipK);free(ipM);

return EXIT_SUCCESS;
}

void leerArchivoConfiguracion(char* argv)
{

	t_config *configuracion;
	configuracion = config_create(argv);
	//ipK = malloc(12);
	//ipM = malloc(10);
	if(config_has_property(configuracion,"PUERTO_KERNEL")&&
			config_has_property(configuracion,"PUERTO_MEMORIA")&&
			config_has_property(configuracion,"IP_KERNEL")&&
			config_has_property(configuracion,"IP_MEMORIA")){
	puertoK = config_get_int_value(configuracion, "PUERTO_KERNEL");
	puertoM = config_get_int_value(configuracion, "PUERTO_MEMORIA");
	//strcpy(ipK, config_get_string_value(configuracion, "IP_KERNEL"));
	//strcpy(ipM, config_get_string_value(configuracion, "IP_MEMORIA"));
	ipK = strdup(config_get_string_value(configuracion, "IP_KERNEL"));
	ipM = strdup(config_get_string_value(configuracion, "IP_MEMORIA"));

	char * aux = string_from_format("archivo de configiguracion leido \n PUERTO_KERNEL:%d \n PUERTO_MEMORIA:%d \n IP_KERNEL:%s \n IP_MEMORIA:%s",puertoK,puertoM,ipK,ipM);
	escribir_log(aux,1);
	free(aux);
	}else {
		escribir_log("archivo de configiguracion incorrecto",2);
	}
	config_destroy(configuracion);
}
void procesar(){
	if(string_equals_ignore_case(pcb->algoritmo,ROUNDROBIN) == 0){
		//PROCESAR SEGUN QUANTUM/QUANTUM_SLEEP EL PCB
		int ins_realizada=0;
		while(ins_realizada < pcb->quantum){
			char * linea= pedir_linea_memoria();
			analizadorLinea(linea,&funcionesTodaviaSirve,&funcionesKernelTodaviaSirve);
			free(linea);
			ins_realizada ++;
			pcb->PC++;
			//sleep(pcb->quantum_sleep);
		}
	}if(string_equals_ignore_case(pcb->algoritmo,FIFO) == 0){

		// PROCESAR SEGUN FIFO
		int n=0;
		while(n == 0){
			char *linea = pedir_linea_memoria();
			analizadorLinea(linea,&funcionesTodaviaSirve,&funcionesKernelTodaviaSirve);
			free(linea);
		}
	}
}
int conexion_Kernel(int puertoK, char* ipK) {
	int controladorConexion = 0;
	sockKerCPU = iniciar_socket_cliente(ipK, puertoK, &controladorConexion);
	if (controladorConexion == 0) {
		escribir_log("Exitos conectandose al Kernel", 1);
	} else {
		escribir_log(string_itoa(controladorConexion),2);
	}
	int resultado = handshakeKernel(sockKerCPU);
	return resultado;
}
void conexion_Memoria(int puerto, char* ip) {
	int controladorConexion = 0;
	sockMemCPU = iniciar_socket_cliente(ip, puerto, &controladorConexion);
	if (controladorConexion == 0) {
		escribir_log("Exitos conectandose al Kernel", 1);
	} else {
		escribir_log(string_itoa(controladorConexion),2);
	}
	tam_pagina_memoria = handshakeMemoria(sockMemCPU);
}
char* pedir_linea_memoria(){
	//if (linea_esta_dividida() == TRUE){
	// calcular pagina 1 -> enviar mensaje memoria;
	// calcular pagina 2 -> enviar mensaje memoria;
	// return append resultado1|resultado2;
	//}else{
	//	calcular pagina -> enviar mensaje memoria;
	// return resultado;
	// }

	// todo: ARMAR MENSAJE PEDIR BYTES A MEMORIA: P|01|0000000000|PID|PAGINA|INICIO|FIN

	char *linea = string_substring(programa,pcb->in_cod[pcb->PC].offset_inicio,pcb->in_cod[pcb->PC].offset_fin);

	return linea;
}
