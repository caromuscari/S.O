#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

typedef struct
{
	//int *instruccion; yo omitiria este campo,con acceder a la posicion del array estaria para saber que numero de instruccion es
	// ¿por qué punteros?
	int offset_inicio;
	int offset_fin;
}t_sentencia;
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
	char ID; //no estoy segura que sea necesario
	int pag;
	int offset;
	int size;
}t_memoria;
typedef struct
{
	int pos;
	t_list args; //lista de t_memoria
	t_list vars; //idem args
	int retPos; //posición en el indice de código, empezando de 0, donde retorna la función
	t_memoria retVar;
}t_stack_element;

#endif /* ESTRUCTURAS_H_ */
