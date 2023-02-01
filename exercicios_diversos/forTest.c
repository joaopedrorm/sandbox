#include <stdio.h>

int main(char argc, char * argv[]) {
    int v[] = {1,2,3,4,5,6,7,8,9,10};
    for (int i = sizeof(v)/sizeof(v[0]); i--;){
        printf(" [i=%d,v[i]=%d] ", i, v[i]);
    }
    return 0;
}
