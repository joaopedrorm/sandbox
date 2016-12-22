all: teste

teste: ncurses_teste.o
	gcc -o teste ncurses_teste.o -lncurses -L/tmp/libncurses/lib/ 

ncurses_teste.o: ncurses_teste.c
	gcc -c -I /tmp/libncurses/include/ ncurses_teste.c

clean:
	rm ncurses_teste.o teste

