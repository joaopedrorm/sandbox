// arquivo: fila.c

#include "fila.h"

#define  FALSE 0
#define  TRUE  1

int InitFila(struct fila *F)
        {
	sem_init(&(F->mutex), 0, 1);
	sem_init(&(F->slots), 0, TAMMAXFILA);
	sem_init(&(F->itens), 0, 0);
	F->inicio = 0;
	F->fim    = 0;
	F->nitens = 0;
	F->tammax = TAMMAXFILA;
        }

int FilaVazia(struct fila *F)
        {
	if (F->nitens == 0)
		return(TRUE);
	else
		return(FALSE);
        }

int FilaCheia(struct fila *F)
        {
	if (F->nitens == F->tammax)
		return(TRUE);
	else
		return(FALSE);
        }

void InserirFila(struct fila *F, int item)
        {
	sem_wait(&(F->slots));
	sem_wait(&(F->mutex));
	F->buffer[F->fim] = item;
	F->fim = (F->fim +1) % F->tammax;
	F->nitens = F->nitens + 1;
	printf("Produzido item %d\n", item);
	sem_post(&(F->itens));
	sem_post(&(F->mutex));
        }

int RetirarFila(struct fila *F)
        {
	sem_wait(&(F->itens));
	sem_wait(&(F->mutex));
	int item;
        item = F->buffer[F->inicio];
	F->inicio = (F->inicio + 1) % F->tammax;
	F->nitens = F->nitens - 1;
	printf("Consumido item %d\n",item);
	sem_post(&(F->slots));
	sem_post(&(F->mutex));
	return(item);
        }

