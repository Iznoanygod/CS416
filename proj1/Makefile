CC = gcc
CFLAGS = -g -Wall

all: TimeSysCall TimeFork TimeSignal

TimeSysCall: TimeSysCall.c
	$(CC) $(CFLAGS) TimeSysCall.c -o TimeSysCall

TimeFork: TimeFork.c
	$(CC) $(CFLAGS) TimeFork.c -o TimeFork

TimeSignal: TimeSignal.c
	$(CC) $(CFLAGS) TimeSignal.c -o TimeSignal

clean:
	rm -f TimeSysCall TimeFork TimeSignal
