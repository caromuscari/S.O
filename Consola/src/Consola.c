/*
 ============================================================================
 Name        : Consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "estructuras.h"
#include "socket_client.h"



t_consola arch_config;
int socket_;

int main(int argc, char * argv[]) {

/*if (argc == 1)
	{
		printf("Falta la ruta del archivo de configuración");
		return 1;
	}*/

	inicializar_parametros();
	leer_archivo_configuracion(argv[1]);
	socket_ = iniciar_socket_cliente(arch_config.ip, arch_config.puerto);

}

void inicializar_parametros()
{
	arch_config.ip= malloc(sizeof * arch_config.ip);
	arch_config.puerto= malloc(sizeof * arch_config.puerto);
}

void leer_archivo_configuracion(char * ruta)
{
	t_config * configConsola = config_create(ruta);
	arch_config.ip = config_get_string_value(configConsola,"IP_KERNEL");
	arch_config.puerto = config_get_string_value(configConsola,"PUERTO_KERNEL");
	printf(arch_config.ip);
	printf(arch_config.puerto);
	config_destroy(configConsola);

}
