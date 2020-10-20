#ifndef _QUEUE_H_

#include "mypthread.h"

#define _QUEUE_H_ 1
typedef struct PQueue{
    struct PQueue* next;
    threadControlBlock* control;
    int quantum;
}PQueue;
void enqueue(PQueue** queue, threadControlBlock* block, int quantum);
threadControlBlock* dequeue(PQueue** queue);
#endif
