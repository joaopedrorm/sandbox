#include <semaphore.h>
#define TAMMAXFILA 10

struct fila {
	int buffer[TAMMAXFILA];
	int inicio;
	int fim;
	int tammax;
	sem_t mutex;
	sem_t itens;
	sem_t slots;
} ;

void InitFila(struct fila *F);
void InserirFila(struct fila *F, int item);
int RetirarFila(struct fila *F);
