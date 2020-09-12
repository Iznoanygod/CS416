#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){
    struct timeval oldtv;
    long double time = 0;
    gettimeofday(&oldtv, NULL);
    for(int i = 0; i < 5000; i++){
        int pid = fork();
        int status;
        if(pid == 0)
            exit(0);
        wait(&status);
    }
    struct timeval newtv;
    gettimeofday(&newtv, NULL);
	
    time = (newtv.tv_sec - oldtv.tv_sec) * 1000000 + (newtv.tv_usec - oldtv.tv_usec);
    printf("Syscalls Performed: 5000\n");
    printf("Total Elapsed Time: %Lf microseconds\n", time);
    printf("Average Time Per Syscall: %Lf microseconds\n", (time/5000));
	return 0;

}
