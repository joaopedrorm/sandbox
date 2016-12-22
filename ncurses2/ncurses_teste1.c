#include <ncurses.h>	
#include <string.h>

#define MAX_COL 80
#define MAX_LINE 50

void clearString(char *buffer, int size)
{
	int j;
	for (j = 0; j < size; j++)
	{
		buffer[j] = ' ';
	}
}

void writeString(char *buffer, int size, char *text)
{
	int j;
	for (j = 0; ((j < size) && (text[j] != '\0')); j++)
	{
		buffer[j] = text[j];
	}
}

int main()
{
	char buffer[MAX_LINE][MAX_COL], msg[MAX_COL];		
	int row,col;				
	int i, j;
//	inicializacao, tomar tamanho do terminal
	initscr();				
	getmaxyx(stdscr,row,col);

//	inicializacao do buffer
	for (i = 0; i < (row-1); i++)
	{
		clearString(buffer[i], MAX_COL);
	} 

	for (j = 0; j < 4; j++)
	{
		clear();		

//		rotina de impressão na tela
		for (i = 0; i < (row-1); i++)
		{
			mvprintw(i,0,"%s",buffer[i]);
		}  

		mvprintw(row-2,0,"> ");

//		capturar texto
//		for (i = 0; ; i++)
//		{
//			
//		}
		scanw("%s", msg);
		writeString(buffer[j], MAX_COL, msg);
		clearString(msg, MAX_COL);

	
		
		refresh();
	}	

	mvprintw(row-1,0,"Programa terminou, digite qualquer tecla para encerrar.");	
	getch();
	endwin();
	return 0;
}
