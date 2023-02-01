#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define CNC_ERRO             1
#define CNC_SUCESSO          0
#define MAX_BUFFER_LENGTH  100

int CalculaDiaPascoa (int pAno, char *pMsg, size_t TamMaxMsg);

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <ano>\nAno deve estar dentro do intervalo 1582-2099\n", argv[0]);
        return CNC_ERRO;
    }
    else 
    {
        char * strtol_end;
        errno = 0;
        long int ano = strtol(argv[1], &strtol_end, 10);
        if (errno == ERANGE || *strtol_end != '\0' || strtol_end == argv[1] || ano < INT_MIN || ano > INT_MAX)
        {
            printf("Erro ao tentar obter ano: entrada=%s, caracteres inválidos=%s, valor obtido=%ld", argv[1], strtol_end, ano);
            return CNC_ERRO;
        }
        char pMsg[MAX_BUFFER_LENGTH] = "";
        int ret = CalculaDiaPascoa(ano, pMsg, MAX_BUFFER_LENGTH);
        printf("%s\n", pMsg);
        return ret;
    }
}

int CalculaDiaPascoa (int pAno, char *pMsg, size_t TamMaxMsg)
{
  /*
      Calcula o Domingo de Páscoa de um ano.
      obs: pDomingoPascoa deve ter no mínimo tamanho 9 bytes.

      Pe: pAno de referência para cálculo do domingo de páscoa, 4 dígitos
          TamMaxMsg c/o tamanho da área apontada por pMsg

      Ps: pDomingoPascoa apontando para área com o dia calculado, formato aaaammdd
          pMsg c/o endereço da área para colocar eventual mensagem de erro.

      Retorna:  0 = CNC_SUCESSO caso domingo de páscoa calculados corretamente
               <0 = CNC_ERRO caso tenha ocorrido algum erro
  */

  int a, b, c, d, e, M, n;

  if ((pAno >= 1582) && (pAno <= 1699))
  {
    M = 22;
    n = 2;
  }
  else
  if ((pAno >= 1700) && (pAno <= 1799))
  {
    M = 23;
    n = 3;
  }
  else
  if ((pAno >= 1800) && (pAno <= 1899))
  {
    M = 24;
    n = 4;
  }
  else
  if ((pAno >= 1900) && (pAno <= 2099))
  {
    M = 24;
    n = 5;
  }
  else
  {
    snprintf (pMsg, TamMaxMsg, "%d é um ano inválido, (1582-2099)", pAno);
    pMsg[TamMaxMsg - 1] = 0;
    return (CNC_ERRO);
  }

  a = pAno % 4;
  b = pAno % 7;
  c = pAno % 19;
  d = (19 * c + M) % 30;
  e = (2 * a + 4 * b + 6 * d + n) % 7;

  if ((22 + d + e) > 31)  // formato aaaammdd
    snprintf (pMsg, TamMaxMsg, "%04d04%02d", pAno, (d + e - 9));
  else
    snprintf (pMsg, TamMaxMsg, "%04d03%02d", pAno, (22 + d + e));
  
  pMsg[TamMaxMsg - 1] = 0;

  return (CNC_SUCESSO);
}
 
