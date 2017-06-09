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
t_TAG *buscar_archivo_TAG(t_list *tag, char *p_sol);
void crear_archivo(char *p);
void pedido_lectura(t_program *prog, int fd, int offs, int size);
t_TAP *buscar_archivo_TAP(t_list *tap, int fd);
char *get_path(int fd);
t_TAG *buscar_archivo_TAG_fd(t_list *tag, int fd);

void crear_tabla_global()
{
	global_fd = list_create();
}

void abrir_archivo(char *path, char* flag, t_program *prog)
{
	t_list *tag_aux = global_fd;

	t_TAG *ag = malloc (sizeof(t_TAG));

	ag = buscar_archivo_TAG(tag_aux, path);

	//agrega a la tabla de archivos globales
	if (ag == NULL)
	{
		ag->open = 1;
		ag->FD = list_size(tag_aux) + 3;
		ag->path = strdup(path);

		list_add(tag_aux, ag);
	}else ag->open ++;

	//agrega a la tabla de archivos del proceso
	t_TAP *ar_p = malloc (sizeof(t_TAP)); //armar para la función para los free :)
	ar_p->flag = strdup(flag);
	ar_p->GFD = ag->FD;
	ar_p->FD = list_size(prog->TAP) + 3;

	list_add(prog->TAP, ar_p);
}

t_TAG *buscar_archivo_TAG(t_list *tag, char *p_sol)
{
	bool _archivo_solicitado(t_TAG ag)
	{
		return !strcmp(ag.path, p_sol);
	}
	t_TAG *ag_encontrado = list_find(tag, (void *)_archivo_solicitado);

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
		char *path = strdup(get_path(fd));
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
	t_list *tag_aux = global_fd;
	t_TAG *ag = buscar_archivo_TAG_fd(tag_aux, fd);

	return ag->path;
}

t_TAG *buscar_archivo_TAG_fd(t_list *tag, int fd)
{
	bool _archivo_fd(t_TAG *ag)
	{
		return ag->FD == fd;
	}

	t_TAG *ag_aux = list_find(tag, (void *)_archivo_fd);
	return ag_aux;
}
