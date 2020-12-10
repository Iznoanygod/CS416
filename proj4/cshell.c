#include "cshell.h"

void shellLoop();
void shellExecute(char* line);
void seperateLines(char* line, char** chain, char** pipe);

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
    int i = 0;
    while(1){
        char in = getchar();
        if(in == EOF || in == '\n'){
            buffer[i] = '\0';
            return;
        }
        buffer[i] = in;
        i++;
    }
}
int countDelimiters(char* line){
    char temp[LINESIZE];
    char delim[] = ">|>>";
    strcpy(temp, line);
    int i = 0;
    char* token = strtok(temp, delim);
    while(token != NULL){
        i++;
        token = strtok(NULL, delim);
    }
    return i;
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

void toCommandArgs(char* line, char* command, char** args){
    
}

void shellExecute(char* line){
    int delimCount = countDelimiters(line);
    char** lineSep = malloc((delimCount + 1) * sizeof(char*));
    char** pipeSep = malloc(delimCount * sizeof(char*));
    for(int i = 0; i < delimCount+1; i++){
        lineSep[i] = malloc(sizeof(char) * LINESIZE);
    }
    seperateLines(line, lineSep, pipeSep);
    

}

void seperateLines(char* line, char** chain, char** pipe){
    char temp[LINESIZE];
    strcpy(temp, line);
    char* token = strtok(temp, ">|>>");
    int i = 0;
    while(token != NULL){
        strcpy(chain[i],token);
        i++;
        token = strtok(NULL, ">|>>");
    }
    int counter = 0;
    i = 0;
    while(line[i] != '\0'){
        if(line[i] == '|'){
            pipe[counter] = "|";
            counter++;
        }
        if(line[i] == '>'){
            i++;
            if(line[i] == '>'){
                pipe[counter] = ">>";
                counter++;
            }
            else{
                pipe[counter] = ">";
                counter++;
            }
        }
        i++;
    }
}
