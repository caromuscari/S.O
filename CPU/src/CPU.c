

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"
#include "log.h"
#include "funcionesCPU.h"
#include <commons/config.h>

int puertoK,puertoM;
char *ipK;
char *ipM;
t_config *configuracion;
int * controlador=0;


void leerArchivoConfiguracion(char* argv);


int main(int argc, char *argv[])
{
	int controladorConexion = 0;
	crear_archivo_log("/home/utnso/CPUlog");
	leerArchivoConfiguracion(argv[1]);
	int sockKerCPU = iniciar_socket_cliente(ipK, puertoK, &controladorConexion);
	if(controladorConexion == 0){
		escribir_log("Exitos conectandose al Kernel",1);
	}else{
		escribir_log("Error conectandose al kernel (implementar manejador de errores en brevedad)",2);
	}
//	conectarse con memoria
//	iniciar_socket_cliente(ipM, puertoM, controladorConexion);
	handshakeKernel(sockKerCPU);


}

void leerArchivoConfiguracion(char* argv)
{
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

