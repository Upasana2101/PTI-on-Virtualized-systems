#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <emmintrin.h>
#include "cacheutils.h"
	
#define CACHE_HIT_THRESHOLD 100
#define MARGIN 1024

int array[256*4096];
void doflush()
{
	int i;

	// Access the array 
	for (i=0; i<256; i++)
		array[i*4096 + MARGIN] = 1;

	// FLush the array elements that could be cached
	for (i=0; i<256; i++)
		_mm_clflush(&array[i*4096 + MARGIN]);
}
/*
uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}
*/
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
	
	doflush();
	array[5*4096 + MARGIN] = 1;
	reload();	
	return 0;
}
