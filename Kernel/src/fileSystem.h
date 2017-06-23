#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdbool.h>
#include "estructuras.h"

void crear_tabla_global();
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
void escribir_archivo(int offset, char *info, char *flags, char *path);
void chequear_respuesta(int socket_cpu, char *path, char *flag, t_program *prog);
void crear_archivo(int socket_cpu, char *path, char *flag, t_program *prog);

#endif /* FILESYSTEM_H_ */
