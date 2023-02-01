all: teste

teste: ncurses_teste.o dataView.o
	gcc -lncurses -o teste ncurses_teste.o dataView.o

ncurses_teste.o: ncurses_teste.c
	gcc -c ncurses_teste.c dataView.c -std=c99

clean:
	rm ncurses_teste.o teste dataView.o

