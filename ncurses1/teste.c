// teste de dataView
#include <stdio.h>
#include "dataView.h"

int main()
{
    text texto;
    unsigned int linesize, colsize, i;
    char teste[80] = {0}, *tmp;
    linesize = 20;
    colsize  = 10;
    textInit(&texto, linesize);
    for (i = 0; i < colsize; i++)
    {
        teste[i] = (i+48);
        writeLine(&texto, i, teste);
        if(!addLineAtEnd(&texto, linesize))
			exit(1);
    }
    for (i = 0; i < colsize; i++)
    {
        tmp = readLine(&texto, i);
        puts(tmp);
    }
    for (i = 0; i < colsize; i++)
    {
        clearLine(&texto, i);
        tmp = readLine(&texto, i);
        puts(tmp);
    }
	freeStructure(&texto);
    getchar();
    return 0;
}
