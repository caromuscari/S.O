#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "estructuras.h"
#include "socket.h"
#include "log.h"
#include "mensaje.h"
#include "manejo_errores.h"

fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;
t_memoria *data_Memoria;
t_log *log;

// Para multi-hilos
int cliente[20];
pthread_t hiloEsperarMensaje[20];
void leerArchivoConfig(char* rutaArchivoConfig);

//Clientes

void esperar_mensaje(void *i);

int main(int argc, char *argv[])
{
	crear_archivo_log("/home/utnso/Escritorio/MEMORIA-DEBUGGING-LOG.txt");
	leerArchivoConfig(argv[1]);
	//Creacion de Socket Server
	printf("\n MARCOS %d  \n",data_Memoria->MARCOS);
	printf("PUERTO %d  \n",data_Memoria->PUERTO);
	//data_Memoria->SOCKET =

	int socketServerMemoria = iniciar_socket_server(data_Memoria->IP,data_Memoria->PUERTO,&controlador);

	escribir_log(string_from_format("Se inicia Servidor de Memoria en Socket: %d ",socketServerMemoria));


	int i = 0;

		while (1) {

			cliente[i] = escuchar_conexiones(socketServerMemoria,&controlador);

			escribir_log(string_from_format("Nueva Conexión entrante: %d ",cliente[i]));

	// Realizar HS y enviar Tamaño MARCO.
			char *mensaje_recibido = recibir(cliente[i], &controlador);
			char *header = get_header(mensaje_recibido);

			if(strcmp(header,"K")==0 || strcmp(header,"P")==0 )
				{
				//Cliente VALIDO
					if (strcmp(header,"K")==0 )
						{
							char* HS_OK = armar_mensaje("M00","128"); // M|00|0000000003|128|
							enviar(cliente[i],HS_OK, &controlador);
						}
						else
						{
							char* HS_OK = armar_mensaje("M00",""); // M|00|0000000000|
							enviar(cliente[i],HS_OK, &controlador);
						}


				}
				else
				{
					escribir_log(string_from_format("Se cierra la conexión con el Cliente luego del HandShake - Socket: %d ",cliente[i]));
					cerrar_conexion(cliente[i]);
					goto salir_handshake;
				}
	//Si es Cliente valido entra a interactuar con la MEMORIA. // Hilo

			pthread_create(&hiloEsperarMensaje[i], NULL, (void*) esperar_mensaje,(void *) cliente[i]);
			escribir_log(string_from_format("El Cliente %d tiene Hilo nro :%d ",cliente[i],i));

			// RECIBIR
			salir_handshake:
			i++;
		}

	//El select esta aca dentro --- ejecutar_server();
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

void esperar_mensaje(void *i) {

	int cliente = (int) i;
	int chau = 0;
	while (chau != 1 )
	{

	char *mensRec = malloc(1024);
	memset(mensRec,'\0', 1024);

	mensRec = recibir(cliente, &controlador);

	if(controlador > 0)
	{
		cerrar_conexion(cliente);
		chau =1;
		goto chau;
	}

	char *header = get_header(mensRec);
	int codigo = get_codigo(mensRec);

	printf("\n header: %s \n",header);
	printf("\n codigo: %d \n",codigo);

	switch (codigo) {
					case 1:
						{
							escribir_log(string_from_format("CASE 1: %d ",cliente));

						}
					break;
					case 2:
						{
							escribir_log(string_from_format("CASE 2: %d ",cliente));
						}
					break;
					case 3:
						{
							escribir_log(string_from_format("CASE 3: %d ",cliente));

						}
					break;
					case 4:
						{
							escribir_log(string_from_format("CASE 4: %d ",cliente));

						}
					break;
					case 5:
						{
							escribir_log(string_from_format("CASE 5: %d ",cliente));

						}
					break;

						}

					chau:
				printf("HOLA");
	}
	}


