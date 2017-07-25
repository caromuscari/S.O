#ifndef CPUMANAGER_H_
#define CPUMANAGER_H_

void realizar_handShake_cpu(int);
void agregar_lista_cpu(int , char *);
void inicializar_listas_cpu();
void actualizar_pcb(t_program *programa, t_PCB *pcb);
void responder_solicitud_cpu(int i, char *mensaje_recibido);
t_cpu *programa_ejecutando(int socket_);
int get_offset(char *mensaje);
int get_fd(char *mensaje);
void manejo_conexion_cpu();
char *get_variable(char *mensaje);
char *get_numero(char *mensaje);
void pedir_pcb_error(t_program *prg, int exit_code);
void eliminar_cpu(int socket_);
t_cpu *buscar_cpu(int socket_);

#endif /* CPUMANAGER_H_ */
