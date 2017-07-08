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
int posicion_pagina;

t_bloque *find_first_fit(t_list *hs, int t_sol);
void reservar_memoria_din(t_program *program, int size_solicitado);
void inicializar_pagina_dinamica(t_program *prog);
void pedir_pagina_dinamica(t_program *prog);
int ubicar_bloque(t_pagina *pagina,int tam_sol);
void compactar(t_pagina *pagina);
void _free_bloque(t_bloque *bloque);
int pedir_pagina();
void liberar_bloque(t_program *prog, int offset);
//void liberar_pagina(t_program *)


void pedir_pagina_dinamica(t_program *prog)
{
	//armar mensaje
	inicializar_pagina_dinamica(prog);
}

void inicializar_pagina_dinamica(t_program *prog)
{
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->heaps = list_create();
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
	if (size_solicitado < (tam_pagina -10))
	{
		if (!list_is_empty(program->memoria_dinamica))
		{
			int size_disponible;
			int n = 0;
			int size_lpages = list_size(program->memoria_dinamica);
			int ubicado = 0;

			while(n < size_lpages && ubicado == 0)
			{
				t_pagina *page = list_get(program->memoria_dinamica, n);
				size_disponible = page->esp_libre;

				if (size_disponible >= size_solicitado)
					ubicado = ubicar_bloque(page, size_solicitado);

				else n++;
			}

			n = 0;
			while(n < size_lpages && ubicado == 0)
			{
				t_pagina *page_ = list_get(program->memoria_dinamica, n);
				compactar(page_);
				n++;
			}

			n = 0;
			while(n < size_lpages && ubicado == 0)
			{
				t_pagina *page = list_get(program->memoria_dinamica, n);
				size_disponible = page->esp_libre;

				if (size_disponible >= size_solicitado)
					ubicado = ubicar_bloque(page, size_solicitado);

				else n++;
			}
		}
		else
		{
			int pedido = pedir_pagina(program);

			if(pedido)
				inicializar_pagina_dinamica(program);
			else
				forzar_finalizacion(program->PID, 0, 5, 1);
		}
	}else
		forzar_finalizacion(program->PID, 0, 8, 1);
}

int ubicar_bloque(t_pagina *pagina,int tam_sol)//usa algoritmo first fit -> el resumen dice que es el mas kpo
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

			list_add_in_index(pagina->heaps, posicion_pagina, bl);
			pagina->esp_libre = pagina->esp_libre - (tam_sol+5);
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
	posicion_pagina = 0;

	bool _first_fit(t_bloque h)
	{
		bool libre = h.metadata->isFree;
		bool entra = (t_sol <= h.metadata->size);
		posicion_pagina++;
		return (libre && entra);
	}
	t_bloque *nes = list_find(hs, (void *)_first_fit);
	return nes;
}

void compactar(t_pagina *pagina)
{
	t_list * pagina_aux = list_create();
	list_add_all(pagina_aux, pagina->heaps);
	int n = 0;

	int contador_libre = 0;
	int cont_bl_libres = 0;

	while(!list_is_empty(pagina_aux))
	{
		t_bloque *bloque = list_remove(pagina_aux, n);

		if(bloque->metadata->isFree)
		{
			contador_libre =+ bloque->metadata->size;
			list_remove_and_destroy_element(pagina->heaps, n, (void *)_free_bloque);
			cont_bl_libres ++;
		}
	}
	t_bloque *bloque_free = malloc(sizeof(t_bloque));
	bloque_free->metadata = malloc(sizeof(HeapMetadata));
	bloque_free->metadata->isFree = true;
	bloque_free->metadata->size = contador_libre + (cont_bl_libres - 1)*(5);

	list_add(pagina->heaps, bloque_free);
}

void _free_bloque(t_bloque *bloque)
{
	free(bloque->metadata);
	free(bloque);
}

int pedir_pagina(t_program *program)
{
	int res, controlador = 0;
	char *mensaje = strdup("K19");

	char *pid_aux = string_itoa(program->PID);
	int size_pid = string_length(pid_aux);
	char *completar = string_repeat('0', 4 - size_pid);

	string_append(&mensaje, completar);
	string_append(&mensaje, pid_aux);
	string_append(&mensaje, "0001");

	enviar(config->cliente_memoria, mensaje, &controlador);

	if (controlador > 0)
		return 0;
	else
	{
		controlador = 0;
		char *respuesta = recibir(config->cliente_memoria, &controlador);
		char *codigo = get_codigo(respuesta);
		int cod = atoi(mensaje);
		if(cod == 2)
		{
			res = 1;
		}else res = 0;

		free(respuesta);
		free(codigo);
	}
	free(mensaje);
	free(completar);
	free(pid_aux);

	return res;
}

void liberar_bloque(t_program *prog, int offset)
{

}
