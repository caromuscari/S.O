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
fd_set master;
fd_set read_fds;
int fdmax;
int controlador = 0;
t_memoria *data_Memoria;
t_log *log;
int cantMarcosTablaPag;

char * Memoria;

//Estructuras administrativas

typedef struct{
	int estado;
	int pid;
	int pag;
} t_tablaPagina;

t_tablaPagina* tablaPaginas;

int tamanioMarco;
int cantMarcos;
int tamTablaPaginas;

//Funciones
void leerArchivoConfig(char* rutaArchivoConfig);
void esperar_mensaje(void *i);
t_tablaPagina* crearEstrucTablaPag(void);
int cantidadDePaginas(int tamanioBytes, int tamanioPaginas);
t_list* asignarFramesLibres(int pid,int cantPaginasAgrabar);
void cargarCodigoEnMemoria(char * datos, char * Memoria ,t_list * frames_pedidos, int cantPaginasAgrabar);
//

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
	cantMarcosTablaPag = cantidadDePaginas(tamTablaPaginas,tamanioMarco);
    tablaPaginas = malloc(sizeof(t_tablaPagina)*cantMarcos);
    tablaPaginas = crearEstrucTablaPag();

    typedef unsigned char data_Marco[tamanioMarco];
    //data_Marco *
    Memoria = malloc(cantMarcos * tamanioMarco);

//   data_Marco MARCO;

/*/Inicializo Memoria
    int r;
        for( r=0;r<cantMarcos;r++){
        	memset(Memoria[r],'\0',tamanioMarco);
          	}
//FinInicializo Memoria
*/

//Imprimir Tabla de Paginas
	int it;
	 /* 	for(it=0;it<data_Memoria->MARCOS;it++){
		printf("%d | %d | %d \n",tablaPaginas[it].estado,tablaPaginas[it].pid,tablaPaginas[it].pag);
	}
*/
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

	//Guardo lo que me llega por Socket


	for(it=0;it<data_Memoria->MARCOS;it++){
		printf("%d | %d | %d \n",tablaPaginas[it].estado,tablaPaginas[it].pid,tablaPaginas[it].pag);
	}

	 //recieve: Guardo lo que me llega por Socket
	 char* codigo="SSSSSSSSSSSSSSSSSSSSAAAAAAAAAAAAAAAAAAAAJJJJJJJJJJJJJJJJJJJJ";
	 int pid=89;

	 int cantPaginasAgrabar=cantidadDePaginas(string_length(codigo),tamanioMarco);
	 printf("%s | %d | %d \n",codigo, string_length(codigo),cantPaginasAgrabar);

	//Guardar en paginas
	//Busco y asigno paginas vacias

	 t_list * frames_pedidos = list_create();
	 frames_pedidos = asignarFramesLibres(pid,cantPaginasAgrabar);

	 cargarCodigoEnMemoria(codigo,Memoria ,frames_pedidos, cantPaginasAgrabar);

	 //Fin Guardo lo que me llega por Socket

/*//--
	int size = list_size(frames_pedidos);
	int c = 0;
	while(c < cantPaginasAgrabar)
	{
		int frame_pos = list_get(frames_pedidos, c);
		printf("\npos:%d \n", frame_pos);
		c++;
	}

	c=0;
	while(c < cantPaginasAgrabar)
	{
		int frame_pos = list_get(frames_pedidos, c);
		memcpy(Memoria[frame_pos],codigo+(c*tamanioMarco),tamanioMarco);
		printf("\nSe guardo en el frame: %d \n", frame_pos);
		c++;
	}
//-- */



	for(it=0;it<data_Memoria->MARCOS;it++){
		printf("%d | %d | %d \n",tablaPaginas[it].estado,tablaPaginas[it].pid,tablaPaginas[it].pag);
	}





	free(tablaPaginas);
	free(Memoria);
	int socketServerMemoria = iniciar_socket_server(data_Memoria->IP,data_Memoria->PUERTO,&controlador);

	escribir_log(string_from_format("Se inicia Servidor de Memoria en Socket: %d ",socketServerMemoria));
/*
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
	 *
	 */
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

	char *header = get_header(mensRec);
	int codigo = get_codigo(mensRec);

	printf("\n header: %s \n",header);
	printf("\n codigo: %d \n",codigo);

	switch (codigo) {
					case 6:
						{

							escribir_log(string_from_format("K06 - Guardando codigo: %d ",cliente));

							//Meter funciones de guardar codigo


						}
					break;
					case 2:
						{
							escribir_log(string_from_format("CASE 2: %d ",cliente));
						}
					break;
					case 3:
						{
							escribir_log(string_from_format("CASE 3: %d ",cliente));

						}
					break;
					case 4:
						{
							escribir_log(string_from_format("CASE 4: %d ",cliente));

						}
					break;
					case 5:
						{
							escribir_log(string_from_format("CASE 5: %d ",cliente));

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

int cantidadDePaginas(int tamanioBytes, int tamanioPaginas)
{
	double Bloques= -1;
	if ((double)tamanioBytes/tamanioPaginas - (int)(tamanioBytes/tamanioPaginas) > 0){
			Bloques = ((tamanioBytes/tamanioPaginas) +1);
		}else{
			Bloques = (int)(tamanioBytes/tamanioPaginas);
		}
	return (int)Bloques;

}

t_list* asignarFramesLibres(int pid,int cantPaginasAgrabar)
{
	//Calcular posición frames libres más próximos para una cantidad solicitada.
	t_list* lt_framesLibres = list_create();
	int z;
	int libres=0;
	int maxPagPid = 0; //Funcion que me devuelva la máxima pagina de un PID.
	for (z=cantMarcosTablaPag; z<cantMarcosTablaPag+cantPaginasAgrabar; z++ )
	{
		if ( tablaPaginas[z].estado == 0 ) {
			list_add(lt_framesLibres,z);
			tablaPaginas[z].estado = 1;
			tablaPaginas[z].pid = pid;
			tablaPaginas[z].pag = maxPagPid;
		}
		maxPagPid ++;
	}

	return lt_framesLibres;

}

void cargarCodigoEnMemoria(char * datos, char * Memoria ,t_list * frames_pedidos, int cantPaginasAgrabar) {
	int c = 0;
	while(c < cantPaginasAgrabar){
		int frame_pos = list_get(frames_pedidos, c);
		int pos = (frame_pos * tamanioMarco)-1;
		memcpy(Memoria+pos,datos+(c*tamanioMarco),tamanioMarco);
		printf("\nSe guardo en el frame: %d \n", frame_pos);
		c++;
	}
}
