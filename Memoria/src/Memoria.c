#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "socket.h"
#include "log.h"
#include <commons/config.h>
#include "estructuras.h"

// Inicio de Variables Globales

int cliente[20];
int puerto;
t_memoria DATA_Memoria;

//Prototipo de Funciones:

t_memoria leerArchivoConfig(char* rutaArchivoConfig);


//Para Cliente
void esperar_mensaje(void *i);

//

//Proceso Principal

int main(int argc, char *argv[])
{
	crear_archivo_log("/home/utnso/Escritorio/MEMORIA-DEBUGGING-LOG");

	DATA_Memoria=leerArchivoConfig(argv[1]); // Parametro: Ruta De Archivo de Configuracion.

	printf("IP SERVER: %s:%s \n", DATA_Memoria.IP, DATA_Memoria.PUERTO);

	int	socketServidorMemoria = iniciar_socket_server(DATA_Memoria.IP, DATA_Memoria.PUERTO);

	int i = 0;

	while (1) {

		cliente[i] = escuchar_conexiones(socketServidorMemoria);
		escribir_log(string_from_format("se conecto un cliente! nr: %d ",cliente[i]));

	//pthread_create(&hiloEsperarMensaje[i], NULL, (void*) esperar_mensaje,(void *) cliente[i]);
	//escribir_log(string_from_format("cliente %d tiene hilo nro :%d ",cliente[i],i));

		//i++;
		}
	return 0;
}

// Funciones

t_memoria leerArchivoConfig(char* rutaArchivoConfig)
{
	t_memoria data_Config;
	printf("Ruta archivo de configuracion:  %s \n", rutaArchivoConfig);
	t_config *configuracion;
 	configuracion = config_create(rutaArchivoConfig);

 	data_Config.IP =config_get_string_value(configuracion, "IP");
 	data_Config.PUERTO =config_get_string_value(configuracion, "PUERTO");
	data_Config.MARCOS = config_get_int_value(configuracion, "MARCOS");
	data_Config.MARCO_SIZE = config_get_int_value(configuracion, "MARCO_SIZE");
	data_Config.ENTRADAS_CACHE = config_get_int_value(configuracion, "ENTRADAS_CACHE");
	data_Config.CACHE_X_PROC = config_get_int_value(configuracion, "CACHE_X_PROC");
	data_Config.REEMPLAZO_CACHE = config_get_int_value(configuracion, "REEMPLAZO_CACHE");
	data_Config.RETARDO_MEMORIA = config_get_int_value(configuracion, "RETARDO_MEMORIA");

	config_destroy(configuracion);
	return data_Config;
}

void esperar_mensaje(void *i)
{
	int cliente = (int) i;
	while (1)
	{

		char *mensajeRecibido = malloc(1024);
		char *emisor=malloc(1);
		char *codigoOP= malloc(2);

		memset(mensajeRecibido, '\0', 1024);
		memset(emisor,'\0',1);
		memset(codigoOP,'\0',2);

		mensajeRecibido=recibir(cliente);
		memcpy(emisor, mensajeRecibido,1);

		int noPuedeAtender= 1; //Es un Cliente valido pero no puede atender por falta de espacio o bla.

			if (strcmp(emisor,"K")!=0 || strcmp(emisor,"P")!=0 ) {
						char *mensajeRespuesta = malloc(1024);
						memset(mensajeRespuesta,'\0',1024);
						memcpy(mensajeRespuesta,"M-1",3);
						enviar(cliente, mensajeRespuesta);
						escribir_log(string_from_format("Rechaza HandShake: No es K ni P - %d ",cliente));
						pthread_exit(0);
			}

			memcpy(codigoOP, mensajeRecibido+1, 2);

		if ( strcmp(emisor,"K")==0 ) {

			switch (atoi(codigoOP)) {
			case 1:
				printf("\n ACA LAS RESPUESTAS AL KERNEL \n");
				break;
			case 2:
				//truncar(mensajeRecibido,cliente);
				break;
			}

		}
		else //RESPUESTA A PEDIDOS DE CPU
		{
			printf("ACA LAS RESPUESTAS AL CPU");
		}
		free(emisor);
		free(mensajeRecibido);
		free(codigoOP);
}

}
