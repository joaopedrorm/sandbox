#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  include para estrutura de dados de socket
#include <netdb.h>

//  include para uso de semaforo
#include <semaphore.h>

//  include para socket
#include <sys/socket.h>
#include <unistd.h>

//  include para threads
#include <pthread.h>


/*////////////////////////////////////////////////////////////////////////////////
// BUFFERSIZE       95 = (80 bytes msg + 5 bytes CMD + 10 bytes USERNAMESIZE)	//
// LOGONBUFFERSIZE  15 = (10 bytes username + 5 bytes CMD)						//
// LOGOFFBUFFERSIZE  5 bytes CMD         				               			//
// USERNAMESIZE     10 bytes username    			     		       			//
// CMDSIZE           5 bytes CMD                                       			//
// MSGSIZE 			80 bytes										   			//
// TX_BUFFER_SIZE = RX_BUFFER_SIZE = BUFFERSIZE			              			//
// 1byte \0  -> levado em consideração na implementação				  			//
////////////////////////////////////////////////////////////////////////////////*/

#define UDP             17
#define RX_BUFFER_SIZE  95
#define TX_BUFFER_SIZE  95
#define MSGSIZE			80
#define USERNAMESIZE 	10
#define CMDSIZE		     5
#define CLIENT_LIMIT     3
#define PORT            "10000"

//	comandos do cabeçalho
#define CMD_USER 	"USER:"
#define CMD_UP      "UP  :"
#define CMD_EXIT 	"EXIT:"
#define CMD_OKOK 	"OKOK:"
#define CMD_DOWN 	"DOWN:"
#define CMD_BUSY 	"BUSY:"
#define CMD_TEST 	"TEST:"

//	estrutura de dados cliente
struct cliente
{
	struct sockaddr_in 	addr;
	char                nome[USERNAMESIZE + 1];
//	se isFree = 0, então estrutura ocupada, 1 caso contrario
	int					isFree;
};

//  variáveis globais
struct fila     *Fila;
struct cliente  clientes[CLIENT_LIMIT];
//  descritor de socket
int             socket_master;
int             continuar;

//  declaração de funções
void inicializar(void);
void receptor(void);
void processaMsg(void);
int	insereCliente(struct sockaddr_in *addr, char *nome);
int procuraCliente(struct sockaddr_in *addr);
int removeCliente(struct sockaddr_in *addr);
void incializarClientes(void);
void escreveMsg(char *cmd, char *msg, int msg_size, char *buffer);
void recebeMsg(char *cmd, char *msg, int msg_size, char *buffer);
void broadcastMsg(char *msg);
char * escreveNome(char *msg, int n_cliente);

#endif
