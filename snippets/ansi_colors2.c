#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "ansi_colors.h"

int main (int argc, char const *argv[]) {
  printf(FBLU("I'm blue.\n"));
  printf(BOLD(FBLU("I'm blue-bold.\n")));

  char * env = getenv("PATH");

  printf("getenv = %s \n", env);

  if (_isatty(_fileno(stdout))) {
	  printf(BOLD(FBLU("In Terminal.\n")));
  }
  else {
	  printf("Not In Terminal.\n");
  }

  return 0;
}