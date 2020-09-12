#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){
    long double time = 0;
    struct timeval oldtv;
    gettimeofday(&oldtv, NULL);
    for(int i = 0; i < 100000; i++){
        getpid();
    }
    struct timeval newtv;
    gettimeofday(&newtv, NULL);
    time = (newtv.tv_sec - oldtv.tv_sec) * 1000000 + (newtv.tv_usec - oldtv.tv_usec);
    printf("Syscalls Performed: 100000\n");
    printf("Total Elapsed Time: %Lf microseconds\n", time);
    printf("Average Time Per Syscall: %Lf microseconds\n", (time/100000));
	return 0;
}
