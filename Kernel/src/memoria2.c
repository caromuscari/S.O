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
int inicio_bloque;


void handshakearMemory();
t_bloque *find_first_fit(t_list *hs, int t_sol);
void reservar_memoria_din(t_program *program, int size_solicitado, int so_cpu);
void inicializar_pagina_dinamica(t_program *prog, int size_sol);
int ubicar_bloque(t_pagina *pagina, int tam_sol, t_program *program, int so_cpu);
void compactar(t_pagina *pagina);
void _free_bloque(t_bloque *bloque);
int pedir_pagina();
void liberar_bloque(t_program *prog, char *offset);
t_pagina *_buscar_pagina(t_list *mem, int num_pag);
void destruir_heap(t_bloque *bl);
void liberar_pagina(t_pagina *pagina);
int chequear_pagina(t_pagina *page);
void juntar_memoria(t_list *hp, t_bloque *blo, t_bloque *blo_liberado, int num_bloque, bool anterior);

void handshakearMemory()
{
	int controlador = 0;
	char *mensaje = armar_mensaje("K00","");
	char *respuesta;

	enviar(config->cliente_memoria, mensaje, &controlador);
	respuesta = recibir(config->cliente_memoria, &controlador);
	char *codigo = get_codigo(respuesta);
	int cod = atoi(codigo);
	char *mensaje33 = get_mensaje(respuesta);

	if(cod == 0)
		tam_pagina = atoi(mensaje33);

	free(mensaje);
	free(mensaje33);
}

void inicializar_pagina_dinamica(t_program *prog, int size_sol)
{
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->heaps = list_create();
	pagina->n_pagina = pag_cod + pag_stack + list_size(prog->memoria_dinamica);
	pagina->esp_libre = tam_pagina - 10;

	HeapMetadata *heap = malloc(sizeof(HeapMetadata));
	heap->size = tam_pagina - 10;
	heap->isFree = 1;

	t_bloque *bloque = malloc(sizeof(t_bloque));
	bloque->metadata = heap;
	bloque->data = malloc(heap->size);

	HeapMetadata *hm_libre = malloc(sizeof(HeapMetadata));
	hm_libre->isFree = 1;
	hm_libre->size = 0;

	t_bloque *bloque_free = malloc(sizeof(t_bloque));
	bloque_free->metadata = hm_libre;

	list_add(pagina->heaps, bloque);
	list_add(pagina->heaps, bloque_free);
	list_add(prog->memoria_dinamica, pagina);
	//reservar_memoria_din(prog, size_sol);
}

void reservar_memoria_din(t_program *program, int size_solicitado, int so_cpu)
{
	int ubicado = 0;
	program->syscall++;

	if (size_solicitado < (tam_pagina -10))
	{
		if (!list_is_empty(program->memoria_dinamica))
		{
			int size_disponible;
			int n = 0;
			int size_lpages = list_size(program->memoria_dinamica);

			while(n < size_lpages && ubicado == 0)
			{
				t_pagina *page = list_get(program->memoria_dinamica, n);
				size_disponible = page->esp_libre;

				if (size_disponible >= size_solicitado)
				{
					ubicado = ubicar_bloque(page, size_solicitado, program, so_cpu);
				}
				else n++;
			}
		}
		else
		{
			int pedido = pedir_pagina(program);

			if(pedido)
			{
				inicializar_pagina_dinamica(program, size_solicitado);
				int size_disponible;
				int n = 0;
				int size_lpages = list_size(program->memoria_dinamica);

				while(n < size_lpages && ubicado == 0)
				{
					t_pagina *page = list_get(program->memoria_dinamica, n);
					size_disponible = page->esp_libre;

					if (size_disponible >= size_solicitado)
					{
						ubicado = ubicar_bloque(page, size_solicitado, program, so_cpu);
					}
					else n++;
				}
			}
			else
				forzar_finalizacion(program->PID, 0, 5, 0);
		}
	}
	else
	{
		forzar_finalizacion(program->PID, 0, 8, 0);
	}
}

int ubicar_bloque(t_pagina *pagina, int tam_sol, t_program *program, int so_cpu)//usa algoritmo first fit -> el resumen dice que es el mas kpo
{
	t_bloque *bloque;// = malloc (sizeof(t_bloque));
	bloque = find_first_fit(pagina->heaps, tam_sol);

	if (bloque != NULL)
	{
		program->allocs++;
		int sz = bloque->metadata->size;
		bloque->metadata->isFree = 0;
		free(bloque->data);
		bloque->data = malloc(tam_sol);
		bloque->metadata->size = tam_sol;

		t_infoheap *infheap = malloc(sizeof(t_infoheap));
		infheap->bloque = posicion_pagina;
		infheap->pagina = pagina->n_pagina;
		int offset = pagina->n_pagina*tam_pagina + inicio_bloque + 5;
		char *offs_ = string_itoa(offset);
		dictionary_put(program->posiciones, offs_ ,infheap);
		char *mens = armar_mensaje("K99", offs_);
		int contr = 0;
		enviar(so_cpu, mens, &contr);
		free(mens);
		if (tam_sol < sz)
		{
			t_bloque *bl = malloc(sizeof(t_bloque));
			bl->metadata = malloc(sizeof(HeapMetadata));
			bl->metadata->isFree = 1;
			bl->metadata->size = sz - tam_sol;

			list_add_in_index(pagina->heaps, posicion_pagina, bl);
			pagina->esp_libre = pagina->esp_libre - (tam_sol+5);
		}
		return offset;
	}
	else
	{
		free(bloque);
		return 0;
	}
}

