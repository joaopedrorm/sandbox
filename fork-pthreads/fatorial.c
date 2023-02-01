#include <stdio.h>

char versao[] = "2.1";
unsigned long int n;
unsigned long int resultado;

unsigned long int fatorial (unsigned long int x)
  {
  unsigned long int y;
  if (x <= 1)
    return(1);
  else
    {
    y = x * fatorial(x-1);
    return (y);
    }
  }

int main()
  {
  printf("Programa fatorial, versao %s \n", versao);
  printf("Entre com o valor: " );
  scanf("%lu",&n);
  resultado = fatorial(n);
  printf("Resultado: %lu \n",resultado);
  }	

