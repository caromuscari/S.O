/*
 * fileSystem.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

void crear_tabla_global();
void abrir_archivo(char *path);
t_TAG *buscar_archivo_TAG(t_list *tag, char *p_sol);
void crear_archivo(char *p);
void pedido_lectura(t_program *prog, int fd, int offs, int size);
t_TAP *buscar_archivo_TAP(t_list *tap, int fd);
char *get_path(int fd);
t_TAG *buscar_archivo_TAG_fd(t_list *tag, int fd);

#endif /* FILESYSTEM_H_ */
