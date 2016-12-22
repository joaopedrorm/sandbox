#include "fila_reentrante.h"
#define  FALSE 0
#define  TRUE  1

void InitFila(struct fila *F)
        {
	F->inicio = 0;
	F->fim    = 0;
	sem_init(&(F->slots),0,TAMMAXFILA);
	sem_init(&(F->itens),0,0);
	sem_init(&(F->mutex),0,1);
	F->tammax = TAMMAXFILA;
        }

void InserirFila(struct fila *F, int item)
        {
	sem_wait(&(F->slots));
	sem_wait(&(F->mutex));
	F->buffer[F->fim] = item;
	F->fim = (F->fim +1) % F->tammax;
	sem_post(&(F->mutex));
	sem_post(&(F->itens));
        }

int RetirarFila(struct fila *F) 
        {
	int item;
	sem_wait(&(F->itens));
	sem_wait(&(F->mutex));
	item = F->buffer[F->inicio];
	F->inicio = (F->inicio + 1) % F->tammax;
	sem_post(&(F->mutex));
	sem_post(&(F->slots));
	return(item);
        }
