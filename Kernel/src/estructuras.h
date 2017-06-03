#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

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
	int *instruccion;
	int *offset_inicio;
	int *offset_fin;
}t_sentencia;

typedef struct
{
	char ID; //no estoy segura que sea necesario
	int pag;
	int offset;
	int size;
}t_memoria;

typedef struct
{
	int PID;
	int PC;
	int cant_pag;
	t_sentencia *in_cod;
	t_dictionary *in_et;
	t_list *in_stack;  //lista de t_stack_element
	int exit_code;
}t_PCB;

typedef struct
{
	int *PID;
	int *control;
	t_PCB *pcb;
	t_list *memoria_dinamica; //lista de paginas pedidas de manera dinámica
}t_program; //cada vez que se crea un programa, además del pcb esta estructura de control

typedef struct
{
	int pos;
	t_list *args; //lista de t_memoria
	t_list *vars; //idem args
	int retPos; //posición en el indice de código, empezando de 0, donde retorna la función
	t_memoria retVar;
}t_stack_element;

typedef struct
{
	int n_pagina;
	t_list *heaps; //de heapMetadata
	int esp_libre;
	t_dictionary *posiciones;
}t_pagina;

/*typedef struct
{
	t_pagina *pagina;
	t_PCB *pcb;
	int esp_libre;
}t_heap; //con esto haríamos una lista de las paginas que se solicitó de manera dinámica
*/
typedef struct
{
	int size;
	bool isFree;
}HeapMetadata;

typedef struct
{
	int *cpu_id;
	int *socket_cpu;
	bool *ejecutando;
	t_PCB *pcb;
}t_cpu;

typedef struct
{
	int CID;
	int socket;
}t_consola;

typedef struct
{
	int value;
	t_queue *procesos;
}t_sem;

#endif /* ESTRUCTURAS_H_ */
