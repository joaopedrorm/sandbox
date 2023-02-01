NCURSES := $(basename $(wildcard *.c))
PANEL   := $(basename $(wildcard ./panel/*.c))
MENU    := $(basename $(wildcard ./menu/*.c))
FORMS   := $(basename $(wildcard ./forms/*.c))
ALL     := $(NCURSES) $(PANEL) $(MENU) $(FORMS)

all: $(ALL)

ncurse: $(NCURSES)

panel: $(PANEL)

menu: $(MENU)

forms: $(FORMS)

$(NCURSES): %: %.c
	gcc  $< -o $@ -Wall -lncurses

$(PANEL): %: %.c
	gcc  $< -o $@ -lncurses -lpanel

$(MENU): %: %.c
	gcc  $< -o $@ -lncurses -lmenu

$(FORMS): %: %.c
	gcc  $< -o $@ -lncurses -lform

clean:
	rm -vf $(ALL)

