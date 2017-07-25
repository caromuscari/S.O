#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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
//extern int pag_cod;
extern int pag_stack;
int posicion_pagina;
int inicio_bloque;

void handshakearMemory();
t_bloque *find_first_fit(t_list *hs, int t_sol);
void reservar_memoria_din(t_program *program, int size_solicitado, int so_cpu);
void inicializar_pagina_dinamica(t_program *prog, int size_sol);
int ubicar_bloque(t_pagina *pagina, int tam_sol, t_program *program, int so_cpu);
void _free_bloque(t_bloque *bloque);
int pedir_pagina();
void liberar_bloque(t_program *prog, char *offset);
t_pagina *_buscar_pagina(t_list *mem, int num_pag);
void destruir_heap(t_bloque *bl);
void liberar_pagina(t_pagina *pagina);
int chequear_pagina(t_pagina *page);
void juntar_memoria(t_list *hp, t_bloque *blo, t_bloque *blo_liberado, int num_bloque, bool anterior);
int almacenar_bytes(int pid, int numpag, int offset, int tam, char *buffer);
char *buffer_bloque(int size, int booleano);
void *pedir_bloque_libre(t_pagina *pagina, int pid, int tam_sol);
char *pedir_bytes_memoria(int pid, int numpag, int offset);
HeapMetadata *armar_metadata(char *metadata);
void liberar_proceso_pagina(int pid);
void compactar_contiguos(int pid, t_pagina *pagina);

void handshakearMemory()
{
	int controlador = 0;
	char res[13];

	enviar(config->cliente_memoria, "K00", &controlador);
	recv(config->cliente_memoria, res, 13, 0);
	//respuesta = recibir(config->cliente_memoria, &controlador);
	char *prim = strdup(res);
	char *str_size = string_substring(prim, 3, 10);
	int size = atoi(str_size);

	char res2[size];

	recv(config->cliente_memoria, res2, size, 0);

	char *respuesta = strdup(prim);
	string_append(&respuesta,res2);

	char *codigo = get_codigo(respuesta);
	int cod = atoi(codigo);
	char *mensaje33 = get_mensaje(respuesta);

	if(cod == 0)
		tam_pagina = atoi(mensaje33);

	free(codigo);
	free(mensaje33);
	free(respuesta);
	free(prim);
	free(str_size);
}

void inicializar_pagina_dinamica(t_program *prog, int size_sol)
{
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->heaps = list_create();
	pagina->n_pagina = prog->pcb->cant_pag + pag_stack + list_size(prog->memoria_dinamica);
	pagina->esp_libre = tam_pagina - 5;

	HeapMetadata *heap = malloc(sizeof(HeapMetadata));
	heap->size = tam_pagina - 5;
	heap->isFree = 1;

	t_bloque *bloque = malloc(sizeof(t_bloque));
	bloque->metadata = heap;
	bloque->data = malloc(heap->size);

	list_add(pagina->heaps, bloque);
	list_add(prog->memoria_dinamica, pagina);

	char *buffer = buffer_bloque(heap->size, heap->isFree);

	int resultado = almacenar_bytes(prog->PID,pagina->n_pagina,0,5,buffer);
	free(buffer);

	if(resultado==3)
	{
		forzar_finalizacion(prog->PID,0,5,0);
	}
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

				if (size_disponible-5 >= size_solicitado)
				{
					ubicado = ubicar_bloque(page, size_solicitado, program, so_cpu);
				}
				else n++;
			}
			if (ubicado == 0)
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

						if (size_disponible-5 >= size_solicitado)
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

					if (size_disponible-5 >= size_solicitado)
					{
						ubicado = ubicar_bloque(page, size_solicitado, program, so_cpu);
						if(!ubicado)
						{
							compactar_contiguos(program->PID, page);
							ubicado = ubicar_bloque(page, size_solicitado, program, so_cpu);
						}
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
		ubicado = 0;
	}
}

