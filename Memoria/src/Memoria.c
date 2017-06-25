#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "estructuras.h"
#include "socket.h"
#include "log.h"
#include "mensaje.h"
#include "manejo_errores.h"
//#include "Memoria.h"
#include <commons/collections/list.h>

//Variables Hilos x Pedido
int cliente[20];
pthread_t hiloEsperarMensaje[20];

// Variables Globales
int tamanioMarco;
int cantMarcos;
int socketServerMemoria;
int controlador = 0;
t_memoria *data_Memoria;
t_log *log;
char * Memoria;
int STACK_SIZE;

//Estructuras administrativas

typedef struct{
	int estado;
	int pid;
	int pag;
} t_tablaPagina;
t_tablaPagina* tablaPaginas;
int cantMarcosTablaPag;

int tamTablaPaginas;

//Funciones
void leerArchivoConfig(char* rutaArchivoConfig);
void esperar_mensaje(void *i);
t_tablaPagina* crearEstrucTablaPag(void);
int cantidadDePaginas(int tamanioBytes);
void cargarCodigoEnMemoria(char * datos, char * Memoria ,t_list * frames_pedidos, int cantPaginasAgrabar);
int posFrameEnMemoria(int nroFrame);
int maxPaginaPid(int p_pid);
bool f_mayor(int a, int b);
void inicializarPrograma (int pid, int paginasRequeridas);
int posPaginaSolicitada(int pid,int paginaSolicitada);
char * hash_lookup(t_list* lista,int pagSol);
void almacenarBytes(int pid, int pag, int offset, int tamanio);

int main(int argc, char *argv[])
{
	crear_archivo_log("/home/utnso/Escritorio/MEMORIA-DEBUGGING-LOG.txt");
	leerArchivoConfig(argv[1]);
	//Creacion de Socket Server

	printf(" \n");
	printf("CANT MARCOS: %d  \n",data_Memoria->MARCOS);
	printf("TAM MARCO: %d  \n",data_Memoria->MARCO_SIZE);

	tamanioMarco =data_Memoria->MARCO_SIZE;
	cantMarcos=data_Memoria->MARCOS;
	tamTablaPaginas = sizeof(t_tablaPagina)*cantMarcos;
	cantMarcosTablaPag = cantidadDePaginas(tamTablaPaginas);
    tablaPaginas = malloc(sizeof(t_tablaPagina)*cantMarcos);
    tablaPaginas = crearEstrucTablaPag();

    Memoria = malloc(cantMarcos * tamanioMarco);
    memset(Memoria,'\0',cantMarcos * tamanioMarco);

	printf("Tamaño de Tabla de paginas %d \n",tamTablaPaginas);
	printf("Tamaño Total de Memoria %d  \n",tamanioMarco * cantMarcos);

	//Cargo tabla de Paginas en Memoria

	int i_tp;
	int inicio=0;
	for(i_tp=0;i_tp<cantMarcosTablaPag;i_tp++){
	tablaPaginas[i_tp].estado = 1;
	tablaPaginas[i_tp].pag= inicio;
	inicio ++;
	}
	memcpy(Memoria,tablaPaginas,tamTablaPaginas);

	int it;
	for(it=0;it<data_Memoria->MARCOS;it++){
		printf("%d | %d | %d \n",tablaPaginas[it].estado,tablaPaginas[it].pid,tablaPaginas[it].pag);
	}



	 //recieve: Guardo lo que me llega por Socket

	char* codigo="SSSSSSSSSSSSXXXXXXXX";
	int pid=89;
	int cantPaginasAgrabar=cantidadDePaginas(string_length(codigo));

	//inicializarPrograma(pid,cantPaginasAgrabar);


	printf("%s | %d | %d \n",codigo, string_length(codigo),cantPaginasAgrabar);

	//printf("\nPOSICION DONDE ESTA LA PAGINA :%d \n",posPaginaSolicitada(pid,0));
	//Guardar en paginas
	//Busco y asigno paginas vacias

//	 t_list * frames_pedidos = list_create();
// frames_pedidos = asignarFramesLibres(pid,cantPaginasAgrabar);
//	 cargarCodigoEnMemoria(codigo,Memoria ,frames_pedidos, cantPaginasAgrabar);

	int pp =maxPaginaPid(pid);
	printf("\n maximaPosicion %d \n",pp);

	socketServerMemoria = iniciar_socket_server(data_Memoria->IP,data_Memoria->PUERTO,&controlador);

	escribir_log(string_from_format("Se inicia Servidor de Memoria en Socket: %d ",socketServerMemoria));



	int i = 0;

		while (1) {

			cliente[i] = escuchar_conexiones(socketServerMemoria,&controlador);

			escribir_log(string_from_format("Nueva Conexión entrante: %d ",cliente[i]));

	// Realizar HS y enviar Tamaño MARCO.
			char *mensaje_recibido = recibir(cliente[i], &controlador);
			char *header = get_header(mensaje_recibido);

			if(strcmp(header,"K")==0 || strcmp(header,"P")==0 )
				{
				//Cliente VALIDO
					char* HS_OK = armar_mensaje("M00",string_itoa(data_Memoria->MARCO_SIZE)); // M|00|0000000003|128|
					enviar(cliente[i],HS_OK, &controlador);
				}
				else
				{
					escribir_log(string_from_format("Se cierra la conexión con el Cliente luego del HandShake - Socket: %d ",cliente[i]));
					cerrar_conexion(cliente[i]);
					goto salir_handshake;
				}

	//Si es Cliente valido entra a interactuar con la MEMORIA. // Hilo

			pthread_create(&hiloEsperarMensaje[i], NULL, (void*) esperar_mensaje,(void *) cliente[i]);
			escribir_log(string_from_format("El Cliente %d tiene Hilo nro :%d ",cliente[i],i));

			// RECIBIR
			salir_handshake:
			i++;
		}

	//El select esta aca dentro --- ejecutar_server();
		free(tablaPaginas);
		free(Memoria);

	return EXIT_SUCCESS;

}

