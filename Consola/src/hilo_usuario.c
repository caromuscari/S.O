#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include "socket_client.h"
#include "mensaje.h"
#include <commons/string.h>
#include "log.h"
#include <commons/log.h>
#include "estructuras.h"
#include <signal.h>

extern int socket_;
extern t_dictionary * p_pid;
extern t_dictionary * h_pid;
extern pthread_t hiloMensaje;
extern t_dictionary * sem;
extern t_dictionary * impresiones;
extern t_dictionary * tiempo;
extern int flag;

char* leer_archivo(FILE* archivo);
void iniciar_programa(char *ruta, int socket_);
void finalizar_programa(char *pid, int socket_);
void desconectar_consola();
void cerrar_programas(char *key, void *data);
void tiempofinal_impresiones(char *pid);
void mostrar_pids(char *key, void *data);
void imprimir_menu();

void hilousuario()
{
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	while(flag == 0)
	{
		imprimir_menu();

		char *ingreso;
		char *identi;
		scanf("%ms", &ingreso);

		switch(atoi(ingreso))
		{
		case 1:
			printf("Ingresar la ruta del programa: ");
			scanf("%ms",&identi);
			iniciar_programa(identi,socket_);
			free(identi);
			break;
		case 2:
			if(dictionary_is_empty(p_pid)){
				printf("No hay programas ejecutando\n");
			}
			else
			{
				printf("Los programas son los siguientes:\n");
				dictionary_iterator(p_pid,(void *)mostrar_pids);

				printf("\nIngresar el pid del hilo a finalizar: ");
				scanf("%ms",&identi);

				finalizar_programa(identi,socket_);

				free(identi);
			}
			printf("\n");
			break;
		case 3:
			printf("Confirmar cierre de consola (Y/N)\n");
			scanf("%ms",&identi);

			if(!strcmp(identi, "Y"))
				desconectar_consola();

			printf("\n");
			break;
		case 4:
			system("clear");
			break;
		default :
			printf("No se reconoce el pedido\n");
			break;
		}
		free(ingreso);
	}
	pthread_exit(NULL);
}

void iniciar_programa(char *ruta, int socket_)
{
	FILE *archivo = fopen(ruta,"r");

	if(archivo)
	{
		char *mensaje_armado;
		char *mensaje;

		mensaje = leer_archivo(archivo);
		mensaje_armado= armar_mensaje("C01", mensaje);
		enviar(socket_, mensaje_armado,string_length(mensaje_armado));

		free(mensaje);
		free(mensaje_armado);
	}
	else printf("El archivo ingresado no existe\n");
	printf("\n");
}

char *leer_archivo(FILE* archivo)
{
	long int final;
	fseek( archivo, 0L, SEEK_END );
	final = ftell( archivo );
	fseek(archivo,0,0);
	char mensaje2[final];

	fread(mensaje2,sizeof(char),final,archivo);

	fclose(archivo);
	char *r = strdup(mensaje2);

	return r;
}

void finalizar_programa(char *pid, int socket_)
{
	char *mensaje;
	char *pid2;

	if(dictionary_has_key(p_pid,pid))
	{
		t_hilo *hilo = dictionary_get(p_pid,pid);
		char *var = string_itoa(hilo->hilo);

		if(pthread_cancel(hilo->hilo)==0)
		{
			pid2 = dictionary_get(h_pid,var);
			mensaje = armar_mensaje("C02",pid2);
			enviar(socket_, mensaje, string_length(mensaje));
			escribir_log_con_numero("Se finalizo el programa: ", atoi(pid));
			tiempofinal_impresiones(pid2);

			free(dictionary_remove(h_pid,var));
			free(dictionary_remove(p_pid,pid));
			free(dictionary_remove(impresiones,pid));
			free(dictionary_remove(sem,pid));
			free(dictionary_remove(tiempo,pid));
			free(mensaje);
		}
		else escribir_log("No se pudo finalizar el programa\n");
	}
	else printf("El programa no existe\n");
}

void mostrar_pids(char *key, void *data)
{
	t_hilo *hilo = data;
	char *var = string_itoa(hilo->hilo);

	printf("PID: %s\n", key);
	free(var);
}

void tiempofinal_impresiones(char* pid)
{
	time_t *tiempoFinal= malloc(sizeof(time_t));
	t_impresiones * cant;
	t_tiempo *tiempoinicial;
	time_t *tiempoI;
	double diferencia;
	struct tm* tm_info2;
	char buffer1[26];

	cant = dictionary_get(impresiones,pid);
	tiempoinicial = dictionary_get(tiempo,pid);

	*tiempoFinal = time(NULL);
	tiempoI = tiempoinicial->tiempo;

	tm_info2 = localtime(tiempoFinal);

	strftime(buffer1, 26, "%Y-%m-%d %H:%M:%S", tm_info2);

	diferencia= difftime(*tiempoFinal, *tiempoI);

	printf("\nInicio de ejecucion: %s\n", (char*)tiempoinicial->buffer);
	printf("Fin de ejecucion: %s\n", (char*)buffer1);
	printf("Cantidad de impresiones: %d\n", cant->cantidad);
	printf("Tiempo total de ejecucion en segundos: %d\n\n", (int)diferencia);

	free(tiempoFinal);
}

void desconectar_consola()
{
	char *mensaje;
	dictionary_iterator(p_pid,(void*)cerrar_programas);
	mensaje = armar_mensaje("C03", "");
	enviar(socket_, mensaje, string_length(mensaje));
	printf("Se desconecta la consola\n");
	free(mensaje);
	pthread_kill(hiloMensaje,SIGKILL);
	pthread_exit(NULL);
}

void cerrar_programas(char *key, void *data)
{
	finalizar_programa(key,socket_);
}

void imprimir_menu()
{
	printf("Seleccione el numero de la opcion a ejecutar\n");
	printf("	1 - Iniciar programa\n");
	printf("	2 - Finalizar programa\n");
	printf("	3 - Desconectar consola\n");
	printf("	4 - Limpiar consola\n\n");
}
