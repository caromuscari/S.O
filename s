[1mdiff --git a/CPU/src/CPU.c b/CPU/src/CPU.c[m
[1mindex 1b5d4df..a644610 100644[m
[1m--- a/CPU/src/CPU.c[m
[1m+++ b/CPU/src/CPU.c[m
[36m@@ -15,6 +15,8 @@[m
 #include "funcionesParser.h"[m
 [m
 [m
[32m+[m
[32m+[m
 int puertoK,puertoM;[m
 char *ipK;[m
 char *ipM;[m
[36m@@ -24,11 +26,13 @@[m [mint tam_pagina_memoria;[m
 t_PCB_CPU* pcb;[m
 char* programa;[m
 int n;[m
[32m+[m
 static const char* bobo_ansisop =[m
 		"#!/usr/bin/ansisop\n"[m
 		"begin\n"[m
 		"variables a\n"[m
 		"a = 2\n"[m
[32m+[m		[32m"prints n a\n"[m
 		"end\n"[m
 		"\n";[m
 static const char* compartida_ansisop =[m
[36m@@ -118,31 +122,27 @@[m [mint main(int argc, char *argv[])[m
 		char* sizemensaje= malloc (10);[m
 		int largomensaje  = 0;[m
 		escribir_log("Esperando mensajes del Kernel para ponerme a trabajar...",1);[m
[32m+[m
 		recibir(sockKerCPU,&controlador,buff,13);[m
[31m-		//controlador = recv(sockKerCPU,buff,13,0);[m
 [m
 		if(controlador != 0){[m
 			escribir_log("error recibiendo mensaje del Kernel, bai",2);[m
 			chau = 1;[m
 		}[m
 		memcpy(idmensaje,buff+1,2);[m
[31m-		//idmensaje= string_substring(buff,1,2);[m
[32m+[m
 		char * aux = string_from_format("mensaje recibido %s",buff);[m
 		escribir_log(aux,1);[m
 		free(aux);[m
[31m-		printf("%d",atoi(idmensaje));[m
[32m+[m
 		switch (atoi(idmensaje)){[m
 		case 07:[m
 			escribir_log("CASE N° 7: iniciar procesamiento de PCB",1);[m
 			//iniciar_pcb_falsa();[m
[31m-			char * aux = string_from_format("sizemensaje (resto del mensaje) :%s-%d",sizemensaje,atoi(sizemensaje));[m
[31m-			escribir_log(aux,1);[m
[31m-			free(aux);[m
[31m-			memcpy(sizemensaje,buff+3,10);largomensaje = atoi(sizemensaje);[m
[31m-			//free(sizemensaje);[m
[32m+[m			[32mmemcpy(sizemensaje,buff+3,10);[m
[32m+[m			[32mlargomensaje = atoi(sizemensaje);[m
 			char *mensajeEntero = malloc(largomensaje);[m
 			recibir(sockKerCPU,&controlador,mensajeEntero,largomensaje);[m
[31m-			//printf("hola\n");[m
 			pcb = deserializarPCB_KerCPU(mensajeEntero);[m
 			procesar();[m
 			break;[m
[36m@@ -203,10 +203,11 @@[m [mvoid procesar(){[m
 [m
 		// PROCESAR SEGUN FIFO[m
 		n=0;[m
[31m-		while(n == 0){[m
[31m-			//@MARU! estan pasando cosas raras aca!!! llamo como 3 veces al "pedir_linea_memoria"[m
[31m-			//y este devolvio char vacios! y despues pincho con strcpy!![m
[32m+[m		[32mwhile(n != FINALIZAR_PROGRAMA){[m
[32m+[m
 			char *linea = pedir_linea_memoria();[m
[32m+[m			[32mescribir_log("linea a ejecutar:",1);[m
[32m+[m			[32mescribir_log(linea,1);[m
 			analizadorLinea(linea,&funcionesTodaviaSirve,&funcionesKernelTodaviaSirve);[m
 			free(linea);[m
 			pcb->PC ++;[m
[36m@@ -247,7 +248,9 @@[m [mchar* pedir_linea_memoria(){[m
 	// }[m
 [m
 	// todo: ARMAR MENSAJE PEDIR BYTES A MEMORIA: P|01|0000000000|PID|PAGINA|INICIO|FIN[m
[31m-[m
[32m+[m	[32mchar * aux = string_from_format("offset inicio:%d-offset fin:%d",pcb->in_cod[pcb->PC].offset_inicio,pcb->in_cod[pcb->PC].offset_fin);[m
[32m+[m	[32mescribir_log(aux,1);[m
[32m+[m	[32mfree(aux);[m
 	char *linea = string_substring(programa,pcb->in_cod[pcb->PC].offset_inicio,pcb->in_cod[pcb->PC].offset_fin);[m
 [m
 	return linea;[m
[36m@@ -256,7 +259,7 @@[m [mvoid iniciar_pcb_falsa(){[m
 [m
 		pcb= malloc(sizeof(t_PCB_CPU));[m
 			pcb->PC =0;[m
[31m-			pcb->PID =2;[m
[32m+[m			[32mpcb->PID =1;[m
 			pcb->SP = 0;[m
 			pcb->cant_pag = 1;[m
 			pcb->exit_code = 0;[m
[1mdiff --git a/CPU/src/estructuras.h b/CPU/src/estructuras.h[m
[1mindex de12513..f077ae4 100644[m
[1m--- a/CPU/src/estructuras.h[m
[1m+++ b/CPU/src/estructuras.h[m
[36m@@ -3,6 +3,8 @@[m
 #include <commons/collections/list.h>[m
 #include <commons/collections/dictionary.h>[m
 [m
[32m+[m[32m#define FINALIZAR_PROGRAMA 1[m
[32m+[m
 typedef struct[m
 {[m
 	int offset_inicio;[m
[1mdiff --git a/CPU/src/funcionesCPU.c b/CPU/src/funcionesCPU.c[m
[1mindex 51f6968..37e3399 100644[m
[1m--- a/CPU/src/funcionesCPU.c[m
[1m+++ b/CPU/src/funcionesCPU.c[m
[36m@@ -315,7 +315,7 @@[m [mint handshakeMemoria(int socketMP){[m
 			memcpy(tammensaje,handshake+3,10);[m
 			tamimensaje = atoi(tammensaje);[m
 			free(tammensaje);[m
[31m-			char *tampag = malloc(tamimensaje);printf("tamimensaje%d\n",tamimensaje);[m
[32m+[m			[32mchar *tampag = malloc(tamimensaje);[m
 			recibir(socketMP,&control,tampag,tamimensaje);[m
 			tamanopag = atoi(tampag); free(tampag);[m
 [m
[36m@@ -387,3 +387,73 @@[m [mvoid t_memoria_destroy(t_memoria *self){[m
 int calcular_offset_respecto_pagina(int offset){[m
 	return offset % tam_pagina_memoria;[m
 }[m
[32m+[m
[32m+[m[32mchar *mensaje_escibir_memoria(int fpid,int direccion_variable,int cant_pag,int valor){[m
[32m+[m
[32m+[m		[32mchar * mensaje = malloc(19+ sizeof(int));[m
[32m+[m		[32mchar * pid; char * pagina; char *offset; char *tam;[m
[32m+[m		[32mchar * aux_ceros;[m
[32m+[m		[32mint desplazamiento=0;[m
[32m+[m		[32mpid = string_itoa(fpid);[m
[32m+[m		[32mpagina = string_itoa(calcular_pagina(direccion_variable,cant_pag));[m
[32m+[m		[32moffset = string_itoa(calcular_offset_respecto_pagina(direccion_variable));[m
[32m+[m		[32mtam = strdup("0004");[m
[32m+[m		[32m// COD[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,"P08",3);[m
[32m+[m		[32mdesplazamiento += 3;[m
[32m+[m		[32m// PID[m
[32m+[m		[32maux_ceros = string_repeat('0',4-strlen(pid));[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pid));[m
[32m+[m		[32mfree(aux_ceros);[m
[32m+[m		[32mdesplazamiento += 4-strlen(pid);[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,pid,strlen(pid));[m
[32m+[m		[32mdesplazamiento += strlen(pid);[m
[32m+[m		[32m// PAGINA[m
[32m+[m		[32maux_ceros = string_repeat('0',4-strlen(pagina));[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pagina));[m
[32m+[m		[32mfree(aux_ceros);[m
[32m+[m		[32mdesplazamiento += 4-strlen(pagina);[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,pagina,strlen(pagina));[m
[32m+[m		[32mdesplazamiento += strlen(pagina);[m
[32m+[m		[32m// OFFSET[m
[32m+[m		[32maux_ceros = string_repeat('0',4-strlen(offset));[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,aux_ceros,4-strlen(offset));[m
[32m+[m		[32mfree(aux_ceros);[m
[32m+[m		[32mdesplazamiento += 4-strlen(offset);[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,offset,strlen(offset));[m
[32m+[m		[32mdesplazamiento += strlen(offset);[m
[32m+[m		[32m// TAMAÑO[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,tam,4);[m
[32m+[m		[32mdesplazamiento +=4;[m
[32m+[m		[32m// VALOR[m
[32m+[m		[32mchar * str_valor = string_itoa(valor);[m
[32m+[m		[32maux_ceros = string_repeat('0',4-strlen(str_valor));[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,aux_ceros,4-strlen(str_valor));[m
[32m+[m		[32mfree(aux_ceros);[m
[32m+[m		[32mdesplazamiento += 4-strlen(str_valor);[m
[32m+[m		[32mmemcpy(mensaje+desplazamiento,str_valor,strlen(str_valor));[m
[32m+[m		[32mdesplazamiento += strlen(str_valor);[m
[32m+[m
[32m+[m		[32mfree(pid); free(pagina); free(offset); free(tam); free(str_valor);[m
[32m+[m		[32mreturn mensaje;[m
[32m+[m[32m}[m
[32m+[m[32mchar *mensaje_semaforo(char * cod,char * semaforo,int *size){[m
[32m+[m	[32mchar *mensaje= malloc(13+strlen(semaforo));[m
[32m+[m	[32mchar *strlen_sem = string_itoa(strlen(semaforo));[m
[32m+[m	[32mchar *aux_ceros = string_repeat(0,10-strlen(strlen_sem));[m
[32m+[m
[32m+[m	[32mint desplazamiento = 0;[m
[32m+[m	[32mmemcpy(mensaje + desplazamiento,cod,3);[m
[32m+[m	[32mdesplazamiento += 3;[m
[32m+[m	[32mmemcpy(mensaje+desplazamiento,aux_ceros,10-strlen(strlen_sem));[m
[32m+[m	[32mdesplazamiento += 10-strlen(strlen_sem);[m
[32m+[m	[32mmemcpy(mensaje+desplazamiento,strlen_sem,strlen(strlen_sem));[m
[32m+[m	[32mdesplazamiento += strlen(strlen_sem);[m
[32m+[m	[32mmemcpy(mensaje+desplazamiento,semaforo,strlen(semaforo));[m
[32m+[m	[32mdesplazamiento += strlen(semaforo);[m
[32m+[m
[32m+[m	[32m*size = desplazamiento;[m
[32m+[m	[32mfree(strlen_sem);[m
[32m+[m	[32mfree(aux_ceros);[m
[32m+[m	[32mreturn mensaje;[m
[32m+[m[32m}[m
[1mdiff --git a/CPU/src/funcionesCPU.h b/CPU/src/funcionesCPU.h[m
[1mindex e35f5d5..91f5b11 100644[m
[1m--- a/CPU/src/funcionesCPU.h[m
[1m+++ b/CPU/src/funcionesCPU.h[m
[36m@@ -21,6 +21,8 @@[m [mvoid stack_destroy(t_stack_element *self);[m
 void t_memoria_destroy(t_memoria *self);[m
 int calcular_pagina(int offset,int paginas);[m
 int calcular_offset_respecto_pagina(int offset);[m
[32m+[m[32mchar *mensaje_escibir_memoria(int fpid,int direccion_variable,int cant_pag,int valor);[m
[32m+[m[32mchar *mensaje_semaforo(char * cod,char * semaforo,int *);[m
 [m
 [m
 #endif /* SRC_FUNCIONESCPU_ */[m
[1mdiff --git a/CPU/src/funcionesParser.c b/CPU/src/funcionesParser.c[m
[1mindex 1c9cdd6..498783f 100644[m
[1m--- a/CPU/src/funcionesParser.c[m
[1m+++ b/CPU/src/funcionesParser.c[m
[36m@@ -12,6 +12,7 @@[m
 #include "funcionesCPU.h"[m
 #include "estructuras.h"[m
 #include "socket.h"[m
[32m+[m[32m#include <sys/socket.h>[m
 #include "log.h"[m
 [m
 extern t_PCB_CPU* pcb;[m
[36m@@ -64,16 +65,14 @@[m [mt_puntero definirVariable(t_nombre_variable identificador_variable) {[m
 t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {[m
 	t_stack_element* stack_actual = list_get(pcb->in_stack,pcb->SP);[m
 	t_puntero posicion_retorno;[m
[31m-		if(identificador_variable >= '0' && identificador_variable <= '9'){[m
 [m
[32m+[m	[32mif(identificador_variable >= '0' && identificador_variable <= '9'){[m
 			int pos = identificador_variable - '0';[m
 			t_memoria* arg=list_get(stack_actual->args,pos);[m
 			posicion_retorno= arg->offset;[m
 [m
 		}else if ((identificador_variable >= 'A' && identificador_variable <= 'Z') || (identificador_variable >= 'a' && identificador_variable <= 'z')){[m
[31m-[m
 			posicion_retorno = buscar_offset_variable(stack_actual->vars,identificador_variable);[m
[31m-[m
 		}[m
 		if(posicion_retorno == -1){[m
 			char * aux= string_from_format("OBTENER POSICION VARIABLE:%c no existe variable",identificador_variable);[m
[36m@@ -91,7 +90,10 @@[m [mt_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {[m
 t_valor_variable dereferenciar(t_puntero direccion_variable) {[m
 [m
 	char * mensaje = malloc(19);[m
[31m-	char * pid; char * pagina; char *offset; char *tam;[m
[32m+[m	[32mchar * pid;[m
[32m+[m	[32mchar * pagina;[m
[32m+[m	[32mchar *offset;[m
[32m+[m	[32mchar *tam;[m
 	char * aux_ceros;[m
 	int desplazamiento=0;[m
 	int controlador=0;[m
[36m@@ -124,20 +126,34 @@[m [mt_valor_variable dereferenciar(t_puntero direccion_variable) {[m
 	memcpy(mensaje+desplazamiento,offset,strlen(offset));[m
 	desplazamiento += strlen(offset);[m
 	// TAMAÑO[m
[31m-	memcpy(mensaje+desplazamiento,tam,4);[m
[32m+[m	[32mmemcpy(mensaje+desplazamiento,tam,strlen(tam));[m
[32m+[m	[32mdesplazamiento += strlen(tam);[m
[32m+[m
 [m
 	enviar(sockMemCPU,mensaje,&controlador,19);[m
[31m-	char * mensaje_aux= malloc(13);[m
[31m-	recibir(sockMemCPU,&controlador,mensaje_aux,13);[m
[31m-	char *tam_rest = string_substring(mensaje_aux,3,13);[m
[32m+[m	[32mfree(mensaje);[m
[32m+[m	[32mfree(pid); free(pagina); free(offset);[m
[32m+[m	[32mfree(tam);[m
[32m+[m
[32m+[m	[32mchar* mensaje_aux= malloc(4);[m
[32m+[m	[32m//recibir(sockMemCPU,&controlador,mensaje_aux,13);[m
[32m+[m	[32mrecv(sockMemCPU,mensaje_aux,4,MSG_WAITALL);[m
[32m+[m	[32m/*char *tam_rest = malloc(10);[m
[32m+[m	[32mmemcpy(tam_rest,mensaje_aux+3,10);[m
 	int tam_resto = atoi(tam_rest);[m
[31m-	free(mensaje_aux); free(tam_rest);[m
[31m-	char * valor_str = malloc(tam_resto);[m
[32m+[m
[32m+[m	[32mif(tam_resto != 2){[m
[32m+[m		[32mtam_resto = 2;[m
[32m+[m	[32m}[m
[32m+[m	[32mchar *valor_str = malloc(tam_resto);[m
 	recibir(sockMemCPU,&controlador,valor_str,tam_resto);[m
 	int valor = atoi(valor_str);[m
[31m-	free(valor_str);[m
[31m-	free(mensaje);[m
[31m-	free(pid); free(pagina); free(offset);free(tam);[m
[32m+[m
[32m+[m	[32mfree(valor_str); ; free(tam_rest);*/[m
[32m+[m	[32mint valor = atoi(mensaje_aux);[m
[32m+[m	[32mfree(mensaje_aux);[m
[32m+[m
[32m+[m
 [m
 [m
 [m
[36m@@ -150,68 +166,25 @@[m [mt_valor_variable dereferenciar(t_puntero direccion_variable) {[m
 [m
 void asignar(t_puntero direccion_variable, t_valor_variable valor) {[m
 [m
[31m-	char * mensaje = malloc(19+ sizeof(int));[m
[31m-	char * pid; char * pagina; char *offset; char *tam;[m
[31m-	char * aux_ceros;[m
[31m-	int desplazamiento=0;[m
[32m+[m	[32mchar *mensaje = mensaje_escibir_memoria(pcb->PID,direccion_variable,pcb->cant_pag,valor);[m
 	int controlador=0;[m
[31m-	pid = string_itoa(pcb->PID);[m
[31m-	pagina = string_itoa(calcular_pagina(direccion_variable,pcb->cant_pag));[m
[31m-	offset = string_itoa(calcular_offset_respecto_pagina(direccion_variable));[m
[31m-	tam = strdup("0004");[m
[31m-	// COD[m
[31m-	memcpy(mensaje+desplazamiento,"P08",3);[m
[31m-	desplazamiento += 3;[m
[31m-	// PID[m
[31m-	aux_ceros = string_repeat('0',4-strlen(pid));[m
[31m-	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pid));[m
[31m-	free(aux_ceros);[m
[31m-	desplazamiento += 4-strlen(pid);[m
[31m-	memcpy(mensaje+desplazamiento,pid,strlen(pid));[m
[31m-	desplazamiento += strlen(pid);[m
[31m-	// PAGINA[m
[31m-	aux_ceros = string_repeat('0',4-strlen(pagina));[m
[31m-	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(pagina));[m
[31m-	free(aux_ceros);[m
[31m-	desplazamiento += 4-strlen(pagina);[m
[31m-	memcpy(mensaje+desplazamiento,pagina,strlen(pagina));[m
[31m-	desplazamiento += strlen(pagina);[m
[31m-	// OFFSET[m
[31m-	aux_ceros = string_repeat('0',4-strlen(offset));[m
[31m-	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(offset));[m
[31m-	free(aux_ceros);[m
[31m-	desplazamiento += 4-strlen(offset);[m
[31m-	memcpy(mensaje+desplazamiento,offset,strlen(offset));[m
[31m-	desplazamiento += strlen(offset);[m
[31m-	// TAMAÑO[m
[31m-	memcpy(mensaje+desplazamiento,tam,4);[m
[31m-	desplazamiento +=4;[m
[31m-	// VALOR[m
[31m-	char * str_valor = string_itoa(valor);[m
[31m-	aux_ceros = string_repeat('0',4-strlen(str_valor));[m
[31m-	memcpy(mensaje+desplazamiento,aux_ceros,4-strlen(str_valor));[m
[31m-	free(aux_ceros);[m
[31m-	desplazamiento += 4-strlen(str_valor);[m
[31m-	memcpy(mensaje+desplazamiento,str_valor,strlen(str_valor));[m
[31m-	desplazamiento += strlen(str_valor);[m
[31m-[m
 	enviar(sockMemCPU,mensaje,&controlador,19+sizeof(int));[m
[31m-[m
 	free(mensaje);[m
[31m-	free(pid); free(pagina); free(offset);free(tam);[m
 [m
[31m-	char * respuesta = malloc(2);[m
[31m-	recibir(sockMemCPU,&controlador,respuesta,2);[m
[31m-	if (string_equals_ignore_case(respuesta,"OK")){[m
[32m+[m	[32mchar * respuesta = malloc(13);[m
[32m+[m	[32mrecibir(sockMemCPU,&controlador,respuesta,13);[m
[32m+[m
[32m+[m	[32mif (strncmp(respuesta,"M02",3)==0){[m
 		char * str_aux= string_from_format("Se ejecuto ASIGNAR en posicion %d y valor %d",direccion_variable,valor);[m
 		escribir_log(str_aux,1);[m
 		free(str_aux);[m
[31m-	} else {[m
[32m+[m
[32m+[m	[32m} else if(strncmp(respuesta,"M03",3)==0){[m
 		char * str_aux= string_from_format("ERROR ASIGNANDO en posicion %d y valor %d",direccion_variable,valor);[m
 		escribir_log(str_aux,1);[m
 		free(str_aux);[m
 	}[m
[31m-[m
[32m+[m	[32mfree(respuesta);[m
 }[m
 [m
 t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {[m
[36m@@ -229,12 +202,13 @@[m [mt_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {[m
 	desplazamiento += 10-strlen(leng);[m
 	memcpy(mensaje+desplazamiento,leng,strlen(leng));[m
 	desplazamiento += strlen(leng);[m
[31m-	free(leng);[m
[32m+[m
 	// VARIABLE COMPARTIDA[m
 	memcpy(mensaje+desplazamiento,variable,strlen(variable));[m
 [m
 	enviar(sockKerCPU,mensaje,&controlador,13+strlen(variable));[m
 	free(mensaje);[m
[32m+[m	[32mfree(leng);[m
 [m
 	char * mensaje_r = malloc(7);[m
 	recibir(sockKerCPU,&controlador,mensaje_r,7);[m
[36m@@ -284,7 +258,7 @@[m [mt_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_var[m
 [m
 	char * respuesta= malloc(2);[m
 	recibir(sockKerCPU,&controlador,respuesta,2);[m
[31m-	if(string_equals_ignore_case(respuesta,"OK")){[m
[32m+[m	[32mif(strncmp(respuesta,"OK",2)){[m
 		char * aux =string_from_format("Ejecute ASIGNAR VALOR COMPARTIDA %s valor %d",variable,valor);[m
 		escribir_log(aux,1);[m
 		free(aux);[m
[36m@@ -298,6 +272,7 @@[m [mt_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_var[m
 		return -1;[m
 	}[m
 }[m
[32m+[m
 void irAlLabel(t_nombre_etiqueta etiqueta) {[m
 [m
 	char *eti = strdup(etiqueta);[m
[36m@@ -312,52 +287,152 @@[m [mvoid irAlLabel(t_nombre_etiqueta etiqueta) {[m
 [m
 void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {[m
 [m
[31m-	escribir_log(string_from_format("Ejecute llamarConRetorno etiqueta:%s,donde_retornar:%d",etiqueta,donde_retornar),1);[m
[32m+[m	[32mpcb->SP ++;[m
[32m+[m	[32mt_stack_element * nuevo_el = malloc(sizeof (t_stack_element));[m
[32m+[m	[32mnuevo_el->pos = pcb->SP;[m
[32m+[m	[32mnuevo_el->retPos = pcb->PC;[m
[32m+[m	[32mnuevo_el->retVar.ID = '\0';[m
[32m+[m	[32mnuevo_el->retVar.offset = donde_retornar;[m
[32m+[m	[32mnuevo_el->retVar.size = 4;[m
[32m+[m	[32mnuevo_el->retVar.pag = calcular_pagina(donde_retornar,0);[m
[32m+[m	[32mnuevo_el->args = list_create();[m
[32m+[m	[32mnuevo_el->vars = list_create();[m
[32m+[m	[32mlist_add(pcb->in_stack,nuevo_el);[m
[32m+[m
[32m+[m	[32mchar * eti = strdup(etiqueta);[m
[32m+[m	[32mstring_trim(&eti);[m
[32m+[m	[32mpcb->PC =(int) dictionary_get(pcb->dicc_et,eti);[m
[32m+[m	[32mfree(eti);[m
[32m+[m
[32m+[m	[32mescribir_log(string_from_format("Ejecute LLAMAR CON RETORNO etiqueta:%s,donde_retornar:%d",etiqueta,donde_retornar),1);[m
 }[m
[32m+[m
 void llamarSinRetorno (t_nombre_etiqueta etiqueta){[m
[32m+[m
 	escribir_log("Ejecute llamarSinRetorno",1);[m
 }[m
 [m
 void finalizar(void) {[m
[31m-	n=1;[m
[31m-	escribir_log("Ejecute finalizar",1);[m
[32m+[m	[32mn= FINALIZAR_PROGRAMA;[m
[32m+[m	[32m/* devolver pbc a kernel avisandole que se retira de la cpu por finalizacion normal del programa[m
[32m+[m	[32m * */[m
[32m+[m
[32m+[m	[32mescribir_log("Ejecute FINALIZAR",1);[m
 }[m
 [m
 void retornar(t_valor_variable retorno) {[m
[31m-	escribir_log("Ejecute retornar",1);[m
[32m+[m
[32m+[m	[32mt_stack_element *aux_stack_el = list_get(pcb->in_stack,pcb->SP);[m
[32m+[m
[32m+[m	[32mchar * mensaje = mensaje_escibir_memoria(pcb->PID,aux_stack_el->retVar.offset,pcb->cant_pag,retorno);[m
[32m+[m	[32mint controlador =0 ;[m
[32m+[m	[32menviar(sockMemCPU,mensaje,&controlador,19);[m
[32m+[m	[32mfree(mensaje);[m
[32m+[m
[32m+[m	[32mchar * respuesta  = malloc(13);[m
[32m+[m	[32mrecibir(sockKerCPU,&controlador,respuesta,13);[m
[32m+[m	[32mif(strncmp(respuesta,"M02",3)==0){[m
[32m+[m		[32mchar * aux = string_from_format("ALMACENAR BYTES de valor de retorno ",retorno);[m
[32m+[m		[32mescribir_log(aux,1);[m
[32m+[m		[32mfree(aux);[m
[32m+[m		[32mpcb->SP --;[m
[32m+[m		[32mpcb->PC = aux_stack_el->retPos;[m
[32m+[m		[32mlist_remove_and_destroy_element(pcb->in_stack, pcb->SP++,(void*)stack_destroy);[m
[32m+[m		[32mescribir_log("Ejecute RETORNAR ",1);[m
[32m+[m
[32m+[m	[32m}else if(strncmp(respuesta,"M03",3)== 0){[m
[32m+[m		[32mchar * aux = string_from_format("ERROR ejecutando RETORNAR con valor de retorno %d",retorno);[m
[32m+[m		[32mescribir_log(aux,1);[m
[32m+[m		[32mfree(aux);[m
[32m+[m	[32m}[m
[32m+[m	[32mfree(respuesta);[m
 }[m
 [m
 void wait(t_nombre_semaforo identificador_semaforo) {[m
[31m-	escribir_log("Ejecute wait",1);[m
[32m+[m
[32m+[m	[32mint controlador=0;int size=0;[m
[32m+[m	[32mchar *mensaje = mensaje_semaforo("P13",identificador_semaforo,&size);[m
[32m+[m	[32menviar(sockKerCPU,mensaje,&controlador,size);[m
[32m+[m	[32mfree(mensaje);[m
[32m+[m
[32m+[m	[32mchar * logi = string_from_format("Ejecute WAIT sobre semaforo %s",identificador_semaforo);[m
[32m+[m	[32mescribir_log(logi,1);[m
[32m+[m	[32mfree(logi);[m
 }[m
 [m
 void signale(t_nombre_semaforo identificador_semaforo) {[m
 	escribir_log("Ejecute signale",1);[m
 }[m
[32m+[m
 void escribir (t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){[m
[32m+[m	[32mchar * logi = string_from_format("Void * informacion:%s, tamanio:%d",(char *) informacion,tamanio);[m
[32m+[m	[32mescribir_log(logi,1);[m
[32m+[m	[32mfree(logi);[m
[32m+[m	[32mif(descriptor_archivo == 1){[m
[32m+[m
[32m+[m		[32mchar * mensaje = malloc(13+tamanio);[m
[32m+[m			[32mint desplazamiento = 0;[m
[32m+[m			[32mchar * aux_ceros;[m
[32m+[m			[32mchar * str_valor = string_itoa(strlen(informacion));[m
[32m+[m			[32mint controlador = 0;[m
[32m+[m			[32m// COD[m
[32m+[m			[32mmemcpy(mensaje+desplazamiento,"P11",3);[m
[32m+[m			[32mdesplazamiento += 3;[m
[32m+[m			[32maux_ceros = string_repeat('0',10-strlen(str_valor));[m
[32m+[m			[32mmemcpy(mensaje+desplazamiento,aux_ceros,10-strlen(str_valor));[m
[32m+[m			[32mfree(aux_ceros);[m
[32m+[m			[32mdesplazamiento += 10-strlen(str_valor);[m
[32m+[m			[32mmemcpy(mensaje+desplazamiento,str_valor,strlen(str_valor));[m
[32m+[m			[32mdesplazamiento += strlen(str_valor);[m
[32m+[m			[32mfree(str_valor);[m
[32m+[m			[32m// VARIABLE[m
[32m+[m			[32mmemcpy(mensaje+desplazamiento,informacion,tamanio);[m
[32m+[m			[32mdesplazamiento += tamanio;[m
[32m+[m
[32m+[m			[32menviar(sockKerCPU,mensaje,&controlador,13+tamanio);[m
[32m+[m			[32mfree(mensaje);[m
[32m+[m
[32m+[m			[32mchar * respuesta= malloc(2);[m
[32m+[m			[32mrecibir(sockKerCPU,&controlador,respuesta,2);[m
[32m+[m			[32mif(strncmp(respuesta,"OK",2)==0){[m
[32m+[m				[32mchar * logi = string_from_format("Ejecute ESCRIBIR con file descriptor %d",descriptor_archivo);[m
[32m+[m				[32mescribir_log(logi,1);[m
[32m+[m				[32mfree(logi);[m
[32m+[m			[32m}else{[m
[32m+[m				[32mchar * logi = string_from_format("ERROR ESCRIBIR con file descriptor %d",descriptor_archivo);[m
[32m+[m				[32mescribir_log(logi,2);[m
[32m+[m				[32mfree(logi);[m
[32m+[m			[32m}[m
 [m
[31m-	escribir_log("Ejecute Escribir",1);[m
[32m+[m	[32m}[m
 }[m
[32m+[m
 void moverCursor (t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){[m
 	escribir_log("Ejecute moverCursor",1);[m
 }[m
[32m+[m
 void liberar (t_puntero puntero){[m
 	escribir_log("Ejecute liberar",1);[m
 }[m
[32m+[m
 t_puntero reservar (t_valor_variable espacio){[m
 	escribir_log("Ejecute reservar",1);[m
 	return 1;[m
 }[m
[32m+[m
 t_descriptor_archivo abrir (t_direccion_archivo direccion, t_banderas flags){[m
 	escribir_log("Ejecute abrir",1);[m
 	return 1;[m
 }[m
[32m+[m
 void leer (t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){[m
 	escribir_log("Ejecute leer",1);[m
 }[m
[32m+[m
 void cerrar (t_descriptor_archivo descriptor_archivo){[m
 	escribir_log("Ejecute cerrar",1);[m
 }[m
[32m+[m
 void borrar (t_descriptor_archivo direccion){[m
 	escribir_log("Ejecute borrar",1);[m
 }[m
[1mdiff --git a/CPU/src/socket.c b/CPU/src/socket.c[m
[1mindex 5aec6ce..556ea1f 100644[m
[1m--- a/CPU/src/socket.c[m
[1m+++ b/CPU/src/socket.c[m
[36m@@ -42,7 +42,7 @@[m [mint iniciar_socket_cliente(char *ip, int puerto_conexion, int *control)[m
 [m
 int enviar(int socket_emisor,void * mensaje_a_enviar, int *controlador,int size)[m
 {[m
[31m-	printf("mensaje_a_enviar: %s\n", mensaje_a_enviar);[m
[32m+[m
 	int ret;[m
 	//signal(SIGPIPE, SIG_IGN);[m
 	*controlador = 0;[m
[36m@@ -78,7 +78,6 @@[m [mvoid recibir(int socket_receptor, int *controlador,void *buff,int size)[m
 		*controlador = 1;[m
 		//close(socket_receptor);[m
 	}[m
[31m-	printf("recibido: %s\n", (char*)buff);[m
 }[m
 [m
 void cerrar_conexion(int socket_)[m
[1mdiff --git a/mensajes.txt b/mensajes.txt[m
[1mindex 5019041..9c8bc10 100644[m
[1m--- a/mensajes.txt[m
[1m+++ b/mensajes.txt[m
[36m@@ -42,6 +42,7 @@[m [mP07 BORRAR[m
 P08 ASIGNAR BYTES EN MEMORIA[m
 P09 PEDIR VALOR VARIABLE COMPARTIDA A KERNEL: P|09|0000000018|"variablecompartida"[m
 P10  ASIGNAR VALOR VARIABLE COMPARTIDA KERNEL: P|10|0000000018|"variablecompartida"|0025 [m
[32m+[m[32mP11 P11|0000000008|"imprimir"[m
 [m
 MEMORIA - M[m
 M00: Respuesta a la solicitud de datos (Kernel) de tamaño de página. Va en el HandShake. Rta Ej: M|00|0128|[m
