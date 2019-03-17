#define _POSIX_SOURCE
#include<stdio.h> 
#include<signal.h> 
#include<stdlib.h>
#include<setjmp.h>
#include<unistd.h>

sigjmp_buf restore_point;

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
    //if (sigsetjmp(jbuf, 1) == 0) { meltdown_asm(0xfb61b000); }
    int fault_code;
    for(int i=0; i< 10; i++) {
        unsigned long data_addr = 0xfb61b000;
        fault_code = sigsetjmp(restore_point, 1);
        if(fault_code == 0)
        {
            char secret_data = *(char*)data_addr; 
        }
        else
        printf("Process Proceeds Normally\n");
    }
    printf("Exiting\n");
    return 0; 
} 