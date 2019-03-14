#include<stdio.h> 
#include<signal.h> 
#include <stdlib.h>
#include <setjmp.h>
#include<unistd.h>

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

int main() 
{ 
    signal(SIGSEGV, handle_segfault); 
 
    int fault_code = setjmp(restore_point);
    unsigned long data_addr = 0xfb61b000;
    if(fault_code == 0)
    {
        char secret_data = *(char*)data_addr; 
    }
    else
    printf("Process Proceeds Normally\n");
    return 0; 
} 