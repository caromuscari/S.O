/*
 * cosas.h
 *
 *  Created on: 20/5/2017
 *      Author: utnso
 */

#ifndef SRC_COSAS_H_
#define SRC_COSAS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <commons/collections/dictionary.h>
#include "estructuras.h"
#include <parser/metadata_program.h>

t_sentencia* armarIndiceCodgigo (char *codigoPrograma);
t_dictionary* armarIndiceEtiquetas(char *codigoPrograma);

#endif /* SRC_COSAS_H_ */
