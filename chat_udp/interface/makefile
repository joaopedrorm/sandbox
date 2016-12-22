all: ncurses_teste

ncurses_teste: ncurses_teste.o
	gcc -lncurses -o ncurses_teste ncurses_teste.o

ncurses_teste.o: ncurses_teste.c
	gcc -c ncurses_teste.c

clean:
	rm ncurses_teste.o ncurses_teste

