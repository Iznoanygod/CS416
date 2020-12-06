#include "cshell.h"

void shellLoop();

void printPrompt(){
    char path[PTHSIZE];
    getcwd(path, PTHSIZE);
    printf("%s $", path);
}

void IntHandler(int signum) {
    printf("Caught signal %d, coming out...\n", signum);
}

int main(){
    signal(SIGINT, IntHandler);

    shellLoop();
    
    return EXIT_SUCCESS;
}

void shellLoop(){
    char running = 1;
    do{
        printPrompt();

        char line[LINESIZE] = {0};
        scanf("%s", line);
    }while(running);
}


