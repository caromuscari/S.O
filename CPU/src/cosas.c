/*
 * cosas.c
 *
 *  Created on: 19/5/2017
 *      Author: utnso
 */

#include "cosas.h"

t_sentencia* armarIndiceCodigo (char *codigoPrograma){
	t_metadata_program *metadata = metadata_desde_literal(codigoPrograma);
	t_sentencia* sentencias = malloc( sizeof(t_sentencia) * (metadata->instrucciones_size+1));

	int i ;
	for(i=0; i < metadata->instrucciones_size; i++){
		t_sentencia nueva;
		nueva.offset_inicio = metadata->instrucciones_serializado[i].start;
		nueva.offset_fin = metadata->instrucciones_serializado[i].offset;
		sentencias[i]=nueva;
	}
	t_sentencia nueva;
	nueva.offset_inicio = -1;
	nueva.offset_fin = -1;
	sentencias[i]= nueva;
	metadata_destruir(metadata);
	return sentencias;
}
void* armarIndiceEtiquetas(char *codigoPrograma){
	t_metadata_program *metadata = metadata_desde_literal(codigoPrograma);
	int cantidadEtiquetas= metadata-> cantidad_de_funciones+ metadata->cantidad_de_etiquetas;
	int largos[cantidadEtiquetas];
	int n=0;
	int largoserializado=0;
	while(n < cantidadEtiquetas){
		if(n==0){
			largos[n]= strlen(metadata->etiquetas);

		}else{
			largos[n]= strlen(metadata->etiquetas+largos[n-1]+n*5);
		}
		largoserializado =largoserializado+ largos[n];
		n++;
	}
	largoserializado = largoserializado+ (4*(cantidadEtiquetas))+6;
	void* etiquetas= malloc(largoserializado+1);
	n=0;int inicio=6;
	memcpy(etiquetas,&largoserializado,4);
	memcpy(etiquetas+4,&cantidadEtiquetas,2);

	while(n < cantidadEtiquetas){
		char *key_aux;
		if(n==0){
			largos[n]= strlen(metadata->etiquetas);
			key_aux= string_substring(metadata->etiquetas,0,largos[n]);
		}else{
			largos[n]= strlen(metadata->etiquetas+largos[n-1]+n*5);
			key_aux= string_substring(metadata->etiquetas,largos[n-1]+n*5,largos[n]);
			printf("%s-%d\n",key_aux,largos[n]);

		}
		memcpy(etiquetas+inicio,&largos[n],2);
		memcpy(etiquetas+inicio+2,key_aux,largos[n]);
		int z = metadata_buscar_etiqueta(key_aux,metadata->etiquetas,metadata->etiquetas_size);
		memcpy(etiquetas+inicio+2+largos[n],&z,2);
		inicio = inicio+4+largos[n];
		free(key_aux);
		n++;
	}

metadata_destruir(metadata);
return etiquetas;


}
t_list* armarIndiceStack (char *codigoPrograma){
	t_list *lista = list_create();
	t_stack_element* inicial = malloc(sizeof (t_stack_element));
	t_memoria aux;
	aux.ID = '\0';
	aux.offset = 0;
	aux.pag = 0;
	aux.size = 0;
	inicial->args = list_create();
	inicial->vars = list_create();
	inicial->pos = 0;
	inicial->retPos = 0;
	inicial->retVar = aux;
	list_add(lista,inicial);
	return lista;
}

void* serializarPCB(t_PCB pcb){
	void *retorno;
	//int sizeretorno = tamaño_PCB(pcb);
	int size_retorno = sizeof(int)*5; //PID,PC,CANT_PAGINAS,SP,EXIT_CODE
	// TAMAÑO_SENTENCIAS_SERIALIZADAS + SENTENCIAS_SERIALIZADAS (c/sentencias : (int)inicio,(int)offset)
	// en la serializacion de etiquetas como en el indice hay una extra, de control, con valores negativos
	int cantidad_sentencias=0;
	while(pcb.in_cod[cantidad_sentencias].offset_inicio != -1 &&pcb.in_cod[cantidad_sentencias].offset_fin != -1 ){
		cantidad_sentencias++;
	}
	size_retorno += (cantidad_sentencias+1)*sizeof(t_sentencia) + 4;
	// TAMAÑO DICCIONARIO ETIQUETAS + ETIQUETAS_SERIALIZADAS (c/etiqueta: (int)long_key,(char *)key,(int)valor_1erinstruccion_pc)
	int etiq= dictionary_size(pcb.in_et);
	int n;
	for(n=0;n<etiq;n++){
		pcb.in_et;
	}

	return retorno;
}

//stack

