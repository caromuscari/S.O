

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"
#include <commons/config.h>

int puerto;
t_config *configuracion;


void leerArchivoConfig(char* argv);


int main(int argc, char *argv[])
{
	leerArchivoConfig(argv[1]);
	int socket_Kernel = iniciar_socket_cliente("127.0.0.1",puerto);

	char mensaje[3];
	memcpy(mensaje,"CPU",3);
	printf(mensaje,"%s");
	int res= enviar(socket_Kernel,mensaje);

	printf("Mensaje enviado al Kernelcito");
}

void leerArchivoConfig(char* argv)
{
	printf("ruta archivo de configuacion: %s \n", argv);
	configuracion = config_create(argv);
	puerto = config_get_int_value(configuracion, "PUERTO");
	printf("Valor puerto para conexion del KERNEL: %d \n", puerto);
	config_destroy(configuracion);
}
