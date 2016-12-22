//arquivo prodcons.c

#include <pthread.h>
#include "fila.h"
#include <stdio.h>

#define  FALSE 0
#define  TRUE  1

struct fila Fila;

void produtor(int id)
{
	int i = 0;
	int item;

	printf("Inicio produtor %d \n", id);
	while (i < 10)
	{
		//produzir item
		item = i + (id * 1000);
		InserirFila(&Fila, item);
		i++;
		sleep(2);
	}
	printf("Produtor %d terminado \n", id);
}

void consumidor(int id)
{
	int item;
	printf("Inicio consumidor %d \n", id);
	while (1)
	{
		item = RetirarFila(&Fila);
		sleep(2);
	}
}

int main()
{
	pthread_t prod1;
	pthread_t prod2;
	pthread_t prod3;
	pthread_t cons1;
	pthread_t cons2;

	InitFila(&Fila);

	printf("\nPrograma Produtor-Consumidor\n");

	printf("\nDisparando threads produtores\n");
	pthread_create(&prod1, NULL, (void*)produtor, 1);
	pthread_create(&prod2, NULL, (void*)produtor, 2);
	pthread_create(&prod3, NULL, (void*)produtor, 3);

	printf("\nDisparando threads consumidores\n");
	pthread_create(&cons1, NULL, (void*)consumidor, 1);
	pthread_create(&cons2, NULL, (void*)consumidor, 2);

	pthread_join(prod1, NULL);
	pthread_join(prod2, NULL);
	pthread_join(prod3, NULL);
	pthread_join(cons1, NULL);
	pthread_join(cons2, NULL);

	printf("\nTerminado processo Produtor-Consumidor.\n\n");
}
