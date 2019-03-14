#include <stdio.h>
#include <signal.h>

void signal_handler(int signal)
{
	if (signal == SIGSEGV)
		printf("Signal handled \n");
}

int main()
{
	unsigned long kernel_addr = 0x000000009de5d7e7;
	
	if (signal (SIGSEGV, signal_handler) == SIG_ERR)
		printf("SIGSEGV not handled \n");

	char kernel_data = *(char*)kernel_addr;
	printf("kernel data : %c \n", kernel_data);
	printf("Continuing.. \n");
	return 0;
}
