#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
void thingy(void* arg){
    printf("hello\n");
    mypthread_exit(NULL);
}
int main(int argc, char **argv) {
    mypthread_t * thread = malloc(sizeof(mypthread_t));
    mypthread_create(thread, NULL, *thingy, NULL);
    mypthread_join(*thread, NULL);
	free(thread);
    printf("Finished\n");
}

