#include "mypthread.h"

void asd(void* arg);
int main(){
    mypthread_t thread;
    mypthread_create(&thread, NULL, (void*) asd, NULL);
    mypthread_yield();
}

void asd(void* arg){
    printf("test\n");
    return;
}
