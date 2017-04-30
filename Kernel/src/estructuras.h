#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include <commons/collections/list.h>

typedef struct
{
	char *ip_kernel;
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
	int server_cpu;
	int server_consola;
	int cliente_fs;
	int cliente_memoria;
} t_configuracion;

typedef struct
{
	int *PID;
	int *control;
}t_program;

typedef struct
{
	int *instruccion;
	int *offset_inicio;
	int *offset_fin;
}t_sentencia;

typedef struct
{

}t_etiqueta;

typedef struct
{
	char ID;
	int pag;
	int off;
	int size;
}t_memoria;

typedef struct
{
	int pos;
	t_memoria arg;
	t_memoria var;
	int retPos;
	t_memoria retVar;
}t_stack;

typedef struct
{
	int PID;
	int PC;
	int cant_pag;
	t_sentencia *in_cod;//no estoy segura de implementarlo como una estructura
	t_etiqueta *in_et; //no estoy segura de como implementarlo
	t_stack *in_st;   //creo que le mandé fruta :D
	int exit_code;
}t_PCB;

#endif /* ESTRUCTURAS_H_ */
