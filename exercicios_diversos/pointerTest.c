#include <stdio.h>

int main(char argc, char * argv[]) {
    char arr[] = "uma string";
    char * p = arr - 1000;
    for (int i = 0; i < 2000; ++i){
        if (*p == 0){
            printf("0");
        } else {
            printf("%c", *p);
        }
        p += 1;
    }
    return 0;
}
