#include <pthread.h>
#include "fila.h"
#define  FALSE 0
#define  TRUE  1

pthread_mutex_t  mutex;
int produtores;

struct fila Fila;

void produtor(int id)
        {
	int i=0;
	int item;
        int aguardar;
	produtores++;

	printf("Inicio produtor %d \n",id);
	while (i < 10)
	    {
            //produzir item
	    item = i + (id*1000);

            do
	        {
                pthread_mutex_lock(&mutex);
	        aguardar = FALSE;
	        if (FilaCheia(&Fila))
		    {
		    aguardar = TRUE;
	            pthread_mutex_unlock(&mutex);
		    }
		} while (aguardar == TRUE);

	    //inserir item
            printf("Produtor %d inserindo item %d\n", id, item); 
	    InserirFila(&Fila, item);
	    pthread_mutex_unlock(&mutex);
	    i++;
	    sleep(2);
	    }
	printf("Produtor %d terminado \n", id);
	produtores--; 
        }

void consumidor(int id)
{
	int item;
	int aguardar;

	printf("Inicio consumidor %d \n",id);
	while (1)
	{
            // retirar item da fila
		do
		{
                	pthread_mutex_lock(&mutex);
	        	aguardar = FALSE;
	        	if (FilaVazia(&Fila))
		    	{
	            		aguardar = TRUE;
		    		pthread_mutex_unlock(&mutex);
		    		if (produtores == 0)
		   		{
					printf("Consumidor %d terminado \n", id);
					return; 
		    		}
			}
	        } while (aguardar == TRUE);
	    // processar item
	    item = RetirarFila(&Fila);
	    printf("Consumidor %d consumiu item %d\n", id, item); 
	    pthread_mutex_unlock(&mutex);
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

	produtores = 0;
	InitFila(&Fila);

	printf("Programa Produtor-Consumidor\n");

	printf("Iniciando variaveis de sincronizacao.\n");
	pthread_mutex_init(&mutex,NULL);

        printf("Disparando threads produtores\n");
	pthread_create(&prod1, NULL, (void*) produtor,1);
	pthread_create(&prod2, NULL, (void*) produtor,2);
	pthread_create(&prod3, NULL, (void*) produtor,3);

        printf("Disparando threads consumidores\n");
	pthread_create(&cons1, NULL, (void*) consumidor,1);
	pthread_create(&cons2, NULL, (void*) consumidor,2);

	pthread_join(prod1,NULL);
	pthread_join(prod2,NULL);
  	pthread_join(prod3,NULL);
	pthread_join(cons1,NULL);
	pthread_join(cons2,NULL);
	
        printf("Terminado processo Produtor-Consumidor.\n\n");
}

