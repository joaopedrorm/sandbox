#include "fila.h"

struct fila * InitFila(void)
{
    struct fila *F;
    F = (struct fila *) malloc(sizeof(struct fila));
    sem_init(&(F->mutex), 0, 1);
	sem_init(&(F->itens), 0, 0);
    F->n_requisicoes = 0;
    F->primeira      = NULL;
    F->ultima        = NULL;
    return F;
}

int FilaVazia(struct fila *F)
{
    //  checar se fila é válida
    if (F == NULL)
    {
        printf("ERRO: fila invalida ao inserir requisicao.");
        exit(1);
    }
    // checar se fila vazia
    if (F->n_requisicoes == 0)
        return(TRUE);
    else
        return(FALSE);
}

void InserirFila(struct fila *F, struct requisicao *Req)
{
    struct requisicao *ultimaRequisicao;

//  mutex e atribuição de ultimaReq para facilitar leitura de codigo 
    sem_wait(&(F->mutex));
    ultimaRequisicao = F->ultima;

//  checar se fila é válida
    if (F == NULL)
    {
        printf("ERRO: fila invalida ao inserir requisicao.");
        exit(1);
    }

//  checar se fila está vazia
    if (FilaVazia(F))
    {
//      nenhuma requisicao na fila
        F->primeira = Req;
        F->ultima   = Req;
        F->n_requisicoes++;
    }
    else
    {
        ultimaRequisicao->proxima   = Req;
        F->ultima                   = Req;
        F->n_requisicoes++;
    }
    sem_post(&(F->itens));
	sem_post(&(F->mutex));
}

struct requisicao * RetirarFila(struct fila *F)
{
    struct requisicao *Req;

//  mutex e semáforo    
    sem_wait(&(F->itens));
    sem_wait(&(F->mutex));

//  checar se fila é válida
    if (F == NULL)
    {
        printf("ERRO: fila invalida ao retirar requisicao.");
        exit(1);
    }

    if (FilaVazia(F))
    {
        printf("ERRO: tentando retirar de fila vazia. \n");
        exit(1);
    }
    else if (F->n_requisicoes == 1)
    {
//      ultima requisicao da lista
        Req                 = F->primeira;
        F->primeira         = NULL;
        F->ultima           = NULL;
        F->n_requisicoes    = 0;
    }
    else
    {
        Req                 = F->primeira;
        F->primeira         = Req->proxima;
        F->n_requisicoes--;
    }
    Req->proxima = NULL;
    sem_post(&(F->mutex));
    return(Req);
}

struct requisicao * NovaRequisicao(void)
{
    struct requisicao *Req;
    Req = (struct requisicao *) malloc(sizeof(struct requisicao));
    if (Req != NULL)
    {
        (Req->addr) = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
        if (Req->addr == NULL)
        {
            free(Req);
            return NULL;
        }
        else
        {
            Req->buffer = malloc(sizeof(char)*(RX_BUFFER_SIZE +1));
            if (Req->buffer == NULL)
            {
                free(Req);
                return NULL;
            }
        }
    }
    return Req;
}