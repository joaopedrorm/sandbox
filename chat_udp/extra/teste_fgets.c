#include <stdio.h>

int main()
{
    char buffer[10];
    fgets(buffer, 10, stdin);
    for (int i = 0; i < 10; i++)
    {
        printf(" %c;%d;%d  ", buffer[i], buffer[i], i);
    }
    getchar();
    return 0;
}
