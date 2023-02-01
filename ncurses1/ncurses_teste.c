#include <ncurses.h>	
#include "dataView.h"


WINDOW *createWin(int height, int width, int starty, int startx)
{	
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
//	wborder(local_win, '|', '|', '-','-','*','*','*','*');
	touchwin(local_win);	
	wrefresh(local_win);		
	return local_win;
}

int main()
{
	int i, j;
	text texto;
	WINDOW *bufferWin;
	WINDOW *commandWin;
	char teste[80] = {0};

//	inicializacao, tomar tamanho do terminal
	initscr();
	refresh();

//	COLS -> numero de colunas
//	LINES -> numero de linhas
//	parametros default do ncurses
	bufferWin = createWin(LINES - 4, COLS, 0, 0);
	commandWin = createWin(4, COLS, LINES - 3, 0);
	textInit(&texto, COLS - 2);

//	LINES - 6 -> 2 linhas para box, 4 para commandWin
	for (i = 0; i < (LINES - 6); i++)
	{
		if(!addLineAtEnd(&texto, COLS - 2))
		{
			exit(1);
		}
	}
	for (i = 0; i < (LINES - 6); i++)
    	{
        	teste[i] = (i+48);
        	writeLine(&texto, i, teste);
    	}
	getch();

//	rotina de impressão na tela
	for (i = 1; i < (LINES - 6); i++)
	{
		mvwprintw(bufferWin, i, 1,"%s",readLine(&texto, i - 1));
	}  
	wrefresh(bufferWin);
	getch();
	
	wclear(bufferWin);
	getch();

	mvwprintw(commandWin, 1, 1,"> ");

	wrefresh(bufferWin);
//	capturar texto
//	for (i = 0; ; i++)
//	{
//		
//	}
	getch();
	refresh();
	mvwprintw(LINES-1,1,"Programa terminou, digite qualquer tecla para encerrar.");	
	getch();
	delwin(bufferWin);
	delwin(commandWin);
	endwin();
	return 0;
}