//A partir de este punto.. las funciones
void leerArchivoConfig(char* rutaArchivoConfig)
{
	data_Memoria = malloc(sizeof(t_memoria));
	data_Memoria->IP = strdup("");

	t_config *configuracion = config_create(rutaArchivoConfig);
	string_append(&data_Memoria->IP,config_get_string_value(configuracion,"IP"));
	data_Memoria->PUERTO = config_get_int_value(configuracion,"PUERTO");
	data_Memoria->MARCOS = config_get_int_value(configuracion,"MARCOS");
	data_Memoria->MARCO_SIZE = config_get_int_value(configuracion,"MARCO_SIZE");
	data_Memoria->ENTRADAS_CACHE = config_get_int_value(configuracion,"ENTRADAS_CACHE");
	data_Memoria->CACHE_X_PROC = config_get_int_value(configuracion,"CACHE_X_PROC");
	data_Memoria->REEMPLAZO_CACHE = config_get_int_value(configuracion,"REEMPLAZO_CACHE");
	data_Memoria->RETARDO_MEMORIA = config_get_int_value(configuracion,"RETARDO_MEMORIA");
	config_destroy(configuracion);
}

void esperar_mensaje(void *i) {

	int cliente = (int) i;
	int chau = 0;
	while (chau != 1 )
	{
	char *mensRec = malloc(1024);
	memset(mensRec,'\0', 1024);

	mensRec = recibir(cliente, &controlador);

	if(controlador > 0)
	{
		cerrar_conexion(cliente);
		chau =1;
		goto chau;
	}

	char *header=get_header(mensRec);
	int codigo = get_codigo(mensRec);

	printf("\n header: %s \n",header);
	printf("\n codigo: %d \n",codigo);

	switch (codigo) {
					case 17: //INICIALIZAR PROGRAMA
						{
							//K|17|PPPP|XXXX|YYYY|
							//PPPP: Pid
							//XXXX: CantidadPaginasCodigo
							//YYYY: CantidadPaginasSTACK
							escribir_log(string_from_format("K17-Guardando codigo: %d",cliente));

							printf("\nMENSAJE RECIBIDO\n%s\n",mensRec);

							int procPid= atoi(string_substring(mensRec, 3, 4));

							char *paginasCodigo= malloc(4); memset(paginasCodigo,'0',4);
							memcpy(paginasCodigo,mensRec+7, sizeof(int));
							int pagsCodigo = atoi(paginasCodigo);
							char *paginasSTACK= malloc(4); memset(paginasSTACK,'0',4);
							memcpy(paginasSTACK,mensRec+11, sizeof(int));
							STACK_SIZE = atoi(paginasSTACK);

							printf("\n PID RECIBIDO %d \n",procPid);
							printf("\n TAMAÑO PAGINAS CODIGO %d \n",pagsCodigo);
							printf("\n TAÑANO PAGINAS STACK %d \n ",STACK_SIZE);

							inicializarPrograma (procPid,pagsCodigo);
							inicializarPrograma (procPid,STACK_SIZE);

							int it;
							for(it=0;it<cantMarcos;it++){
								printf("%d | %d | %d \n",tablaPaginas[it].estado,tablaPaginas[it].pid,tablaPaginas[it].pag);
							}
							free(paginasCodigo);
							free(paginasSTACK);
						}
					break;
					case 18: //K|18|PIDD|0000000006|CODIGO
								{
								escribir_log(string_from_format("K06 - Guardando codigo: %d ",cliente));
								int procPid= atoi(string_substring(mensRec, 3, 4));

								int tamanio= atoi(string_substring(mensRec, 7, 10));

								char * codigo = malloc(tamanio); memset(codigo,'\0', tamanio);
								codigo = string_substring(mensRec, 17, tamanio);
								int pagsCod = cantidadDePaginas(tamanio);
								printf("\n ----------------------------- \n");
								printf("%d \n",procPid);
								printf("%d \n",pagsCod);
								printf("%s", codigo);
								printf("\n ----------------------------- \n");
							//Meter funciones de guardar codigo





							}
										break;
						}

					chau:
				printf("HOLA");
	}
	}


