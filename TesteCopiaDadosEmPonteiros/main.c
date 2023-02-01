#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void func(char * test);

int main(int argc, char * argv[])
{
    char teste[16];
    func(teste);
    printf("teste=%s", teste);
    return 0;
}

void func(char * test)
{
    char * ptr = test;
    char * text = malloc(16*sizeof(char));
    sprintf(text,"teste");
    
    //ptr = text;
    //strncpy(ptr, text, 16);
    //sprintf(ptr, text);
    memcpy(ptr, text, 16);
    free(text);
}
