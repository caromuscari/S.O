#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "estructuras.h"
//#include "funciones_memoria.h"	NI IDEA QUE ES ESTO!!!
#include "socket.h"
#include "log.h"
#include "manejo_errores.h"

fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;
t_memoria *data_Memoria;
t_log *log;

void leerArchivoConfig(char* rutaArchivoConfig);
void realizar_handShake(int nuevo_socket);
void ejecutar_server();

int main(int argc, char *argv[])
{
	crear_archivo_log("/home/utnso/Escritorio/memoria_log");
	leerArchivoConfig(argv[1]);
	//Creacion de Socket Server
	data_Memoria->SOCKET = iniciar_socket_server(data_Memoria->IP,data_Memoria->PUERTO,&controlador);

	//El select esta aca dentro
	ejecutar_server();

	return EXIT_SUCCESS;
}

//A partir de este punto.. las funciones
void leerArchivoConfig(char* rutaArchivoConfig)
{
	data_Memoria = malloc(sizeof(t_memoria));
	data_Memoria->IP = strdup("");

	t_config *configuracion = config_create(rutaArchivoConfig);
	string_append(&data_Memoria->IP,config_get_string_value(configuracion,"IP"));
	data_Memoria->PUERTO = config_get_int_value(configuracion,"PUERTO");
	data_Memoria->MARCOS = config_get_int_value(configuracion,"MARCOS");
	data_Memoria->MARCO_SIZE = config_get_int_value(configuracion,"MARCO_SIZE");
	data_Memoria->ENTRADAS_CACHE = config_get_int_value(configuracion,"ENTRADAS_CACHE");
	data_Memoria->CACHE_X_PROC = config_get_int_value(configuracion,"CACHE_X_PROC");
	data_Memoria->REEMPLAZO_CACHE = config_get_int_value(configuracion,"REEMPLAZO_CACHE");
	data_Memoria->RETARDO_MEMORIA = config_get_int_value(configuracion,"RETARDO_MEMORIA");
	config_destroy(configuracion);
}

void ejecutar_server()
{
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//Cargo el socket server
	FD_SET(data_Memoria->SOCKET, &master);

	//Cargo el socket mas grande
	fdmax = data_Memoria->SOCKET;

	//Bucle principal
	while (1)
	{
		read_fds = master;

		int selectResult = select(fdmax + 1, &read_fds, NULL, NULL, NULL);

		if (selectResult == -1)
		{
			break;
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
					if (i == data_Memoria->SOCKET)
					{
						//Gestiono la conexion entrante
						//Los clientes te envian tambien su identificador para responder en consecuencia.
						int nuevo_socket = aceptar_conexion(i, &controlador);
						escribir_log_con_numero("Realizando HANDSHAKE con ",nuevo_socket);

						//Controlo que no haya pasado nada raro y acepto al nuevo

						if(controlador == 0)
						{
							//realizar_handShake(nuevo_socket);
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

						if(controlador == 0)
						{
							// Empezar a responder los codigos
						}
					}
				}
			}
		}
	}
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
				//PENDIENTE DE REVISAR!!!
					char *mensajeRespuesta = malloc(1024); memset(mensajeRespuesta,'\0',1024);
					memcpy(mensajeRespuesta,"M00",3);
					memcpy(mensajeRespuesta+3,string_itoa(data_Memoria->MARCO_SIZE),4);
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
				escribir_log_con_numero("Rechaza HandShake: No es K ni P para Socket: ",nuevo_socket);
			}
		}
}