t_bloque *find_first_fit(t_list *hs, int t_sol)
{
//	t_bloque *nes;
//	int encontrado = 1;
	posicion_pagina = 0;
	inicio_bloque  = 0;

	bool _first_fit(t_bloque *h)
	{
		//bool libre = h->metadata->isFree;
		//bool entra = (t_sol <= h->metadata->size);
		posicion_pagina++;
		if(!(h->metadata->isFree==1) || (!(t_sol <= h->metadata->size)))
			inicio_bloque =+ h->metadata->size;

		return ((h->metadata->isFree==1) && ((t_sol <= h->metadata->size)));
	}

	//list_iterate(hs, (void *)_first_fit);
	return list_find(hs, (void *)_first_fit);//nes;
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
	bloque_free->metadata->isFree = 1;
	bloque_free->metadata->size = contador_libre + (cont_bl_libres - 1)*(5);

	list_add(pagina->heaps, bloque_free);
}

void _free_bloque(t_bloque *bloque)
{
	free(bloque->data);
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
		int cod = atoi(codigo);
		if(cod == 2)
			res = 1;
		else
			res = 0;

		free(respuesta);
		free(codigo);
	}
	free(mensaje);
	free(completar);
	free(pid_aux);

	return res;
}

void liberar_bloque(t_program *prog, char *offset)
{
	prog->syscall++;

	t_infoheap *heap = dictionary_get(prog->posiciones, offset);
	t_pagina *page = _buscar_pagina(prog->memoria_dinamica, heap->pagina);

	if(page != NULL)
	{
		prog->frees++;
		t_bloque *bloque = list_get(page->heaps, heap->bloque);
		free(bloque->data);
		bloque->metadata->isFree = true;
		if((list_size(page->heaps) - 1) > heap->bloque && heap->bloque != 0)
		{
			t_bloque *bloque2 = list_get(page->heaps, (heap->bloque - 1));
			t_bloque *bloque3 = list_get(page->heaps, (heap->bloque + 1));

			if(bloque2->metadata->isFree)
			{
				juntar_memoria(page->heaps, bloque2, bloque, heap->bloque, true);
			}
			if(bloque3->metadata->isFree)
			{
				juntar_memoria(page->heaps, bloque3, bloque, heap->bloque, false);
			}
		}
		if(heap->bloque == 0)
		{
			t_bloque *bloque4 = list_get(page->heaps, (heap->bloque + 1));
			juntar_memoria(page->heaps, bloque4, bloque, heap->bloque, false);
		}
		if(heap->bloque == (list_size(page->heaps) -1))
		{
			t_bloque *bloque5 = list_get(page->heaps, (heap->bloque - 1));
			juntar_memoria(page->heaps, bloque5, bloque, heap->bloque, true);
		}

		if(chequear_pagina(page))
		{
			list_remove_and_destroy_element(prog->memoria_dinamica, (page->n_pagina - pag_cod - pag_stack -1),(void *) liberar_pagina);

			int controlador;
			char *mensaje = strdup("K24");
			char *pid_aux = string_itoa(prog->PID);
			int size_pid = string_length(pid_aux);
			char *completar_pid = string_repeat('0', 4 - size_pid);
			char *pagina = string_itoa(page->n_pagina);
			int size_pagina = string_length(pagina);
			char *completar = string_repeat('0', 4 - size_pagina);
			string_append(&mensaje, completar_pid);
			string_append(&mensaje, pid_aux);
			string_append(&mensaje, completar);
			string_append(&mensaje, pagina);

			enviar(config->cliente_memoria, mensaje, &controlador);

			free(completar_pid);
			free(pid_aux);
			free(completar);
			free(pagina);
			free(mensaje);
		}
	}
	else
	{
		forzar_finalizacion(prog->PID, 0, 5, 1);//el error puede estar mal!
	}
}

void liberar_pagina(t_pagina *pagina)
{
	//comeme el k26 usar este para enviar liberar pagina a Memoria
	list_destroy_and_destroy_elements(pagina->heaps, (void *)destruir_heap);
	free(pagina);
}

void destruir_heap(t_bloque *bl)
{
	free(bl->metadata);
	free(bl);
}
t_pagina *_buscar_pagina(t_list *mem, int num_pag)
{
	bool _pagina(t_pagina *pag)
	{
		return pag->n_pagina == num_pag;
	}

	return list_find(mem,(void *)_pagina);
}

int chequear_pagina(t_pagina *page)
{
	bool _esta_libre(t_bloque bloque)
	{
		return bloque.metadata->isFree;
	}

	if(list_all_satisfy(page->heaps, (void *)_esta_libre))
	{
		return 1;
	}
	else
		return 0;
}

void lib_bloque(t_bloque *bl)
{
	free(bl->metadata);
	free(bl);
}

void juntar_memoria(t_list *hp, t_bloque *blo, t_bloque *blo_liberado, int num_bloque, bool anterior)
{
	if(anterior)
	{
		blo->metadata->size =+ blo_liberado->metadata->size;
		free(blo->data);
		blo->data = malloc(blo->metadata->size);

		list_remove_and_destroy_element(hp, num_bloque, (void *)lib_bloque);
	}
	else
	{
		blo_liberado->metadata->size =+ blo->metadata->size;
		blo_liberado->data = malloc(blo_liberado->metadata->size);

		list_remove_and_destroy_element(hp, num_bloque - 1, (void *)lib_bloque);
	}
}
