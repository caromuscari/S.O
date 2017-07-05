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

char* leer_archivo(char*);
void iniciar_programa(char *ruta, int socket_);
void finalizar_programa(char *pid, int socket_);
void desconectar_consola();
void cerrar_programas(char *key, void *data);
void tiempofinal_impresiones(char *pid);
void mostrar_pids(char *key, void *data);
void imprimir_menu();

void hilousuario()
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	//t_dictionary* switch_;
	//cargar_switch(switch_);

	while(flag == 0)
	{
		char *ingreso;
		char *identi;
		scanf("%ms", &ingreso);

		/*int data=dictionary_get(switch_,ingreso);
				  switch(data){
				  	case 1:
				  		printf("ingresar la ruta del programa: ");
						scanf("%s",identi);
						iniciar_programa(identi,socket_);
						break;
					case 2:
						printf("ingresar el PID del programa: ");
						scanf("%s",identi);
						finalizar_programa(atol(identi),socket_);
						break;
					case 3:
						desconectar_consola();
						break;
					case 4:
						system("clear");
						break;
					default:

		}
		*/
		imprimir_menu();

		switch(atoi(ingreso))
		{
		case 1:
		//if(!strcmp(ingreso, "iniciar_programa"))
		//{
			printf("Ingresar la ruta del programa: ");
			scanf("%ms",&identi);
			iniciar_programa(identi,socket_);
			free(identi);
			break;
		//}
		case 2:
		//else if(!strcmp(ingreso, "finalizar_programa"))
		//{
			if(dictionary_is_empty(p_pid)){
				printf("No hay programas abiertos\n");
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
			break;
		//}
		case 3:
		//else if(!strcmp(ingreso, "desconectar_consola"))
		//{
			desconectar_consola();
			break;
		//}
		case 4:
		//else if(!strcmp(ingreso, "limpiar_consola"))
		//{
			system("clear");
			break;
		//}
		default :
		//else
		//{
			printf("No se reconoce el pedido\n");
			break;
		//}
		}

		free(ingreso);
	}
	pthread_exit(NULL);
}

/*void cargar_switch(t_dictionary * switch_){
	switch_=dictionary_create();
	dictionary_put(switch_,"iniciar_programa",1);
	dictionary_put(switch_,"finalizar_programa",2);
	dictionary_put(switch_,"desconectar_consola",3);
	dictionary_put(switch_,"limpiar_consola",4);
}*/

void iniciar_programa(char *ruta, int socket_)
{
	char *mensaje_armado;
	char *mensaje;
	mensaje = leer_archivo(ruta);
	mensaje_armado= armar_mensaje("C01", mensaje);
	enviar(socket_, mensaje_armado,string_length(mensaje_armado));

	free(mensaje);
	free(mensaje_armado);
}

char *leer_archivo(char *ruta)
{
	FILE* archivo;
	long int final;

	archivo = fopen(ruta,"r");
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
			printf("Se finalizo el programa: %d\n", atoi(pid));
			tiempofinal_impresiones(pid2);

			free(dictionary_remove(h_pid,var));
			free(dictionary_remove(p_pid,pid2));
			free(dictionary_remove(impresiones,pid2));
			free(dictionary_remove(sem,pid2));
			free(dictionary_remove(tiempo,pid2));
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
	//struct tm* tm_info;
	struct tm* tm_info2;
	//char buffer[26];
	char buffer1[26];

	cant = dictionary_get(impresiones,pid);
	tiempoinicial = dictionary_get(tiempo,pid);

	*tiempoFinal = time(NULL);
	tiempoI = tiempoinicial->tiempo;

	tm_info2 = localtime(tiempoFinal);

	strftime(buffer1, 26, "%Y-%m-%d %H:%M:%S", tm_info2);

	diferencia= difftime(*tiempoFinal, *tiempoI);

	printf("Inicio de ejecucion: %s\n", (char*)tiempoinicial->buffer);
	printf("Fin de ejecucion: %s\n", (char*)buffer1);
	printf("Cantidad de impresiones: %d\n", cant->cantidad);
	printf("Tiempo total de ejecucion en segundos: %d\n\n", (int)diferencia);

	free(tiempoFinal);
}

void desconectar_consola()
{
	char *mensaje;
	dictionary_iterator(h_pid,(void*)cerrar_programas);
	mensaje = armar_mensaje("C03", "");
	enviar(socket_, mensaje, string_length(mensaje));
	printf("Se desconecta la consola\n");
	free(mensaje);
	//pthread_cancel(hiloMensaje);
	pthread_kill(hiloMensaje,SIGKILL);
	//pthread_cancel(hiloUsuario);
	//flag = 1;
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
