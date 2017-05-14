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
#include "mensaje.h"

extern t_configuracion *config;
int tam_pagina = 0;

void manejar_respuesta(char *);
void handshakearMemory();
void pedir_tamano_pag();
void reservar_memoria_din(t_program *, int);
int ubicar_bloque(t_pagina *,int, int *);
/*HeapMetadata*/ void * find_first_fit(t_list *hs, int t_sol);

void handshakearMemory()
{
	int controlador = 0;
	char *mensaje = malloc(7);
	strcpy(mensaje,"K02");
	enviar(config->cliente_memoria, mensaje, &controlador);
	if(controlador > 0) error_sockets(&controlador, "Memoria");
	free(mensaje);
}

void pedir_tamano_pag()
{
	int controlador = 0;
	char *mensaje = "K01";
	char *respuesta;
	enviar(config->cliente_memoria, mensaje, &controlador);
	if (controlador > 0) error_sockets(&controlador,"Memoria");
	else
	{
		controlador = 0;
		respuesta = recibir(config->cliente_memoria, &controlador);
		manejar_respuesta(respuesta);
	}
}

void reservar_memoria_din(t_program *program, int size_solicitado)
{
	if (!list_is_empty(program->memoria_dinamica))
	{
		int size_disponible;
		int n = 0;
		int size_lpages = list_size(program->memoria_dinamica);
		int flag = 0;
		while(n < size_lpages && flag == 0)
		{
			t_pagina *page = malloc(sizeof(t_pagina));
			page = list_get(program->memoria_dinamica, n);
			size_disponible = page->esp_libre;

			if (size_disponible >= size_solicitado)
			{
				ubicar_bloque(page, size_solicitado, &flag);

			}
			else n++;
			free(page);
		}
	}//else ;//pedir página a memoria
}

void manejar_respuesta(char *respuesta)
{
	int codigo = get_codigo(respuesta);
	char *mensaje = strdup("");
	mensaje = get_mensaje(respuesta);
	int tam_page;
	switch (codigo)
	{
		case 0:
			tam_page = atoi(mensaje);
			tam_pagina = tam_page;
			break;
		default:
			printf("Mensaje desconocido");
			//desconectar de memoria????? o ke? que no pase esto plis
	}
	free(mensaje);
}

int ubicar_bloque(t_pagina *pagina,int tam_sol, int *flag)//usa algoritmo first fit -> el resumen dice que es el mas kpo
{
//	int flag = 0;
//	int c = 0;

//	int cant_heaps = list_size(pagina->heaps);
	HeapMetadata *heap = malloc (sizeof(HeapMetadata));

	heap = find_first_fit(pagina->heaps, tam_sol);
	//	while(flag == 0 && c < cant_heaps)
	if (heap != NULL)
	{

		free(heap);
		return 1;

	}
	else {
		free(heap);
		return 0;
	}

}

/*HeapMetadata */ void *find_first_fit(t_list *hs, int t_sol)
{
	bool _first_fit(HeapMetadata h){
		bool libre = h.isFree;
		bool entra = (t_sol <= h.size);
		return (libre && entra);
	}
	return list_find(hs, (void *)_first_fit);
}

