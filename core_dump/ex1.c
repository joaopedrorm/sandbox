#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
 
static void core_dump(int sigid)
{
  printf("Signal %d", sigid);
  abort();
}

int main()
{
    signal(SIGINT, core_dump);
    while(1){
      printf("Waiting...\n");
      usleep(100000);
    }
}