// Arquivo: fila.h

// Declaracao da fila
#include <semaphore.h>

#define TAMMAXFILA 4

struct fila {
	sem_t mutex;
	sem_t slots;
	sem_t itens;
	int buffer[TAMMAXFILA];
	int inicio;
	int fim;
	int nitens;
	int tammax;
} ;

int InitFila(struct fila *F);
int FilaVazia(struct fila *F);
int FilaCheia(struct fila *F);
void InserirFila(struct fila *F, int item);
int RetirarFila(struct fila *F);



