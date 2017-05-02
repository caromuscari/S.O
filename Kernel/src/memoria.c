#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"
#include "configuracion.h"
#include "socket.h"
#include "manejo_errores.h"
#include "log.h"

extern t_configuracion *config;

void handshakearMemory()
{
	int controlador = 0;
	char *mensaje = malloc(7);
	strcpy(mensaje,"K02");
	enviar(config->cliente_memoria, mensaje, &controlador);
	if(controlador > 0) {	error_sockets(&controlador, "Memoria");	}
	free(mensaje);
}
void reservar_memoria_din(int pid, int size_solicitado)
{
	//si ya hay página asignada, verificar que haya suficiente tamaño
	//si no hay, pedir otra página a memoria
}
