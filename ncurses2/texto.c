#include <stdio.h>
#include <stdlib.h>

typedef struct Line line;

struct Line
{
	unsigned int	index;
	int		buffersize;
	char		*buffer;
	line		*next;
};

void lineInit (line *first, int buffersize)
{
	first->index = 0;
	first->next = NULL;
	first->buffer = malloc((size_t)buffersize*sizeof(char));
	first->buffersize = buffersize;
}

void addLine (line *first, int buffersize)
{
	line		*ptr;
	unsigned int	index;
	line		newline;
	index = 1;	
	ptr = first;
	while (ptr->next != NULL)
	{
		ptr = ptr->next;
		index++;
	}
	ptr->next = &newline;
	ptr = ptr->next;
	ptr->index = index;
	ptr->next = NULL;
	ptr->buffer = malloc((size_t)buffersize*sizeof(char));
	ptr->buffersize = buffersize;
}

int removeLastLine (line *first)
{
	line		*ptr;
	line		*previous;
	if (first->next == NULL)
	{
//		retornar sem fazer nada
		return 0;
	}	
	previous = first;
	ptr = first->next;
	while (ptr->next != NULL)
	{
		previous = previous->next;
		ptr = ptr->next;
	}
	previous->next = NULL;
	free(ptr);
	return 1;
}

void lineDestructor (line *first)
{
	while (removeLastLine(first));
	free(first);
}

int writeLine (line *first, unsigned int index, char *msg)
{
	line		*ptr;
	int		i;
	ptr = first;
	while ( (ptr->index != index) && (ptr->next != NULL) )
	{
		ptr = ptr->next;
	}
	if ( (ptr->index != index) && (ptr->next == NULL) )
	{
//		retornar, index não encontrado
		return 0;
	}
	for (i=0; i < ptr->buffersize; i++)
	{
		ptr->buffer[i] = msg[i];
		if (msg[i] == '\0')
		{
			break;
		}
	}
	return 1;
}

void clearLine (line *first, unsigned int index)
{
	line		*ptr;
	int		i;
	ptr = first;
	while ( (ptr->index != index) && (ptr->next != NULL) )
	{
		ptr = ptr->next;
	}
	if ( (ptr->index != index) && (ptr->next == NULL) )
	{
//		retornar, index não encontrado
		return;
	}
	for (i=0; i < ptr->buffersize; i++)
	{
		ptr->buffer[i] = ' ';
	}
}

char *readLine(line *first, unsigned int index)
{
	line		*ptr;
	int		i;
	ptr = first;
	while ( (ptr->index != index) && (ptr->next != NULL) )
	{
		ptr = ptr->next;
	}
	if ( (ptr->index != index) && (ptr->next == NULL) )
	{
//		retornar, index não encontrado
		return " ";
	}
	return ptr->buffer;
}

int main()
{
	return 0;
}
