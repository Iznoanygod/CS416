#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"
pthread_mutex_t mutex;
void idle(){
    printf("EXIT\n");
    mypthread_mutex_lock(&mutex);

    mypthread_mutex_unlock(&mutex);
    mypthread_exit(4);
}

int main(){
    int returnv;
    mypthread_mutex_init(&mutex, NULL);
    mypthread_t thread;
    mypthread_create(&thread, NULL, &idle, NULL);
    printf("Joining\n");
    mypthread_mutex_lock(&mutex);
    mypthread_yield();
    mypthread_mutex_unlock(&mutex);
    mypthread_join(thread, &returnv);
    printf("%d\n", returnv);
}
