/*
 * cosas.c
 *
 *  Created on: 19/5/2017
 *      Author: utnso
 */

#include "cosas.h"

t_sentencia* armarIndiceCodgigo (char *codigoPrograma){
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
t_dictionary* armarIndiceEtiquetas(char *codigoPrograma){
	t_metadata_program *metadata = metadata_desde_literal(codigoPrograma);
	t_dictionary* dicc = dictionary_create();
	int n=0;
	int largos[metadata-> cantidad_de_funciones+ metadata->cantidad_de_etiquetas];
	while(n < metadata-> cantidad_de_funciones+ metadata->cantidad_de_etiquetas){
		if(n==0){
			largos[n]= strlen(metadata->etiquetas);
		}else{
			largos[n]= strlen(metadata->etiquetas+largos[n-1]+n*5);
		}
		n++;
	}
	n=0;
	while(n <metadata-> cantidad_de_funciones+ metadata->cantidad_de_etiquetas){
		if(n==0){
			char *key_aux= string_substring(metadata->etiquetas,0,largos[n]);
			dictionary_put(dicc,key_aux,(void *)metadata_buscar_etiqueta(key_aux,metadata->etiquetas,metadata->etiquetas_size));
			free(key_aux);
		}else{
		char *key_aux= string_substring(metadata->etiquetas,largos[n-1]+n*5,largos[n]);
		dictionary_put(dicc,key_aux,(void *)metadata_buscar_etiqueta(key_aux,metadata->etiquetas,metadata->etiquetas_size));
		free(key_aux);
		}
	n++;
	}
	metadata_destruir(metadata);
	return dicc;

}

