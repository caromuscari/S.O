/*
 * log.c
 *
 *  Created on: 29/11/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/string.h>

t_log *log;
pthread_mutex_t mutex_log;

void crear_archivo_log(char *file)
{
	log = log_create(file,"MAPA",false, LOG_LEVEL_INFO);
	log_info(log, "Mapa - Se crea el archivo de log");
	pthread_mutex_init(&mutex_log,NULL);
}

void escribir_log(char *mensaje)
{
	pthread_mutex_lock(&mutex_log);
	log_info(log, mensaje);
	pthread_mutex_unlock(&mutex_log);
}

void escribir_log_con_numero(char *mensaje, int un_numero)
{
	char *final = strdup(mensaje);
	char *num = string_itoa(un_numero);
	string_append(&final, num);
	pthread_mutex_lock(&mutex_log);
	log_info(log, final);
	pthread_mutex_unlock(&mutex_log);
	free(final);
	free(num);
}

void escribir_log_compuesto(char *mensaje, char *otro_mensaje)
{
	char *final = strdup("");
	string_append(&final, mensaje);
	string_append(&final, otro_mensaje);
	pthread_mutex_lock(&mutex_log);
	log_info(log, final);
	pthread_mutex_unlock(&mutex_log);
	free(final);
}

char *armar_mensaje(char *identificador, char *mensaje)
{
	char *resultado = strdup(identificador);
	char *payload_char = string_itoa(string_length(mensaje));
	int size_payload = string_length(payload_char);
	char *completar = string_repeat('0', 4 - size_payload);

	string_append(&resultado, completar);
	string_append(&resultado, payload_char);
	string_append(&resultado, mensaje);

	free(payload_char);
	free(completar);
	return resultado;
}

//Exclusivo del planificador
char *armar_mensaje_con_coordenadas(char *identificador, int eje_x, int eje_y)
{
	char *resultado = strdup(identificador);
	char *eje_x_char = string_itoa(eje_x);
	char *eje_y_char = string_itoa(eje_y);
	int size_payload_x = string_length(eje_x_char);
	int size_payload_y = string_length(eje_y_char);
	char *completar_x = string_repeat('0', 3 - size_payload_x);
	char *completar_y = string_repeat('0', 3 - size_payload_y);

	string_append(&resultado, completar_x);
	string_append(&resultado, eje_x_char);
	string_append(&resultado, completar_y);
	string_append(&resultado, eje_y_char);

	free(eje_x_char);
	free(eje_y_char);
	free(completar_x);
	free(completar_y);
	return resultado;
}

void liberar_log()
{
	log_destroy(log);
}
