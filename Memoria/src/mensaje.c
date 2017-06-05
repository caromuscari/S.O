/*
 * mensaje.c

 *
 *  Created on: 23/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>

char *armar_mensaje(char *identificador, char *mensaje)
{
	char *resultado = strdup(identificador);
	char *payload_char = string_itoa(string_length(mensaje));
	int size_payload = string_length(payload_char);
	char *completar = string_repeat('0', 10 - size_payload);

	string_append(&resultado, completar);
	string_append(&resultado, payload_char);
	string_append(&resultado, mensaje);

	free(payload_char);
	free(completar);
	return resultado;
}

//devuelve el header del mensaje
char *get_header(char *mensaje)
{
	return string_substring(mensaje, 0, 1);
}

int comparar_header(char *identificador, char *mensaje)
{
	return !strcmp(string_substring(mensaje, 0, 1), identificador);
}

//devuelve el codigo del mensaje
int get_codigo(char *mensaje)
{
	return atoi(string_substring(mensaje, 1, 2));
}

//obtiene el mensaje
char * get_mensaje(char *mensaje)
{
	char *payload = string_substring(mensaje, 3, 10);
	int payload1 = atoi(payload);
	return string_substring(mensaje, 13, payload1);
}