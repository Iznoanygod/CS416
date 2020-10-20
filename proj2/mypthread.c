// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"
#include "queue.h"
#include <valgrind/valgrind.h>

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
PQueue* ThreadQueue = NULL;
static unsigned int newTID = 0;
static mypthread_t currentThread;
static threadControlBlock* runningBlock;

static void schedule();
static void sched_stcf();

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
    int mSet = 0;
    if(!newTID){
        currentThread = newTID;
        threadControlBlock* mainBlock = malloc(sizeof(threadControlBlock));
        mainBlock->tid = currentThread;
        mainBlock->waiting = -1;
        runningBlock = mainBlock;
        newTID++;
        mainBlock->status = RUNNABLE;
        mainBlock->quantum = 0;
        mSet = 1;
        
        struct sigaction action;
        struct itimerval timer;

        memset(&action, 0, sizeof(struct sigaction));
        action.sa_handler = &schedule;
        sigaction(SIGPROF, &action, NULL);

        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = TIME_QUANTUM;

        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = TIME_QUANTUM;

        setitimer(ITIMER_PROF, &timer, NULL);
    }

    currentThread = newTID;
    threadControlBlock* threadBlock = malloc(sizeof(threadControlBlock));
    threadBlock->tid = newTID;
    threadBlock->waiting = -1;
    threadBlock->quantum = 0;
    *thread = newTID;
    newTID++;
    threadBlock->status = RUNNABLE;
    enqueue(&ThreadQueue, threadBlock, 0);
    getcontext(&(threadBlock->context));
    threadBlock->context.uc_link = NULL;
    threadBlock->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    threadBlock->context.uc_stack.ss_size = SIGSTKSZ;
    VALGRIND_STACK_REGISTER(threadBlock->context.uc_stack.ss_sp, threadBlock->context.uc_stack.ss_sp + SIGSTKSZ);
    makecontext(&(threadBlock->context), (void*) function, 1, arg); 

    if(mSet){
        getcontext(&(runningBlock->context));
    }
};  

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {
    schedule();
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
    runningBlock->status = FINISHED;
    updateQueueRunnable(&ThreadQueue, runningBlock->tid);
    schedule();
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
    if(checkIfFinished(&ThreadQueue, thread))
        return 0;
    runningBlock->status = SLEEP;
    runningBlock->waiting = thread;
    schedule();
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

    mutex->waiting = NULL;
    mutex->lock = 0;
	
    return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread
    while(1){
        if(mutex->lock == 0){
            mutex->lock = 1;
            return 0;
        }
        else{
            runningBlock->status = SLEEP;
            tcbList* temp = malloc(sizeof(temp));
            temp->next = mutex->waiting;
            temp->block = runningBlock;
            mutex->waiting = temp;
            schedule();
        }
    }
    return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// sfo that they could compete for mutex later.
    
    tcbList* temp = mutex->waiting;
    while(temp != NULL){
        temp->block->status = RUNNABLE;
        tcbList* k = temp;
        temp = temp->next;
        free(k);
    }
    mutex->waiting = NULL;
    mutex->lock = 0;

	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init
	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
    sched_stcf();
#else
	// Choose MLFQ
#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
    signal(SIGPROF, SIG_IGN);
    
    threadControlBlock* prevThread = runningBlock;
    runningBlock = dequeue(&ThreadQueue);
    if(runningBlock == NULL){
        runningBlock = prevThread;
        return;
    }
    prevThread->quantum++;
    enqueue(&ThreadQueue, prevThread, prevThread->quantum);
    struct sigaction action;
    struct itimerval timer;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = &schedule;
    sigaction(SIGPROF, &action, NULL);

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = TIME_QUANTUM;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = TIME_QUANTUM;
    setitimer(ITIMER_PROF, &timer, NULL);

    swapcontext(&(prevThread->context), &(runningBlock->context));
}

// Feel free to add any other functions you need

// YOUR CODE HERE
