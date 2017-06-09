#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
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
void reservar_memoria_din(t_program *, int);
int ubicar_bloque(t_pagina *,int, int *);
HeapMetadata*find_first_fit(t_list *hs, int t_sol);
void pedir_pagina();
void ubicar_en_pagina(t_dictionary *, char *, int);
void crear_dict_pagina(t_dictionary *);

void handshakearMemory()
{
	int controlador = 0;
	char *mensaje = "K02";
	char *respuesta;

	enviar(config->cliente_memoria, mensaje, &controlador);
	respuesta = recibir(config->cliente_memoria, &controlador);
	manejar_respuesta(respuesta);
}

void pedir_pagina()
{
	int controlador = 0;
	char *mensaje = "K05";
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
				int ubicado = ubicar_bloque(page, size_solicitado, &flag);

				if (ubicado)
				{
					flag = 0;
				}

			}
			else n++;
			free(page);
		}
	}else pedir_pagina();
}

void manejar_respuesta(char *respuesta)
{
	int codigo = get_codigo(respuesta);
	char *mensaje = strdup("");
	mensaje = get_mensaje(respuesta);

	switch (codigo)
	{
		case 0:
			tam_pagina = atoi(mensaje);
			break;
		case 1:
			//cómo se a que programa le estoy asignando memoria
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
	else
	{
		free(heap);
		return 0;
	}

}

HeapMetadata *find_first_fit(t_list *hs, int t_sol)
{
	bool _first_fit(HeapMetadata h){
		bool libre = h.isFree;
		bool entra = (t_sol <= h.size);
		return (libre && entra);
	}
	HeapMetadata *nes = list_find(hs, (void *)_first_fit);
	return nes;
}

void crear_dict_pagina(t_dictionary *pos)
{
	pos = dictionary_create();
}

void ubicar_en_pagina(t_dictionary *posiciones, char *first_pos, int pos)
{
	dictionary_put(posiciones, first_pos, (void *)pos);
}

void compactar(t_list *paginas)
{
	t_list *pages_aux = paginas;
	int size = list_size(pages_aux);
	int n = 0;

}
