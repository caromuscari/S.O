#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "log.h"
#include <commons/config.h>
#include "estructuras.h"
#include "funciones_memoria.h"
#include "socket.h"
#include "manejo_errores.h"

// Inicio de Variables Globales

int cliente[20];
int puerto;
int controlador = 0;

t_memoria DATA_Memoria;


//Prototipo de Funciones:

t_memoria leerArchivoConfig(char* rutaArchivoConfig);
void realizar_handShake(int nuevo_socket);


//Proceso Principal

int main(int argc, char *argv[])
{


	crear_archivo_log("/home/utnso/Escritorio/MEMORIA-DEBUGGING-LOG");
	DATA_Memoria=leerArchivoConfig(argv[1]); // Parametro: Ruta De Archivo de Configuracion.
	printf("IP SERVER: %s:%s \n", DATA_Memoria.IP, DATA_Memoria.PUERTO);
	printf("MARCOS: %d \n", DATA_Memoria.MARCOS);

	//int SIZE_PAGE_TABLE = REG_PAGE_TABLE * DATA_Memoria.MARCOS;

	//printf("SIZE_PAGE_TABLE : %d \n", SIZE_PAGE_TABLE);

	printf("SIZE_MEMORIA: %d \n", DATA_Memoria.MARCO_SIZE + DATA_Memoria.MARCO_SIZE);


	char *ip_nuevo = "127.0.0.1";
	int nuevo_puerto = 5003;

	int	socketServidorMemoria = iniciar_socket_server(ip_nuevo, nuevo_puerto,&controlador);
	//int	socketServidorMemoria = iniciar_socket_server(DATA_Memoria.IP, (int)DATA_Memoria.PUERTO,&controlador);

	fd_set master;
	fd_set read_fds;
	int fdmax;

	escribir_log(string_from_format("El FileDescriptor del Servidor es: %d",socketServidorMemoria));


	//Crea estructura administrativa

	//unsigned char frame[20] * MEMORIA;
	//frame;
	//MEMORIA = malloc (DATA_Memoria.MARCO_SIZE * DATA_Memoria.MARCOS);

	//bzero(MEMORIA, 0);




	//Crea estructura administrativa

	//Seteo en 0 el master y temporal
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//Cargo el socket server
	FD_SET(socketServidorMemoria , &master);

	//Cargo el socket mas grande
	fdmax = socketServidorMemoria;

	escribir_log(string_from_format("FDMAX: %d",fdmax));

	//Bucle principal
	while (1)
	{
		read_fds = master;

		escribir_log(string_from_format("ANTES DEL SELECT: %d",fdmax));

		int selectResult = select(fdmax + 1, &read_fds, NULL, NULL, NULL);

		escribir_log(string_from_format("Despues del Select: %d",selectResult));

		if (selectResult == -1) {
			break; // Error en el SELECT.
		}
		else
		{
			//Recorro los descriptores para ver quien llamo
			int i;
			for (i = 0; i <= fdmax; i++)
			{
				if (FD_ISSET(i, &read_fds))
				{
					//Se detecta alguien nuevo llamando?
					if (i == socketServidorMemoria)
					{
						//Gestiono la conexion entrante
						//Los clientes te envian tambien su identificador para responder en consecuencia.

						int nuevo_socket = aceptar_conexion(i, &controlador);

						escribir_log(string_from_format("Realizando HANDSHAKE %d",nuevo_socket ));
						//Controlo que no haya pasado nada raro y acepto al nuevo
						if (controlador > 0)
						{
							error_sockets(&controlador, "");
						}
						else
						{
						//funcion para realizar handshake con nueva conexion


						//	realizar_handShake(nuevo_socket);
							///
						}

						//Cargo la nueva conexion a la lista y actualizo el maximo
						FD_SET(nuevo_socket, &master);

						if (nuevo_socket > fdmax)
						{
							fdmax = nuevo_socket;
						}
					}
					else
					{
						//Es una conexion existente, respondo a lo que me pide
						char *mensaje_recibido = recibir(i, &controlador);
						if(controlador > 0)
						{
							error_sockets(&controlador, atoi(i));

						}
						else
						{

// Empezar a responder los codigos

						}
					}
				}
			}
		}
	}







	return 0;
}

/*      Funciones     */

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
	//config_destroy(configuracion);
	free(configuracion);
	return data_Config;
}

void realizar_handShake(int nuevo_socket)
{
	char *mensajeRecibido = malloc(1024);
	memset(mensajeRecibido, '\0', 1024);
	char *emisor=malloc(1);
	char *codigoOP= malloc(2);
	memset(emisor,'\0',1);
	memset(codigoOP,'\0',2);

	mensajeRecibido = recibir(nuevo_socket, &controlador);
		if (controlador > 0)
		{
			error_sockets(&controlador, string_itoa(nuevo_socket));
			cerrar_conexion(nuevo_socket);
		}
		else
		{
			memcpy(emisor, mensajeRecibido,1);

			if (strcmp(emisor,"K")==0 || strcmp(emisor,"P")==0 ) {
				// es K o P envía el OK y el tamanio de pagina.
					char *mensajeRespuesta = malloc(1024); memset(mensajeRespuesta,'\0',1024);
					memcpy(mensajeRespuesta,"M00",3);
					memcpy(mensajeRespuesta+3,string_itoa(DATA_Memoria.MARCO_SIZE),4);
					//Cuando valida el cliente, le da OK enviandole el tamaño de pagina
					enviar(nuevo_socket, mensajeRespuesta, &controlador);
			}
			else
			{
				//no es ni K ni P.
				char *mensajeRespuesta = malloc(1024);
				memset(mensajeRespuesta,'\0',1024);
				memcpy(mensajeRespuesta,"M01",3);
				enviar(nuevo_socket, mensajeRespuesta,&controlador);
				escribir_log(string_from_format("Rechaza HandShake: No es K ni P - %d ",nuevo_socket));

			}
		}
	}







