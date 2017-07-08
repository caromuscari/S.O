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
#include "consolaManager.h"
#include "planificador.h"

extern t_configuracion *config;
extern int tam_pagina;
extern int pag_cod;
extern int pag_stack;

t_bloque *find_first_fit(t_list *hs, int t_sol);
void reservar_memoria_din(t_program *program, int size_solicitado);
void inicializar_pagina_dinamica(t_program *prog);
void pedir_pagina_dinamica(t_program *prog);
int ubicar_bloque(t_pagina *pagina,int tam_sol, int *flag);
void compactar(t_list *pagina);
void _free_bloque(t_bloque *bloque);
int pedir_pagina();
void liberar_bloque(t_program *prog, int offset);

void pedir_pagina_dinamica(t_program *prog)
{
	//armar mensaje
	inicializar_pagina_dinamica(prog);
}

void inicializar_pagina_dinamica(t_program *prog)
{
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->heaps = list_create();
	pagina->posiciones = dictionary_create();
	pagina->n_pagina = pag_cod + pag_stack + list_size(prog->memoria_dinamica);
	pagina->esp_libre = tam_pagina - 10;

	HeapMetadata *heap = malloc(sizeof(HeapMetadata));
	heap->size = tam_pagina - 10;
	heap->isFree = true;

	t_bloque *bloque = malloc(sizeof(t_bloque));
	bloque->metadata = heap;

	list_add(pagina->heaps, heap);
	list_add(prog->memoria_dinamica, bloque);
}

void reservar_memoria_din(t_program *program, int size_solicitado)
{
	if (size_solicitado < tam_pagina -10)
	{
		if (!list_is_empty(program->memoria_dinamica))
		{
			int size_disponible;
			int n = 0;
			int size_lpages = list_size(program->memoria_dinamica);
			int flag = 0;

			while(n < size_lpages && flag == 0)
			{
				t_pagina *page =  list_get(program->memoria_dinamica, n);
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
		}
		else
		{
			int pedido = pedir_pagina();
			if(pedido)
			{
				inicializar_pagina(program);
			}
			else
			{
				forzar_finalizacion(program->PID, 0, 8, 20); //ver codigo
			}
		}
	}else forzar_finalizacion(program->PID, 0, 8, 1);
}

int ubicar_bloque(t_pagina *pagina,int tam_sol, int *flag)//usa algoritmo first fit -> el resumen dice que es el mas kpo
{
	t_bloque *bloque = malloc (sizeof(t_bloque));

	bloque = find_first_fit(pagina->heaps, tam_sol);

	if (bloque != NULL)
	{
		int sz = bloque->metadata->size;
		bloque->metadata->isFree = false;
		bloque->data = malloc(tam_sol);
		bloque->metadata->size = tam_sol;

		if (tam_sol < sz)
		{
			t_bloque *bl = malloc(sizeof(t_bloque));
			bl->metadata->isFree = true;
			bl->metadata->size = sz - tam_sol;

			list_add(pagina->heaps, bl);
		}
		return 1;
	}
	else
	{
		free(bloque);
		return 0;
	}

}

t_bloque *find_first_fit(t_list *hs, int t_sol)
{
	bool _first_fit(t_bloque h)
	{
		bool libre = h.metadata->isFree;
		bool entra = (t_sol <= h.metadata->size);
		return (libre && entra);
	}
	t_bloque *nes = list_find(hs, (void *)_first_fit);
	return nes;
}

void compactar(t_list *pagina)
{
	t_list * pagina_aux = list_create();
	list_add_all(pagina_aux, pagina);
	int n = 0;

	int contador_libre = 0;
	int cont_bl_libres = 0;

	while(!list_is_empty(pagina_aux))
	{
		t_bloque *bloque = list_remove(pagina_aux, n);

		if(bloque->metadata->isFree)
		{
			contador_libre =+ bloque->metadata->size;
			list_remove_and_destroy_element(pagina, n, (void *)_free_bloque);
			cont_bl_libres ++;
		}
	}
	t_bloque *bloque_free = malloc(sizeof(bloque_free));
	bloque_free->metadata = malloc(sizeof(HeapMetadata));
	bloque_free->metadata->isFree = true;
	bloque_free->metadata->size = contador_libre + (cont_bl_libres - 1)*(5);

	list_add(pagina, bloque_free);
}

void _free_bloque(t_bloque *bloque)
{
	free(bloque->metadata);
	free(bloque);
}

int pedir_pagina()
{
	int controlador = 0;
	char *mensaje = "K05";
	char *respuesta;
	enviar(config->cliente_memoria, mensaje, &controlador);
	if (controlador > 0)
	{
		error_sockets(&controlador,"Memoria");
		return 0;
	}
	else
	{
		controlador = 0;
		respuesta = recibir(config->cliente_memoria, &controlador);
		char *codigo = get_codigo(respuesta);
		int cod = atoi(mensaje);
		int res;
		if(cod == 2)
		{
			res = 1;
		}else res = 0;
		free(respuesta);
		free(codigo);
		return res;
	}
}
void liberar_bloque(t_program *prog, int offset)
{

}
