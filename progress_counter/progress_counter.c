#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  for(int i = 0; i <= 100; i++){
    printf("%*d%%", 3, i);
    fflush(stdout);
    usleep(50000);
    putchar('\r');
  }

  putchar('\n');
  
  return 0;
}
