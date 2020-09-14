#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

// Place any necessary global variables here
int total = 0;
struct timeval oldtv;

void handle_sigfpe(int signum){
    if(total == 100000){
        //end of the loop i guess 
        struct timeval newtv;
        gettimeofday(&newtv, NULL);
        long double time = (newtv.tv_sec - oldtv.tv_sec) * 1000000 + (newtv.tv_usec - oldtv.tv_usec);

        printf("Exceptions Occurred: 100000\n");
        printf("Total Elapsed Time: %Lf microseconds\n", time);
        printf("Average Time Per Exception: %Lf microseconds\n", (time / 100000));


        exit(0);
    }
    total++;
    return;
}

int main(int argc, char *argv[]){
    gettimeofday(&oldtv, NULL);
    signal(SIGFPE, handle_sigfpe);
    int x = 1;
    int y = 0;
    int z = 1;
    z = x / y;
	return z;

}
