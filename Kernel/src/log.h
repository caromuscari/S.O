/*
 * log.h
 *
 *  Created on: 29/11/2016
 *      Author: utnso
 */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

//Funcion para crear archivo de log
void crear_archivo_log(char *file);

//Funcion que escribe en archivo de log
void escribir_log(char *mensaje);

//Recibe un texto y un numerico, muy util!
void escribir_log_con_numero(char *mensaje, int un_numero);

//Recibe y genera log recibiendo dos textos
void escribir_log_compuesto(char *mensaje, char *otro_mensaje);

//Arma el texto de envio para mensajes
char *armar_mensaje(char *identificador, char *mensaje);

//Armado de mensaje exclusivo para el planificador
char *armar_mensaje_con_coordenadas(char *identificador, int eje_x, int eje_y);

void liberar_log();

#endif /* SRC_LOG_H_ */
