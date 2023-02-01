#include <ncurses.h>
#include <string.h>

#define WIDTH 30
#define HEIGHT 10

int startx = 0;
int starty = 0;

char *choices[] = { 	"Choice 1",
			"Choice 2",
			"Choice 3",
			"Choice 4",
			"Exit",
		  };

int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight);
void report_choice(int mouse_x, int mouse_y, int *p_choice);

int main()
{	int c, choice = 0;
	WINDOW *menu_win;
	MEVENT event;

	/* Initialize curses */
	initscr();
	clear();
	noecho();
	cbreak();	//Line buffering disabled. pass on everything

	/* Make cursor invisible */
	curs_set(0);

	/* Try to put the window in the middle of screen */
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;

	attron(A_REVERSE);
	mvprintw(23, 1, "Double Click on Exit to quit (Works best in a virtual console), click to select option and double click to accept option");
	refresh();
	attroff(A_REVERSE);

	/* Print the menu for the first time */
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	print_menu(menu_win, choice);

    // Enables keypad mode. This makes (at least for me) mouse events getting
    // reported as KEY_MOUSE, instead as of random letters.
	keypad(menu_win, TRUE);

    /* Get all the mouse events */
	mousemask(ALL_MOUSE_EVENTS, NULL);

	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_MOUSE:
                if(getmouse(&event) == OK)
                {	/* When the user clicks left mouse button */
					mvprintw(26, 0, "Mouse button x = %3d , y = %3d", event.x, event.y);
                    if(event.bstate & BUTTON1_CLICKED)
                    {	/* Only report choice */
						report_choice(event.x + 1, event.y + 1, &choice);
                    }
                    else if(event.bstate & BUTTON1_DOUBLE_CLICKED)
					{   report_choice(event.x + 1, event.y + 1, &choice);
                        if(choice == n_choices) //Exit chosen
                            goto end;
                        mvprintw(22, 1, "Choice made is : %d String Chosen is \"%10s\"", choice, choices[choice - 1]);
                        refresh();
					}
                }
                else
				{
					mvprintw(26, 0, "Mouse button pressed is = %3d Hopefully it can be printed as '%c'", c, c);
					refresh();
				}
                print_menu(menu_win, choice);
                break;
            case 'q':
                goto end;
            default:
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
	}
end:
	endwin();
	return 0;
}


void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1)
		{	wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

/* Report the choice according to mouse position */
void report_choice(int mouse_x, int mouse_y, int *p_choice)
{	int i,j, choice;

	i = startx + 2;
	j = starty + 3;

	for(choice = 0; choice < n_choices; ++choice)
		if(mouse_y == j + choice && mouse_x >= i && mouse_x <= i + strlen(choices[choice]))
		{
			// if(choice == n_choices - 1)
			// 	*p_choice = -1;
			// else
				*p_choice = choice + 1;
			break;
		}
}
