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
extern t_configuracion *config;

int abrir_archivo(char *path, char* flag, t_program *prog);
t_TAG *buscar_archivo_TAG(char *p_sol);
void abrir_crear(char *mensaje, t_program *prog, int socket_cpu);
void pedido_lectura(t_program *prog, int fd, int offs, int size, char *path, int socket_cpu);
t_TAP *buscar_archivo_TAP(t_list *tap, int fd);
char *get_path(int fd);
t_TAG *buscar_archivo_TAG_fd(int fd);
void mover_puntero(int socket_prog, int offset, int fd, t_program *prog);
bool existe_archivo(t_list *tap, int fd);
void destruir_file(t_TAP *ap);
void destruir_file_TAG(t_TAG *tg);
void cerrar_file(t_list *tap, int fd);
char *get_path_msg(char *mensaje, int *payload1);
char *get_info(char *mensaje, int payload1, int tam_info);
void abrir_crear(char *mensaje, t_program *prog, int socket_cpu);
void escribir_archivo(int offset, char *info, char *flags, char *path, int socket_cpu);
void chequear_respuesta(int socket_cpu, char *path, char *flag, t_program *prog);
void crear_archivo(int socket_cpu, char *path, char *flag, t_program *prog);
char *armar_info_mensaje(char *info, char* path);

void abrir_crear(char *mensaje, t_program *prog, int socket_cpu)
{
	int payload;
	char *path = get_path_msg(mensaje, &payload);
	char *flag = get_info(mensaje, payload, 1);

	if(string_contains(flag, "c"))
	{
		crear_archivo(socket_cpu,path, flag, prog);
	} else
	{
		char *mensaje = armar_mensaje("K12", path);
		int controlador;
		enviar(config->cliente_fs, mensaje, &controlador);
		free(mensaje);

		chequear_respuesta(socket_cpu, path, flag, prog);
	}
	abrir_archivo(path, flag, prog);
}

int abrir_archivo(char *path, char* flag, t_program *prog)
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
	free(path);
	free(flag);

	return ar_p->FD;
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

void crear_archivo(int socket_cpu, char *path, char *flag, t_program *prog)
{
	int controlador;
	char *mensaje;

	mensaje= armar_mensaje("K12",path);
	enviar(config->cliente_fs, mensaje, &controlador);
	free(mensaje);

	chequear_respuesta(socket_cpu, path, flag, prog);

}

void pedido_lectura(t_program *prog, int fd, int offs, int size, char *path, int socket_cpu)
{
	t_TAP *ap = malloc(sizeof(t_TAP));
	ap = buscar_archivo_TAP(prog->TAP, fd);

	if (ap != NULL)
	{
		if(string_contains(ap->flag,"r"))
		{
			int controlador;
		//	char *path = get_path(ap->FD);
			char *mensaje = armar_mensaje("K14", path);
			enviar(config->cliente_fs, mensaje, &controlador);
			free(mensaje);

			char *mensaje_recibido = recibir(config->cliente_fs, &controlador);
			char *header = get_header(mensaje);

			if(comparar_header(header,"F"))
			{
				char *codig = get_codigo(mensaje);
				int cod = atoi(codig);
				if(cod == 4)
				{
					char *mensaje_leido = get_mensaje(mensaje_recibido);
					enviar(socket_cpu, mensaje_leido, &controlador);
					free(mensaje_leido);
				}
			}
			free(header);
			free(mensaje_recibido);

		}else /* eliminar programa, pedido de lectura sin permiso*/;

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

void mover_puntero(int socket_cpu, int offset, int fd, t_program *prog)
{
	int control;
	char *mensaje = recibir(socket_cpu, &control);
	char *info = get_mensaje(mensaje);
	char *path = get_path(fd);
	char *header = get_header(mensaje);

	if (comparar_header("P",header))
	{
		char *cod = get_codigo(mensaje);
		int codigo = atoi(cod);
		switch (codigo)
		{
			case 5 : ;//pedido de escritura
				/*char *info = strdup("");
				info = get_mensaje(mensaje);*/
				t_TAP *arch = buscar_archivo_TAP(prog->TAP, fd);
				if (arch == NULL)
				{
					//hablar con lean
				}else escribir_archivo(offset, info, arch->flag, path, socket_cpu);
				free(info);
				break;
			case 6: ;//pedido de lectura
				int size = atoi(mensaje);
				pedido_lectura(prog, fd, offset, size, path, socket_cpu);
				break;
		}
		free(cod);
	}
	free(mensaje);
	free(info);
	free(path);
	free(header);
}

void escribir_archivo(int offset, char *info, char *flags, char *path, int socket_cpu)
{
	if (string_contains(flags, "w"))
	{
		int controlador;
		char *mensaje = armar_info_mensaje(info, path);
		char *mensaje_enviar = armar_mensaje("K15", mensaje);
		enviar(config->cliente_fs, mensaje_enviar, &controlador);

		char *mensaje_recibido = recibir(config->cliente_fs, &controlador);
		char *header = get_header(mensaje);

		if(comparar_header(header,"F"))
		{
			char *cod = get_codigo(mensaje);
			int codigo = atoi(mensaje);
			if(codigo== 5)
			{
				/*char *mensaje_leido = get_mensaje(mensaje_recibido);
				enviar(socket_cpu, mensaje_leido, &controlador);*/
			}
			free(cod);
		}
		free(header);
		free(mensaje_recibido);
		free(mensaje);
		free(mensaje_enviar);
	}else ;
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

char *get_path_msg(char *mensaje, int *payload1)
{
	char *payload = string_substring(mensaje, 0, 3);
	*payload1 = atoi(payload);
	free(payload);
	return string_substring(mensaje, 4, *payload1);
}

char *get_info(char *mensaje, int payload1, int tam_info)
{
	char *payload = string_substring(mensaje, (4+payload1), tam_info);
	int payload2 = atoi(payload);
	free(payload);
	return string_substring(mensaje, (4+payload1+tam_info), payload2);
}

void chequear_respuesta(int socket_cpu, char *path, char *flag, t_program *prog)
{
	int controlador;
	char *mensaje_recibido = recibir(config->cliente_fs, &controlador);
	char *header = get_header(mensaje_recibido);
	if(comparar_header(header, "F"))
	{
		char *info = get_mensaje(mensaje_recibido);
		if(!strcmp(info,"ok"))
		{
			int fd = abrir_archivo(path, flag, prog);
			char *mensaje = armar_mensaje("K16", string_itoa(fd));
			enviar(socket_cpu, mensaje, &controlador);
			free(mensaje);
		}
		free(info);
	}else; //cerrar conexión
	free(mensaje_recibido);
	free(header);
}

char *armar_info_mensaje(char *info, char* path)
{
	char *payload_char = string_itoa(string_length(info));
	int size_payload = string_length(payload_char);
	free(payload_char);
	char *primera_parte = string_repeat('0', 4 - size_payload);

	char *payload_char2 = string_itoa(string_length(path));
	int size_payload2 = string_length(payload_char2);
	free(payload_char2);
	char *segunda_parte = string_repeat('0', 4 - size_payload2);

	char *mensaje = strdup("");

	string_append(&mensaje, primera_parte);
	string_append(&mensaje, segunda_parte);

	return mensaje;
}
