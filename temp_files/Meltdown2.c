#define _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

#define CACHE_HIT_THRESHOLD 100
int array[256*4096];
static sigjmp_buf restore_point;


void handle_segfault(int sig)
{

  // printf("Caught signal %d\n", sig);
  signal(SIGSEGV, &handle_segfault);
  longjmp(restore_point, SIGSEGV);

}


int main()
{
    signal(SIGSEGV, handle_segfault);

    
    //make scores array as 0 
    //read proc entry
    //repeat 5 times 
    
}

