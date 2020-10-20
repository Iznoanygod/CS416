CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

SCHED = PSJF

all: queue.o mypthread.a

queue.o:
	$(CC) $(CFLAGS) queue.c -o queue.o

mypthread.a: mypthread.o
	$(AR) libmypthread.a mypthread.o queue.o
	$(RANLIB) libmypthread.a

mypthread.o: mypthread.h

ifeq ($(SCHED), PSJF)
	$(CC) -pthread $(CFLAGS) mypthread.c
else ifeq ($(SCHED), MLFQ)
	$(CC) -pthread $(CFLAGS) -DMLFQ mypthread.c
else
	echo "no such scheduling algorithm"
endif

clean:
	rm -rf testfile *.o *.a
