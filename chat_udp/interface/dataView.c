#include "dataView.h"

// Estrutura de dados para exibir texto
// Lista duplamente ligada
// struct Linha

struct Linha
{
    int     index;
    line    *next;
    line    *previous;
    char    *data;
};

int alloc(line **ptr)
{
//  alocar linha
    *ptr = (line *) malloc(sizeof(line));
    if (ptr == NULL)
    {
//      falha
        return 0;
    }
    else
    {
//      sucesso
        return 1;
    }
}

int textInit(text *txt, unsigned int lineSize)
{
	line *ptr;
    if (!alloc(&ptr))
    {
//          falha de alocacao
            return 0;
    }
//  inicializar estrutura
    ptr->index      = 0;
    ptr->previous   = NULL;
    ptr->next       = NULL;
	ptr->data       = (char *) malloc((lineSize + 1)*sizeof(char));
	initLineData(ptr, lineSize);
	txt->pointer = ptr;
    return 1;
}

int addLineAtEnd(text *txt, unsigned int lineSize)
{
	line *ptr = txt->pointer;
    line *temp = 0;
//  Checar se ponteiro aponta para lista
    if (ptr == NULL)
    {
//      falha, ponteiro n�o aponta para lista
        return 0;
    }
//  procurar final da lista
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
    }
//  adicionar linha
    if (!alloc(&temp))
    {
//      aloca��o falhou
        return 0;
    }
    ptr->next       = temp;
    temp->index     = ptr->index + 1;
    temp->previous  = ptr;
    temp->next      = NULL;
	temp->data      = (char *) calloc((lineSize + 1), sizeof(char));
//  ptr aponta para o final da lista
	txt->pointer = temp;
	initLineData(temp, lineSize);
    return 1;
}

int searchLineByIndex(line **ptr, int index)
{
//	come�o da gambiarra para tratar ponteiro do mal
	line *temp;
	temp = *ptr;
//  cheacar se ponteiro aponta para lista v�lida
    if (temp == NULL)
    {
//      falha, ponteiro nulo
        return 0;
    }
//  procura de indice
    while (temp->index != index)
    {
//      verifica se indice atual � menor que indice procurado
        if (temp->index < index)
        {
            if (temp->next == NULL)
            {
//              falha se proxima linha � nula
                return 0;
            }
            else
            {
//              proxima linha
                temp = temp->next;
            }
        }
//      se indice atual � maior que indice procurado
        else
        {
            if (temp->previous == NULL)
            {
//              falha se linha anterior � nula
                return 0;
            }
            else
            {
//              linha anterior
                temp = temp->previous;
            }
        }

    }
//  la�o executado com sucesso
//	conclus�o da gabiarra, funciona! s� n�o sei como
	*ptr = temp;
    return 1;
}

int writeLine(text *txt, int index, char *data)
{
	line *ptr = txt->pointer;
    int i;
    if (!searchLineByIndex(&ptr, index))
    {
//      se procura falhou, retornar falha
        return 0;
    }
//  ponteiro na posi��o correta
    else
    {
//      condi��o, enquanto n�o encontra caractere nulo, continue
        for (i = 0; (ptr->data[i] != '\0') && (data[i] != '\0'); i++)
        {
//          copiar dados
            ptr->data[i] = data[i];
        }
    }
	txt->pointer = ptr;
//  copia completa retornar
    return 1;
}

char * readLine(text *txt, int index)
{
	line *ptr = txt->pointer;
    if(!searchLineByIndex(&ptr, index))
    {
//      se procura falhar retornar ponteiro NULO
        return NULL;
    }
    else
    {
//      retorno de dados
		txt->pointer = ptr;
        return ptr->data;
    }
}

int removeLastLine(line **ptr)
{
    line *last;
//  testa ponteiro para lista
    if (*ptr == NULL)
    {
//      ponteiro nulo, retornar erro
        return 0;
    }
//  procurar ultima linha
    while ((*ptr)->next != NULL)
    {
        *ptr = (*ptr)->next;
    }
    if ((*ptr)->previous == NULL)
    {
//      n�o h� linhas anteriores
		free((*ptr)->data);
        return 0;
    }
//  h� linhas anteriores
    last            = *ptr;
    *ptr            = (*ptr)->previous;
    (*ptr)->next    = NULL;
    last->previous  = NULL;
	free(last->data);
    free(last);
    return 1;
}

void freeStructure(text *txt)
{
	line *ptr = txt->pointer;
	txt->pointer = NULL;
    while (removeLastLine(&ptr));
}

int clearLine(text *txt, int index)
{
	line *ptr = txt->pointer;
    int i;
    if (!searchLineByIndex(&ptr, index))
    {
//      se procura falhou, retornar falha
        return 0;
    }
//  ponteiro na posi��o correta
    else
    {
//      condi��o, enquanto n�o encontra caractere nulo, continue
        for (i = 0; ptr->data[i] != '\0'; i++)
        {
//          copiar dados
            ptr->data[i] = ' ';
        }
    }
//  clear completo, retornar
	txt->pointer = ptr;
    return 1;
}

void removeReturn(char *data)
{
    int i;
    if (data == NULL)
    {
        return;
    }
    for (i = 0; (data[i] != '\0'); i++)
    {
        if (data[i] == '\n')
        {
            data[i] = ' ';
        }
    }
}

void initLineData(line *ptr, unsigned int lineSize)
{
	unsigned int i;
//	tamanho da string � lineSize + 1
	for (i = 0; i < lineSize; i++)
    {
//          limpar dados
            ptr->data[i] = ' ';
    }
	ptr->data[lineSize] = '\0';
}