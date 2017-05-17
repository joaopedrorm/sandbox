#include <stdio.h>
#include <ctype.h>

#define BYTE_ARRAY_SIZE 256

int numPlaces (unsigned long n) {
    int r = 1;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

int main(int argc, char* argv[]) {
  long bytes[BYTE_ARRAY_SIZE] = { 0 };
  char c = '\0';
  FILE* pFile;
  unsigned long fileLen;
  int ret;

  if(argc != 2) {
    printf("USAGE: %s <FILENAME>", argv[0]);
    return 0;
  }

  pFile = fopen(argv[1],"rb");
  if(pFile == NULL) {
    perror("Erro ao abrir arquivo");
    return 1;
  }

  ret = fseek(pFile, 0, SEEK_END);
  if(ret != 0) {
    printf("Erro ao procurar fim do arquivo");
    return 2;
  }

  fileLen = ftell(pFile);
  if(fileLen == 0) {
    printf("Arquivo vazio");
    return 0;
  }

  ret = fseek(pFile, 0 ,SEEK_SET);
  if(ret != 0) {
    printf("Erro ao procurar começo do arquivo");
    return 3;
  }

  for(unsigned long i = 0; i < fileLen; i++) {
    c = fgetc(pFile);
    bytes[c] += 1;
  }

  printf("Arquivo: %s - Tamanho: %d bytes\n", argv[1], fileLen);

  for(int i = 0; i < BYTE_ARRAY_SIZE; i++) {
    printf("| 0x%-2X - %c - %5d - %3d %% |", i, isprint(i) ? i : '?', bytes[i], (bytes[i]*100/fileLen));
    if( (i+1)%2 == 0) {
      printf("\n");
    }
  }
  
  return 0;
}
