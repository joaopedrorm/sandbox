#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if(argc == 1 || argc > 4) {
    fprintf(stderr ,"USAGE: %s [-hk] file1 file2\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if(strncmp(argv[1], "-h", 2) == 0){
    char * description = "Apply bitwise xor between 2 files. \n";
    char * hOption = " -h show this message \n";
    char * kOption = " -k repeat file2 until file1 is consumed (will apply pads if necessary) \n";
    char * stdinOption = " - use stdin for file \n";
    fprintf(stdout, "USAGE: %s [-hk] file1 file2\n%s\n%s\n%s\n%s", argv[0], description, hOption, kOption, stdinOption);
    exit(EXIT_FAILURE);
  }

  if(strncmp(argv[1], "-k", 2) == 0){
    fprintf(stdout, "argumento encontrado = %s \n", argv[1]);
  }

  if(strncmp(argv[1], "-", 1) == 0){
    fprintf(stdout, "argumento encontrado = %s \n ", argv[1]);
  }

  return 0;
}