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
        exit(0);
    }
    total++;
    return;
}

int main(int argc, char *argv[]){
    gettimeofday(&oldtv, NULL);
    signal(SIGFPE, handle_sigfpe);
    int foo = 1 / 0;
	return 0;

}
