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
int countDelimiters(char* line, char* delim){
    char temp[LINESIZE];
    strcpy(temp, line);
    int i = 0;
    char* token = strtok(temp, delim);
    while(token != NULL){
        i++;
        token = strtok(NULL, delim);
    }
    return i-1;
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

void toCommandArgs(char* cline, char* command, char** args){
    char line[LINESIZE];
    strcpy(line, cline);
    char* exec = strtok(line, " ");
    strcpy(command, exec);
    strcpy(args[0], exec);
    int i = 1;
    char* token = strtok(NULL, " ");
    while(token != NULL){
        strcpy(args[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

int isProper(char* cline){
    int delimiters = 0;
    for(int i = 0; cline[i] != '\0'; i++){
        if(cline[i] == ';')
            delimiters++;
        if(cline[i] == '|')
            delimiters++;
        if(cline[i] == '>'){
            if(i-1 == -1 || cline[i-1] == '>')
                continue;
            delimiters++;
        }
    }
    char line[LINESIZE];
    
    strcpy(line, cline);
    if(strtok(line, " \t\n") == NULL)
        return -1;
    strcpy(line, cline);
    char* saveptr;
    int cmd = 0;
    char* token = strtok_r(line, ";>|>>", &saveptr);
    while(token != NULL){
        if(strtok(token, " ") == NULL){
            return 0;
        }
        token = strtok_r(NULL, ";>|>>", &saveptr);
        cmd++;
    }
    if(cmd != (delimiters + 1))
        return 0;
    return 1;
}

void shellExecute(char* line){
    int linestate = isProper(line);
    if(!linestate){
        printf("Error: incomplete redirect\n");
        return;
    }
    if(linestate == -1){
        return;
    }
    int pipeCount = countDelimiters(line, ";>|>>");
    char** lineSep = malloc((pipeCount + 1) * sizeof(char*));
    char** pipeSep = malloc(pipeCount * sizeof(char*));
    for(int i = 0; i < pipeCount+1; i++){
        lineSep[i] = malloc(sizeof(char) * LINESIZE);
    }
    
    seperateLines(line, lineSep, pipeSep);
    
    //loop after here
    int useIn = 0;
    int useOut = 0;
    int inPipe[2];
    int outPipe[2];
    int fileRedir = 0;
    int trunc = 0;
    for(int i = 0; i < pipeCount+1; i++){
        int lineC = countDelimiters(lineSep[i], " ");
        char command[LINESIZE];
        char** args = malloc(sizeof(char*) * (lineC + 2));
        for(int i = 0; i < lineC + 1; i++)
            args[i] = malloc(LINESIZE);
        toCommandArgs(lineSep[i], command, args);

        //start parsing commands
        if(!strcmp("exit", command)){
            free(pipeSep);
            for(int i = 0; i < pipeCount + 1; i++)
                free(lineSep[i]);
            free(lineSep);
            for(int i = 0; i < lineC + 1; i++)
                free(args[i]);
            free(args);
            running = 0;
            return;
        }
        if(!strcmp("cd", command)){
            int success = chdir(args[1]);
            if(success == -1){
                printf("Error: no such file or directory: %s\n", args[1]);
            }
            continue;
        }
        if(!strcmp("csh-reload", command)){
            execl("./cshell", NULL);
        }
        //set up pipe stuffs
        if(useIn)
            inPipe[READ] = outPipe[READ];
        if(pipeCount != 0 && pipeCount != i && pipeSep[i][0] == '|'){
            //pipe output
            useOut = 1;
            pipe(outPipe);
        }
        if(pipeCount != 0 && pipeCount != i && !strcmp(pipeSep[i],">>")){
            useOut = 1;
            fileRedir = 1;
            pipe(outPipe);
        }
        if(pipeCount != 0 && pipeCount != i && !strcmp(pipeSep[i], ">")){
            useOut = 1;
            fileRedir = 1;
            trunc = 1;
            pipe(outPipe);
        }
        int PID = fork();
        if(PID == 0){
            //child process
            if(useOut){
                dup2(outPipe[WRITE],WRITE);
            }
            if(useIn){
                dup2(inPipe[READ], READ);
            }
            execvp(command, args);
            printf("Error: command not found: %s\n", command);
            exit(0);
        }
        wait(NULL);
        if(fileRedir){
            close(outPipe[WRITE]);
            char* fileP = strtok(lineSep[i+1], " ");
            int fd = open(fileP, O_CREAT | O_RDWR | ((trunc) ? O_TRUNC : O_APPEND), S_IRUSR | S_IWUSR);
            if(fd == -1){
                printf("Error: cannot open file '%s'\n", fileP);
                longjmp(env, 32);
            }
            char buffer[512];
            int inByte = read(outPipe[READ], buffer, 512);
            while(inByte){
                write(fd, buffer, inByte);
                inByte = read(outPipe[READ], buffer, 512);
            }
            close(outPipe[READ]);
            free(pipeSep);
            for(int i = 0; i < lineC+1; i++)
                free(args[i]);
            free(args);
            for(int i = 0; i < pipeCount + 1; i++)
                free(lineSep[i]);
            free(lineSep);
            longjmp(env, 32);
        }
        if(useIn){
            useIn = 0;
            close(inPipe[READ]);
        }
        if(useOut){
            useOut = 0;
            close(outPipe[WRITE]);
            useIn = 1;
        }
        for(int i = 0; i < lineC+1; i++)
            free(args[i]);
        free(args);
    }
    free(pipeSep);
    for(int i = 0; i < pipeCount + 1; i++)
        free(lineSep[i]);
    free(lineSep);
}

void seperateLines(char* line, char** chain, char** pipe){
    char temp[LINESIZE];
    strcpy(temp, line);
    char* token = strtok(temp, ";>|>>");
    int i = 0;
    while(token != NULL){
        strcpy(chain[i],token);
        i++;
        token = strtok(NULL, ";>|>>");
    }
    int counter = 0;
    i = 0;
    while(line[i] != '\0'){
        if(line[i] == '|'){
            pipe[counter] = "|";
            counter++;
        }
        if(line[i] == ';'){
            pipe[counter] = ";";
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
