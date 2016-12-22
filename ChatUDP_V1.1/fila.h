#ifndef _FILA_H_
#define _FILA_H_

#include "servidorChatUDP.h"

#define  FALSE 0
#define  TRUE  1

struct requisicao
{
    struct sockaddr_in  *addr;
    struct requisicao   *proxima;
    char                *buffer;
};

struct fila
{
    sem_t               mutex;
	sem_t               itens;
    struct requisicao   *primeira;
    struct requisicao   *ultima;
    int                 n_requisicoes;
};

struct fila * InitFila(void);
int FilaVazia(struct fila *F);
void InserirFila(struct fila *F, struct requisicao *R);
struct requisicao * RetirarFila(struct fila *F);
struct requisicao * NovaRequisicao(void);

#endif // _FILA_H_
