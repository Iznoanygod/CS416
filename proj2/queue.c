#include <stdlib.h>
#include "queue.h"

void enqueue(PQueue** queue, threadControlBlock* block, int quantum){
    if(queue == NULL)
        return;
    PQueue * temp = malloc(sizeof(PQueue));
    temp->next = NULL;
    temp->control = block;
    temp->quantum = quantum;
    
    PQueue* front = *queue;
    
    if(front == NULL){
        *queue = temp;
        return;
    }

    if(front->quantum > quantum){
        temp->next = front;
        *queue = temp;
        return;
    }

    PQueue* trail = front;
    PQueue* lead = front->next;
    while(lead != NULL && lead->quantum < quantum){
        trail = lead;
        lead = lead->next;
    }
    temp->next = lead;
    trail->next = temp;
    *queue = front;
}
threadControlBlock* dequeue(PQueue** queue){
    PQueue* front = *queue;
    if(front == NULL)
        return NULL;
    if(front->control->status == RUNNABLE){
        *queue = front->next;
        threadControlBlock* block = front->control;
        free(front);
        return block;
    }
    PQueue* trail = front;
    PQueue* lead = front->next;
    while(lead != NULL && lead->control->status !=RUNNABLE){
        trail = lead;
        lead = lead->next;
    }
    if(lead == NULL)
        return NULL;
    trail->next = lead->next;
    threadControlBlock* block = lead->control;
    free(lead);
    return block;
}
void updateQueueRunnable(PQueue** queue, mypthread_t waiting){
    PQueue *temp = *queue;
    while(temp != NULL){
        if(temp->control->waiting == waiting && temp->control->status == SLEEP){
            temp->control->waiting = -1;
            temp->control->status = RUNNABLE;
        }
        temp = temp->next;
    }
    return;
}
int checkIfFinished(PQueue** queue, mypthread_t waiting){
    PQueue* temp = *queue;
    while(temp != NULL){
        if(temp->control->tid == waiting &&temp->control->status == FINISHED)
            return 1;
        temp = temp->next;
    }
    return 0;
}
threadControlBlock* getBlock(PQueue** queue, mypthread_t tid){
    PQueue* temp = *queue;
    while(temp != NULL){
        if(temp->control->tid == tid)
            return temp->control;
        temp = temp->next;

    }
    return NULL;
}