t_tablaPagina* crearEstrucTablaPag(void){

	t_tablaPagina* estructuraTablaPaginas=malloc(sizeof(t_tablaPagina)*cantMarcos);
	int i;
	for(i=0;i<data_Memoria->MARCOS;i++){
		estructuraTablaPaginas[i].estado=0;
		estructuraTablaPaginas[i].pag=-1;
		estructuraTablaPaginas[i].pid=-1;
	}
	return estructuraTablaPaginas;
}

int cantidadDePaginas(int tamanioBytes)
{
	double Bloques= -1;
	if ((double)tamanioBytes/tamanioMarco - (int)(tamanioBytes/tamanioMarco) > 0){
			Bloques = ((tamanioBytes/tamanioMarco) +1);
		}else{
			Bloques = (int)(tamanioBytes/tamanioMarco);
		}
	return (int)Bloques;

}

void cargarCodigoEnMemoria(char * datos, char * Memoria ,t_list * frames_pedidos, int cantPaginasAgrabar) {
	int c = 0;
	while(c < cantPaginasAgrabar){
		int frame_pos = list_get(frames_pedidos, c);
		int pos = posFrameEnMemoria(frame_pos);
		memcpy(Memoria+pos,datos+(c*tamanioMarco),tamanioMarco);
		printf("\nSe guardo en el frame: %d \n", frame_pos);
		c++;
	}
}

/*void almacenarBytes(int pid, int pag, int offset, int tamanio)
{
	int c = 0;

	while(c < ){
		int frame_pos = list_get(frames_pedidos, c);
		int pos = posFrameEnMemoria(frame_pos);
		memcpy(Memoria+pos,datos+(c*tamanioMarco),tamanioMarco);
		printf("\nSe guardo en el frame: %d \n", frame_pos);
		c++;
	}

}
*/


int posFrameEnMemoria(int nroFrame){
	//Devuelve la posicion de inicio del Frame
	int pos= (nroFrame* tamanioMarco)-1;
	return pos;
}

int maxPaginaPid(int p_pid)
{
t_list* lt_pagsXpid= list_create();
int primeraPosicion = 0;
int z;
int max=-1;
	  for (z=cantMarcosTablaPag; z<cantMarcos; z++ )
	 {
	 	if ( tablaPaginas[z].estado == 1 && tablaPaginas[z].pid == p_pid) {
	 		list_add(lt_pagsXpid,tablaPaginas[z].pag);
	 	}
	 }
	list_sort(lt_pagsXpid,(void*) f_mayor);

	if ( list_size(lt_pagsXpid) != 0 ){
		max=list_get(lt_pagsXpid,primeraPosicion);
	}

	return max;
}

bool f_mayor(int a, int b) {
        return a > b;
    }


void inicializarPrograma (int pid,int cantPaginasAgrabar)
	{
		//Calcular posición frames libres más próximos para una cantidad solicitada.
		int z;
		int paginas=0;
		int maxPagPid = maxPaginaPid(pid); //Funcion que me devuelva la máxima pagina de un PID.
		for (z=cantMarcosTablaPag; z<cantMarcos; z++ )
		{
			if (tablaPaginas[z].estado == 0 && paginas < cantPaginasAgrabar) {
				tablaPaginas[z].estado = 1;
				tablaPaginas[z].pid = pid;
				tablaPaginas[z].pag = maxPagPid+1;
				maxPagPid ++;
				paginas ++;
			} else if (paginas >=cantPaginasAgrabar){
				z=cantMarcos;
				}
		}
	}

int posPaginaSolicitada(int pid,int paginaSolicitada)
{
	//Calcular posición frames libres más próximos para una cantidad solicitada.
	t_list* lt_paginas = list_create();

	int z;

	char*valor=string_new();


	for (z=cantMarcosTablaPag; z<cantMarcos; z++ )
	{
		if ( tablaPaginas[z].estado == 1 && tablaPaginas[z].pid==pid) {
				string_append(&valor,string_itoa(z));
				string_append(&valor,",");
				string_append(&valor, string_itoa(tablaPaginas[z].pag));
			//printf(" \n POS: %s", valor);
		list_add(lt_paginas, valor);
		valor=string_new();
		}
	}

	char* pp = hash_lookup(lt_paginas,paginaSolicitada);
	char** s_frame = string_split(pp, ",");
	//printf(" \n POS: %s",pp);
	return atoi(s_frame[0]) ;
}

char * hash_lookup(t_list* lista,int pagSol) {
		int _is_the_one(char *posPag) {

			char** substrings = string_split(posPag, ",");

			return string_equals_ignore_case(substrings[1], string_itoa(pagSol));
		}

		return list_find((lista), (void*) _is_the_one);
	}

