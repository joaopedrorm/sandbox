#include <ncurses.h>

int main()
{
        initscr();

        printw("Checker board (stipple)     "); addch(ACS_CKBOARD); printw("\n");  printw("\n");

        printw("     "); addch(ACS_ULCORNER); for(int j = 0; j < 20; j++) addch(ACS_HLINE); addch(ACS_URCORNER); printw("\n");
        for(int j = 0; j < 10; j++) {
                printw("     ");addch(ACS_VLINE);
                for(int i = 0; i < 10; i++) {
                        if((j%2==0 && i%2==0)||(j%2==1 && i%2==1)) {
                                addch(ACS_CKBOARD);
                                addch(ACS_CKBOARD);
                        } else {
                                printw("  ");
                        }
                }
                addch(ACS_VLINE);printw("\n");
        }
        printw("     "); addch(ACS_LLCORNER); for(int j = 0; j < 20; j++) addch(ACS_HLINE); addch(ACS_LRCORNER);
        printw("\n\n");

        refresh();
        getch();
        endwin();

	return 0;
}
