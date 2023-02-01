#include <stdlib.h>
#include <curses.h>


int main(int argc, char *argv[])
{
    int num = 0;
	int i;
    
    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) echo();         /* echo input - in color */

    if (has_colors())
    {
        start_color();

        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }

    for (i = 0; i < 80; i++)
    {
        printw(" teste %d\n", num);   /* refresh, accept single keystroke of input */
        attrset(COLOR_PAIR(num % 8));
        num++;
	if (i == 47)
	{
			getch();
		clear();
	}
	refresh();
        /* process the command keystroke */
    }
	getch();
	endwin();

    /* do your non-curses wrapup here */

    exit(0);
           /* we're done */
}

