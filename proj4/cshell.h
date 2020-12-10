#ifndef _CSHELL_H_
#define _CSHELL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PTHSIZE 512
#define LINESIZE 512

enum{READ = 0, WRITE = 1};

#endif
