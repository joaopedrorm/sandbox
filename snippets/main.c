#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[])
{
  printf("argc = %d \n", argc);
  int i;
  for(i = 0; i < argc; i++){
    printf("%d - %s\n", i, argv[i]);    
  }
        
  printf("sizeof = %d\n", strlen(argv[0]));
  return 0;
}