int ubicar_bloque(t_pagina *pagina, int tam_sol, t_program *program, int so_cpu)//usa algoritmo first fit -> el resumen dice que es el mas kpo
{
	t_bloque *bloque;// = malloc (sizeof(t_bloque));
	//bloque = find_first_fit(pagina->heaps, tam_sol);
	bloque = pedir_bloque_libre(pagina, program->PID, tam_sol);

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
		int offset = pagina->n_pagina * tam_pagina + inicio_bloque + 5;
		char *offs_ = string_itoa(offset);
		dictionary_put(program->posiciones, offs_ ,infheap);
		char *mens = armar_mensaje("K99", offs_);
		int contr = 0;
		//int pid, int numpag, int offset, int cant, int bool_free

		char *buffer = buffer_bloque(bloque->metadata->size, 0);
		int respuesta = almacenar_bytes(program->PID, pagina->n_pagina, inicio_bloque, 5, buffer);
		free(buffer);

		if(respuesta==3)
		{
			forzar_finalizacion(program->PID,0,5,0);
			return 0;
		}

		enviar(so_cpu, mens, &contr);
		free(mens);

		if (tam_sol < sz)
		{
			t_bloque *bl = malloc(sizeof(t_bloque));
			bl->metadata = malloc(sizeof(HeapMetadata));
			bl->metadata->isFree = 1;
			bl->metadata->size = sz - tam_sol;

			char *buffer = buffer_bloque(bl->metadata->size, 1);
			almacenar_bytes(program->PID, pagina->n_pagina, (inicio_bloque + tam_sol + 5), 5, buffer);
			free(buffer);

			list_add_in_index(pagina->heaps, posicion_pagina, bl);
			pagina->esp_libre = pagina->esp_libre - (tam_sol+5);
		}
		return 1; //offset;
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
		char *bytes = pedir_bytes_memoria(prog->PID, page->n_pagina, (atoi(offset)/tam_pagina)-5);
		HeapMetadata *meta = armar_metadata(bytes);
		t_bloque *bloque = malloc(sizeof(t_bloque));//list_get(page->heaps, heap->bloque);
		bloque->metadata = meta;
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
			list_remove_and_destroy_element(prog->memoria_dinamica, (page->n_pagina - prog->pcb->cant_pag - pag_stack -1),(void *) liberar_pagina);

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
		forzar_finalizacion(prog->PID, 0, 5, 0);//el error puede estar mal!
	}
}

void compactar_contiguos(int pid, t_pagina *pagina)
{
	int fin_raiz = 1;
	int offset = 0;
	HeapMetadata *h_anterior;

	while(fin_raiz)
	{
		fin_raiz = 0;
		offset = 0;
		h_anterior = NULL;

		while(offset < tam_pagina)
		{
			char *info_bloque = pedir_bytes_memoria(pid, pagina->n_pagina, offset);
			char *metadata = get_mensaje(info_bloque);
			HeapMetadata *heapMeta = armar_metadata(metadata);

			if((heapMeta->isFree)&&((h_anterior != NULL)))
			{
				int size = heapMeta->size + h_anterior->size + 5;
				char *buffer = buffer_bloque(size, 1);
				almacenar_bytes(pid, pagina->n_pagina, offset, 5, buffer);

				fin_raiz = 1;
				free(buffer);
				break;
			}
			else if(heapMeta->isFree)
			{
				h_anterior = heapMeta;
			}
			else//porque es false
			{
				h_anterior = NULL;
			}

			offset = offset + heapMeta->size + 5;
			free(info_bloque);
			free(metadata);
			free(heapMeta);
		}
	}
}

void liberar_pagina(t_pagina *pagina)
{
	//comeme el k25 usar este para enviar liberar pagina a Memoria
	list_destroy_and_destroy_elements(pagina->heaps, (void *)destruir_heap);
	free(pagina);
}

void liberar_proceso_pagina(int pid)
{
	int contr = 0;
	char *men_env = strdup("K25");

	char *pid_ = string_itoa(pid);
	int len = string_length(pid_);
	char *completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, pid_);

	enviar(config->cliente_memoria, men_env, &contr);

	free(men_env);
	free(pid_);
	free(completar);

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

