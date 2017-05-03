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
void reservar_memoria_din(t_program *program, int size_solicitado)
{
	if (!list_is_empty(program->memoria_dinamica))
	{
		int size_disponible;
		t_pagina *page = malloc(sizeof(t_pagina));
		page = list_get(program.memoria_dinamica, (list_size(program.memoria_dinamica)-1));
		size_disponible = page->esp_libre;

		if (size_disponible >= size_solicitado)
		{
			// acá habría que ver si el espacio está contiguo y si no lo está qué mierda hacer :)
		} // también dudo si pedir si o sí en la ultima o fijarme si se liberó memoria en otra pagina, dudoooo
		free(page);
	}else ;//pedir página a memoria
}
