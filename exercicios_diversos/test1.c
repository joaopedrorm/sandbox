#include <stdio.h> 
#include <string.h>

void FirstReverse(char * str[]) { 
  
  // code goes here 
  int i;
  int j;
  for(i = 0; str[i]; i ++){
      int str_size = strlen(str[i]);
      for (j = 0; j < (str_size/2) ; j++){
          char temp = str[i][j];
          str[i][j] = str[i][str_size - j - 1];
          str[i][str_size - j - 1] = temp;
      }
  }
  
  printf("%s", str); 
            
}

int main(void) { 

  // disable stdout buffering
  setvbuf(stdout, NULL, _IONBF, 0);
  
  // keep this function call here
  FirstReverse(gets(stdin));
  return 0;
    
} 