int almacenar_bytes(int pid, int numpag, int offset, int tam, char *buffer)
{
	int contr = 0;
	char *men_env = strdup("K90");

	char *pid_ = string_itoa(pid);
	int len = string_length(pid_);
	char *completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, pid_);

	free(completar);

	char *numpag_ = string_itoa(numpag);
	len = string_length(numpag_);
	completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, numpag_);

	free(completar);

	char *offset_ = string_itoa(offset);
	len = string_length(offset_);
	completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, offset_);

	free(completar);

	char *tam_ = string_itoa(tam);
	len = string_length(tam_);
	completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, tam_);

	free(completar);
	string_append(&men_env, buffer);

	enviar(config->cliente_memoria, men_env, &contr);

	char *men_rec = recibir(config->cliente_memoria, &contr);

	free(men_env);
	free(pid_);
	free(numpag_);
	free(offset_);
	free(tam_);

	char *codigo = get_codigo(men_rec);
	int int_codigo = atoi(codigo);

	if(int_codigo==3)
		escribir_log_error_con_numero("No se pudieron alocar los bytes para el PID: ",pid);
	else
		escribir_log_con_numero("Se logro alocar los bytes para el PID: ",pid);

	free(codigo);
	free(men_rec);
	return int_codigo;
}

char *pedir_bytes_memoria(int pid, int numpag, int offset)
{
	int contr = 0;
	char *men_env = strdup("K94");

	char *pid_ = string_itoa(pid);
	int len = string_length(pid_);
	char *completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, pid_);

	free(completar);

	char *numpag_ = string_itoa(numpag);
	len = string_length(numpag_);
	completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, numpag_);

	free(completar);

	char *offset_ = string_itoa(offset);
	len = string_length(offset_);
	completar = string_repeat('0', 4 - len);

	string_append(&men_env, completar);
	string_append(&men_env, offset_);

	free(completar);

	char *tam = strdup("0005");
	string_append(&men_env, tam);

	enviar(config->cliente_memoria, men_env, &contr);

	free(men_env);
	free(pid_);
	free(numpag_);
	free(offset_);
	free(tam);
	return recibir(config->cliente_memoria, &contr);
}

char *buffer_bloque(int size, int booleano)
{
	char *metadata = string_itoa(size);
	int meta_len = string_length(metadata);
	char* completar = string_repeat('0', 4 - meta_len);

	char *char_meta = string_itoa(booleano);

	char *buffer = strdup(completar);
	string_append(&buffer, metadata);
	string_append(&buffer, char_meta);

	free(metadata);
	free(completar);
	free(char_meta);

	return buffer;
}

void *pedir_bloque_libre(t_pagina *pagina, int pid, int tam_sol)
{
	inicio_bloque = 0;
	int encontrado = 0;
	int fin = 0;
	int offset = 0;
	t_bloque *bloque;
	//char *pedir_bytes_memoria(int pid, int numpag, int offset)
	while(encontrado == 0 && fin == 0)
	{
		char *info_bloque = pedir_bytes_memoria(pid, pagina->n_pagina, offset);
		char *metadata = get_mensaje(info_bloque);
		HeapMetadata *heapMeta = armar_metadata(metadata);

		if(heapMeta->isFree && heapMeta->size >= tam_sol)
		{
			encontrado = 1;
			bloque = malloc(sizeof(t_bloque));
			bloque->metadata = heapMeta;
		}
		else
		{
			offset = offset + (5 + heapMeta->size);
			inicio_bloque = offset;
			if(offset > tam_pagina)
				fin = 1;
			free(heapMeta);
		}

		free(info_bloque);
		free(metadata);
	}

	if (fin)
		bloque = NULL;

	return bloque;
}

HeapMetadata *armar_metadata(char *metadata)
{
	HeapMetadata *metadaHeap = malloc(sizeof(HeapMetadata));
	char *cantidad = string_substring(metadata, 0, 4);
	int cant = atoi(cantidad);
	free(cantidad);
	char *free_ = string_substring(metadata, 4, 1);
	bool is_free;

	if (atoi(free_))
		is_free = true;
	else
		is_free = false;
	free(free_);

	metadaHeap->isFree = is_free;
	metadaHeap->size = cant;

	return metadaHeap;
}
