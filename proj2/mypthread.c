// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"
#include "queue.h"

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
        mainBlock->waiting = NULL;
        runningBlock = mainBlock;
        newTID++;
        mainBlock->status = RUNNABLE;
        mainBlock->quantum = 0;
        mSet = 1;
        
        struct sigaction action;
        struct itimerval timer;

        memset(&action, 0, sizeof(action));
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
    threadBlock->waiting = NULL;
    *thread = newTID;
    newTID++;
    threadBlock->status = RUNNABLE;
    enqueue(&ThreadQueue, threadBlock, 0);
    getcontext(&(threadBlock->context));
    threadBlock->context.uc_link = NULL;
    threadBlock->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    threadBlock->context.uc_stack.ss_size = SIGSTKSZ;
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
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
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
    
    struct sigaction action;
    struct itimerval timer;

    memset(&action, 0, sizeof(action));
    action.sa_handler = &schedule;
    sigaction(SIGPROF, &action, NULL);

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = TIME_QUANTUM;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = TIME_QUANTUM;
    setitimer(ITIMER_PROF, &timer, NULL);

    ucontext_t currContext;
    getcontext(&currContext);
    swapcontext(&(prevThread->context), &(runningBlock->context));
}

// Feel free to add any other functions you need

// YOUR CODE HERE
