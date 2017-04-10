#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include <commons/collections/list.h>

typedef struct
{
	int puerto_prog;
	int puerto_cpu;
	char *ip_memoria;
	int puerto_memoria;
	char *ip_fs;
	int puerto_fs;
	int quantum;
	int quantum_sleep;
	char *algoritmo;
	int grado_multiprog;
	t_list *sem_ids; //alfanumérico
	t_list *sem_init; //numérico
	t_list *shared_vars; //alfanumérico
	int stack_size;
}t_configuracion;

typedef struct
{
	int *PID;
	int *control;
}t_program;

#endif /* ESTRUCTURAS_H_ */
