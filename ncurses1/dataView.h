#ifndef _data_view_h_
#define _data_view_h_

// include for malloc()
#include <stdlib.h>

// Estrutura de dados para exibir texto
// Lista duplamente ligada

/*/////////////////////////////////////
//  Estrutura de line                //
//  int     index                    //
//  line    *next                    //
//  line    *previou                 //
//  char    *data                    //
/////////////////////////////////////*/

typedef struct Linha line;
typedef struct Texto text;

// Funções comuns

int textInit(text *txt, unsigned int lineSize);
int addLineAtEnd(text *txt, unsigned int lineSize);
int writeLine(text *txt, int index, char *data);
char * readLine(text *txt, int index);
int clearLine(text *txt, int index);
void freeStructure(text *txt);

// Outras funções

void removeReturn(char *data);
int alloc(line **ptr);
int searchLineByIndex(line **ptr, int index);
int removeLastLine(line **ptr);
void initLineData(line *ptr, unsigned int lineSize);

// struct text
struct Texto
{
	line *pointer;
};

#endif // _data_view_h_
