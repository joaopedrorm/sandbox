#include <stdio.h>
#include <string.h>

int main()
{
  char * str[] = gets(stdin);
  int i;
  for(i = 0; i < 1; i++){
    printf("%d - %s\n", i, str[i]);    
  }
        
  return 0;
}
