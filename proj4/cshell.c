#include "cshell.h"

void shellLoop();
void shellExecute(char* line);

int isExec = 0;
char running = 1;
static jmp_buf env;

void printPrompt(){
    char path[PTHSIZE];
    
    getcwd(path, PTHSIZE);

    printf("%s >", path);
}

void IntHandler(int signum) {
    siglongjmp(env, 86);
}

int main(){
    signal(SIGINT, IntHandler);

    shellLoop();
    
    return EXIT_SUCCESS;
}

void readLine(char* buffer){
    //read a whole ass line
}
void shellLoop(){
    do{
        if(sigsetjmp(env, SIGINT) == 86){
            printf("\n");
        }
        printPrompt();
        char line[LINESIZE] = {0};
        readLine(line);
        shellExecute(line);
    }while(running);
}

void shellExecute(char* line){
    
}

