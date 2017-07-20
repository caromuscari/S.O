/*#include <stdio.h>
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
//extern int pag_cod;
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
int almacenar_bytes(int pid, int numpag, int offset, int cant, int bool_free);
char *pedir_bytes_memoria(int pid, int numpag, int offset);
char *buffer_bloque(int size, int booleano);

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

	enviar(config->cliente_memoria, men_env, &contr);

	free(men_env);
	free(pid_);
	free(numpag_);
	free(offset_);

	return recibir(config->cliente_memoria, &contr);
}

void inicializar_pagina_dinamica(t_program *prog, int size_sol)
{
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->heaps = list_create();
	pagina->n_pagina = prog->pcb->cant_pag + pag_stack + list_size(prog->memoria_dinamica);
	pagina->esp_libre = tam_pagina - 10;

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
		int offset = pagina->n_pagina * tam_pagina + inicio_bloque + 5;
		char *offs_ = string_itoa(offset);
		dictionary_put(program->posiciones, offs_ ,infheap);
		char *mens = armar_mensaje("K99", offs_);
		int contr = 0;
		//int pid, int numpag, int offset, int cant, int bool_free

		char *buffer = buffer_bloque(bloque->metadata->size, 0);
		int respuesta = almacenar_bytes(program->PID, pagina->n_pagina, offset, 5, buffer);
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
			almacenar_bytes(program->PID, pagina->n_pagina, offset, 5, buffer);
			free(buffer);

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
*/
