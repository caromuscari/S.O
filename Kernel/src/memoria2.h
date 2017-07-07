/*
 * memoria2.h
 *
 *  Created on: 7/7/2017
 *      Author: utnso
 */

#ifndef MEMORIA2_H_
#define MEMORIA2_H_

t_bloque *find_first_fit(t_list *hs, int t_sol);
void reservar_memoria_din(t_program *program, int size_solicitado);
void inicializar_pagina_dinamica(t_program *prog);
void pedir_pagina_dinamica(t_program *prog);
int ubicar_bloque(t_pagina *pagina,int tam_sol, int *flag);
void compactar(t_list *pagina);
void _free_bloque(t_bloque *bloque);
int pedir_pagina();

#endif /* MEMORIA2_H_ */
