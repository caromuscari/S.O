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

extern t_list *global_fd;

void crear_tabla_global();
void abrir_archivo(char *path, char* flag, t_program *prog);
t_TAG *buscar_archivo_TAG(char *p_sol);
void crear_archivo(char *p);
void pedido_lectura(t_program *prog, int fd, int offs, int size);
t_TAP *buscar_archivo_TAP(t_list *tap, int fd);
char *get_path(int fd);
t_TAG *buscar_archivo_TAG_fd(int fd);
void mover_puntero(int socket_prog, int offset);
char *leer_escribir(int socket_prog);
bool existe_archivo(t_list *tap, int fd);
void destruir_file(t_TAP *ap);
void destruir_file_TAG(t_TAG *tg);
void cerrar_file(t_list *tap, int fd);

void crear_tabla_global()
{
	global_fd = list_create();
}

void abrir_archivo(char *path, char* flag, t_program *prog)
{
	t_TAG *ag = malloc (sizeof(t_TAG));

	ag = buscar_archivo_TAG(path);

	//agrega a la tabla de archivos globales
	if (ag == NULL)
	{
		ag->open = 1;
		ag->FD = list_size(global_fd) + 3;
		ag->path = strdup(path);

		list_add(global_fd, ag);
	}else ag->open ++;

	//agrega a la tabla de archivos del proceso
	t_TAP *ar_p = malloc (sizeof(t_TAP)); //armar para la función para los free :)
	ar_p->flag = strdup(flag);
	ar_p->GFD = ag->FD;
	ar_p->FD = list_size(prog->TAP) + 3;

	list_add(prog->TAP, ar_p);
}

t_TAG *buscar_archivo_TAG(char *path)
{
	bool _archivo_solicitado(t_TAG ag)
	{
		return !strcmp(ag.path, path);
	}
	t_TAG *ag_encontrado = list_find(global_fd, (void *)_archivo_solicitado);

	return ag_encontrado;
}

void crear_archivo(char *p)
{
	//enviar mensaje a fs
}

void pedido_lectura(t_program *prog, int fd, int offs, int size)
{
	t_TAP *ap = malloc(sizeof(t_TAP));
	ap = buscar_archivo_TAP(prog->TAP, fd);

	if (ap != NULL)
	{
	//	char *path = strdup(get_path(fd));
		//enviar el path al fs
	}//else //eliminar programa por querer leer un arch no abierto

}

t_TAP *buscar_archivo_TAP(t_list *tap, int fd)
{
	bool _archivo_TAP(t_TAP ap)
	{
		return (ap.FD == fd);
	}

	t_TAP *ap_b = list_find(tap, (void *)_archivo_TAP);
	return ap_b;
}

char *get_path(int fd)
{
	t_TAG *ag = buscar_archivo_TAG_fd(fd);

	return ag->path;
}

t_TAG *buscar_archivo_TAG_fd(int fd)
{
	bool _archivo_fd(t_TAG *ag)
	{
		return ag->FD == fd;
	}

	t_TAG *ag_aux = list_find(global_fd, (void *)_archivo_fd);
	return ag_aux;
}

void mover_puntero(int socket_prog, int offset)
{
	//recibir para saber si hay que leer o escribir

}

char *leer_escribir(int socket_prog)
{
	int control;
	char *mensaje = recibir(socket_prog, &control);
	int cod = get_codigo(mensaje);
	char *m2;
	//char *m3;

	switch (cod)
	{
		case 3:
			m2 = get_mensaje(mensaje);
			return m2;
			break;
		case 4:
			m2 = get_mensaje(mensaje);
			// acá sacar mensaje y de acá sacar lo que se va a escribir
			break;
		default:
			break;
	}
	free(mensaje);
	return (m2);
}

bool existe_archivo(t_list *tap, int fd)
{
	bool _archivo_TAP(t_TAP ap)
	{
		return (ap.FD == fd);
	}
	return list_any_satisfy(tap,(void *) existe_archivo);
}

void destruir_file(t_TAP *ap)
{
	free(ap->flag);
	free(ap);
}

void destruir_file_TAG(t_TAG *tg)
{
	free(tg->path);
	free(tg);
}

void cerrar_file(t_list *tap, int fd)
{
	bool ex_f = existe_archivo(tap, fd);

	if(ex_f)
	{

		t_TAP *p_ap = malloc(sizeof(t_TAP));
		p_ap = buscar_archivo_TAP(tap, fd);

		t_TAG *ag = buscar_archivo_TAG_fd(p_ap->FD);
		ag->open --;

		bool _archivo_fd(t_TAG *ag2)
		{
			return ag2->FD == fd;
		}

		if(ag->open == 0)
		{
			list_remove_and_destroy_by_condition(global_fd, (void *)_archivo_fd, (void *)destruir_file_TAG);
		}

		bool _archivo_TAP(t_TAP ap)
		{
			return (ap.FD == fd);
		}
		list_remove_and_destroy_by_condition(tap,(void *) _archivo_TAP, (void *) destruir_file);

	}//else avisar a cpu que hay error
}
