#ifndef CPUMANAGER_H_
#define CPUMANAGER_H_

void manejo_conexiones_cpu();
void realizar_handShake_cpu(int);
void agregar_lista_cpu(int , char *);
void inicializar_listas_cpu();
void actualizar_pcb();
void responder_solicitud_cpu(int socket_);
t_program *programa_ejecutando(int socket_);
int get_offset(char *mensaje);
int get_fd(char *mensaje);

#endif /* CPUMANAGER_H_ */
