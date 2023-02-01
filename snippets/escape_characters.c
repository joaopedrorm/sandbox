#include <stdio.h>

#define RESET_COLOR        "\x1b[0m"

#define BLACK              "\x1b[30m"
#define RED                "\x1b[31m"
#define GREEN              "\x1b[32m"
#define YELLOW             "\x1b[33m"
#define BLUE               "\x1b[34m"
#define MAGENTA            "\x1b[35m"
#define CYAN               "\x1b[36m"
#define WHITE              "\x1b[37m"

#define BRIGHT_BLACK       "\x1b[30;1m"
#define BRIGHT_RED         "\x1b[31;1m"
#define BRIGHT_GREEN       "\x1b[32;1m"
#define BRIGHT_YELLOW      "\x1b[33;1m"
#define BRIGHT_BLUE        "\x1b[34;1m"
#define BRIGHT_MAGENTA     "\x1b[35;1m"
#define BRIGHT_CYAN        "\x1b[36;1m"
#define BRIGHT_WHITE       "\x1b[37;1m"

#define CURSOR_UP(N)       "\x1b["#N"A"
#define CURSOR_DOWN(N)     "\x1b["#N"B"
#define CURSOR_RIGHT(N)    "\x1b["#N"C"
#define CURSOR_LEFT(N)     "\x1b["#N"D"


#define CLEAR_SCREEN_END   "\x1b[0J"         // clears screen from cursor until end of screen,
#define CLEAR_SCREEN_START "\x1b[1J"         // clears screen from cursor to beginning of screen
#define CLEAR_SCREEN       "\x1b[2J"         // clears entire screen

#define CLEAR_LINE_END     "\x1b[0K"         // clears line from cursor to end of line
#define CLEAR_LINE_START   "\x1b[1K"         // clears line from cursor to start of line
#define CLEAR_LINE         "\x1b[2K"         // clears entire line

#define NEXT_LINE(N)       "\x1b["#N"E"      // moves cursor to beginning of line n lines down
#define PREV_LINE(N)       "\x1b["#N"F"      // moves cursor to beginning of line n lines down

#define SET_COLUMN(X)      "\x1b["#X"G"      // moves cursor to column X
#define SET_POSITION(X,Y)  "\x1b["#Y";"#X"H" // moves cursor to row Y column X

#define POSITION_SAVE      "\x1b[{s}"        // saves the current cursor position
#define POSITION_RESTORE   "\x1b[{u}"        // restores the cursor to the last saved position

// https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

int main() {
    printf("Hello, world!\n");
    printf(BRIGHT_RED "HelloWorld\n" RESET_COLOR);
    printf (CURSOR_UP(2) "" CURSOR_RIGHT(20) " Testing ");
    printf (CURSOR_DOWN(2) "\n");
    return 0;
}
  
